// The replicator library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

/// replicant_connection is a subclass of rpc_connection that manages the transmission (replicating) and updating of NetObjects over a connection.  The replicant_connection is responsible for doing scoping calculations (on the server side) and transmitting most-recent replicant information to the client.
/// replicating is the most complex, and most powerful, part of replicator's capabilities. It allows the information sent to clients to be very precisely matched to what they need, so that no excess bandwidth is wasted.  Each replicant_connection has a <b>scope object</b> that is responsible for determining what other net_object instances are relevant to that connection's client.  Each time replicant_connection sends a packet, net_object::perform_scope_query() is called on the scope object, which calls replicant_connection::object_in_scope() for each relevant object.
/// Each object that is in scope, and in need of update (based on its maskbits) is given a priority ranking by calling that object's getUpdatePriority() method.  The packet is then filled with updates, ordered by priority. This way the most important updates get through first, with less important updates being sent as space is available.
/// There is a cap on the maximum number of replicants that can be active through a replicant_connection at once.  The enum replicant_id_bit_size (defaults to 10) determines how many bits will be used to transmit the ID for each replicant, so the maximum number is 2^replicant_id_bit_size or 1024.  This can be easily raised; see the  replicant_constants enum.
/// Each object replicated is assigned a replicant ID; the client is <b>only</b> aware of the replicant ID. This acts to enhance simulation security, as it becomes difficult to map objects from one connection to another, or to reliably identify objects from ID alone. IDs are also reassigned based on need, making it hard to track objects that have fallen out of scope (as any object which the player shouldn't see would).
/// resolve_replicant() is used on the client side, and resolveObjectFromGhostIndex() on the server side, to convert replicant IDs to object references.
/// @see net_object for more information on network object functionality.
class replicant_connection : public rpc_connection
{
	typedef rpc_connection parent;
public:
	/// replicant_ref tracks an update sent in one packet for the replicant of one net_object.
	///
	/// When we are notified that a pack is sent/lost, this is used to determine what updates need to be resent and so forth.
	struct replicant_ref
	{
		uint32 mask; ///< The mask of bits that were updated in this packet
		uint32 replicant_info_flags; ///< replicant_info::Flags bitset, determes if the replicant is in a special processing mode (created/deleted)
		replicant_info *replicant; ///< The replicant information for the object on the connection that sent the packet this replicant_ref is attached to
		replicant_ref *next_ref; ///< The next replicant updated in this packet
		replicant_ref *update_chain; ///< A pointer to the replicant_ref on the least previous packet that updated this replicant, or NULL, if no prior packet updated this replicant
	};
	
	/// Notify structure attached to each packet with information about the replicant updates in the packet
	struct replicant_packet_notify : public rpc_connection::event_packet_notify
	{
		replicant_ref *replicant_list; ///< list of replicants updated in this packet
		replicant_packet_notify() { replicant_list = NULL; }
	};
	
	type_database *_type_database;
	
	void set_type_database(type_database *type_db)
	{
		_type_database = type_db;
	}
protected:
	
	/// Override of rpc_connection's alloc_notify, to use the replicant_packet_notify structure.
	packet_notify *alloc_notify() { return new replicant_packet_notify; }
	
	/// Override to properly update the replicant_info's for all replicants that had upates in the dropped packet.
	void packet_dropped(packet_notify *pnotify)
	{
		parent::packet_dropped(pnotify);
		replicant_packet_notify *notify = static_cast<replicant_packet_notify *>(pnotify);
		
		replicant_ref *packet_ref = notify->replicant_list;
		// loop through all the packRefs in the packet
		
		while(packet_ref)
		{
			replicant_ref *temp = packet_ref->next_ref;
			
			uint32 update_flags = packet_ref->mask;
			
			// figure out which flags need to be updated on the object
			for(replicant_ref *walk = packet_ref->update_chain; walk && update_flags; walk = walk->update_chain)
				update_flags &= ~walk->mask;
			
			// for any flags we haven't updated since this (dropped) packet
			// or them into the mask so they'll get updated soon
			
			if(update_flags)
			{
				if(!packet_ref->replicant->update_mask)
				{
					packet_ref->replicant->update_mask = update_flags;
					replicant_push_non_zero(packet_ref->replicant);
				}
				else
					packet_ref->replicant->update_mask |= update_flags;
			}
			
			// make sure this packet_ref isn't the last one on the replicant_info
			if(packet_ref->replicant->last_update_chain == packet_ref)
				packet_ref->replicant->last_update_chain = NULL;
			
			// if this packet was replicating an object, set it
			// to re replicant at it's earliest convenience
			
			if(packet_ref->replicant_info_flags & replicant_info::replicating)
			{
				packet_ref->replicant->flags |= replicant_info::not_yet_replicated;
				packet_ref->replicant->flags &= ~replicant_info::replicating;
			}
			
			// otherwise, if it was being deleted,
			// set it to re-delete
			
			else if(packet_ref->replicant_info_flags & replicant_info::killing_replicant)
			{
				packet_ref->replicant->flags |= replicant_info::kill_replicant;
				packet_ref->replicant->flags &= ~replicant_info::killing_replicant;
			}
			
			delete packet_ref;
			packet_ref = temp;
		}
	}
	
	/// Override to update flags associated with the replicants updated in this packet.
	void packet_received(packet_notify *pnotify)
	{
		parent::packet_received(pnotify);
		replicant_packet_notify *notify = static_cast<replicant_packet_notify *>(pnotify);
		
		replicant_ref *packet_ref = notify->replicant_list;
		
		// loop through all the notifies in this packet
		
		while(packet_ref)
		{
			// make sure this packet_ref isn't the last one on the replicant_info
			if(packet_ref->replicant->last_update_chain == packet_ref)
				packet_ref->replicant->last_update_chain = NULL;
			
			replicant_ref *temp = packet_ref->next_ref;      
			// if this object was replicating , it is now replicated
			
			if(packet_ref->replicant_info_flags & replicant_info::replicating)
			{
				packet_ref->replicant->flags &= ~replicant_info::replicating;
				if(packet_ref->replicant->obj)
					packet_ref->replicant->obj->on_replicant_available(this);
			}
			// otherwise, if it was dieing, free the replicant
			
			else if(packet_ref->replicant_info_flags & replicant_info::killing_replicant)
				free_replicant_info(packet_ref->replicant);
			
			delete packet_ref;
			packet_ref = temp;
		}
	}
	
	static int compare_priority(const void *a,const void *b)
	{
		replicant_info *ga = *((replicant_info **) a);
		replicant_info *gb = *((replicant_info **) b);

		float32 ret = ga->priority - gb->priority;
		return (ret < 0) ? -1 : ((ret > 0) ? 1 : 0);
	}
	
	/// Performs the scoping query in order to determine if there is data to send from this replicant_connection.
	void prepare_write_packet()
	{
		parent::prepare_write_packet();
		
		if(!does_replicant_from() || !_replicating)
			return;
		// first step is to check all our polled replicants:
		
		// 1. Scope query - find if any new objects have come into
		//    scope and if any have gone out.
		
		// Each packet we loop through all the objects with non-zero masks and
		// mark them as "out of scope" before the scope query runs.
		// if the object has a zero update mask, we wait to remove it until it requests
		// an update
		
		for(int32 i = 0; i < _replicant_zero_update_index; i++)
		{
			// increment the updateSkip for everyone... it's all good
			replicant_info *walk = _replicant_array[i];
			walk->update_skip_count++;
			if(!(walk->flags & (replicant_info::scope_local_always)))
				walk->flags &= ~replicant_info::in_scope;
		}
		if(_scope_object)
		{
            logf(replicant_connection, ("performing scope query."));
			_scope_object->perform_scope_query(this);
		}
	}
	
	/// Override to write replicant updates into each packet.
	void write_packet(bit_stream &bstream, packet_notify *pnotify)
	{
		parent::write_packet(bstream, pnotify);
		replicant_packet_notify *notify = static_cast<replicant_packet_notify *>(pnotify);
		
		//if(mConnectionParameters.mDebugObjectSizes)
		//  bstream.write_integer(DebugChecksum, 32);
		
		notify->replicant_list = NULL;
		
		if(!does_replicant_from())
			return;
		
		if(!bstream.write_bool(_replicating && _scope_object.is_valid()))
			return;
		
        logf(replicant_connection, ("Filling packet -- %d replicants to update", _replicant_zero_update_index));
		// fill a packet (or two) with replicating data
		
		// 2. call scoped objects' priority functions if the flag set is nonzero
		//    A removed replicant is assumed to have a high priority
		// 3. call updates based on sorted priority until the packet is
		//    full.  set flags to zero for all updated objects
		
		replicant_info *walk;
		
		for(int32 i = _replicant_zero_update_index - 1; i >= 0; i--)
		{
			if(!(_replicant_array[i]->flags & replicant_info::in_scope))
				detach_object(_replicant_array[i]);
		}
		
		uint32 max_index = 0;
		for(int32 i = _replicant_zero_update_index - 1; i >= 0; i--)
		{
			walk = _replicant_array[i];
			if(walk->index > max_index)
				max_index = walk->index;
			
			// clear out any kill objects that haven't been replicated yet
			if((walk->flags & replicant_info::kill_replicant) && (walk->flags & replicant_info::not_yet_replicated))
			{
				free_replicant_info(walk);
				continue;
			}
			// don't do any replicant processing on objects that are being killed
			// or in the process of replicating
			else if(!(walk->flags & (replicant_info::killing_replicant | replicant_info::replicating)))
			{
				if(walk->flags & replicant_info::kill_replicant)
					walk->priority = 10000;
				else
					walk->priority = walk->obj->get_update_priority(_scope_object, walk->update_mask, walk->update_skip_count);
			}
			else
				walk->priority = 0;
		}
		replicant_ref *update_list = NULL;
		qsort(_replicant_array, _replicant_zero_update_index, sizeof(replicant_info *), compare_priority);
		//quickSort(_replicant_array, _replicant_array + _replicant_zero_update_index, compare_priority);
		// reset the array indices...
		for(int32 i = _replicant_zero_update_index - 1; i >= 0; i--)
			_replicant_array[i]->array_index = i;
		
		int32 send_size = 1;
		while(max_index >>= 1)
			send_size++;
		
		if(send_size < 3)
			send_size = 3;
		
		bstream.write_integer(send_size - 3, 3); // 0-7 3 bit number
		
		uint32 count = 0;
		// 
		for(int32 i = _replicant_zero_update_index - 1; i >= 0 && !bstream.is_full(); i--)
		{
			replicant_info *walk = _replicant_array[i];
			if(walk->flags & (replicant_info::killing_replicant | replicant_info::replicating))
				continue;
			
			uint32 update_start = bstream.get_bit_position();
			uint32 update_mask = walk->update_mask;
			uint32 returned_mask = 0;
			
			bstream.write_bool(true);
			bstream.write_integer(walk->index, send_size);
			if(!bstream.write_bool(walk->flags & replicant_info::kill_replicant))
			{
				// this is an update of some kind:
				//if(mConnectionParameters.mDebugObjectSizes)
				//	bstream.advanceBitPosition(BitStreamPosBitSize);
				
				int32 start_position = bstream.get_bit_position();
				bool is_initial_update = false;
				type_database::type_rep *type_rep = walk->type_rep;
				
				if(walk->flags & replicant_info::not_yet_replicated)
				{
					uint32 class_index = type_rep->class_index;
					bstream.write_ranged_uint32(class_index, 0, _type_database->get_indexed_class_count() - 1);
					is_initial_update = true;
				}
				
				// write out the mask unless this is the first update (in which case the mask will be all dirty):
				if(!is_initial_update)
                {
                    // first let the object filter out any dirty states for this particular connection
                    update_mask = walk->obj->filter_dirty_mask(this, update_mask);
                    bstream.write_integer(update_mask, type_rep->max_state_index);
                }
				// now loop through all the fields, and if it's in the update mask, blast it into the bit stream:

				void *object_pointer = (void *) walk->obj;
                logf(replicant_connection, ("Ghost type %s -- mask %08x", type_rep->name.c_str(), update_mask));

				returned_mask = write_object_update(bstream, object_pointer, type_rep, update_mask);

				if(is_initial_update)
				{
					walk->type_rep->initial_update_count++;
					walk->type_rep->initial_update_bit_total += bstream.get_bit_position() - start_position;
				}
				else
				{
					walk->type_rep->total_update_count++;
					walk->type_rep->total_update_bit_total += bstream.get_bit_position() - start_position;
				}
				logf(replicant_connection, ("replicant_connection %s GHOST %d", walk->type_rep->name.c_str(), bstream.get_bit_position() - start_position));
				
				assert((returned_mask & (~update_mask)) == 0); // Cannot set new bits in packUpdate return
			}
			
			// check for packet overrun, and rewind this update if there
			// was one:
			if(bstream.get_bit_space_available() < minimum_padding_bits)
			{
				bstream.set_bit_position(update_start);
				break;
			}
			
			// otherwise, create a record of this replicant update and
			// attach it to the packet.
			replicant_ref *upd = new replicant_ref;
			
			upd->next_ref = update_list;
			update_list = upd;
			
			if(walk->last_update_chain)
				walk->last_update_chain->update_chain = upd;
			walk->last_update_chain = upd;
			
			upd->replicant = walk;
			upd->replicant_info_flags = 0;
			upd->update_chain = NULL;
			
			if(walk->flags & replicant_info::kill_replicant)
			{
				walk->flags &= ~replicant_info::kill_replicant;
				walk->flags |= replicant_info::killing_replicant;
				walk->update_mask = 0;
				upd->mask = update_mask;
				replicant_push_to_zero(walk);
				upd->replicant_info_flags = replicant_info::killing_replicant;
			}
			else
			{
				if(walk->flags & replicant_info::not_yet_replicated)
				{
					walk->flags &= ~replicant_info::not_yet_replicated;
					walk->flags |= replicant_info::replicating;
					upd->replicant_info_flags = replicant_info::replicating;
				}
				walk->update_mask = returned_mask;
				if(!returned_mask)
					replicant_push_to_zero(walk);
				upd->mask = update_mask & ~returned_mask;
				walk->update_skip_count = 0;
				count++;
			}
		}
		// count # of replicants have been updated,
		// _replicant_zero_update_index # of replicants remain to be updated.
		// no more objects...
		bstream.write_bool(false);
		notify->replicant_list = update_list;
	}
	
	/// Override to read updated replicant information from the packet stream.
	void read_packet(bit_stream &bstream)
	{
		parent::read_packet(bstream);
		
		if(!does_replicant_to())
			return;
		if(!bstream.read_bool())
			return;
		
		int32 id_size;
		id_size = bstream.read_integer( 3 );
		id_size += 3;
		
		// while there's an object waiting...
		
		while(bstream.read_bool())
		{
			uint32 index;
			bool is_initial_update = false;
			//int32 start_position = bstream.getCurPos();
			index = (uint32) bstream.read_integer(id_size);
			if(bstream.read_bool()) // is this replicant being deleted?
			{
				assert(_local_replicants[index] != NULL); // Error, NULL replicant encountered.
				if(_local_replicants[index])
				{
					_local_replicants[index]->on_replicant_remove();
					delete _local_replicants[index];
					_local_replicants[index] = NULL;
				}
			}
			else
			{
				int32 start_position = bstream.get_bit_position();
				uint32 end_position = 0;
				void *object_pointer;
				type_database::type_rep *ttr;
				
				if(!_local_replicants[index]) // it's a new replicant... cool
				{
					uint32 class_index = bstream.read_ranged_uint32(0, _type_database->get_indexed_class_count() -  1);
					
					type_database::type_rep *type_rep = _type_database->get_indexed_class(class_index);
					ttr = type_rep;
					
					uint32 instance_size = type_rep->type->size;
					object_pointer = mem_alloc(instance_size);
					type_rep->type->construct_object(object_pointer);
					
					net_object *obj = (net_object *) object_pointer;
					
					obj->_type_rep = type_rep;
					obj->_owning_connection = this;
					obj->_net_flags = net_object::is_replicant_flag;
					
					// object gets initial update before adding to the manager
					
					obj->_remote_index = index;
					_local_replicants[index] = obj;
					
					is_initial_update = true;
                    obj->on_replicant_pre_update(this, 0xFFFFFFFF);
					read_object_update(bstream, object_pointer, type_rep, 0xFFFFFFFF);
					
					if(!obj->on_replicant_add(this))
						throw cradle_exception_replicant_add_failed;
					obj->on_replicant_update(this, 0xFFFFFFFF);
				}
				else
				{
					object_pointer = (void *) _local_replicants[index];
					type_database::type_rep *type_rep = _local_replicants[index]->_type_rep;
					ttr = type_rep;

					uint32 update_mask = bstream.read_integer(type_rep->max_state_index);
                    _local_replicants[index]->on_replicant_pre_update(this, update_mask);
					read_object_update(bstream, object_pointer, type_rep, update_mask);
					_local_replicants[index]->on_replicant_update(this, update_mask);
				}
				logf(replicant_connection, ("replicant_connection %s read GHOST %d", ttr->name.c_str(), bstream.get_bit_position() - start_position));
			}
		}
	}
	
	uint32 write_object_update(bit_stream &bstream, void *object_pointer, type_database::type_rep *type_rep, uint32 update_mask)
	{
		uint32 returned_mask = 0;
		while(type_rep)
		{
			for(dictionary<type_database::field_rep>::pointer i = type_rep->fields.first(); i; ++i)
			{
				type_database::field_rep *field_rep = i.value();
				uint32 state_mask = 1 << field_rep->state_index;
				
				if(update_mask & state_mask)
				{
					bool res;
					void *field_ptr = (void *) ((uint8 *)object_pointer + field_rep->offset);
                    logf(replicant_connection, ("dirty state %d(%s)", field_rep->state_index, field_rep->name.c_str()));

					if(field_rep->compound_type)
						res = field_rep->compound_type->write(bstream, field_ptr);
					else
						res = field_rep->write_function(bstream, field_ptr);
					if(!res)
						returned_mask |= state_mask;					
				}
			}
			type_rep = type_rep->parent_class;
		}
		return returned_mask;
	}
	
	void read_object_update(bit_stream &bstream, void *object_pointer, type_database::type_rep *type_rep, uint32 update_mask)
	{
		while(type_rep)
		{
			for(dictionary<type_database::field_rep>::pointer i = type_rep->fields.first(); i; ++i)
			{
				type_database::field_rep *field_rep = i.value();
				uint32 state_mask = 1 << field_rep->state_index;
				
				if(update_mask & state_mask)
				{
					void *field_ptr = (void *) ((uint8 *)object_pointer + field_rep->offset);
					if(field_rep->compound_type)
						field_rep->compound_type->read(bstream, field_ptr);
					else
						field_rep->read_function(bstream, field_ptr);	
				}
			}
			type_rep = type_rep->parent_class;
		}
	}
	
	/// Override to check if there is data pending on this replicant_connection.
	bool is_data_to_transmit()
	{
		// once we've run the scope query - if there are no objects that need to be updated,
		// we return false
		return parent::is_data_to_transmit() || _replicant_zero_update_index != 0;
	}
	
	//----------------------------------------------------------------
	// replicant manager functions/code:
	//----------------------------------------------------------------
	
protected:
	/// Array of replicant_info structures used to track all the objects replicated by this side of the connection. For efficiency, replicants are stored in three segments - the first segment contains GhostInfos that have pending updates, the second replicantrefs that need no updating, and last, free GhostInfos that may be reused.
	replicant_info **_replicant_array;
	
	
	int32 _replicant_zero_update_index; ///< Index in _replicant_array of first replicant with 0 update mask (ie, with no updates).
	
	int32 _replicant_free_index; ///< index in _replicant_array of first free replicant.
	
	bool _replicating; ///< Am I currently replicating objects over?
	bool _scoping; ///< Am I currently allowing objects to be scoped?
	uint32  _replicating_sequence; ///< Sequence number describing this replicating session.
	
	net_object **_local_replicants; ///< Local replicant array for remote objects, or NULL if mGhostTo is false.
	
	replicant_info *_replicant_refs; ///< Allocated array of replicantInfos, or NULL if mGhostFrom is false.
	replicant_info **_replicant_lookup_table; ///< Table indexed by object id->replicant_info, or NULL if mGhostFrom is false.
	
	safe_ptr<net_object> _scope_object;///< The local net_object that performs scoping queries to determine what objects to replicant to the client.
	
	void clear_replicant_info()
	{
		// gotta clear out the replicants...
		for(packet_notify *walk = _notify_queue_head; walk; walk = walk->next_packet)
		{
			replicant_packet_notify *note = static_cast<replicant_packet_notify *>(walk);
			replicant_ref *del_walk = note->replicant_list;
			note->replicant_list = NULL;
			while(del_walk)
			{
				replicant_ref *next = del_walk->next_ref;
				delete del_walk;
				del_walk = next;
			}
		}
		for(int32 i = 0; i < max_replicant_count; i++)
		{
			if(_replicant_refs[i].array_index < _replicant_free_index)
			{
				detach_object(&_replicant_refs[i]);
				_replicant_refs[i].last_update_chain = NULL;
				free_replicant_info(&_replicant_refs[i]);
			}
		}
		assert((_replicant_free_index == 0) && (_replicant_zero_update_index == 0));
	}
	
	void delete_local_replicants()
	{
		if(!_local_replicants)
			return;
		// just delete all the local replicants,
		// and delete all the replicants in the current save list
		for(int32 i = 0; i < max_replicant_count; i++)
		{
			if(_local_replicants[i])
			{
				_local_replicants[i]->on_replicant_remove();
				delete _local_replicants[i];
				_local_replicants[i] = NULL;
			}
		}
	}
	
	
	
	bool validate_replicant_array()
	{
		assert(_replicant_zero_update_index >= 0 && _replicant_zero_update_index <= _replicant_free_index);
		assert(_replicant_free_index <= max_replicant_count);
		int32 i;
		for(i = 0; i < _replicant_zero_update_index; i ++)
		{
			assert(_replicant_array[i]->array_index == i);
			assert(_replicant_array[i]->update_mask != 0);
		}
		for(; i < _replicant_free_index; i ++)
		{
			assert(_replicant_array[i]->array_index == i);
			assert(_replicant_array[i]->update_mask == 0);
		}
		for(; i < max_replicant_count; i++)
		{
			assert(_replicant_array[i]->array_index == i);
		}
		return true;
	}
	
	void free_replicant_info(replicant_info *replicant)
	{
		assert(replicant->array_index < _replicant_free_index);
		if(replicant->array_index < _replicant_zero_update_index)
		{
			assert(replicant->update_mask != 0);
			replicant->update_mask = 0;
			replicant_push_to_zero(replicant);
		}
		replicant_push_zero_to_free(replicant);
		assert(replicant->last_update_chain == NULL);
	}
	
	/// Notifies subclasses that the remote host is about to start replicating objects.
	virtual void on_start_replicating() {}                              
	
	/// Notifies subclasses that the server has stopped replicating objects on this connection.
	virtual void on_end_replicating() {}
	
public:
	replicant_connection(bool is_initiator = false) : rpc_connection(is_initiator)
	{
		// replicant management data:
		_scope_object = NULL;
		_replicating_sequence = 0;
		_replicating = false;
		_scoping = false;
		_replicant_array = NULL;
		_replicant_refs = NULL;
		_replicant_lookup_table = NULL;
		_local_replicants = NULL;
		_replicant_zero_update_index = 0;
		register_rpc_methods();
	}
	
	~replicant_connection()
	{
		_clear_all_packet_notifies();
		
		// delete any replicants that may exist for this connection, but aren't added
		if(_replicant_array)
			clear_replicant_info();
		delete_local_replicants();
		delete[] _local_replicants;
		delete[] _replicant_lookup_table;
		delete[] _replicant_refs;
		delete[] _replicant_array;
	}
	
	/// Sets whether replicants transmit from this side of the connection.
	void set_replicant_from(bool replicant_from)
	{
		if(_replicant_array)
			return;
		
		if(replicant_from)
		{
			_replicant_free_index = _replicant_zero_update_index = 0;
			_replicant_array = new replicant_info *[max_replicant_count];
			_replicant_refs = new replicant_info[max_replicant_count];
			int32 i;
			for(i = 0; i < max_replicant_count; i++)
			{
				_replicant_refs[i].obj = NULL;
				_replicant_refs[i].index = i;
				_replicant_refs[i].update_mask = 0;
			}
			_replicant_lookup_table = new replicant_info *[replicant_lookup_table_size];
			for(i = 0; i < replicant_lookup_table_size; i++)
				_replicant_lookup_table[i] = 0;
		}
	}
	
	/// Sets whether replicants are allowed from the other side of the connection.
	void set_replicant_to(bool replicant_to)
	{
		if(_local_replicants) // if replicating to this is already enabled, silently return
			return;
		
		if(replicant_to)
		{
			_local_replicants = new net_object *[max_replicant_count];
			for(int32 i = 0; i < max_replicant_count; i++)
				_local_replicants[i] = NULL;
		}
	}
	
	
	/// Does this replicant_connection replicant NetObjects to the remote host?
	bool does_replicant_from() { return _replicant_array != NULL; } 
	
	/// Does this replicant_connection receive replicants from the remote host?
	bool does_replicant_to() { return _local_replicants != NULL; }
	
	/// Returns the sequence number of this replicating session.
	uint32 get_replicating_sequence() { return _replicating_sequence; }
	
	enum replicant_constants {
		replicant_id_bit_size = 10, ///< Size, in bits, of the integer used to transmit replicant IDs
		replicant_lookup_table_size_shift = 10, ///< The size of the hash table used to lookup source NetObjects by remote replicant ID is 1 << replicant_lookup_table_size_shift.
		
		max_replicant_count = (1 << replicant_id_bit_size), ///< Maximum number of replicants that can be active at any one time.
		replicant_count_bit_size = replicant_id_bit_size + 1, ///< Size of the field needed to transmit the total number of replicants.
		
		replicant_lookup_table_size = (1 << replicant_lookup_table_size_shift), ///< Size of the hash table used to lookup source NetObjects by remote replicant ID.
		replicant_lookup_table_mask = (replicant_lookup_table_size - 1), ///< Hashing mask for table lookups.
	};
	
	/// Sets the object that is queried at each packet to determine what NetObjects should be replicated on this connection.
	void set_scope_object(net_object *object)
	{
		if(((net_object *) _scope_object) == object)
			return;
		_scope_object = object;
	}
	
	
	/// Returns the current scope object.
	net_object *get_scope_object() { return (net_object*)_scope_object; }; 
	
	/// Indicate that the specified object is currently in scope.  Method called by the scope object to indicate that the specified object is in scope.
	void object_in_scope(net_object *object)
	{
		if (!_scoping || !does_replicant_from())
			return;
		
		type_database::type_rep *type_rep = _type_database->find_type(object->get_type_record());
						
		assert(type_rep != 0);
		assert(object->_interface == 0 || object->_interface == _interface);
		
		object->_interface = _interface;
						
		int32 index = object->get_hash_id() & replicant_lookup_table_mask;
		
		// check if it's already in scope
		// the object may have been cleared out without the lookupTable being cleared
		// so validate that the object pointers are the same.
		
		for(replicant_info *walk = _replicant_lookup_table[index ]; walk; walk = walk->next_lookup_info)
		{
			if(walk->obj != object)
				continue;
			walk->flags |= replicant_info::in_scope;
			return;
		}
		
		if (_replicant_free_index == max_replicant_count)
			return;
		
		replicant_info *giptr = _replicant_array[_replicant_free_index];
		replicant_push_free_to_zero(giptr);
		giptr->update_mask = 0xFFFFFFFF;
		replicant_push_non_zero(giptr);
		
		giptr->flags = replicant_info::not_yet_replicated | replicant_info::in_scope;
		
		giptr->obj = object;
		giptr->type_rep = type_rep;
		
		giptr->last_update_chain = NULL;
		giptr->update_skip_count = 0;
		
		giptr->connection = this;
		
		giptr->next_object_ref = object->_first_object_ref;
		if(object->_first_object_ref)
			object->_first_object_ref->prev_object_ref = giptr;
		giptr->prev_object_ref = NULL;
		object->_first_object_ref = giptr;
		
		giptr->next_lookup_info = _replicant_lookup_table[index];
		_replicant_lookup_table[index] = giptr;
		//assert(validate_replicant_array(), "Invalid replicant array!");
	}
	
	/// The specified object should be always in scope for this connection.
	void object_local_scope_always(net_object *obj)
	{
		if(!does_replicant_from())
			return;
		object_in_scope(obj);
		for(replicant_info *walk = _replicant_lookup_table[obj->get_hash_id() & replicant_lookup_table_mask]; walk; walk = walk->next_lookup_info)
		{
			if(walk->obj != obj)
				continue;
			walk->flags |= replicant_info::scope_local_always;
			return;
		}
	}
	
	/// The specified object should not be always in scope for this connection.
	void object_local_clear_always(net_object *object)
	{
		if(!does_replicant_from())
			return;
		for(replicant_info *walk = _replicant_lookup_table[object->get_hash_id() & replicant_lookup_table_mask]; walk; walk = walk->next_lookup_info)
		{
			if(walk->obj != object)
				continue;
			walk->flags &= ~replicant_info::scope_local_always;
			return;
		}
	}
	
	
	/// Given an object's replicant id, returns the replicant of the object (on the client side).
	net_object *resolve_replicant(int32 id)
	{
		if(id == -1)
			return NULL;
		
		return _local_replicants[id];
	}
	
	
	/// Given an object's replicant id, returns the source object (on the server side).
	net_object *resolve_replicant_parent(int32 id)
	{
		return _replicant_refs[id].obj;
	}
	
	/// Moves the specified replicant_info into the range of the replicant array for non-zero updateMasks.
	void replicant_push_non_zero(replicant_info *info)
	{
		assert(info->array_index >= _replicant_zero_update_index && info->array_index < _replicant_free_index); // Out of range array_index
		assert(_replicant_array[info->array_index] == info); // Invalid array object
		if(info->array_index != _replicant_zero_update_index)
		{
			_replicant_array[_replicant_zero_update_index]->array_index = info->array_index;
			_replicant_array[info->array_index] = _replicant_array[_replicant_zero_update_index];
			_replicant_array[_replicant_zero_update_index] = info;
			info->array_index = _replicant_zero_update_index;
		}
		_replicant_zero_update_index++;
		//assert(validate_replicant_array(), "Invalid replicant array!");
	}
	
	/// Moves the specified replicant_info into the range of the replicant array for zero updateMasks.
	void replicant_push_to_zero(replicant_info *info)
	{
		assert(info->array_index < _replicant_zero_update_index);
		assert(_replicant_array[info->array_index] == info);
		_replicant_zero_update_index--;
		if(info->array_index != _replicant_zero_update_index)
		{
			_replicant_array[_replicant_zero_update_index]->array_index = info->array_index;
			_replicant_array[info->array_index] = _replicant_array[_replicant_zero_update_index];
			_replicant_array[_replicant_zero_update_index] = info;
			info->array_index = _replicant_zero_update_index;
		}
		//assert(validate_replicant_array(), "Invalid replicant array!");
	}
	
	/// Moves the specified replicant_info into the range of the replicant array for free (unused) GhostInfos.
	void replicant_push_zero_to_free(replicant_info *info)
	{
		assert(info->array_index >= _replicant_zero_update_index && info->array_index < _replicant_free_index);
		assert(_replicant_array[info->array_index] == info);
		_replicant_free_index--;
		if(info->array_index != _replicant_free_index)
		{
			_replicant_array[_replicant_free_index]->array_index = info->array_index;
			_replicant_array[info->array_index] = _replicant_array[_replicant_free_index];
			_replicant_array[_replicant_free_index] = info;
			info->array_index = _replicant_free_index;
		}
		//assert(validate_replicant_array(), "Invalid replicant array!");
	}
	
	/// Moves the specified replicant_info from the free area into the range of the replicant array for zero updateMasks.
	inline void replicant_push_free_to_zero(replicant_info *info)
	{
		assert(info->array_index >= _replicant_free_index);
		assert(_replicant_array[info->array_index] == info);
		if(info->array_index != _replicant_free_index)
		{
			_replicant_array[_replicant_free_index]->array_index = info->array_index;
			_replicant_array[info->array_index] = _replicant_array[_replicant_free_index];
			_replicant_array[_replicant_free_index] = info;
			info->array_index = _replicant_free_index;
		}
		_replicant_free_index++;
		//assert(validate_replicant_array(), "Invalid replicant array!");
	}
	
	/// Returns the client-side replicantIndex of the specified server object, or -1 if the object is not available on the client.
	int32 get_replicant_index(net_object *object)
	{
		if(!object)
			return -1;
		if(!does_replicant_from())
			return object->_remote_index;
		int32 index = object->get_hash_id() & replicant_lookup_table_mask;
		
		for(replicant_info *gptr = _replicant_lookup_table[index]; gptr; gptr = gptr->next_lookup_info)
		{
			if(gptr->obj == object && (gptr->flags & (replicant_info::killing_replicant | replicant_info::replicating | replicant_info::not_yet_replicated | replicant_info::kill_replicant)) == 0)
				return gptr->index;
		}
		return -1;
	}
	
	/// Returns true if the object is available on the client.
	bool is_replicant_available(net_object *object) { return get_replicant_index(object) != -1; }
	
	/// Stops replicating objects from this replicant_connection to the remote host, which causes all replicants to be destroyed on the client.
	void reset_replicating()
	{
		if(!does_replicant_from())
			return;
		// stop all replicating activity
		// send a message to the other side notifying of this
		
		_replicating = false;
		_scoping = false;
		call(&replicant_connection::rpc_end_replicating);
		_replicating_sequence++;
		clear_replicant_info();
		//assert(validate_replicant_array(), "Invalid replicant array!");
	}
	
	/// Begins replicating objects from this replicant_connection to the remote host, starting with the GhostAlways objects.
	void activate_replicating()
	{
		if(!does_replicant_from())
			return;
		
		_replicating_sequence++;
		logf(replicant_connection, ("replicating activated - %d", _replicating_sequence));
		
		assert((_replicant_free_index == 0) && (_replicant_zero_update_index == 0));
		
		// iterate through the replicant always objects and in_scope them...
		// also post em all to the other side.
		
		int32 j;
		for(j = 0; j < max_replicant_count; j++)
		{
			_replicant_array[j] = _replicant_refs + j;
			_replicant_array[j]->array_index = j;
		}
		_scoping = true; // so that object_in_scope will work
		
		call(&replicant_connection::rpc_start_replicating, _replicating_sequence);
		//assert(validate_replicant_array(), "Invalid replicant array!");
	}
	
	bool is_replicating() { return _replicating; } ///< Returns true if this connection is currently replicating objects to the remote host.
	
	/// Notifies the replicant_connection that the specified replicant_info should no longer be scoped to the client.
	void detach_object(replicant_info *info)
	{
		// mark it for replicant killin'
		info->flags |= replicant_info::kill_replicant;
		
		// if the mask is in the zero range, we've got to move it up...
		if(!info->update_mask)
		{
			info->update_mask = 0xFFFFFFFF;
			replicant_push_non_zero(info);
		}
		if(info->obj)
		{
			if(info->prev_object_ref)
				info->prev_object_ref->next_object_ref = info->next_object_ref;
			else
				info->obj->_first_object_ref = info->next_object_ref;
			if(info->next_object_ref)
				info->next_object_ref->prev_object_ref = info->prev_object_ref;
			// remove it from the lookup table
			
			uint32 id = info->obj->get_hash_id();
			for(replicant_info **walk = &_replicant_lookup_table[id & replicant_lookup_table_mask]; *walk; walk = &((*walk)->next_lookup_info))
			{
				replicant_info *temp = *walk;
				if(temp == info)
				{
					*walk = temp->next_lookup_info;
					break;
				}
			}
			info->prev_object_ref = info->next_object_ref = NULL;
			info->obj = NULL;
		}
	}
	
	/// RPC from server to client before the GhostAlwaysObjects are transmitted
	void rpc_start_replicating(uint32 sequence)
	{
		logf(replicant_connection, ("Got GhostingStarting %d", sequence));
		
		if(!does_replicant_to())
			throw cradle_exception_illegal_rpc;
		
		on_start_replicating();
		call(&replicant_connection::rpc_ready_for_normal_replicants, sequence);
	}
	
	/// RPC from client to server sent when the client receives the rpcGhostAlwaysActivated
	void rpc_ready_for_normal_replicants(uint32 sequence)
	{
		logf(replicant_connection, ("Got ready for normal replicants %d %d", sequence, _replicating_sequence));
		if(!does_replicant_from())
			throw cradle_exception_illegal_rpc;
		if(sequence != _replicating_sequence)
			return;
		_replicating = true;
	}
	
	/// RPC from server to client sent to notify that replicating should stop	
	void rpc_end_replicating()
	{
		if(!does_replicant_to())
			throw cradle_exception_illegal_rpc;
		delete_local_replicants();
		on_end_replicating();
	}
	void register_rpc_methods()
	{
		register_rpc(&replicant_connection::rpc_end_replicating, rpc_guaranteed_ordered, rpc_bidirectional);
		register_rpc(&replicant_connection::rpc_ready_for_normal_replicants, rpc_guaranteed_ordered, rpc_bidirectional);
		register_rpc(&replicant_connection::rpc_start_replicating, rpc_guaranteed_ordered, rpc_bidirectional);
	}
};

//----------------------------------------------------------------------------

/// Each replicant_info structure tracks the state of a single net_object's replicant for a single replicant_connection.
struct replicant_info
{
	// NOTE:
	// if the size of this structure changes, the NetConnection::get_replicant_index function MUST be changed to reflect.

	net_object *obj; ///< The real object on the server.
	uint32 update_mask; ///< The current out-of-date state mask for the object for this connection.
	replicant_connection::replicant_ref *last_update_chain; ///< The replicant_ref for this object in the last packet it was updated in,
	///   or NULL if that last packet has been notified yet.
	replicant_info *next_object_ref;  ///< Next replicant_info for this object in the doubly linked list of GhostInfos across
	///  all connections that scope this object
	replicant_info *prev_object_ref;  ///< Previous replicant_info for this object in the doubly linked list of GhostInfos across
	///  all connections that scope this object

	replicant_connection *connection; ///< The connection that owns this replicant_info
	replicant_info *next_lookup_info;   ///< Next replicant_info in the hash table for net_object*->replicant_info*
	uint32 update_skip_count;         ///< How many times this object has NOT been updated in write_packet

	uint32 flags; ///< Current flag status of this object for this connection.
	float32 priority; ///< Priority for the update of this object, computed after the scoping process has run.
	uint16 index; ///< Fixed index of the object in the _replicant_refs array for the connection, and the replicantId of the object on the client.
	int16 array_index; ///< Position of the object in the _replicant_array for the connection, which changes as the object is pushed to zero, non-zero and free.
	type_database::type_rep *type_rep; ///< type descriptor for this object

	enum Flags
	{
		in_scope = bit(0),             ///< This replicant_info's net_object is currently in scope for this connection.
		scope_local_always = bit(1),    ///< This replicant_info's net_object is always in scope for this connection.
		not_yet_replicated = bit(2),       ///< This replicant_info's net_object has not been sent to or constructed on the remote host.
		replicating = bit(3),            ///< This replicant_info's net_object has been sent to the client, but the packet it was sent in hasn't been acked yet.
		kill_replicant = bit(4),           ///< The replicant of this replicant_info's net_object should be destroyed ASAP.
		killing_replicant = bit(5),        ///< The replicant of this replicant_info's net_object is in the process of being destroyed.
		
		/// Flag mask - if any of these are set, the object is not yet available for replicant ID lookup.
		not_available = (not_yet_replicated | replicating | kill_replicant | killing_replicant),
	};
};
