// CradleTest - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

/// test_net_interface - the net_interface subclass used in CradleTest.  test_net_interface subclasses CradleTest to provide server pinging and response info packets.  When a client test_net_interface starts, it begins sending ping packets to the ping_addr address specified in the constructor.  When a server receives a GamePingRequest packet, it sends a GamePingResponse packet to the source address of the GamePingRequest.  Upon receipt of this response packet, the client attempts to connect to the server that returned the response.  When the connection or connection attempt to that server is terminated, the test_net_interface resumes pinging for available CradleTest servers. 
class test_net_interface : public net_interface
{
	typedef net_interface parent;
public:
	/// Constants used in this net_interface
	enum Constants {
		PingDelayTime = 2000, ///< Milliseconds to wait between sending GamePingRequest packets.
		GamePingRequest = notify_socket::first_valid_info_packet_id, ///< Byte value of the first byte of a GamePingRequest packet.
		GamePingResponse, ///< Byte value of the first byte of a GamePingResponse packet.
	};
	
	bool _pinging_servers; ///< True if this is a client that is pinging for active servers.
	time _last_ping_time; ///< The last platform time that a GamePingRequest was sent from this network interface.
	bool _is_server; ///< True if this network interface is a server, false if it is a client.
	
	safe_ptr<test_connection> _connection_to_server; ///< A safe pointer to the current connection to the server, if this is a client.
	address _ping_address; ///< Network address to ping in searching for a server.  This can be a specific host address or it can be a LAN broadcast address.
	test_game *_game; ///< The game object associated with this network interface.
	
	/// Constructor for this network interface, called from the constructor for test_game.  The constructor initializes and binds the network interface, as well as sets parameters for the associated game and whether or not it is a server.
	test_net_interface(test_game *the_game, bool server)
	{
		_game = the_game;
		_is_server = server;
		_pinging_servers = !server;
		_last_ping_time = 0;
	}
	
	void set_ping_address(address &addr)
	{
		_ping_address = addr;		
	}

	test_game *get_game()
	{
		return _game;
	}
	
	/// handle_info_packet overrides the method in the net_interface class to handle processing of the GamePingRequest and GamePingResponse packet types.
	void process_socket_packet(const address &from, bit_stream &stream)
	{
		packet_stream write_stream;
		uint8 packet_type;
        read(stream, packet_type);
		string from_string = from.to_string();
		logf(cradle_test, ("%s - received socket packet, packet_type == %d.", from_string.c_str(), packet_type));
		if(packet_type == GamePingRequest && _is_server)
		{
            logf(cradle_test, ("%s - received ping.", from_string.c_str()));
			// we're a server, and we got a ping packet from a client, so send back a GamePingResponse to let the client know it has found a server.
			write(write_stream, uint8(GamePingResponse));
            send_socket_packet(from, write_stream);
			
            logf(cradle_test, ("%s - sending ping response.", from_string.c_str()));
		}
		else if(packet_type == GamePingResponse && _pinging_servers)
		{
			// we were pinging servers and we got a response.  Stop the server pinging, and try to connect to the server.
			
            logf(cradle_test, ("%s - received ping response.", from_string.c_str()));
			
			ref_ptr<net_connection> the_connection = new test_connection(true);
			connect(from, the_connection);
            logf(cradle_test, ("Connecting to server: %s", from_string.c_str()));
			
			_pinging_servers = false;
		}
	}	
	
	/// send_ping sends a GamePingRequest packet to _ping_address of this test_net_interface.
	void send_ping()
	{
		packet_stream write_stream;
		
		write(write_stream, uint8(GamePingRequest));
        send_socket_packet(_ping_address, write_stream);

		string ping_address_string = _ping_address.to_string();
        logf(cradle_test, ("%s - sending ping.", ping_address_string.c_str()));
	}
	
	/// Tick checks to see if it is an appropriate time to send a ping packet, in addition to checking for incoming packets and processing connections.
	void tick()
	{
		time current_time = time::get_current();
		
		if(_pinging_servers && (_last_ping_time + PingDelayTime) < current_time)
		{
			_last_ping_time = current_time;
			send_ping();
		}
		process_socket();
		check_for_packet_sends();
	}
};

