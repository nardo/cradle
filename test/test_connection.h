// CradleTest - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

/// test_connection is the CradleTest connection class.
///
/// The test_connection class overrides particular methods for connection housekeeping, and for connected clients manages the transmission of client inputs to the server for player position updates.
///
/// When a client's connection to the server is lost, the test_connection notifies the network interface that it should begin searching for a new server to join.
class test_connection : public replicant_connection
{
	typedef replicant_connection parent;
public:
	declare_dynamic_class()

	void rpc_set_control_object (safe_ptr<player> &the_object)
	{
        if(the_object.is_valid())
		{
            logf(cradle_test, ("My player set to replicant %d", the_object->get_net_index()));
            ((test_net_interface *) get_interface())->get_game()->_client_player = the_object;
		}
	}

	void rpc_move_my_player_to(unit_float<12> x, unit_float<12> y)
	{
		position new_position;
		new_position.x = x;
		new_position.y = y;

        logf(cradle_test, ("received new position (%g, %g) from client",
					   float32(new_position.x), float32(new_position.y)));
		_player->server_set_position(_player->_render_pos, new_position, 0, 0.2f);
	};

    void rpc_array_test(max_sized_array<unit_float<5>, 10 > v)
    {
        for(uint32 i = 0; i < v.size(); i++)
            logf(cradle_test, ("v[%d] = %g", i, v[i].value));
    }
    void rpc_string_test(indexed_string &the_string, max_sized_array<indexed_string, 5> &v)
    {
        logf(cradle_test, ("got string: %s", the_string.c_str()));
        for(uint32 i = 0; i < v.size(); i++)
            logf(cradle_test, ("v[%d] = %s", i, v[i].c_str()));
    }
	test_connection(bool is_initiator = false) : parent(is_initiator)
	{
        use_indexed_strings();
        register_rpc(&test_connection::rpc_string_test, rpc_guaranteed_ordered, rpc_host_to_initiator);
        
        register_rpc(&test_connection::rpc_set_control_object, rpc_guaranteed_ordered, rpc_host_to_initiator);
        register_rpc(&test_connection::rpc_array_test, rpc_guaranteed_ordered, rpc_host_to_initiator);
		register_rpc(&test_connection::rpc_move_my_player_to, rpc_guaranteed_ordered, rpc_initiator_to_host);
	}
	
	/// The player object associated with this connection.
	safe_ptr<player> _player;
	
	void on_connection_rejected(bit_stream &reject_stream)
	{
		((test_net_interface *) get_interface())->_pinging_servers = true;
	}
	
	/// on_connect_terminated is called when the connection request to the server is unable to complete due to server rejection, timeout or other error.  When a test_connection connect request to a server is terminated, the client's network interface is notified so it can begin searching for another server to connect to.
	void on_connection_timed_out()
	{
		on_connection_terminated(0);
	}
	
	void on_connection_disconnected(bit_stream &disconnected_stream)
	{
		on_connection_terminated(&disconnected_stream);
	}
	
	void on_connection_terminated(bit_stream *the_stream)
	{
		if(is_connection_initiator())
			((test_net_interface *) get_interface())->_pinging_servers = true;
		else
			delete (player*) _player;
	}
	
	string get_address_string()
	{
		
	}
	/// on_connection_established is called on both ends of a connection when the connection is established.  On the server this will create a player for this client, and set it as the client's scope object.  On both sides this will set the proper replicating behavior for the connection (ie server to client).
	void on_connection_established()
	{
		// call the parent's on_connection_established.  by default this will set the initiator to be a connection to "server" and the non-initiator to be a connection to "client"
		parent::on_connection_established();
		
		// To see how this program performs with 50% packet loss, Try uncommenting the next line :)
		//setSimulatedNetParams(0.5, 0);
		
		set_type_database(((test_net_interface *) get_interface())->get_game()->get_type_database());
		if(is_connection_initiator())
		{
			set_replicant_from(false);
			set_replicant_to(true);
            logf(cradle_test, ("%d - connected to server.", _connection));
			((test_net_interface *) get_interface())->_connection_to_server = this;
		}
		else
		{
			// on the server, we create a player object that will be the scope object
			// for this client.
			_player = new player;
			_player->add_to_game(((test_net_interface *) get_interface())->_game);
			set_scope_object(_player);
			set_replicant_from(true);
			set_replicant_to(false);
			activate_replicating();
            max_sized_array<unit_float<5>, 10 > v;
            v.push_back(0.5);
            v.push_back(0.62);
            v.push_back(0.123);
            v.push_back(0.522);
            call(&test_connection::rpc_array_test, v);
            indexed_string s(context::get_current()->get_string_table(), "Foobar!");
            max_sized_array<indexed_string, 5> vs;
            
            bool testVal[4];
            testVal[0] = is_pointer<indexed_string>::is_true;
            testVal[1] = is_convertible<indexed_string, int>::is_true;
            testVal[2] = is_enum<indexed_string>::is_true;
            testVal[3] = has_trivial_copy<indexed_string>::is_true;
            logf(cradle_test, ("%d %d %d %d", testVal[0], testVal[1], testVal[2], testVal[3]));
            
            for(uint32 i = 0; i < 4; i++)
            {
                static const char *strings[4] = {
                    "Hello, world.", "DMMFIX wuz here", "testing 123", "another_string",
                };
                indexed_string s1(context::get_current()->get_string_table(), strings[i]);
                vs.push_back(s1);
            }
            call(&test_connection::rpc_string_test, s, vs);

            logf(cradle_test, ("%d - client connected.", _connection));
		}
	}
	
	
	/// is_data_to_transmit is called each time the connection is ready to send a packet.  If the NetConnection subclass has data to send it should return true.  In the case of a simulation, this should always return true.
	bool is_data_to_transmit()
	{
		// we always want packets to be sent.
		return true;
	}
};

