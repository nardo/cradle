// The replicator library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

class net_connection;
class net_object;

class net_interface : public ref_object
{
	friend class net_object;
	
	typedef hash_table_array<notify_connection_id, ref_ptr<net_connection> >::pointer connection_pointer;
	struct connection_type_record
	{
		uint32 identifier;
		type_record *type;
	};
	
public:
	void set_key_pair(asymmetric_key_ptr the_key)
	{
        _socket->set_private_key(the_key);
//		byte_buffer_ptr key_buffer = the_key->get_private_key();
//		get_socket_interface()->set_key_pair(_socket, key_buffer->get_buffer_size(), key_buffer->get_buffer());
		
		//byte_buffer_ptr private_key = the_key->get_private_key();
		//net_socket_set_private_key(_socket, private_key->get_buffer_size(), private_key->get_buffer());
	}
	
	void set_challenge_response_data(byte_buffer_ptr data)
	{
		_socket->set_challenge_response(data);
	}
	
	void set_allows_connections(bool allow)
	{
		_socket->set_allows_connections(allow);
	}
    
    void ban_host(const address &remote_host)
    {
        _socket->ban_host(remote_host);
    }
    void unban_host(const address &remote_host)
    {
        _socket->unban_host(remote_host);
    }

	void process_socket()
	{
		net_socket_event *event;
		while((event = _socket->get_next_event()) != NULL)
		{
            logf(net_interface, ("Processing event of type %d, connection_index = %d, size = %d", event->event_type, event->connection, event->data_size));
			switch(event->event_type)
			{
					
				case notify_connection_challenge_response_event_type:
					_process_challenge_response(event);
					break;
				case notify_connection_requested_event_type:
					_process_connection_requested(event);
					break;
				case notify_connection_accepted_event_type:
					_process_connection_accepted(event);
					break;
				case notify_connection_timed_out_event_type:
					_process_connection_timed_out(event);
					break;
				case notify_connection_disconnected_event_type:
					_process_connection_disconnected(event);
					break;
				case notify_connection_established_event_type:
					_process_connection_established(event);
					break;
				case notify_connection_packet_event_type:
					_process_connection_packet(event);
					break;
				case notify_connection_packet_notify_event_type:
					_process_connection_packet_notify(event);
					break;
				case net_socket_packet_event_type:
					_process_socket_packet(event);
					break;
			}
		}
	}
	template<class connection_type> void add_connection_type(uint32 identifier)
	{
		type_record *the_type_record = get_global_type_record<connection_type>();
		for(uint32 i = 0; i < _connection_class_table.size(); i++)
		{
			assert(_connection_class_table[i].identifier != identifier);
			assert(_connection_class_table[i].type != the_type_record);
		}
		
		connection_type_record rec;
		rec.identifier = identifier;
		rec.type = the_type_record;
		
		_connection_class_table.push_back(rec);
	}
	
	type_record *find_connection_type(uint32 type_identifier)
	{
		for(uint32 i = 0; i < _connection_class_table.size(); i++)
			if(_connection_class_table[i].identifier == type_identifier)
				return _connection_class_table[i].type;
		return 0;
	}
	
	uint32 get_type_identifier(net_connection *connection)
	{
		type_record *rec = connection->get_type_record();
		for(uint32 i = 0; i < _connection_class_table.size(); i++)
			if(_connection_class_table[i].type == rec)
				return _connection_class_table[i].identifier;
		assert(false);
		return 0;
	}
	
	void collapse_dirty_list()
	{
		for(net_object *obj = _dirty_list_head._next_dirty_list; obj != &_dirty_list_tail; )
		{
			net_object *next = obj->_next_dirty_list;
			uint32 or_mask = obj->_dirty_mask_bits;
			
			obj->_next_dirty_list = NULL;
			obj->_prev_dirty_list = NULL;
			obj->_dirty_mask_bits = 0;
			
			if(or_mask)
			{
				for(replicant_info *walk = obj->_first_object_ref; walk; walk = walk->next_object_ref)
				{
					if(!walk->update_mask)
					{
						walk->update_mask = or_mask;
						walk->connection->replicant_push_non_zero(walk);
					}
					else
						walk->update_mask |= or_mask;
				}
			}
			obj = next;
		}
		_dirty_list_head._next_dirty_list = &_dirty_list_tail;
		_dirty_list_tail._prev_dirty_list = &_dirty_list_head;
	}
	void add_to_dirty_list(net_object *obj)
	{
		assert(obj->_next_dirty_list == 0);
		assert(obj->_prev_dirty_list == 0);
		obj->_next_dirty_list = _dirty_list_head._next_dirty_list;
		obj->_next_dirty_list->_prev_dirty_list = obj;
		_dirty_list_head._next_dirty_list = obj;
		obj->_prev_dirty_list = &_dirty_list_head;
	}
	
	time get_process_start_time()
	{
		return _process_start_time;
	}
	void check_for_packet_sends()
	{
		_process_start_time = time::get_current();
		collapse_dirty_list();
		for(uint32 i = 0; i < _connection_table.size(); i++)
		{
			net_connection *the_connection = *_connection_table[i].value();
			if(the_connection->get_connection_state() == net_connection::state_established)
				the_connection->check_packet_send(false, get_process_start_time());
		}
	}
		
	void _add_connection(net_connection *the_net_connection, notify_connection_id the_notify_connection)
	{
        ref_ptr<net_connection> nc(the_net_connection);
        _connection_table.insert(the_notify_connection, nc);
		the_net_connection->set_notify_connection(the_notify_connection);
	}
	
	void _process_challenge_response(net_socket_event *event)
	{
		bit_stream challenge_response(event->data, event->data_size);
		byte_buffer_ptr public_key = new byte_buffer(event->key, event->key_size);
		connection_pointer p = _connection_table.find(event->connection);
		ref_ptr<net_connection> *the_connection = p.value();
        logf(net_interface, ("Got a challenge response -- %d", bool(p)));
		if(the_connection)
		{
			(*the_connection)->set_connection_state(net_connection::state_requesting_connection);
			(*the_connection)->on_challenge_response(challenge_response, public_key);
			_socket->accept_connection_challenge(event->connection);
		}
	}
		
	void _process_connection_requested(net_socket_event *event)
	{
		bit_stream key_stream(event->key, event->key_size);
		bit_stream request_stream(event->data, event->data_size);
		uint8 response_buffer[net_sockets_max_status_datagram_size];
		bit_stream response_stream(response_buffer, net_sockets_max_status_datagram_size);
		
        logf(net_interface, ("got connect request\n%s\n%s", buffer_encode_base_16(event->key, event->key_size)->get_buffer(), buffer_encode_base_16(event->data, event->data_size)->get_buffer()));
		
		uint32 type_identifier;
		read(request_stream, type_identifier);
		type_record *rec = find_connection_type(type_identifier);
		if(!rec)
		{
			_socket->disconnect(event->connection, 0, 0);
			return;
		}
		net_connection *allocated = (net_connection *) mem_alloc(rec->size);
		rec->construct_object(allocated);
		ref_ptr<net_connection> the_connection = allocated;
		the_connection->set_interface(this);
        address source_address(event->source_address);
		if(the_connection->read_connect_request(source_address, request_stream, response_stream))
		{
			_add_connection(the_connection, event->connection);
			_socket->accept_connection(event->connection);
		}
		else
			_socket->disconnect(event->connection, response_stream.get_buffer(), response_stream.get_next_byte_position());
	}
	
	void _process_connection_accepted(net_socket_event *event)
	{
		packet_stream response_stream;

		connection_pointer p = _connection_table.find(event->connection);

		ref_ptr<net_connection> *the_connection = p.value();
		if(the_connection)
		{
			(*the_connection)->set_connection_state(net_connection::state_accepted);
		}
	}
	
	void _process_connection_rejected(net_socket_event *event)
	{
		bit_stream connection_rejected_stream(event->data, event->data_size);
		connection_pointer p = _connection_table.find(event->connection);
		
		ref_ptr<net_connection> *the_connection = p.value();
		if(the_connection)
		{
			(*the_connection)->set_connection_state(net_connection::state_rejected);
			(*the_connection)->on_connection_rejected(connection_rejected_stream);
			p.remove();
		}
	}
	
	void _process_connection_timed_out(net_socket_event *event)
	{
		connection_pointer p = _connection_table.find(event->connection);
		
		ref_ptr<net_connection> *the_connection = p.value();
		if(the_connection)
		{
			(*the_connection)->set_connection_state(net_connection::state_timed_out);
			(*the_connection)->on_connection_timed_out();
			p.remove();
		}
	}
	
	void _process_connection_disconnected(net_socket_event *event)
	{
		byte_buffer_ptr disconnect_buffer = new byte_buffer(event->data, event->data_size);
		connection_pointer p = _connection_table.find(event->connection);
		
		ref_ptr<net_connection> *the_connection = p.value();
		if(the_connection)
		{
			(*the_connection)->set_connection_state(net_connection::state_disconnected);
			(*the_connection)->on_connection_disconnected(disconnect_buffer);
			p.remove();
		}
	}
	
	void _process_connection_established(net_socket_event *event)
	{
		ref_ptr<net_connection> *the_connection = _connection_table.find(event->connection).value();
		if(the_connection)
		{
            net_connection *c = *the_connection;
			c->set_connection_state(net_connection::state_established);
			c->on_connection_established();
            float32 packet_loss;
            uint32 latency;
            c->get_simulated_net_params(packet_loss, latency);
            get_socket()->set_simulated_net_params(c->get_notify_connection(), packet_loss, latency);

		}
	}
    
	void _process_connection_packet(net_socket_event *event)
	{
		ref_ptr<net_connection> *the_connection = _connection_table.find(event->connection).value();
		if(the_connection)
		{
			bit_stream packet(event->data, event->data_size);
			(*the_connection)->on_packet(event->packet_sequence, packet);
		}
	}
	
	void _process_connection_packet_notify(net_socket_event *event)
	{
		ref_ptr<net_connection> *the_connection = _connection_table.find(event->connection).value();
		if(the_connection)
			(*the_connection)->on_packet_notify(event->packet_sequence, event->delivered);
	}
	
    virtual void process_socket_packet(const address &remote_address, bit_stream &stream)
    {
    }
    
    void send_socket_packet(const address &remote_address, bit_stream &stream)
    {
        _socket->send_to(remote_address, stream.get_next_byte_position(), stream.get_buffer());
    }
	void _process_socket_packet(net_socket_event *event)
	{
        address remote_address(event->source_address);
        bit_stream stream(event->data, event->data_size);
        process_socket_packet(event->source_address, stream);
	}
	
	void connect(const address &remote_host, net_connection *the_connection)
	{
		uint8 connect_buffer[net_sockets_max_status_datagram_size];
		bit_stream connect_stream(connect_buffer, sizeof(connect_buffer));
		
		uint32 type_identifier = get_type_identifier(the_connection);
		write(connect_stream, type_identifier);
		the_connection->set_interface(this);
        the_connection->set_introduced(false);

		the_connection->write_connect_request(connect_stream);

		sockaddr the_sockaddr;
		remote_host.to_sockaddr(&the_sockaddr);
		notify_connection_id connection_id = _socket->connect(remote_host, connect_stream.get_buffer(), connect_stream.get_next_byte_position());
		
        logf(net_interface, ("sent connect request\n%s", buffer_encode_base_16(connect_stream.get_buffer(), connect_stream.get_next_byte_position())->get_buffer()));

        logf(net_interface, ("opened connection id = %d", connection_id));
		_add_connection(the_connection, connection_id);
	}
    
    void introduce(net_connection *initiator, net_connection *host)
    {
        _socket->introduce_connection(initiator->get_notify_connection(), host->get_notify_connection());
    }
    
    void connect_introduced(net_connection *introducer, notify_connection_id remote_host, net_connection *the_connection)
    {
        uint8 connect_buffer[net_sockets_max_status_datagram_size];
        bit_stream connect_stream(connect_buffer, sizeof(connect_buffer));
        
        the_connection->set_interface(this);
        the_connection->set_introduced(true);
        
        uint32 type_identifier = get_type_identifier(the_connection);
        write(connect_stream, type_identifier);
        the_connection->write_connect_request(connect_stream);

        notify_connection_id connection_id = _socket->connect_introduced( introducer->get_notify_connection(), remote_host, connect_stream.get_next_byte_position(), connect_stream.get_buffer());
        logf(net_interface, ("connect introduced %d via %d \n%s", remote_host, introducer->get_notify_connection(), buffer_encode_base_16(connect_stream.get_buffer(), connect_stream.get_next_byte_position())->get_buffer()));

        logf(net_interface, ("introduced connection id = %d", connection_id));
        _add_connection(the_connection, connection_id);
    }
    
    void accept_introduction(net_connection *introducer, notify_connection_id remote_initiator)
    {
        notify_connection_id connection_id = _socket->accept_introduction( introducer->get_notify_connection(), remote_initiator);
        logf(net_interface, ("accept intro %d via %d = %d", remote_initiator, introducer->get_notify_connection(), connection_id));
    }
    
    void disconnect(net_connection *the_connection, byte_buffer_ptr &disconnect_message)
    {
        notify_connection_id connection_id = the_connection->get_notify_connection();
        connection_pointer p = _connection_table.find(connection_id);
        ref_ptr<net_connection> *connection_ref = p.value();
        assert(connection_ref && *connection_ref == the_connection);
        
        the_connection->set_connection_state(net_connection::state_disconnected);
        _socket->disconnect(connection_id, disconnect_message->get_buffer(), disconnect_message->get_buffer_size());
        p.remove();
    }

	virtual ~net_interface()
	{
		collapse_dirty_list();
		_dirty_list_head._next_dirty_list = 0;
	}
	
	ref_ptr<notify_socket> &get_socket()
	{
		return _socket;
	}
	
    net_connection *find_connection(notify_connection_id connection_id)
    {
        ref_ptr<net_connection> *the_connection = _connection_table.find(connection_id).value();
        if(the_connection)
            return *the_connection;
        return NULL;
    }
    
	bind_result bind(const address &bind_address)
	{
		return _socket->bind(bind_address);
	}
    
	net_interface()
	{
		_socket = new notify_socket(false, 0, 0);

		_dirty_list_head._next_dirty_list = &_dirty_list_tail;
		_dirty_list_tail._prev_dirty_list = &_dirty_list_head;
		_dirty_list_head._prev_dirty_list = 0;
		_dirty_list_tail._next_dirty_list = 0;
	}
protected:
	ref_ptr<notify_socket> _socket;
	time _process_start_time;
	net_object _dirty_list_head;
	net_object _dirty_list_tail;	
	array<connection_type_record> _connection_class_table;
	hash_table_array<notify_connection_id, ref_ptr<net_connection> > _connection_table;
};

