// The notify_sockets library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

class notify_socket;

/// All data associated with the negotiation of the connection
class pending_connection
{
public:
	/// enum of possible states of a pending connection.  A pending connection can be created in one of four states: initiator, host, introduced initiator, introduced host.  In the case of an introduced connection, the initial state will be requesting_introduction.  A connection created as an initiator will begin in the requesting_challenge_response state, and a pending_connection host will be created in the awaiting_local_accept state.
	enum pending_connection_state {
		
		requesting_introduction, ///< Requesting the address and shared secret for connection to an introduced initiator or host
		sending_punch_packets, ///< The state of a pending introduced connection after receiving an introduction from the introducer; when the initiator receives a punch it requests a challenge response; the host stays in punch state until it receives a challenge request.
		requesting_challenge_response, ///< This initiator is sending challenge requests, awaiting the response.  An introdced initiator will move into this state after punch confirmation.
		awaiting_local_challenge_accept, ///< This pending connection is awaiting either an accept_connection_challenge or a disconnect call to the socket for this connection.  A pending connection initiator will be in this state after receiving a challenge response from the host.
		
		awaiting_connect_request, ///< this introduced connection has received a challenge request, response sent, awaiting connect packet
		computing_puzzle_solution, ///< This initiator has accepted a challenge response, and is in the process of computing a solution to the client puzzle offered by the host.
		requesting_connection, ///< After computing the puzzle solution, this initiator is now requesting a connection to the host.
		awaiting_local_accept, ///< This pending connection is is awaiting either an accept_connection or disconnect call.
		pending_connection_state_count,
	};
	enum pending_connection_type
	{
		connection_initiator,
		connection_host,
		introduced_connection_initiator,
		introduced_connection_host,
	};
	
	/// Sets the current connection state of this connection.
	void set_state(pending_connection_state state)
	{
		_state = state;
	}
	
	/// Gets the current connection state of this connection.
	pending_connection_state get_state()
	{
		return _state;
	}
	
	pending_connection_type get_type()
	{
		return _type;
	}

	byte_buffer_ptr &get_shared_secret()
	{
		return _shared_secret;
	}
	void set_shared_secret(byte_buffer_ptr secret)
	{
		_shared_secret = secret;
	}

	pending_connection(pending_connection_type type, nonce initiator_nonce, uint32 initial_send_sequence, uint32 connection_index)
	{
		_type = type;
		if(type == connection_initiator)
			_state = requesting_challenge_response;
		else if(type == connection_host)
			_state = awaiting_local_accept;
		else
			_state = requesting_introduction;
		_state_send_retry_count = 0;
		_puzzle_retried = false;
		_connection_index = connection_index;
		_initiator_nonce = initiator_nonce;
		_initial_send_sequence = initial_send_sequence;
		_introducer = 0;
		_remote_client_id = 0;
		_next = 0;
	}
	
	nonce &get_initiator_nonce()
	{
		return _initiator_nonce;
	}
	
	nonce &get_host_nonce()
	{
		return _host_nonce;
	}
	
	address &get_address()
	{
		return _address;
	}

	void set_address(const address &addr)
	{
		_address = addr;
	}
	
	uint32 get_initial_send_sequence()
	{
		return _initial_send_sequence;
	}

	void set_initial_recv_sequence(uint32 sequence)
	{
		_initial_recv_sequence = sequence;
	}

	/// Sets the symmetric_cipher this connection will use for encryption
	void set_symmetric_cipher(symmetric_cipher *the_cipher)
	{
		_symmetric_cipher = the_cipher;
	}

	/// Gets the symmetric_cipher this connection will use for encryption
	symmetric_cipher *get_symmetric_cipher()
	{
		return _symmetric_cipher;
	}

	pending_connection *_next;
	nonce _initiator_nonce;
	nonce _host_nonce;
	uint32 _initial_send_sequence;
	uint32 _initial_recv_sequence;
	byte_buffer_ptr _shared_secret; ///< The shared secret key
	notify_connection_id _introducer; ///< The remote host that will be introducing this connection
	notify_connection_id _remote_client_id; ///< The connection id to the introduced party on the _introducer
	
	address _address; ///< address of the remote host
	pending_connection_type _type; ///< the type of this pending connection
	pending_connection_state _state; ///< the current state of this pending connection
	uint32 _connection_index; ///< the index of this connection on the socket
	array<address> _possible_addresses; ///< List of possible addresses for the remote host in an introduced connection.	
	bool _puzzle_retried; ///< True if a puzzle solution was already rejected by the host once.	
	uint8 _symmetric_key[symmetric_cipher::key_size]; ///< The symmetric key for the connection, generated by the initiator
	uint8 _init_vector[symmetric_cipher::key_size]; ///< The init vector, generated by the host
	
	uint32 _puzzle_difficulty; ///< Difficulty of the client puzzle solved by this client.
	uint32 _puzzle_solution; ///< Solution to the client puzzle the host sends to the initiator.
	uint32 _client_identity; ///< The client identity as computed by the host - basically a hash of the client's address, nonce and some special random data on the host.
	
	uint32 _puzzle_request_index; ///< The index of the puzzle solver thread queue request.
	ref_ptr<asymmetric_key> _public_key; ///< The public key of the remote host.
	ref_ptr<asymmetric_key> _private_key;///< The private key for this connection.  May be generated on the connection attempt.
	
	byte_buffer_ptr _arranged_secret; ///< The shared secret for the introduced connection, generated by the introducer.  Note that unless the peers have some way to validate each other's public keys (upon challenge/connect), the connection will be vulnerable to a man-in-the-middle attack from an untrustworthy introducer.
	ref_ptr<symmetric_cipher> _symmetric_cipher; ///< The helper object that performs symmetric encryption on packets

	uint32 _state_send_retry_count; ///< number of requests to send before timing out while the pending_connection is in this state.
	uint32 _state_send_retry_interval;
	
	time _state_last_send_time; ///< The send time of the last challenge or connect request.
	byte_buffer_ptr _packet_data; ///< data sent along with this connection request, connection accept, 
};

