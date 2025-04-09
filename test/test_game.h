// CradleTest - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

/// The test_game class manages a CradleTest client or server instance.  test_game maintains a list of all the player and building objects in the playing area, and interfaces with the specific platform's windowing system to respond to user input and render the current game display.
class test_net_interface;
class test_game {
public:
	array<player *> _players; ///< vector of player objects in the game
	array<building *> _buildings; ///< vector of _buildings in the game
	bool _is_server; ///< was this game created to be a server?
	test_net_interface *_net_interface; ///< network interface for this game
	safe_ptr<player> _server_player; ///< the player that the server controls
	safe_ptr<player> _client_player; ///< the player that this client controls, if this game is a client
	random_generator _random;
	context _context;
	type_database _type_database;
	
	enum {
		test_connection_identifier_token = 0xBEEF,
	};
	/// Constructor for test_game, determines whether this game will be a client or a server, and what addresses to bind to and ping.  If this game is a server, it will construct 50 random _buildings and 15 random AI _players to populate the "world" with.  test_game also constructs an AsymmetricKey to demonstrate establishing secure connections with clients and servers.
	test_game(bool server, address &ping_address) : _type_database(&_context)
	{
		_is_server = server;
        _context.set_current_on_thread();
		_net_interface = new test_net_interface(this, _is_server);
		
		_net_interface->set_ping_address(ping_address);
		
		_net_interface->add_connection_type<test_connection>(test_connection_identifier_token);

		random_generator g;
		
        position::register_class(_type_database);
		player::register_class(_type_database);
		building::register_class(_type_database);

		if(_is_server)
		{
			// generate some _buildings and AIs:
			for(int32 i = 0; i < 50; i ++)
			{
				building *the_building = new building(&g);
				the_building->add_to_game(this);
			}
			for(int32 i = 0; i < 15; i ++)
			{
				player *ai_player = new player(player::player_type_ai, &g);
				ai_player->add_to_game(this);
			}
			_server_player = new player(player::player_type_my_client);
			_server_player->add_to_game(this);
		}
		
        logf(cradle_test, ("Created a %s...", (server ? "server" : "client")));
	}
	
	/// Destroys a game, freeing all player and building objects associated with it.
	~test_game()
	{
        _context.set_current_on_thread();
		delete _net_interface;
		for(int32 i = 0; i < _buildings.size(); i++)
			delete _buildings[i];
		for(int32 i = 0; i < _players.size(); i++)
			delete _players[i];
		
        logf(cradle_test, ("Destroyed a %s...", (this->_is_server ? "server" : "client")));
	}
	
	type_database *get_type_database()
	{
		return &_type_database;
	}
		
	/// Called periodically by the platform windowing code, tick will update all the _players in the simulation as well as tick() the game's network interface.
	void tick(uint32 delta)
	{
		if(!delta)
			return;
        _context.set_current_on_thread();

		float32 delta_seconds = delta / 1000.0f;
		for(int32 i = 0; i < _players.size(); i++)
			_players[i]->update(delta_seconds, &_random);
		_net_interface->tick();
	}
	
	/// move_my_player_to is called by the platform windowing code in response to
	/// user input.
	void move_my_player_to(position new_position)
	{
        _context.set_current_on_thread();

		if(_is_server)
		{
			_server_player->server_set_position(_server_player->_render_pos, new_position, 0, 0.2f);
		}
		else if(!_net_interface->_connection_to_server.is_null())
		{
			((test_connection *) _net_interface->_connection_to_server)->call(&test_connection::rpc_move_my_player_to, new_position.x, new_position.y);
            logf(cradle_test, ("posting new position (%g, %g) to server", float32(new_position.x), float32(new_position.y)));
			//if(!_client_player.is_null())
			//	_client_player->rpcPlayerWillMove("Whee! Foo!");
			//_net_interface->connection_to_server->rpcSetPlayerPos(new_position.x, new_position.y);
		}
	}
    void render_frame(vec2 frame_pos, vec2 frame_ext)
    {
        _context.set_current_on_thread();

        vec2 offset = frame_pos;
        vec2 scale = frame_ext;
        A.C.colorf(1,1,0);
        A.C.fill_rect(offset, scale);
        
        
        // first draw all the _buildings.
        for(int32 i = 0; i < _buildings.size(); i++)
        {
            building *b = _buildings[i];
            A.C.colorf(1,0,0);
            vec2 ul(b->upper_left.x, b->upper_left.y);
            vec2 lr(b->lower_right.x, b->lower_right.y);
            
            A.C.fill_rect(offset + ul * scale, (lr - ul) * scale);
        }
        
        // then render the alpha blended circle around the player,
        // to show the scoping range.
       if(_client_player)
        {
            position p = _client_player->_render_pos;
            A.C.set_blend_mode(BlendModeBlend);
            A.C.colorf(0.5f, 0.5f, 0.5f, 0.65f);
            A.C.fill_circle(offset + vec2(p.x * scale.x, p.y * scale.y), 0.25f * scale.x);
            A.C.set_blend_mode(BlendModeNone);
        }

        // last, draw all the _players in the game.
        for(int32 i = 0; i < _players.size(); i++)
        {
            player *p = _players[i];
            A.C.colorf(0,0,0);
            vec2 outline_half_ext(0.012f, 0.012f);
            vec2 half_ext(0.01f, 0.01f);
            vec2 rp(p->_render_pos.x, p->_render_pos.y);
            A.C.fill_rect(offset + (rp - outline_half_ext) * scale, outline_half_ext * scale * 2);
                        
            switch(p->_player_type)
            {
                case player::player_type_ai:
                case player::player_type_ai_dummy:
                    A.C.colorf(0, 0, 1);
                    break;
                case player::player_type_client:
                    A.C.colorf(0.5, 0.5, 1);
                    break;
                case player::player_type_my_client:
                    A.C.colorf(1, 1, 1);
                    break;
            }
            A.C.draw_rect(offset + (rp - half_ext) * scale, half_ext * scale * 2);
        }
    }
};
