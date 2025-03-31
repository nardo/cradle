// The notify_sockets library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

// socket_event_queue.h - A queue for net_socket_event records.
// Copyright GarageGames.  notify_sockets API and prototype implementation are released under the MIT license.  See /license/info.txt in this distribution for specific details.

class socket_event_queue
{
public:
	struct queue_entry {
		net_socket_event *event;
		queue_entry *next_event;
	};
	
	queue_entry *_event_queue_tail;
	queue_entry *_event_queue_head;
	arena_allocator<16> _allocator;
	
	socket_event_queue(page_allocator *allocator) : _allocator(allocator)
	{
		_event_queue_head = 0;
		_event_queue_tail = 0;
	}
	
	bool has_event()
	{
		return _event_queue_head != 0;
	}
	void clear()
	{
		_allocator.clear();
	}
	
	net_socket_event *dequeue()
	{
		assert(_event_queue_head);
		net_socket_event *ret = _event_queue_head->event;
		_event_queue_head = _event_queue_head->next_event;
		if(!_event_queue_head)
			_event_queue_tail = 0;
		return ret;
	}
	
	uint8 *allocate_queue_data(uint32 data_size)
	{
		return (uint8 *) _allocator.allocate(data_size);
	}
	
	net_socket_event *post_event(uint32 event_type, const address &source_address, notify_connection_id connection_id = 0)
	{
		net_socket_event *ret = (net_socket_event *) _allocator.allocate(sizeof(net_socket_event));
		queue_entry *entry = (queue_entry *) _allocator.allocate(sizeof(queue_entry));
		
		entry->event = ret;
		
		ret->event_type = event_type;
		ret->data = 0;
		ret->key = 0;
		ret->data_size = 0;
		ret->key_size = 0;
		ret->connection = connection_id;
        ret->source_address = source_address;

		entry->next_event = 0;
		if(_event_queue_tail)
		{
			_event_queue_tail->next_event = entry;
			_event_queue_tail = entry;
		}
		else
			_event_queue_head = _event_queue_tail = entry;
		return entry->event;
	}
	
	void set_event_data(net_socket_event *event, uint8 *data, uint32 data_size)
	{
		event->data_size = data_size;
		event->data = allocate_queue_data(data_size);
		memcpy(event->data, data, data_size);
	}
	
	void set_event_key(net_socket_event *event, uint8 *key, uint32 key_size)
	{
		event->key_size = key_size;
		event->key = allocate_queue_data(key_size);
		memcpy(event->key, key, key_size);
	}
};

