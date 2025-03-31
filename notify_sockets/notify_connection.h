// The notify_sockets library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

/// notify_connection class that manages an individual data connection in notify_sockets. It implements a notification protocol on the unreliable packet transport of UDP.  notify_connection manages the flow of packets over the network, and posts net_socket_event events to the notify_socket event queue for data packets and packet delivery notification.

class notify_connection
{
public:
	notify_connection *_next; ///< next connection in the doubly-linked list of connections on a socket.
	notify_connection *_prev; ///< previous connection in the doubly-linked list of connections on a socket.
	
	friend class notify_socket;
	/// Constants controlling the data representation of each packet header
	enum connection_constants {
		// NOTE - IMPORTANT!
		// The first bytes of each packet are made up of:
		// 1 bit - game data packet flag
		// 2 bits - packet type
		// sequence_number_bit_size bits - sequence number
		// ack_sequence_number_bit_size bits - high ack sequence received
		// these values should be set to align to a byte boundary, otherwise
		// bits will just be wasted.
		
		max_packet_window_size_shift = 5, ///< Packet window size is 2^max_packet_window_size_shift.
		max_packet_window_size = (1 << max_packet_window_size_shift), ///< Maximum number of packets in the packet window.
		packet_window_mask = max_packet_window_size - 1, ///< Mask for accessing the packet window.
		max_ack_mask_size = 1 << (max_packet_window_size_shift - 5), ///< Each ack word can ack 32 packets.
		max_ack_byte_count = max_ack_mask_size << 2, ///< The maximum number of ack bytes sent in each packet.
		sequence_number_bit_size = 11, ///< Bit size of the send and sequence number.
		sequence_number_window_size = (1 << sequence_number_bit_size), ///< Size of the send sequence number window.
		sequence_number_mask = -sequence_number_window_size, ///< Mask used to reconstruct the full send sequence number of the packet from the partial sequence number sent.
		ack_sequence_number_bit_size = 10, ///< Bit size of the ack receive sequence number.
		ack_sequence_number_window_size = (1 << ack_sequence_number_bit_size), ///< Size of the ack receive sequence number window.
		ack_sequence_number_mask = -ack_sequence_number_window_size, ///< Mask used to reconstruct the full ack receive sequence number of the packet from the partial sequence number sent.
		
		packet_header_bit_size = 3 + ack_sequence_number_bit_size + sequence_number_bit_size, ///< Size, in bits, of the packet header sequence number section
		packet_header_byte_size = (packet_header_bit_size + 7) >> 3, ///< Size, in bytes, of the packet header sequence number information
		packet_header_pad_bits = (packet_header_byte_size << 3) - packet_header_bit_size, ///< Padding bits to get header bytes to align on a byte boundary, for encryption purposes.
		
		message_signature_bytes = 5, ///< Special data bytes written into the end of the packet to guarantee data consistency
	};
	enum net_packet_type
	{
		data_packet, ///< Standard data packet.  Each data packet sent increments the current packet sequence number (_last_send_seq).
		ping_packet, ///< Ping packet, sent if this instance hasn't heard from the remote host for a while.  Sending a
		///  ping packet does not increment the packet sequence number.
		ack_packet,  ///< Packet sent in response to a ping packet.  Sending an ack packet does not increment the sequence number.
		invalid_packet_type,
	};
	/// Constants controlling the behavior of pings and timeouts
	enum default_ping_constants {
		default_ping_timeout = 5000,  ///< Default milliseconds to wait before sending a ping packet.
		default_ping_retry_count = 5, ///< Default number of unacknowledged pings to send before timing out.
	};
protected:
	/// Reads a raw packet from a bit_stream, as dispatched from notify_socket.
	bool read_raw_packet(bit_stream &bstream)
	{
		if(_simulated_packet_loss && _net_socket->random().random_unit_float() < _simulated_packet_loss)
		{
			logf(notify_connection, ("notify_connection %d: RECVDROP - %d", _connection_index, get_last_send_sequence() - _initial_send_seq));
			return false;
		}
		logf(notify_connection, ("notify_connection %d: RECV bytes", _connection_index));
		
		if(read_packet_header(bstream))
		{
            net_socket_event *event = _net_socket->_event_queue.post_event(notify_connection_packet_event_type, _address);
			event->packet_sequence = get_last_received_sequence();
			event->connection = get_connection_index();
			event->data_size = bstream.get_stream_byte_size() - bstream.get_byte_position();
			event->data = _net_socket->_event_queue.allocate_queue_data(event->data_size);
			memcpy(event->data, bstream.get_buffer() + bstream.get_byte_position(), event->data_size);
			return true;
		}
		return false;
	}
	
	/// Sends a packet that was written into a bit_stream to the remote host, or the _remote_connection on this host.
	void send_packet(net_packet_type packet_type, uint8 *data, uint32 data_size, uint32 *sequence = 0)
	{
		packet_stream ps;
		write_packet_header(ps, packet_type);
		if(packet_type == data_packet)
		{
			int32 start = ps.get_bit_position();
			logf(notify_connection, ("notify_connection %d: START", _connection_index) );
			ps.write_bytes(data, data_size);
			logf(notify_connection, ("notify_connection %d: END - %llu bits", _connection_index, ps.get_bit_position() - start) );			
		}
		if(!_symmetric_cipher.is_null())
		{
			_symmetric_cipher->setup_counter(_last_send_seq, _last_seq_recvd, packet_type, 0);
			// bit_stream_hash_and_encrypt(ps, message_signature_bytes, packet_header_byte_size, _symmetric_cipher);
		}
		if(_simulated_packet_loss && _net_socket->random().random_unit_float() < _simulated_packet_loss)
		{
            logf(notify_connection, (" %d: SENDDROP - %d", _connection_index, get_last_send_sequence() - _initial_send_seq));
		}
		
		logf(notify_connection, ("notify_connection %d: SEND - %d bytes", _connection_index, ps.get_next_byte_position()));
		
		if(_simulated_latency)
		{
			_net_socket->send_to_delayed(get_address(), ps, _simulated_latency);
		}
		else
			_net_socket->send_to(get_address(), ps.get_next_byte_position(),  ps.get_buffer());
		if(sequence)
            *sequence = _last_send_seq - _initial_send_seq;
	}

	/// Writes the notify protocol's packet header into the bit_stream.
	void write_packet_header(bit_stream &stream, net_packet_type packet_type)
	{
		assert(!window_full() || packet_type != data_packet);
		
		int32 ack_byte_count = ((_last_seq_recvd - _last_recv_ack_ack + 7) >> 3);
		assert(ack_byte_count <= max_ack_byte_count);
		
		if(packet_type == data_packet)
			_last_send_seq++;
		
		stream.write_integer(packet_type, 2);
		stream.write_integer(_last_send_seq, 5); // write the first 5 bits of the send sequence
		stream.write_bool(true); // high bit of first byte indicates this is a data packet.
		stream.write_integer(_last_send_seq >> 5, sequence_number_bit_size - 5); // write the rest of the send sequence
		stream.write_integer(_last_seq_recvd, ack_sequence_number_bit_size);
		stream.write_integer(0, packet_header_pad_bits);
		
		stream.write_ranged_uint32(ack_byte_count, 0, max_ack_byte_count);
		
		uint32 word_count = (ack_byte_count + 3) >> 2;
		
		for(uint32 i = 0; i < word_count; i++)
			stream.write_integer(_ack_mask[i], i == word_count - 1 ?
								  (ack_byte_count - (i * 4)) * 8 : 32);
		stream.advance_to_next_byte();
        logf(notify_connection, ("header write %d bits.", stream.get_bit_position()));

		// if we're resending this header, we can't advance the
		// sequence recieved (in case this packet drops and the prev one
		// goes through) 
		
		if(packet_type == data_packet)
			_last_seq_recvd_at_send[_last_send_seq & packet_window_mask] = _last_seq_recvd;
		
		//if(is_network_connection())
		//{
		//   logf(LogBlah, ("SND: mLSQ: %08x  pkLS: %08x  pt: %d abc: %d",
		//      _last_send_seq, _last_seq_recvd, packet_type, ack_byte_count));
		//}
		
		logf(notify_connection, ("build hdr %d %d", _last_send_seq - _initial_send_seq, packet_type));
	}
	
	/// Reads a notify protocol packet header from the bit_stream and returns true if it was a data packet that needs more processing.
	bool read_packet_header(bit_stream &pstream)
	{
		// read in the packet header:
		//
		//   2 bits packet type
		//   low 5 bits of the packet sequence number
		//   1 bit game packet
		//   sequence_number_bit_size-5 bits (packet seq number >> 5)
		//   ack_sequence_number_bit_size bits ackstart seq number
		//   packet_header_pad_bits = 0 - padding to byte boundary
		//   after this point, if this is an encrypted packet, all the rest of the data will be encrypted
		
		//   rangedU32 - 0...max_ack_byte_count
		//
		// type is:
		//    00 data packet
		//    01 ping packet
		//    02 ack packet
		
		// next 0...ack_byte_count bytes are ack flags
		//
		// return value is true if this is a valid data packet or false if there is nothing more that should be read
		
		uint32 pk_packet_type = pstream.read_integer(2);
		uint32 pk_sequence_number = pstream.read_integer(5);
		bool pk_data_packet_flag = pstream.read_bool();
		pk_sequence_number = pk_sequence_number | (pstream.read_integer(sequence_number_bit_size - 5) << 5);
		
		uint32 pk_highest_ack = pstream.read_integer(ack_sequence_number_bit_size);
		uint32 pk_pad_bits = pstream.read_integer(packet_header_pad_bits);
		
		if(pk_pad_bits != 0)
			return false;
		
		assert(pk_data_packet_flag);
		
		// verify packet ordering and acking and stuff - check if the 9-bit sequence is within the packet window (within sequence_number_window_size packets of the last received sequence number).
		
		pk_sequence_number |= (_last_seq_recvd & sequence_number_mask);
		// account for wrap around
		if(pk_sequence_number < _last_seq_recvd)
			pk_sequence_number += sequence_number_window_size;
		
		// in the following test, account for wrap around from 0
		if(pk_sequence_number - _last_seq_recvd > (max_packet_window_size - 1))
		{
			// the sequence number is outside the window... must be out of order discard.
			return false;
		}
		
		pk_highest_ack |= (_highest_acked_seq & ack_sequence_number_mask);
		// account for wrap around
		
		if(pk_highest_ack < _highest_acked_seq)
			pk_highest_ack += ack_sequence_number_window_size;
		
		if(pk_highest_ack > _last_send_seq)
		{
			// the ack number is outside the window... must be an out of order packet, discard.
			return false;
		}
		
		if(!_symmetric_cipher.is_null())
		{
			_symmetric_cipher->setup_counter(pk_sequence_number, pk_highest_ack, pk_packet_type, 0);
			/*if(!bit_stream_decrypt_and_check_hash(pstream, message_signature_bytes, packet_header_byte_size, _symmetric_cipher))
			{
				CradleLogMessage(notify_connection, ("Packet failed crypto"));
				return false;
			}*/
		}
		
		uint32 pk_ack_byte_count = pstream.read_ranged_uint32(0, max_ack_byte_count);
		if(pk_ack_byte_count > max_ack_byte_count || pk_packet_type >= invalid_packet_type)
			return false;
		
		uint32 pk_ack_mask[max_ack_mask_size];
		uint32 pk_ack_word_count = (pk_ack_byte_count + 3) >> 2;
		
		for(uint32 i = 0; i < pk_ack_word_count; i++)
			pk_ack_mask[i] = pstream.read_integer(i == pk_ack_word_count - 1 ? (pk_ack_byte_count - (i * 4)) * 8 : 32);
		pstream.advance_to_next_byte();
		logf(notify_connection, ("header read %d bits.", pstream.get_bit_position()));
		//if(is_network_connection())
		//{
		//   logf(LogBlah, ("RCV: mHA: %08x  pkHA: %08x  mLSQ: %08x  pkSN: %08x  pkLS: %08x  pkAM: %08x",
		//      _highest_acked_seq, pk_highest_ack, _last_send_seq, pk_sequence_number, _last_seq_recvd, pk_ack_mask[0]));
		//}
		
		static const char *packet_type_names[] = 
		{
			"data_packet",
			"ping_packet",
			"ack_packet",
		};
		
		log_block(notify_connection,
					   for(uint32 i = _last_seq_recvd+1; i < pk_sequence_number; i++)
                       logf(notify_connection, ("Not recv %d", i - _initial_recv_seq));
                       logf(notify_connection, ("Recv %d %s", pk_sequence_number - _initial_recv_seq, packet_type_names[pk_packet_type]));
					   );
		
		// shift up the ack mask by the packet difference this essentially nacks all the packets dropped
		
		uint32 ack_mask_shift = pk_sequence_number - _last_seq_recvd;
		
		// if we've missed more than a full word of packets, shift up by words
		while(ack_mask_shift > 32)
		{
			for(int32 i = max_ack_mask_size - 1; i > 0; i--)
				_ack_mask[i] = _ack_mask[i-1];
			_ack_mask[0] = 0;
			ack_mask_shift -= 32;
		}
		
		// the first word upshifts all NACKs, except for the low bit, which is a 1 if this is a data packet (i.e. not a ping packet or an ack packet)
		uint32 up_shifted = (pk_packet_type == data_packet) ? 1 : 0; 
		
		for(uint32 i = 0; i < max_ack_mask_size; i++)
		{
			uint32 next_shift = _ack_mask[i] >> (32 - ack_mask_shift);
			_ack_mask[i] = (_ack_mask[i] << ack_mask_shift) | up_shifted;
			up_shifted = next_shift;
		}
		
		// do all the notifies...
		uint32 notify_count = pk_highest_ack - _highest_acked_seq;
		for(uint32 i = 0; i < notify_count; i++) 
		{
			uint32 notify_index = _highest_acked_seq + i + 1;
			
			uint32 ack_mask_bit = (pk_highest_ack - notify_index) & 0x1F;
			uint32 ack_mask_word = (pk_highest_ack - notify_index) >> 5;
			
			bool packet_transmit_success = (pk_ack_mask[ack_mask_word] & (1 << ack_mask_bit)) != 0;
            logf(notify_connection, ("Ack %d %d", notify_index - _initial_send_seq, packet_transmit_success));
			
			net_socket_event *event = _net_socket->_event_queue.post_event(notify_connection_packet_notify_event_type, _address, _connection_index);
			event->delivered = packet_transmit_success;
            event->packet_sequence = notify_index - _initial_send_seq;
						
			if(packet_transmit_success)
				_last_recv_ack_ack = _last_seq_recvd_at_send[notify_index & packet_window_mask];
		}
		// the other side knows more about its window than we do.
		if(pk_sequence_number - _last_recv_ack_ack > max_packet_window_size)
			_last_recv_ack_ack = pk_sequence_number - max_packet_window_size;
		
		_highest_acked_seq = pk_highest_ack;
		
		// first things first... ackback any pings or half-full windows
		
		keep_alive(); // notification that the connection is ok
		
		uint32 prev_last_sequence = _last_seq_recvd;
		_last_seq_recvd = pk_sequence_number;
		
		if(pk_packet_type == ping_packet || (pk_sequence_number - _last_recv_ack_ack > (max_packet_window_size >> 1)))
		{
			// send an ack to the other side the ack will have the same packet sequence as our last sent packet if the last packet we sent was the connection accepted packet we must resend that packet
			send_ack_packet();
		}
		return prev_last_sequence != pk_sequence_number && pk_packet_type == data_packet;
	}
	
	/// Sends a ping packet to the remote host, to determine if it is still alive and what its packet window status is.
	void send_ping_packet()
	{
		send_packet(ping_packet, 0, 0);
        logf(notify_connection, ("send ping %d", _last_send_seq - _initial_send_seq));
	}
	
	/// Sends an ack packet to the remote host, in response to receiving a ping packet.
	void send_ack_packet()
	{
		send_packet(ack_packet, 0, 0);
        logf(notify_connection, ("send ack %d", _last_send_seq - _initial_send_seq));
	}
	
	/// Called when a packet is received to stop any timeout action in progress.
	void keep_alive()
	{
		_last_ping_send_time = time(0);
		_ping_send_count = 0;
	}
	
public:
	/// Sets the initial sequence number of packets read from the remote host.
	void set_initial_recv_sequence(uint32 sequence)
	{ 
		_initial_recv_seq = _last_seq_recvd = _last_recv_ack_ack = sequence;
	}
	
	/// Returns the initial sequence number of packets sent from the remote host.
	uint32 get_initial_recv_sequence()
	{
		return _initial_recv_seq;
	}
	
	/// Returns the initial sequence number of packets sent to the remote host.
	uint32 get_initial_send_sequence()
	{
		return _initial_send_seq;
	}
	
	/// Returns true if this notify_connection has sent packets that have not yet been acked by the remote host.
	bool has_unacked_sent_packets()
	{
		return _last_send_seq != _highest_acked_seq;
	}
	
	uint32 get_last_received_sequence()
	{
		return _last_seq_recvd;
	}
	/// Returns the next send sequence that will be sent by this side.
	uint32 get_next_send_sequence()
	{
		return _last_send_seq + 1;
	}
	
	/// Returns the sequence of the last packet sent by this notify_connection, or the current packet's send sequence if called from within write_packet().
	uint32 get_last_send_sequence()
	{
		return _last_send_seq;
	}
	
	uint32 get_connection_index()
	{
		return _connection_index;
	}
	
	/// Returns true if the packet send window is full and no more data packets can be sent.
	bool window_full()
	{
		if(_last_send_seq - _highest_acked_seq >= (max_packet_window_size - 2))
			return true;
		return false;
	}
	
	//----------------------------------------------------------------
	// Connection functions
	//----------------------------------------------------------------
public:
	/// Sets the notify_socket this notify_connection will communicate through.
	void set_net_socket(notify_socket *the_net_socket)
	{
		_net_socket = the_net_socket;
	}
	
	/// Returns the notify_socket this notify_connection communicates through.
	notify_socket *get_net_socket()
	{
		return _net_socket;
	}
	
	/// Sets the symmetric_cipher this connection will use for encryption
	void set_symmetric_cipher(symmetric_cipher *the_cipher)
	{
		_symmetric_cipher = the_cipher;
	}
	
	ref_ptr<symmetric_cipher> get_symmetric_cipher()
	{
		return _symmetric_cipher;
	}

	byte_buffer_ptr &get_shared_secret()
	{
		return _shared_secret;
	}
	void set_shared_secret(byte_buffer_ptr secret)
	{
		_shared_secret = secret;
	}
	/// Sets the ping/timeout characteristics for a fixed-rate connection.  Total timeout is msPerPing * ping_retry_count.
	void set_ping_timeouts(time time_per_ping, uint32 ping_retry_count)
	{
		_ping_retry_count = ping_retry_count;
		_ping_timeout = time_per_ping;
	}
	
	/// Simulates a network situation with a percentage random packet loss and a connection one way latency as specified.
	void set_simulated_net_params(float32 packet_loss, uint32 latency)
	{
		_simulated_packet_loss = packet_loss;
		_simulated_latency = latency;
	}
	
	/// Returns the remote address of the host we're connected or trying to connect to.
	const address &get_address()
	{
		return _address;
	}
	
	/// Sets the address of the remote host we want to connect to.
	void set_address(const address &the_address)
	{
		_address = the_address;
	}
	
	bool is_initiator()
	{
		return _is_initiator;
	}
	
	nonce &get_initiator_nonce()
	{
		return _initiator_nonce;
	}
	
	nonce &get_host_nonce()
	{
		return _host_nonce;
	}
	
	void set_initiator_nonce(nonce the_nonce)
	{
		_initiator_nonce = the_nonce;
	}
	void set_host_nonce(nonce &the_nonce)
	{
		_host_nonce = the_nonce;
	}
	
	/// Checks to see if the notify_connection has timed out, possibly sending a ping packet to the remote host.  Returns true if the notify_connection timed out.
	bool check_timeout(time current_time)
	{
		if(_last_ping_send_time.get_milliseconds() == 0)
			_last_ping_send_time = current_time;
		
		time timeout = _ping_timeout;
		uint32 timeout_count = _ping_retry_count;

		if((current_time - _last_ping_send_time) > timeout)
		{
			if(_ping_send_count >= timeout_count)
				return true;
			_last_ping_send_time = current_time;
			_ping_send_count++;
			send_ping_packet();
		}
		return false;
	}

	notify_connection(nonce initiator_nonce, uint32 initial_send_sequence, uint32 connection_index, bool is_initiator)
	{
		_is_initiator = is_initiator;
		_connection_index = connection_index;
		_initial_send_seq = initial_send_sequence;
		_initiator_nonce = initiator_nonce;
		
		_simulated_latency = 0;
		_simulated_packet_loss = 0;
		
		_last_ping_send_time = time(0);
		_ping_send_count = 0;
		
		_last_seq_recvd = 0;
		_highest_acked_seq = _initial_send_seq;
		_last_send_seq = _initial_send_seq; // start sending at _initial_send_seq + 1
		_ack_mask[0] = 0;
		_last_recv_ack_ack = 0;
		
		_ping_timeout = time(default_ping_timeout);
		_ping_retry_count = default_ping_retry_count;
	}
protected:
	safe_ptr<notify_socket> _net_socket; ///< The notify_socket of which this notify_connection is a member.
	address _address; ///< The network address of the host this instance is connected to.
	uint32 _connection_index; ///< The id of this connection on its socket.
	bool _is_initiator; ///< True if this host initiated the arranged connection.
	nonce _initiator_nonce; ///< Unique nonce generated for this connection to send to the server.
	nonce _host_nonce; ///< Unique nonce generated by the server for the connection.	
	byte_buffer_ptr _shared_secret; ///< The shared secret key 
	ref_ptr<symmetric_cipher> _symmetric_cipher; ///< The helper object that performs symmetric encryption on packets

	uint32 _last_seq_recvd_at_send[max_packet_window_size]; ///< The sequence number of the last packet received from the remote host when we sent the packet with sequence X & packet_window_mask.
	uint32 _last_seq_recvd; ///< The sequence number of the most recently received packet from the remote host.
	uint32 _highest_acked_seq; ///< The highest sequence number the remote side has acknowledged.
	uint32 _last_send_seq; ///< The sequence number of the last packet sent.
	uint32 _ack_mask[max_ack_mask_size]; ///< long string of bits, each acking a packet sent by the remote host.
	///< The bit associated with _last_seq_recvd is the low bit of the 0'th word of _ack_mask.
	uint32 _last_recv_ack_ack; ///< The highest sequence this side knows the other side has received an ACK or NACK for.
	uint32 _initial_send_seq; ///< The first _last_send_seq for this side of the notify_connection.
	uint32 _initial_recv_seq; ///< The first _last_seq_recvd (the first _last_send_seq for the remote host).
	time _highest_acked_send_time; ///< The send time of the highest packet sequence acked by the remote host.  Used in the computation of round trip time.
	time _last_update_time; ///< The last time a packet was sent from this instance.
	
	time _ping_timeout; ///< time to wait before sending a ping packet.
	uint32 _ping_retry_count; ///< Number of unacknowledged pings to send before timing out.
	// timeout management stuff:
	uint32 _ping_send_count; ///< Number of unacknowledged ping packets sent to the remote host
	time _last_ping_send_time; ///< Last time a ping packet was sent from this connection
	uint32 _simulated_latency; ///< Amount of additional time this connection delays its packet sends to simulate latency in the connection
	float32 _simulated_packet_loss; ///< Function to simulate packet loss on a network
};
