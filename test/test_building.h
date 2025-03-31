// CradleTest - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

/// The building class is an example of a net_object that is always scoped by the scoping class (player).  Objects that are for the most part static and generally relevant to the client are often worth always scoping, so the hit of transferring the basic state of the object is only felt once.  The "buildings" are represented by red rectangles on the playing field, and are constructed with random position and extents.
class building : public net_object
{
	typedef net_object parent;
public:
	declare_dynamic_class()

	/// Mask bits used to determine what states are out of date for this object and what then represent.
	enum {
		initial_state = 1, ///< building's only mask bit is the initial mask, as no other states are ever set.
	};
	test_game *_game;      ///< The _game object this building is associated with
	position upper_left;  ///< Upper left corner of the building rectangle on the screen.
	position lower_right; ///< Lower right corner of the building rectangle on the screen.
	
	/// The building constructor creates a random position and extent for the building, and marks it as scopeAlways.
	building(random_generator *random_gen = 0)
	{
		// place the "building" in a random position on the screen
		if(random_gen)
		{
			upper_left.x = random_gen->random_unit_float();
			upper_left.y = random_gen->random_unit_float();

			lower_right.x = upper_left.x + random_gen->random_unit_float() * 0.1f + 0.025f;
			lower_right.y = upper_left.y + random_gen->random_unit_float() * 0.1f + 0.025f;
			if(lower_right.x > 1)
				lower_right.x = 1;
			if(lower_right.y > 1)
				lower_right.y = 1;
		}
		else
		{
			upper_left.x = upper_left.y = 0;
			lower_right.x = lower_right.y = 1;
		}
		_game = NULL;
	}
	
	/// The building destructor removes the building from the _game, if it is associated with a _game object.
	~building()
	{
		if(!_game)
			return;
		
		// remove the building from the list of buildings in the _game.
		for( int32 i = 0; i < _game->_buildings.size(); i++)
		{
			if(_game->_buildings[i] == this)
			{
				_game->_buildings.erase_fast(i);
				return;
			}
		}   
	}
	
	/// Called on the client when this building object has been replicated to the client and its first unpackUpdate has been called.  on_replicant_add adds the building to the client _game.
	bool on_replicant_add(replicant_connection *the_connection)
	{
		add_to_game(((test_net_interface *) the_connection->get_interface())->_game);
		return true;
	}
	
	/// add_to_game is a helper function called by on_replicant_add and on the server to add the building to the specified _game.
	void add_to_game(test_game *the_game)
	{
		// add it to the list of buildings in the game
		the_game->_buildings.push_back(this);
		_game = the_game;
	}
	
	static void register_class(type_database &the_database)
	{
		core_type_begin_class(the_database, building, net_object, true);
		core_type_compound_slot(the_database, building, upper_left, initial_state);
		core_type_compound_slot(the_database, building, lower_right, initial_state);
		core_type_end_class(the_database);
	}
};

