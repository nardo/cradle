// The notify_sockets library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

class notify_connection;
struct connection_parameters;

/// notify_socket class.
///
/// Manages all valid and pending notify protocol connections for a single open network socket (port/IP address).  A notify_socket instance is not thread safe, so should only be used from a single thread.  The notify_sockets library is thread safe in the sense that notify_socket instances may be created on different threads.
class notify_socket : public ref_object
{
	friend class notify_connection;

/// packet_type is encoded as the first byte of each packet.
	///
	/// Packets received on a socket with a first byte >= first_valid_info_packet_id and < 128 will be passed along in the form of net_socket_packet_event events.  All other first byte values are reserved by the implementation of notify_sockets.
	
	/// Packets that arrive with the high bit of the first byte set (i.e. the first unsigned byte is greater than 127), are assumed to be connected protocol packets, and are dispatched to the appropriate connection for further processing.
public:
	enum packet_type
	{
		connect_challenge_request_packet, ///< Initial packet of the two-phase connect process
		connect_challenge_response_packet, ///< Response packet to the ChallengeRequest containing client identity, a client puzzle, and possibly the server's public key.
		connect_reject_packet, ///< A client puzzle submission failed - this could be because of server puzzle timing rotation, failure to supply randomized data to the API (two connecting clients supply identical nonces) or because of an attack.  If a connection attempt receives this, notify_sockets will by default re-randomize the client nonce and try again once.
		connect_request_packet, ///< A connect request packet, including all puzzle solution data and connection initiation data.
		connect_accept_packet, ///< A packet sent to notify a host that a connection was accepted.
		disconnect_packet, ///< A packet sent to notify a host that the specified connection has terminated or that the requested connection is politely rejected.
		introduced_connection_request_packet, ///< sent from the initiator and host to the introducer.  An introducer will ignore introduced_connection_request packets until a call to net_socket_introduce is made.  Once introduced_connection_request packets are received from both initiator and host, and upon subsequent receipt of introduced_connection_request packets, the introducer will send connection_introduction packets to introducer and host.
		connection_introduction_packet, ///< Packet sent by introducer to properly connect initiator and host.
		punch_packet, ///< Packets sent by initiator or host of an introduced connection to "punch" a connection hole through NATs and firewalls.

		first_valid_info_packet_id = 32, ///< The first valid first byte of an info packet sent from a notify_socket
		last_valid_info_packet_id = 127, ///< The last valid first byte of an info packet sent from a notify_socket 
	};
protected:
	enum net_socket_constants
	{
		challenge_retry_count = 4, ///< Number of times to send connect challenge requests before giving up.
		challenge_retry_time = 2500, ///< Timeout interval in milliseconds before retrying connect challenge.
		
		connect_retry_count = 4, ///< Number of times to send connect requests before giving up.
		connect_retry_time = 2500, ///< Timeout interval in milliseconds before retrying connect request.
		
		punch_retry_count = 6, ///< Number of times to send groups of firewall punch packets before giving up.
		punch_retry_time = 2500, ///< Timeout interval in milliseconds before retrying punch sends.
		
		introduced_connection_connect_timeout = 45000, ///< interval a pending hosted introduced connection will wait between challenge response and connect request
		timeout_check_interval = 1500, ///< Interval in milliseconds between checking for connection timeouts.
		puzzle_solution_timeout = 30000, ///< If the server gives us a puzzle that takes more than 30 seconds, time out.
		introduction_timeout = 30000, ///< Amount of time the introducer tracks a connection introduction request.
	};
	
	enum disconnect_reason
	{
		reason_failed_puzzle,
		reason_self_disconnect,
		reason_shutdown,
		reason_reconnecting,
		reason_disconnect_call,
	};
	

	/// returns the current process time for this notify_socket
	time get_process_start_time()
	{
		return _process_start_time;
	}
	
	void _send_introduction_request(pending_connection *the_connection)
	{
		notify_connection *introducing_connection = _find_connection(the_connection->_introducer);
		if(!introducing_connection)
			return;
        logf(notify_socket, ("Sending Introduction Request for %d to %d via %d (%s)", the_connection->_connection_index, the_connection->_remote_client_id, introducing_connection->_connection_index, introducing_connection->get_address().to_string().c_str()));
		packet_stream out;
		write(out, uint8(introduced_connection_request_packet));
		write(out, introducing_connection->get_initiator_nonce());
		write(out, uint32(the_connection->_remote_client_id));
		write(out, uint8(the_connection->get_type() == pending_connection::introduced_connection_initiator));
		out.send_to(_socket, introducing_connection->get_address());		
	}
	
	void _handle_introduction_request(const address &addr, bit_stream &stream)
	{
		nonce requestor_nonce;
		uint32 remote_id;
		uint8 is_initiator;
		
		read(stream, requestor_nonce);
		read(stream, remote_id);
		read(stream, is_initiator);
		// TODO - validate/encrypt introduction requests
		// TODO - add self-determined addresses for each party to the introduction request.
		// TODO - refactor introductions as indexes off is_host
		notify_connection *requesting_connection = _find_connection(addr);
        logf(notify_socket, ("received introduction request: %d to %d (%d)", requesting_connection ? requesting_connection->get_connection_index() : 0, remote_id, is_initiator));
		if(!requesting_connection || requesting_connection->get_initiator_nonce() != requestor_nonce)
			return;
		
		uint32 initiating_connection, hosting_connection;
		if(is_initiator)
		{
			initiating_connection = requesting_connection->get_connection_index();
			hosting_connection = remote_id;
		}
		else
		{
			initiating_connection = remote_id;
			hosting_connection = requesting_connection->get_connection_index();
		}
		
        logf(notify_socket, ("received introduced connection request: %d(initiator) %d(host), from %s", initiating_connection, hosting_connection, is_initiator ? "initiator" : "host"));
		
		for(uint32 i = 0; i < _introductions.size(); i++)
		{
			introduction_record &intro = _introductions[i];
			if(intro.initiator == initiating_connection && intro.host == hosting_connection)
			{
                logf(notify_socket, ("found introduction record"));
				if(is_initiator)
					intro.initiator_request_received = true;
				else
					intro.host_request_received = true;
				if(intro.initiator_request_received && intro.host_request_received)
				{
					if(!intro.initial_intro_sent)
					{
						_send_introduction(intro.initiator, intro.host, intro.initiator_nonce, intro.host_nonce);
						_send_introduction(intro.host, intro.initiator, intro.initiator_nonce, intro.host_nonce);
					}
					else
					{
						if(is_initiator)
							_send_introduction(intro.initiator, intro.host, intro.initiator_nonce, intro.host_nonce);
						else
							_send_introduction(intro.host, intro.initiator, intro.initiator_nonce, intro.host_nonce);
					}
				}
			}
		}
	}
	
	void _send_introduction(notify_connection_id intro_target, notify_connection_id remote_host, nonce &initiator_nonce, nonce &host_nonce)
	{
		notify_connection *connection = _find_connection(intro_target);
		notify_connection *remote = _find_connection(remote_host);
		if(!connection || !remote)
			return;
		
		logf(notify_socket, ("Sending introduction for %d to %d", intro_target, remote_host));
		packet_stream out;
		write(out, uint8(connection_introduction_packet));
		write(out, connection->get_initiator_nonce());
		write(out, uint32(remote_host));
		write(out, remote->get_address());
		write(out, initiator_nonce);
		write(out, host_nonce);
		
		out.send_to(_socket, connection->get_address());
	}

	void _handle_introduction(const address &the_address, bit_stream &packet_stream)
	{
		notify_connection *introducer = _find_connection(the_address);
		nonce introducer_nonce;
		uint32 remote_id;
		address remote_address;
		nonce initiator_nonce, host_nonce;
		
		read(packet_stream, introducer_nonce);
		
		if(introducer->get_initiator_nonce() != introducer_nonce)
			return;
		
		read(packet_stream, remote_id);
		read(packet_stream, remote_address);
		read(packet_stream, initiator_nonce);
		read(packet_stream, host_nonce);
		for(pending_connection *walk = _pending_connections; walk; walk = walk->_next)
		{
			if(walk->_introducer == introducer->get_connection_index() && walk->_remote_client_id == remote_id && walk->get_state() == pending_connection::requesting_introduction)
			{
                logf(notify_socket, ("connection %d received introduction from %d to %d", walk->_connection_index, walk->_introducer, remote_id));
				walk->_initiator_nonce = initiator_nonce;
				walk->_host_nonce = host_nonce;
				walk->_possible_addresses.push_back(remote_address);
				walk->set_state(pending_connection::sending_punch_packets);
				walk->_state_send_retry_count = punch_retry_count;
				walk->_state_send_retry_interval = punch_retry_time;
				walk->_state_last_send_time = get_process_start_time();
				_send_punch(walk);
				return;
			}
		}
	}
	
	void _send_punch(pending_connection *the_connection)
	{
		for(uint32 i = 0; i < the_connection->_possible_addresses.size(); i++)
		{
			logf(notify_socket, ("Sending punch packet to %s", the_connection->_possible_addresses[i].to_string().c_str()));
			packet_stream out;
			write(out, uint8(punch_packet));
			write(out, the_connection->get_initiator_nonce());
			write(out, the_connection->get_host_nonce());
			out.send_to(_socket, the_connection->_possible_addresses[i]);			
		}
	}
	
	void _handle_punch(const address &the_address, bit_stream &packet_stream)
	{
		nonce initiator_nonce, host_nonce;
		read(packet_stream, initiator_nonce);
		read(packet_stream, host_nonce);
		
		pending_connection *the_connection;
		for(pending_connection *walk = _pending_connections; walk; walk = walk->_next)
		{
			if(walk->get_initiator_nonce() == initiator_nonce && walk->get_host_nonce() == host_nonce && walk->get_state() == pending_connection::sending_punch_packets && walk->get_type() == pending_connection::introduced_connection_initiator)
			{
                logf(notify_socket, ("received punch for %d", walk->_connection_index));
				walk->set_state(pending_connection::requesting_challenge_response);
				walk->_address = the_address;
				walk->_state_send_retry_count = challenge_retry_count;
				walk->_state_send_retry_interval = challenge_retry_time;
				walk->_state_last_send_time = get_process_start_time();
				_send_challenge_request(walk);
			}
		}
	}
		
	/// Sends a connect challenge request on behalf of the connection to the remote host.
	void _send_challenge_request(pending_connection *the_connection)
	{
		logf(notify_socket, ("Sending Connect Challenge Request to %s", the_connection->get_address().to_string().c_str()));
		packet_stream out;
		write(out, uint8(connect_challenge_request_packet));
		write(out, the_connection->get_initiator_nonce());
		write(out, the_connection->get_host_nonce());
		out.send_to(_socket, the_connection->get_address());
	}
	
	/// Handles a connect challenge request by replying to the requestor of a connection with a unique token for that connection, as well as (possibly) a client puzzle (for DoS prevention), or this net_socket's public key.
	void _handle_connect_challenge_request(const address &addr, bit_stream &stream)
	{
		logf(notify_socket, ("Received Connect Challenge Request from %s", addr.to_string().c_str()));
		
		if(!_allow_connections)
			return;
		
		nonce initiator_nonce, host_nonce;
		read(stream, initiator_nonce);

		// In the case of an introduced connection we will already have a pending connenection for this address.  If so, validate that the nonce is correct.
		pending_connection *conn;
		for(conn = _pending_connections; conn; conn = conn->_next)
		{
			if(conn->get_state() == pending_connection::sending_punch_packets && conn->get_type() == pending_connection::introduced_connection_host && initiator_nonce == conn->get_initiator_nonce() && host_nonce == conn->get_host_nonce())
			{
				conn->_address = addr;
				conn->set_state(pending_connection::awaiting_connect_request);
				conn->_state_send_retry_count = 0;
				conn->_state_send_retry_interval = introduced_connection_connect_timeout;
				conn->_state_last_send_time = get_process_start_time();
				break;
			}
		}
		_send_connect_challenge_response(addr, initiator_nonce);
	}
	
	/// Sends a connect challenge request to the specified address.  This can happen as a result of receiving a connect challenge request, or during an "arranged" connection for the non-initiator of the connection.
	void _send_connect_challenge_response(const address &addr, nonce &initiator_nonce)
	{
		packet_stream out;
		write(out, uint8(connect_challenge_response_packet));
		write(out, initiator_nonce);
		
		uint32 identity_token = compute_client_identity_token(addr, initiator_nonce);
		write(out, identity_token);
		
		// write out a client puzzle
		uint32 difficulty = _puzzle_manager.get_current_difficulty();
		write(out, _puzzle_manager.get_current_nonce());
		write(out, difficulty);
		write(out, _private_key->get_public_key());
		write(out, _challenge_response);

		logf(notify_socket, ("Sending Challenge Response: %8x", identity_token));
		out.send_to(_socket, addr);
	}
	
	/// Processes a connect_challenge_response; if it's correctly formed and for a pending connection that is requesting_challenge_response, post a challenge_response event and awayt a local_challenge_accept.
	
	void _handle_connect_challenge_response(const address &the_address, bit_stream &stream)
	{
		pending_connection *conn = _find_pending_connection(the_address);
		if(!conn || conn->get_state() != pending_connection::requesting_challenge_response)
			return;
		
		nonce initiator_nonce, host_nonce;
		read(stream, initiator_nonce);
		
		if(initiator_nonce != conn->get_initiator_nonce())
			return;

		read(stream, conn->_client_identity);
		
		// see if the server wants us to solve a client puzzle
		read(stream, conn->get_host_nonce());
		read(stream, conn->_puzzle_difficulty);
		
		if(conn->_puzzle_difficulty > client_puzzle_manager::max_puzzle_difficulty)
			return;
		
		conn->_public_key = new asymmetric_key(stream);
		if(!conn->_public_key->is_valid())
			return;

		if(_private_key.is_null() || _private_key->get_key_size() != conn->_public_key->get_key_size())
		{
			// we don't have a private key, so generate one for this connection
			conn->_private_key = new asymmetric_key(conn->_public_key->get_key_size());
		}
		else
			conn->_private_key = _private_key;
		conn->set_shared_secret(conn->_private_key->compute_shared_secret_key(conn->_public_key));
		//logf(notify_socket, ("shared secret (client) %s", conn->get_shared_secret()->encodeBase64()->get_buffer()));
		_random_generator.random_buffer(conn->_symmetric_key, symmetric_cipher::key_size);

		logf(notify_socket, ("Received Challenge Response: %8x", conn->_client_identity ));

		byte_buffer_ptr response_data;
		read(stream, response_data);

		net_socket_event *event = _event_queue.post_event(notify_connection_challenge_response_event_type, the_address, conn->_connection_index);
		_event_queue.set_event_key(event, conn->_public_key->get_public_key()->get_buffer(), conn->_public_key->get_public_key()->get_buffer_size());
		_event_queue.set_event_data(event, response_data->get_buffer(), response_data->get_buffer_size());

		conn->set_state(pending_connection::awaiting_local_challenge_accept);
		conn->_state_send_retry_count = 0;
		conn->_state_send_retry_interval = introduction_timeout;
		conn->_state_last_send_time = get_process_start_time();
	}
	
	/// Sends a connect request on behalf of a pending connection.
	void _send_connect_request(pending_connection *conn)
	{
		logf(notify_socket, ("Sending Connect Request"));
		packet_stream out;
		
		write(out, uint8(connect_request_packet));
		write(out, conn->get_initiator_nonce());
		write(out, conn->get_host_nonce());
		write(out, conn->_client_identity);
		write(out, conn->_puzzle_difficulty);
		write(out, conn->_puzzle_solution);
		
		uint32 encrypt_pos = 0;
	
		write(out, conn->_private_key->get_public_key());
		encrypt_pos = out.get_next_byte_position();
		out.set_byte_position(encrypt_pos);
		out.write_bytes(conn->_symmetric_key, symmetric_cipher::key_size);

		write(out, conn->get_initial_send_sequence());
		write(out, conn->_packet_data);
		
		// Write a hash of everything written into the packet, then  symmetrically encrypt the packet from the end of the public key to the end of the signature.
		symmetric_cipher the_cipher(conn->get_shared_secret());
		bit_stream_hash_and_encrypt(out, notify_connection::message_signature_bytes, encrypt_pos, &the_cipher);
		out.send_to(_socket, conn->get_address());
	}
	
	/// Handles a connection request from a remote host.
	///
	/// This will verify the validity of the connection token, as well as any solution to a client puzzle this notify_socket sent to the remote host.  If those tests pass, and there is not an existing pending connection in awaiting_connect_request state it will construct a pending connection instance to track the rest of the connection negotiation.
	void _handle_connect_request(const address &the_address, bit_stream &stream)
	{
		nonce initiator_nonce;
		nonce host_nonce;
		
		read(stream, initiator_nonce);
		read(stream, host_nonce);
		
		// check if this connection has already been accepted:
		notify_connection *existing = _find_connection(the_address);
		if(existing && existing->get_initiator_nonce() == initiator_nonce && existing->get_host_nonce() == host_nonce)
			_send_connect_accept(existing);
		
		// see if there's a pending connection from that address
		pending_connection *pending = _find_pending_connection(the_address);
		
		// if the pending connection has the same nonces and is in an awaiting_local_accept state, assume this is a duplicated connection request packet
		if(pending && pending->get_state() == pending_connection::awaiting_local_accept && pending->get_initiator_nonce() == initiator_nonce && pending->get_host_nonce() == host_nonce)
			return;
		
		// if anonymous connections are not allowed, there must be a pending introduced connection waiting for a connect request
		if(!_allow_connections && !(pending && pending->get_state() == pending_connection::awaiting_connect_request))
			return;

		uint32 client_identity;
		read(stream, client_identity);		
		if(client_identity != compute_client_identity_token(the_address, initiator_nonce))
		{
			logf(notify_socket, ("Client identity disagreement, params say %i, I say %i", client_identity, compute_client_identity_token(the_address, initiator_nonce)));
			return;
		}

		uint32 puzzle_difficulty;
		uint32 puzzle_solution;
		read(stream, puzzle_difficulty);
		read(stream, puzzle_solution);
		
		// check the puzzle solution
		client_puzzle_manager::result_code result = _puzzle_manager.check_solution(puzzle_solution, initiator_nonce, host_nonce, puzzle_difficulty, client_identity);
		
		if(result != client_puzzle_manager::success)
		{
			_send_connect_reject(initiator_nonce, host_nonce, the_address, reason_failed_puzzle);
			return;
		}
		if(pending && (pending->get_initiator_nonce() != initiator_nonce || pending->get_host_nonce() != host_nonce))
			return;
		
		if(_private_key.is_null())
			return;
		
		ref_ptr<asymmetric_key> public_key = new asymmetric_key(stream);
		uint32 decrypt_pos = stream.get_next_byte_position();
		
		stream.set_byte_position(decrypt_pos);
		byte_buffer_ptr shared_secret = _private_key->compute_shared_secret_key(public_key);
		//logf(notify_socket, ("shared secret (server) %s", shared_secret->encodeBase64()->get_buffer()));
		
		symmetric_cipher the_cipher(shared_secret);
		
		if(!bit_stream_decrypt_and_check_hash(stream, notify_connection::message_signature_bytes, decrypt_pos, &the_cipher))
			return;
		
		if(!pending)
			pending = new pending_connection(pending_connection::connection_host, initiator_nonce, _random_generator.random_integer(), _next_connection_index++);
		
		// now read the first part of the connection's symmetric key
		stream.read_bytes(pending->_symmetric_key, symmetric_cipher::key_size);
		_random_generator.random_buffer(pending->_init_vector, symmetric_cipher::key_size);
		
		uint32 connect_sequence;
		read(stream, connect_sequence);
		logf(notify_socket, ("Received Connect Request %8x", client_identity));
		
		if(existing)
			_disconnect(existing->get_connection_index(), reason_self_disconnect, 0, 0);
				
		pending->_host_nonce = host_nonce;
		pending->set_shared_secret(shared_secret);
		pending->set_address(the_address);
		pending->set_initial_recv_sequence(connect_sequence);
		
		pending->set_symmetric_cipher(new symmetric_cipher(pending->_symmetric_key, pending->_init_vector));
		
		byte_buffer_ptr connect_request_data;
		read(stream, connect_request_data);

		_add_pending_connection(pending);

		net_socket_event *event = _event_queue.post_event(notify_connection_requested_event_type, the_address, pending->_connection_index);
		_event_queue.set_event_key(event, public_key->get_public_key()->get_buffer(), public_key->get_public_key()->get_buffer_size());
		_event_queue.set_event_data(event, connect_request_data->get_buffer(), connect_request_data->get_buffer_size());
	}
	
	/// Sends a connect accept packet to acknowledge the successful acceptance of a connect request.
	void _send_connect_accept(notify_connection *conn)
	{
		logf(notify_socket, ("Sending Connect Accept - connection established."));
		packet_stream out;
		write(out, uint8(connect_accept_packet));
		
		write(out, conn->get_initiator_nonce());
		write(out, conn->get_host_nonce());
		uint32 encrypt_pos = out.get_next_byte_position();
		out.set_byte_position(encrypt_pos);
		
		write(out, conn->get_initial_send_sequence());

		uint8 init_vector[symmetric_cipher::block_size];
		conn->get_symmetric_cipher()->get_init_vector(init_vector);
		out.write_bytes(init_vector, symmetric_cipher::key_size);
		
		symmetric_cipher the_cipher(conn->get_shared_secret());
		bit_stream_hash_and_encrypt(out, notify_connection::message_signature_bytes, encrypt_pos, &the_cipher);

		out.send_to(_socket, conn->get_address());
	}
	
	/// Handles a connect accept packet, putting the connection associated with the remote host (if there is one) into an active state.
	void _handle_connect_accept(const address &the_address, bit_stream &stream)
	{
		nonce initiator_nonce, host_nonce;
		
		read(stream, initiator_nonce);
		read(stream, host_nonce);
		uint32 decrypt_pos = stream.get_next_byte_position();
		stream.set_byte_position(decrypt_pos);
		
		pending_connection *pending = _find_pending_connection(the_address);
		if(!pending || pending->get_state() != pending_connection::requesting_connection || pending->get_initiator_nonce() != initiator_nonce || pending->get_host_nonce() != host_nonce)
			return;
		
		symmetric_cipher the_cipher(pending->get_shared_secret());

		if(!bit_stream_decrypt_and_check_hash(stream, notify_connection::message_signature_bytes, decrypt_pos, &the_cipher))
			return;

		uint32 recv_sequence;
		read(stream, recv_sequence);
		
		uint8 init_vector[symmetric_cipher::block_size];
		
		stream.read_bytes(init_vector, symmetric_cipher::block_size);
		symmetric_cipher *cipher = new symmetric_cipher(pending->_symmetric_key, init_vector);
		
		notify_connection *the_connection = new notify_connection(pending->get_initiator_nonce(), pending->get_initial_send_sequence(), pending->_connection_index, true);
		the_connection->set_initial_recv_sequence(recv_sequence);
		the_connection->set_address(pending->get_address());
		the_connection->set_shared_secret(pending->get_shared_secret());
		the_connection->_host_nonce = pending->_host_nonce;
		the_connection->set_net_socket(this);
		_remove_pending_connection(pending); // remove the pending connection

		_add_connection(the_connection); // first, add it as a regular connection
		logf(notify_socket, ("Received Connect Accept - connection established."));

		_event_queue.post_event(notify_connection_established_event_type, the_address, the_connection->get_connection_index());
	}
	
	/// Sends a connect rejection to a valid connect request in response to possible error conditions (server full, wrong password, etc).
	void _send_connect_reject(nonce &initiator_nonce, nonce &host_nonce, const address &the_address, uint32 reason)
	{
		packet_stream out;
		write(out, uint8(connect_reject_packet));
		write(out, initiator_nonce);
		write(out, host_nonce);
		write(out, reason);
		out.send_to(_socket, the_address);
	}
	
	
	/// Handles a connect rejection packet by notifying the connection ref_object that the connection was rejected.
	void _handle_connect_reject(const address &the_address, bit_stream &stream)
	{
		nonce initiator_nonce;
		nonce host_nonce;
		
		read(stream, initiator_nonce);
		read(stream, host_nonce);
		
		pending_connection *pending = _find_pending_connection(the_address);
		if(!pending || (pending->get_state() != pending_connection::requesting_challenge_response && pending->get_state() != pending_connection::requesting_connection))
			return;
		if(pending->get_initiator_nonce() != initiator_nonce || pending->get_host_nonce() != host_nonce)
			return;
		
		uint32 reason;
		read(stream, reason);
		
		logf(notify_socket, ("Received Connect Reject - reason %d", reason));

		// if the reason is a bad puzzle solution, try once more with a new nonce.
		if(reason == reason_failed_puzzle && !pending->_puzzle_retried)
		{
			pending->_puzzle_retried = true;
			pending->set_state(pending_connection::requesting_challenge_response);
			pending->_state_send_retry_count = challenge_retry_count;
			pending->_state_send_retry_interval = challenge_retry_time;
			pending->_state_last_send_time = get_process_start_time();
			pending->_initiator_nonce = _random_generator.random_nonce();
			
			_send_challenge_request(pending);
			return;
		}
		byte_buffer_ptr null;

		_event_queue.post_event(notify_connection_disconnected_event_type, the_address, pending->_connection_index);
		_remove_pending_connection(pending);
	}
	
	/// Dispatches a disconnect packet for a specified connection.
	void _handle_disconnect(const address &the_address, bit_stream &stream)
	{
		nonce initiator_nonce, host_nonce;
		read(stream, initiator_nonce);
		read(stream, host_nonce);
		uint32 reason_code;
		uint32 disconnect_data_size;
		uint8 disconnect_data[net_sockets_max_status_datagram_size];
		
		notify_connection *conn = _find_connection(the_address);
		if(conn)
		{
			if(initiator_nonce != conn->get_initiator_nonce() || host_nonce != conn->get_host_nonce())
				return;
			
			uint32 decrypt_pos = stream.get_next_byte_position();
			stream.set_byte_position(decrypt_pos);
			
			symmetric_cipher the_cipher(conn->get_shared_secret());
			if(!bit_stream_decrypt_and_check_hash(stream, notify_connection::message_signature_bytes, decrypt_pos, &the_cipher))
				return;
			read(stream, reason_code);
			read(stream, disconnect_data_size);
			if(disconnect_data_size > net_sockets_max_status_datagram_size)
				disconnect_data_size = net_sockets_max_status_datagram_size;
			stream.read_bytes(disconnect_data, disconnect_data_size);
			net_socket_event *event = _event_queue.post_event(notify_connection_disconnected_event_type, the_address, conn->get_connection_index());
			_event_queue.set_event_data(event, disconnect_data, disconnect_data_size);
            logf(notify_socket, ("disconnect packet for %d", conn->get_connection_index()));

			_remove_connection(conn);
		}
		else
		{
			pending_connection *pending = _find_pending_connection(the_address);
			if(!pending)
				return;
			if(pending->get_initiator_nonce() != initiator_nonce || pending->get_host_nonce() != host_nonce)
				return;
			_event_queue.post_event(notify_connection_disconnected_event_type, the_address, conn->get_connection_index());
			_remove_pending_connection(pending);
		}
	}
	
	/// Handles all packets that don't fall into the category of notify_connection handshake or game data.
	void _handle_info_packet(const address &address, uint8 packet_type, bit_stream &stream)
	{
		net_socket_event *event = _event_queue.post_event(net_socket_packet_event_type, address);
		event->data_size = stream.get_stream_byte_size();
		event->data = (uint8 *) _event_queue.allocate_queue_data(event->data_size);
		memcpy(event->data, stream.get_buffer(), event->data_size);
	}
	
	/// Processes a single packet, and dispatches either to handle_info_packet or to the connection associated with the remote address.
	void _process_packet(const address &the_address, bit_stream &packet_stream)
	{
		
		// Determine what to do with this packet:
		
		if(packet_stream.get_buffer()[0] & 0x80) // it's a protocol packet...
		{
			// if the MSB of the first byte is set, it's a protocol data packet so pass it to the appropriate connection.
			notify_connection *conn = _find_connection(the_address);
            //logf(notify_socket, ("got data packet for %d", conn ? conn->_connection_index : 0));

			if(conn)
				conn->read_raw_packet(packet_stream);
		}
		else
		{
			// Otherwise, it's either a game info packet or a connection handshake packet.
			
			uint8 packet_type;
			read(packet_stream, packet_type);
			
			if(packet_type >= first_valid_info_packet_id)
				_handle_info_packet(the_address, packet_type, packet_stream);
			else
			{
				// check if there's a connection already:
				switch(packet_type)
				{
					case connect_challenge_request_packet:
						_handle_connect_challenge_request(the_address, packet_stream);
						break;
					case connect_challenge_response_packet:
						_handle_connect_challenge_response(the_address, packet_stream);
						break;
					case connect_reject_packet:
						_handle_connect_reject(the_address, packet_stream);
						break;
					case connect_request_packet:
						_handle_connect_request(the_address, packet_stream);
						break;
					case connect_accept_packet:
						_handle_connect_accept(the_address, packet_stream);
						break;
					case disconnect_packet:
						_handle_disconnect(the_address, packet_stream);
						break;
					case introduced_connection_request_packet:
						_handle_introduction_request(the_address, packet_stream);
						break;
					case connection_introduction_packet:
						_handle_introduction(the_address, packet_stream);
						break;
					case punch_packet:
						_handle_punch(the_address, packet_stream);
						break;
				}
			}
		}
	}
protected:
	/// Structure used to track packets that read by the background packet reader or are delayed in sending for simulating a high-latency connection.  The packet_record is allocated as sizeof(packet_record) + packet_size;
	struct packet_record
	{
		packet_record *next_packet; ///< The next packet in the list of delayed packets.
		address remote_address; ///< The address to send this packet to.
		time send_time; ///< time when we should send the packet.
		uint32 packet_size; ///< Size, in bytes, of the packet data.
		uint8 packet_data[1]; ///< Packet data.
	};
	
	/// Checks all connections on this notify_socket for packet sends, and for timeouts and all valid and pending connections.
	void process_connections()
	{
		_process_start_time = time::get_current();
		_puzzle_manager.tick(_process_start_time, _random_generator);
		
		// first see if there are any delayed packets that need to be sent...
		while(_send_packet_list && _send_packet_list->send_time < get_process_start_time())
		{
			packet_record *next = _send_packet_list->next_packet;
			_socket.send_to(_send_packet_list->remote_address,
							_send_packet_list->packet_data, _send_packet_list->packet_size);
			mem_free(_send_packet_list);
			_send_packet_list = next;
		}
		
		if(get_process_start_time() > _last_timeout_check_time + time(timeout_check_interval))
		{
            // see if we need to clear out any introductions
            for(uint32 i = 0; i < _introductions.size(); )
            {
                if(_introductions[i].introduction_time + introduction_timeout < get_process_start_time())
                {
                    logf(notify_socket, ("clearing introduction from %d to %d", _introductions[i].initiator, _introductions[i].host));
                    _introductions.erase_fast(i);
                }
                else
                    i++;
            }
			_last_timeout_check_time = get_process_start_time();
			for(pending_connection **walk = &_pending_connections; *walk;)
			{
				pending_connection *pending = *walk;
				if(get_process_start_time() > pending->_state_last_send_time + time(pending->_state_send_retry_interval))
				{
					if(!pending->_state_send_retry_count)
					{
						// this pending connection request has timed out.
                        _event_queue.post_event(notify_connection_timed_out_event_type, pending->get_address(), pending->_connection_index);
						*walk = pending->_next;
						delete pending;
					}
					else
					{
						pending->_state_send_retry_count--;
                        pending->_state_last_send_time = get_process_start_time();
						switch(pending->get_state())
						{
							case pending_connection::requesting_introduction:
								_send_introduction_request(pending);
								break;
							case pending_connection::sending_punch_packets:
								_send_punch(pending);
								break;
						case pending_connection::requesting_challenge_response:
								_send_challenge_request(pending);
								break;
						default:
								break;
						}
						walk = &pending->_next;
					}
				}
				else
					walk = &pending->_next;

			}
			for(notify_connection *connection_walk = _connection_list; connection_walk;)
			{
				notify_connection *the_connection = connection_walk;
				connection_walk = connection_walk->_next;

				if(the_connection->check_timeout(get_process_start_time()))
				{
                    _event_queue.post_event(notify_connection_timed_out_event_type, the_connection->get_address(), the_connection->_connection_index);
					_remove_connection(the_connection);
				}
			}
		}
		
		// check if we're trying to solve any client connection puzzles
		byte_buffer_ptr result;
		uint32 request_index;
		while(_puzzle_solver.get_next_result(result, request_index))
		{
			uint32 solution;
			bit_stream s(result->get_buffer(), result->get_buffer_size());
			read(s, solution);
			
			for(pending_connection *walk = _pending_connections; walk; walk = walk->_next)
			{
				if(walk->get_state() == pending_connection::computing_puzzle_solution)
				{
					if(walk->_puzzle_request_index != request_index)
						continue;
					// this was the solution for this client...
					walk->_puzzle_solution = solution;
					
					walk->set_state(pending_connection::requesting_connection);
					_send_connect_request(walk);
					break;
				}
			}
		}
	}
	
	/// looks up a connected connection on this notify_socket
	notify_connection *_find_connection(const address &remote_address)
	{
		//if(_connection_list)
		//	return _connection_list;
		hash_table_flat<address, notify_connection *>::pointer p = _connection_address_lookup_table.find(remote_address);
		if(p)
			return *(p.value());
		return 0;
	}
	
	notify_connection *_find_connection(notify_connection_id id)
	{
		hash_table_flat<notify_connection_id, notify_connection *>::pointer p = _connection_id_lookup_table.find(id);
		if(p)
			return *(p.value());
		return 0;
	}
	
	/// Finds a connection instance that this notify_socket has initiated.
	pending_connection *_find_pending_connection(const address &the_address)
	{
		for(pending_connection *walk = _pending_connections; walk; walk = walk->_next)
			if(walk->get_address() == the_address)
				return walk;
		return NULL;
	}
	
	pending_connection * _find_pending_connection(notify_connection_id connection_id)
	{
		for(pending_connection *walk = _pending_connections; walk; walk = walk->_next)
			if(walk->_connection_index == connection_id)
				return walk;
		return 0;
	}
	
	void _remove_pending_connection(pending_connection *the_connection)
	{
		for(pending_connection **walk = &_pending_connections; *walk; walk = &((*walk)->_next))
			if(*walk == the_connection)
			{
				*walk = the_connection->_next;
				delete the_connection;
				return;
			}
	}
	
	/// Adds a pending connection the list of pending connections.
	void _add_pending_connection(pending_connection *the_connection)
	{
		the_connection->_next = _pending_connections;
		_pending_connections = the_connection;
	}
	
	/// Adds a connection to the internal connection list.
	void _add_connection(notify_connection *the_connection)
	{
		the_connection->_next = _connection_list;
		the_connection->_prev = 0;
		if(the_connection->_next)
			the_connection->_next->_prev = the_connection;
		_connection_list = the_connection;
		
		_connection_id_lookup_table.insert(the_connection->_connection_index, the_connection);
        logf(notify_socket, ("inserting connection %d at %s", the_connection->_connection_index, the_connection->get_address().to_string().c_str()));
		_connection_address_lookup_table.insert(the_connection->get_address(), the_connection);
	}
	
	void _remove_connection(notify_connection *the_connection)
	{
		if(the_connection->_prev)
			the_connection->_prev->_next = the_connection->_next;
		else
			_connection_list = the_connection->_next;
		if(the_connection->_next)
			the_connection->_next->_prev = the_connection->_prev;
		
		_connection_id_lookup_table.remove(the_connection->get_connection_index());
		_connection_address_lookup_table.remove(the_connection->get_address());
		delete the_connection;
	}
	
	class socket_thread : public thread
	{
		notify_socket *_socket;
	public:
		socket_thread(notify_socket *socket)
		{
			_socket = socket;
		}
		virtual uint32 run()
		{
			_socket->thread_socket_process();
			return 0;
		}
	};
	
	packet_record *allocate_packet_record(const address &the_address, bit_stream &stream)
	{
		uint32 data_size = stream.get_next_byte_position();
		
		// allocate the send packet, with the data size added on
		packet_record *the_packet = (packet_record *) mem_alloc(sizeof(packet_record) + data_size);
		the_packet->remote_address = the_address;
		the_packet->packet_size = data_size;
		memcpy(the_packet->packet_data, stream.get_buffer(), data_size);
		the_packet->next_packet = 0;
		return the_packet;
	}

	void thread_socket_process()
	{
		packet_stream stream;
		address addr;
		for(;;)
		{
			udp_socket::recv_from_result result = stream.recv_from(_socket, &addr);
			if(result == udp_socket::invalid_socket)
				return;
			
			if(result == udp_socket::packet_received)
			{
				stream.set_bit_position(stream.get_stream_bit_size());
				packet_record *new_packet = allocate_packet_record(addr, stream);
				_packet_queue_mutex.lock();
				packet_record **walk = &_received_packet_list;
				while(*walk)
					walk = &((*walk)->next_packet);
				*walk = new_packet;
				_packet_queue_mutex.unlock();
			}
			if(_event_ready_notify_fn)
				_event_ready_notify_fn(_event_ready_user_data);
		}
	}
	
	bool _get_next_packet(packet_stream &stream, address &addr)
	{
		if(_thread_socket)
		{
			_packet_queue_mutex.lock();
			packet_record *packet = _received_packet_list;
			if(_received_packet_list)
				_received_packet_list = _received_packet_list->next_packet;
			_packet_queue_mutex.unlock();
			if(!packet)
				return false;
			stream.set_from_buffer(packet->packet_data, packet->packet_size);
			addr = packet->remote_address;
			mem_free(packet);
			return true;
		}
		else
		{
			udp_socket::recv_from_result result;
			return stream.recv_from(_socket, &addr) == udp_socket::packet_received;
		}
	}
public:
    /// Returns the address of the first network notify_socket in the list that the socket on this notify_socket is bound to.
    address get_first_bound_interface_address()
    {
        address the_address = _socket.get_bound_address();
        
        if(the_address.is_same_host(address(address::any, 0)))
        {
            array<address> net_socket_addresses;
            address::get_interface_addresses(net_socket_addresses);
            uint16 save_port = the_address.get_port();
            if(net_socket_addresses.size())
            {
                the_address = net_socket_addresses[0];
                the_address.set_port(save_port);
            }
        }
        return the_address;
    }
    
    /// Computes an identity token for the connecting client based on the address of the client and the client's unique nonce value.
    uint32 compute_client_identity_token(const address &the_address, const nonce &the_nonce)
    {
        hash_state state;
        uint32 hash[8];
        uint32 host = htonl(the_address.get_host());
        uint32 port = htonl(the_address.get_port());
        
        sha256_init(&state);
        sha256_process(&state, (uint8 *) &host, sizeof(host));
        sha256_process(&state, (uint8 *) &port, sizeof(port));
        sha256_process(&state, (uint8 *) &the_nonce, sizeof(the_nonce));
        sha256_process(&state, _random_hash_data, sizeof(_random_hash_data));
        sha256_done(&state, (uint8 *) hash);
        
        return hash[0];
    }

    /// Sets the private key this notify_socket will use for authentication and key exchange
	void set_private_key(asymmetric_key *the_key)
	{
		_private_key = the_key;
	}
	
	/// Returns the udp_socket associated with this notify_socket
	udp_socket &get_network_socket()
	{
		return _socket;
	}	
	
	void set_challenge_response(byte_buffer_ptr data)
	{
		_challenge_response = data;
	}
	
	random_generator &random()
	{
		return _random_generator;
	}
	
	/// Returns whether or not this notify_socket allows connections from remote hosts.
	bool does_allow_connections()
	{
		return _allow_connections;
	}
	
	/// Sets whether or not this notify_socket allows connections from remote hosts.
	void set_allows_connections(bool conn)
	{
		_allow_connections = conn;
	}
	
    void ban_host(const address &remote_host )
    {
        
    }
    
    void unban_host(const address &remote_host )
    {
        
    }
	void _disconnect_existing_connection(const address &remote_host)
	{
		
	}
	
	/// open a connection to the remote host
	notify_connection_id connect(const address &remote_host, uint8 *connect_data, uint32 connect_data_size)
	{
		_disconnect_existing_connection(remote_host);
		uint32 initial_send_sequence = _random_generator.random_integer();
		
		pending_connection *new_connection = new pending_connection(pending_connection::connection_initiator, _random_generator.random_nonce(), initial_send_sequence, _next_connection_index++);
		
		new_connection->_packet_data = new byte_buffer(connect_data, connect_data_size);
		new_connection->_address = remote_host;
		new_connection->_state_send_retry_count = challenge_retry_count;
		new_connection->_state_send_retry_interval = challenge_retry_time;
		new_connection->_state_last_send_time = get_process_start_time();
		

		_add_pending_connection(new_connection);
        logf(notify_socket, ("connect %d - %s", new_connection->_connection_index, buffer_encode_base_16(connect_data, connect_data_size)->get_buffer()));

		_send_challenge_request(new_connection);
		return new_connection->_connection_index;
	}
	
	struct introduction_record
	{
		nonce initiator_nonce, host_nonce;
		
		notify_connection_id initiator;
		notify_connection_id host;
		bool initiator_request_received;
		bool host_request_received;
		bool initial_intro_sent;
		time introduction_time;
	};
	
	array<introduction_record> _introductions;
	
	/// This is called on the middleman of an introduced connection and will allow this host to broker a connection start between the remote hosts at either connection point.
	void introduce_connection(notify_connection_id initiator, notify_connection_id host)
	{
		if(_find_connection(initiator) && _find_connection(host))
		{
            logf(notify_socket, ("introducing connections %d(initiator) to %d(host)", initiator, host));
			introduction_record r;
			r.initiator_nonce = _random_generator.random_nonce();
			r.host_nonce = _random_generator.random_nonce();
			r.initiator = initiator;
			r.host = host;
			r.initiator_request_received = false;
			r.host_request_received = false;
			r.initial_intro_sent = false;
			r.introduction_time = time::get_current();
			_introductions.push_back(r);
		}
	}
	
	/// Connect to a client connected to the host at middle_man.
	notify_connection_id setup_introduced_connection(notify_connection_id introducer, notify_connection_id remote_client_identity, int is_host, uint32 connect_data_size, uint8 *connect_data)
	{
		notify_connection *introducing_connection = _find_connection(introducer);
		
		if(!introducing_connection)
			return invalid_notify_connection;
		
		uint32 initial_send_sequence = _random_generator.random_integer();
		
		pending_connection *new_connection = new pending_connection(is_host ? pending_connection::introduced_connection_host : pending_connection::introduced_connection_initiator, _random_generator.random_nonce(), initial_send_sequence, _next_connection_index++);
		
		new_connection->_introducer = introducer;
		new_connection->_remote_client_id = remote_client_identity;
		new_connection->_packet_data = new byte_buffer(connect_data, connect_data_size);
		new_connection->_state_send_retry_count = challenge_retry_count;
		new_connection->_state_send_retry_interval = challenge_retry_time;
		new_connection->_state_last_send_time = get_process_start_time();
		
		_add_pending_connection(new_connection);
		_send_introduction_request(new_connection);
		return new_connection->_connection_index;
	}
    
    notify_connection_id connect_introduced(notify_connection_id introducer, notify_connection_id remote_host_identity, unsigned connect_data_size, unsigned char *connect_data)
    {
        notify_connection_id ret = setup_introduced_connection(introducer, remote_host_identity, false, connect_data_size, connect_data);
        logf(notify_socket, ("Initiating introduced connection %d to %d from %d.", ret, remote_host_identity, introducer));
        return ret;
    }
    
    notify_connection_id accept_introduction(notify_connection_id introducer, notify_connection_id remote_initiator_identity)
    {
        notify_connection_id ret = setup_introduced_connection(introducer, remote_initiator_identity, true, 0, 0);
        logf(notify_socket, ("Accepting introduced connection %d to %d from %d.", ret, remote_initiator_identity, introducer));
        return ret;
    }
	void accept_connection_challenge(notify_connection_id the_connection)
	{
		pending_connection *conn = _find_pending_connection(the_connection);
		if(!conn || conn->get_state() != pending_connection::awaiting_local_challenge_accept)
        {
            logf(notify_socket, ("Accepting connection %d fail.", the_connection));
            return;
        }
		
		conn->set_state(pending_connection::computing_puzzle_solution);
		conn->_state_send_retry_count = 0;
		conn->_state_send_retry_interval = puzzle_solution_timeout;
		conn->_state_last_send_time = get_process_start_time();
		
		packet_stream s;
		write(s, conn->get_initiator_nonce());
		write(s, conn->get_host_nonce());
		write(s, conn->_puzzle_difficulty);
		write(s, conn->_client_identity);
        logf(notify_socket, ("Attempting to solve a client puzzle for %d.", the_connection));
		byte_buffer_ptr request = new byte_buffer(s.get_buffer(), s.get_next_byte_position());
		conn->_puzzle_request_index = _puzzle_solver.post_request(request);
	}
	
	/// accept an incoming connection request.
	void accept_connection(notify_connection_id connection_id)
	{
		pending_connection* pending = _find_pending_connection(connection_id);
		if(!pending || pending->get_state() != pending_connection::awaiting_local_accept)
		{
			logf(notify_socket, ("Trying to accept a non-pending connection."));
			return;
		}
        logf(notify_socket, ("accepting connection %d", connection_id));
		notify_connection *new_connection = new notify_connection(pending->_initiator_nonce, pending->_initial_send_sequence, pending->_connection_index, false);
		new_connection->set_net_socket(this);
		new_connection->set_symmetric_cipher(pending->get_symmetric_cipher());
		new_connection->set_shared_secret(pending->get_shared_secret());
		new_connection->set_initial_recv_sequence(pending->_initial_recv_sequence);
		new_connection->_host_nonce = pending->_host_nonce;
		new_connection->set_address(pending->get_address());
		
		_remove_pending_connection(pending);
		_add_connection(new_connection);
		_event_queue.post_event(notify_connection_established_event_type, pending->get_address(), new_connection->_connection_index);
		_send_connect_accept(new_connection);
	}
	
	void disconnect(notify_connection_id connection_id, uint8 *disconnect_data, uint32 disconnect_data_size)
	{
		_disconnect(connection_id, reason_disconnect_call, disconnect_data, disconnect_data_size);
		
	}
	
	
	/// Disconnects the given notify_connection and removes it from the notify_socket
	void _disconnect(notify_connection_id connection_id, uint32 reason_code, uint8 *disconnect_data, uint32 disconnect_data_size)
	{
		assert(disconnect_data_size <= net_sockets_max_status_datagram_size);
		// see if it's a connected instance
		notify_connection *connection = _find_connection(connection_id);
		if(connection)
		{
			// send a disconnect packet...
			packet_stream out;
			write(out, uint8(disconnect_packet));
			write(out, connection->get_initiator_nonce());
			write(out, connection->get_host_nonce());
			uint32 encrypt_pos = out.get_next_byte_position();
			out.set_byte_position(encrypt_pos);
			
			write(out, reason_code);
			write(out, disconnect_data_size);
			out.write_bytes(disconnect_data, disconnect_data_size);
			symmetric_cipher the_cipher(connection->get_shared_secret());
			bit_stream_hash_and_encrypt(out, notify_connection::message_signature_bytes, encrypt_pos, &the_cipher);
			
			out.send_to(_socket, connection->get_address());
            logf(notify_socket, ("sending disconnect for %d", connection_id));

			_remove_connection(connection);
			return;
		}
		pending_connection *pending = _find_pending_connection(connection_id);
		if(pending)
		{
            logf(notify_socket, ("disconnecting pending connection %d", connection_id));
			_remove_pending_connection(pending);
		}
	}
	/// Send a datagram packet to the remote host on the other side of the connection.  Returns the sequence number of the packet sent.
	void send_to_connection(notify_connection_id connection_id, uint8 *data, uint32 data_size, uint32 *sequence = 0)
	{
		notify_connection *conn = _find_connection(connection_id);
		conn->send_packet(notify_connection::data_packet, data, data_size, sequence);
	}
    
    /// Set simulated latency and packet loss for a connection
    void set_simulated_net_params(notify_connection_id connection_id, float32 packet_loss, uint32 latency)
    {
        notify_connection *conn = _find_connection(connection_id);
        conn->set_simulated_net_params(packet_loss, latency);
    }
	
	/// Sends a packet to the remote address over this net_socket's socket.
	udp_socket::send_to_result send_to(const address &the_address, uint32 data_size, uint8 *data)
	{
		string addr_string = the_address.to_string();
		return _socket.send_to(the_address, data, data_size);
	}
	
	/// Sends a packet to the remote address after millisecond_delay time has elapsed.  This is used to simulate network latency on a LAN or single computer.
	void send_to_delayed(const address &the_address, bit_stream &stream, uint32 millisecond_delay)
	{
		packet_record *the_packet = allocate_packet_record(the_address, stream);
		the_packet->send_time = get_process_start_time() + time(millisecond_delay);

		// insert it into the packet_record list, sorted by time
		packet_record **list;
		for(list = &_send_packet_list; *list && ((*list)->send_time <= the_packet->send_time); list = &((*list)->next_packet))
			;
		the_packet->next_packet = *list;
		*list = the_packet;
	}
	
	/// Gets the next event on this socket; returns NULL if there are no events to be read.
	net_socket_event *get_next_event()
	{
		process_connections();
		if(!_event_queue.has_event())
		{
			_event_queue.clear();
			// if there's nothing in the event queue, see if a new packet's come in.
			packet_stream stream;
			address source_address;
			
			while(_get_next_packet(stream, source_address))
			{
				//logf(cradle, ("Got a packet: %s.", stream.to_string().c_str()));
				_process_start_time = time::get_current();
				_process_packet(source_address, stream);
				if(_event_queue.has_event())
					break;
			}
		}
		if(_event_queue.has_event())
			return _event_queue.dequeue();
		else
			return 0;
	}	
	
	bind_result bind(const address &bind_address)
	{
		time block_timeout = 0;
		if(_thread_socket)
			block_timeout = 500;
		
		bind_result the_result = _socket.bind(bind_address, !_thread_socket, block_timeout);
		
        logf(notify_socket, ("Bind result = %d", the_result));
		if(_thread_socket && (the_result == bind_success) && !_packet_thread.is_running())
			_packet_thread.start();
		return the_result;
	}
	
	~notify_socket()
	{
		// gracefully close all the connections on this notify_socket:
        logf(notify_socket, ("Disconnecting connections."));
		while(_connection_list)
			_disconnect(_connection_list->get_connection_index(), reason_self_disconnect, 0, 0);
        logf(notify_socket, ("Done."));

	}
	
	/// @param bind_address Local network address to bind this notify_socket to.
	notify_socket(bool thread_socket = false, void (*socket_notify_fn)(void *) = 0, void *socket_notify_data = 0) : _puzzle_manager(_random_generator, &_allocator), _event_queue(&_allocator), _packet_thread(this)
	{
        _next_connection_index = first_valid_notify_connection_id;
		_random_generator.random_buffer(_random_hash_data, sizeof(_random_hash_data));

		_private_key = new asymmetric_key(20, _random_generator);
		
		_last_timeout_check_time = time(0);
		_allow_connections = true;
		
		_send_packet_list = NULL;
		_process_start_time = time::get_current();
		
		_event_ready_notify_fn = socket_notify_fn;
		_event_ready_user_data = socket_notify_data;
		_thread_socket = thread_socket;
		_received_packet_list = 0;

		// Supply our own (small) unique private key for the time being.
		_private_key = new asymmetric_key(16, _random_generator);
		_challenge_response = new byte_buffer("");
		_pending_connections = 0;
		_connection_list = 0;
	}
	
	socket_event_queue _event_queue;
	
	mutex _packet_queue_mutex;
	packet_record *_received_packet_list;
	bool _thread_socket;
	socket_thread _packet_thread; ///< background thread that blocks on socket read and calls the socket_notify_fn whenever it posts something into the packet queue
	void *_event_ready_user_data;
	void (*_event_ready_notify_fn)(void *); ///< When the socket operates with a background reader thread, this function is called when each new packet arrives.  This function is called from the background thread, so beware of thread safety issues.  Mostly this is just here for the NPAPI version.
	udp_socket _socket; ///< Network socket this notify_socket communicates over.
	random_generator _random_generator;	///< cryptographic random number generator for this socket
	puzzle_solver _puzzle_solver; ///< helper class for solving client puzzles
	page_allocator _allocator; ///< memory allocator helper class for this socket

	pending_connection *_pending_connections; ///< Linked list of all the pending connections on this socket
	notify_connection *_connection_list; ///< Doubly-linked list of all the connections that are in a connected state on this notify_socket.
	hash_table_flat<notify_connection_id, notify_connection *> _connection_id_lookup_table; ///< quick lookup table for active connections by id.
	hash_table_flat<address, notify_connection *> _connection_address_lookup_table; ///< quick lookup table for active connections by address.
	notify_connection_id _next_connection_index; ///< Next available connection id

	byte_buffer_ptr _challenge_response; ///< Challenge response set by the host as response to all incoming challenge requests on this socket.
	
	ref_ptr<asymmetric_key> _private_key; ///< The private key used by this notify_socket for secure key exchange.
	client_puzzle_manager _puzzle_manager; ///< The ref_object that tracks the current client puzzle difficulty, current puzzle and solutions for this notify_socket.

	time _process_start_time; ///< Current time tracked by this notify_socket.
	bool _requires_key_exchange; ///< True if all connections outgoing and incoming require key exchange.
	time _last_timeout_check_time; ///< Last time all the active connections were checked for timeouts.
	uint8  _random_hash_data[12]; ///< Data that gets hashed with connect challenge requests to prevent connection spoofing.
	bool _allow_connections; ///< Set if this notify_socket allows connections from remote instances.
	
	hash_table_flat<uint32, notify_connection *> _connection_index_table;

	packet_record *_send_packet_list; ///< List of delayed packets pending to send.
};
