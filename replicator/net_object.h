// The replicator library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

//-----------------------------------------------------------------------------
/// Superclass for replicable networked objects.
///
/// @section net_object_intro Introduction To net_object And Replicating
///
/// The replicator library's core purpose is the replication and prioritized most-recent-state network updates of objects. The way this works is a bit complex under the covers, but , but it is immensely efficient. Let's run through the steps that the server goes through for each client in this part of NR's architecture:
///   - First, the server determines what objects are in-scope for the client.  This is done by calling perform_scope_query() on the ref_object which is considered the "scope" ref_object. This could be a simulation avatar of the character, a flyby camera, a vehicle the user is controlling, or something else.
///  - Second, it instructs the client to create "replicant" instances of those objects to represent the source objects on the server.  Finally, it sends updates to the replicants of those objects whose state has been updated on the server, prioritizing the updates based on how relevant the ref_object is to that particular client.
///
/// There several significant advantages to using this networking system:
///  - Efficient network usage, since we only send data that has changed. In addition, since we only care about most-recent data, if a packet is dropped, we don't waste effort trying to deliver stale data.
///  - Cheating protection; since replicator doesn't deliver information about game objects which aren't "in scope", the ability for clients to learn about objects outside their immediate perceptions can be curtailed by an agressive scoping function.
///
/// @section NetObject_Implementation An Example Implementation
///
/// The basis of the replicant implementation is net_object.  Each net_object maintains an <b>update_mask</b>, a 32 bit word representing up to 32 independent states for the ref_object.  When a net_object's state changes it calls the set_mask_bits method to notify the network layer that the state has changed and needs to be updated on all clients that have that net_object in scope.
///
/// Using a net_object is very simple; let's go through a simple example implementation:
///
/// @code
/// class SimpleNetObject : public net_object
/// {
/// public:
///   typedef net_object parent;
///   declare_dynamic_class()
/// @endcode
///
/// Above is the standard boilerplate code for a net_object subclass.
///
/// @code
///    char message1[256];
///    char message2[256];
///    enum States {
///       Message1Mask = BIT(0),
///       Message2Mask = BIT(1),
///    };
/// @endcode
///
/// The example class has two ref_object "states" that each instance keeps track of, message1 and message2.  A real game ref_object might have states for health, velocity and position, or some other set of fields.  Each class has 32 bits to work with, so it's possible to be very specific when defining states.  In general, individual state bits should be assigned only to things that are updated independently - so if you update the position field and the velocity at the same time always, you could use a single bit to represent that state change.
///
/// @code
///    SimpleNetObject()
///    {
///       // in order for an ref_object to be considered by the network system,
///       // the replicable net flag must be set.
///       // the ScopeAlways flag indicates that the ref_object is always scoped
///       // on all active connections.
///       _net_flags.set(ScopeAlways | replicable);
///       strcpy(message1, "Hello World 1!");
///       strcpy(message2, "Hello World 2!");
///    }
/// @endcode
///
/// Here is the constructor. The code initializes the net flags, indicating that the SimpleNetObject should always be scoped, and that it can be replicated to remote hosts
///
///
/// @nosubgrouping
struct replicant_info;
class replicant_connection;
class net_interface;

class net_object : public ref_object
{
	friend class replicant_connection;
	friend class net_interface;
	
	typedef ref_object parent;
	
	bit_set _dirty_mask_bits;
	uint32 _remote_index; ///< The index of this replicant on the other side of the connection.
	replicant_info *_first_object_ref; ///< Head of the linked list of GhostInfos for this ref_object.
	
	replicant_connection *_owning_connection; ///< The connection that owns this replicant, if it's a replicant
	net_interface *_interface; ///< The net_interface this object is visible to -- a limitation of the replicating system is that any one net_object can only be replicated over connections within a single interface.
	
protected:
	enum
	{
		is_replicant_flag = bit(1),  ///< Set if this is a replicant.
        next_free_net_flag = bit(2),
	};
	
	uint32 _net_flags;  ///< Flags field describing this ref_object, from NetFlag.
	
public:
	declare_dynamic_class()
	net_object()
	{
		// netFlags will clear itself to 0
		_remote_index = uint32(-1);
		_first_object_ref = NULL;
		_interface = NULL;
		_prev_dirty_list = NULL;
		_next_dirty_list = NULL;
		_dirty_mask_bits = 0;
		_net_flags = 0;
		_type_rep = 0;
	}
	net_object *_prev_dirty_list;
	net_object *_next_dirty_list;
	type_database::type_rep *_type_rep; ///< set only for replicants, this is the type descriptor of this replicant object
	
	~net_object()
	{
		while(_first_object_ref)
			_first_object_ref->connection->detach_object(_first_object_ref);
		
		if(_next_dirty_list)
		{
			_prev_dirty_list->_next_dirty_list = _next_dirty_list;
			_next_dirty_list->_prev_dirty_list = _prev_dirty_list;
		}
	}
    
    uint32 get_net_flags() { return _net_flags; }
    
	/// on_replicant_add is called on the client side of a connection after the constructor and after the first data update.
	virtual bool on_replicant_add(replicant_connection *the_connection)
	{
		return true;
	}
	
	/// on_replicant_remove is called on the client side before the destructor when replicant has gone out of scope and is about to be deleted from the client.
	virtual void on_replicant_remove()
	{ }
	
	/// on_replicant_available is called on the server side after the server knows that the replicant is available and addressable via the get_replicant_index().
	virtual void on_replicant_available(replicant_connection * the_connection)
	{ }
	
	/// Notify the network system that one or more of this ref_object's states have
	/// been changed.
	///
	/// @note This is a server side call. It has no meaning for replicants.
	void set_dirty_mask_bits(bit_set or_mask)
	{
		if(is_replicant())
			return;
		assert(or_mask != 0);
		//Assert(_dirty_mask_bits == 0 || (_prev_dirty_list != NULL || _next_dirty_list != NULL || _dirty_list == this), "Invalid dirty list state.");
		if(_interface)
		{
			if(!_dirty_mask_bits)
				_interface->add_to_dirty_list(this);

			_dirty_mask_bits |= or_mask;
		}
	}
	
	void set_dirty_state(uint32 state_index)
	{
		set_dirty_mask_bits(1 << state_index);
	}
    
    // filter_dirty_mask is called immediately prior to writing an update into a packet on a connection. This is particularly useful for objects that do client-side prediction - i.e. a player control object that uses an alternate path to write out high-precision movement state, for example, so that redundant data isn't sent across the wire.
    virtual bit_set filter_dirty_mask(replicant_connection *the_connection, bit_set dirty_mask)
    {
        return dirty_mask;
    }
	
	/// Called to determine the relative update priority of an ref_object.
	///
	/// All objects that are in scope and that have out of date states are queried and sorted by priority before being updated.  If there is not enough room in a single packet for all out of date objects, the skipped objects will have an incremented update_skips the next time that connection prepares to send a packet. Typically the update priority is scaled by update_skips so that as data becomes stale, it becomes more of a priority to  update.
	virtual float32 get_update_priority(net_object *scope_object, bit_set update_mask, uint32 update_skips)
	{
		return float32(update_skips) * 0.1f;
	}
	
	/// For a scope ref_object, determine what is in scope.
	///
	/// perform_scope_query is called on a NetConnection's scope ref_object
	/// to determine which objects in the world are in scope for that
	/// connection.
	virtual void perform_scope_query( replicant_connection * connection)
	{
        assert(0);
	}
	
    /// on_replicant_pre_update is called on the replicant before a portion of its states have been updated from the host.
    virtual void on_replicant_pre_update(replicant_connection *connection, bit_set mask_bits)
    {

    }

	/// on_replicant_update is called on the replicant when a portion of its states have been updated from the host.  For the initial update this will be called after on_replicant_add
	virtual void on_replicant_update(replicant_connection *connection, bit_set mask_bits)
	{

	}

	/// get_net_index returns the index tag used to identify the server copy
	/// of a client ref_object.
	uint32 get_net_index() { return _remote_index; }
	
	/// is_replicant returns true if this ref_object is a replicant of a server ref_object.
	bool is_replicant() const
	{
		return (_net_flags & is_replicant_flag) != 0;
	}
	
	/// return a hash for this net_object.
	///
	/// @note This is based on its location in memory.
	uint32 get_hash_id() const
	{
		const net_object *ret = this;
		return *((uint32 *) &ret);
	}
};


