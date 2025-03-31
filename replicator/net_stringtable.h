// The replicator library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

/// net_string_table is a helper class to net_connection for reducing duplicated string data sends
class net_string_table
{
public:
   enum {
      entry_count = 1024,
   };

   struct entry;

   struct packet_ref {
      packet_ref *_next_in_packet; ///< The next net_string_table entry updated in the packet this is linked in.
      entry *_table_entry; ///< The net_string_table entry this refers to
      indexed_string _string; ///< The indexed_string that was set in that string
   };

public:
   struct packet_ref_list {
       packet_ref *_head;  ///< The head of the linked list of strings sent in this packet.
      packet_ref *_tail; ///< The tail of the linked list of strings sent in this packet.

       packet_ref_list() { _head = _tail = NULL; }
   };

   /// An entry in the EventConnection's string table
   struct entry {
      indexed_string _string; ///< the string
      uint32 _index;           ///< Index of this entry.
      entry *_next_hash;     ///< The next hash entry for this id.
      entry *_next_link;     ///< The next entry in the LRU list.
      entry *_prev_link;     ///< The prev entry in the LRU list.

      /// Does the other side now have this string?
      bool _receipt_confirmed;
   };

private:
   entry _table[entry_count];
   entry *_hash_table[entry_count];
   indexed_string _remote_strings[entry_count];
   entry _lru_head, _lru_tail;

   net_connection *_connection;

   /// Pushes an entry to the back of the LRU list.
   inline void push_back(entry *the_entry)
   {
       the_entry->_prev_link->_next_link = the_entry->_next_link;
       the_entry->_next_link->_prev_link = the_entry->_prev_link;
       the_entry->_next_link = &_lru_tail;
       the_entry->_prev_link = _lru_tail._prev_link;
       the_entry->_next_link->_prev_link = the_entry;
       the_entry->_prev_link->_next_link = the_entry;
   }
public:
   net_string_table(net_connection *the_connection)
    {
       _connection = the_connection;
       for(uint32 i = 0; i < entry_count; i++)
       {
          _table[i]._next_hash = NULL;
           _table[i]._next_link = &_table[i+1];
          _table[i]._prev_link = &_table[i-1];
           _table[i]._index = i;
          _hash_table[i] = NULL;
       }
       _lru_head._next_link = &_table[0];
       _table[0]._prev_link = &_lru_head;
       _lru_tail._prev_link = &_table[entry_count-1];
       _table[entry_count-1]._next_link = &_lru_tail;
    }


   void write_indexed_string(bit_stream &stream, const indexed_string &the_string)
    {
       // see if the entry is in the hash table right now
       uint32 hash_index = the_string.hash() % entry_count;
       entry *send_entry = NULL;
       for(entry *walk = _hash_table[hash_index]; walk; walk = walk->_next_hash)
       {
          if(walk->_string == the_string)
          {
             // it's in the table
             // first, push it to the back of the LRU list.
             push_back(walk);
             send_entry = walk;
             break;
          }
       }
       if(!send_entry)
       {
          // not in the hash table, means we have to add it
          // pull the new entry from the LRU list.
          send_entry = _lru_head._next_link;

          // push it to the end of the LRU list
          push_back(send_entry);

          // remove the string from the hash table
          entry **hash_walk;
          for (hash_walk = &_hash_table[send_entry->_string.hash() % entry_count]; *hash_walk; hash_walk = &((*hash_walk)->_next_hash))
          {
             if(*hash_walk == send_entry)
             {
                *hash_walk = send_entry->_next_hash;
                break;
             }
          }
          
          send_entry->_string = the_string;
          send_entry->_receipt_confirmed = false;
          send_entry->_next_hash = _hash_table[hash_index];
          _hash_table[hash_index] = send_entry;
       }
       
       stream.write_ranged_uint32(send_entry->_index, 0, entry_count - 1);
       if(!stream.write_bool(send_entry->_receipt_confirmed))
       {
           const char *string_data = send_entry->_string.c_str();
           stream.write_c_str(string_data, indexed_string::max_net_indexed_string_len);
           
           packet_ref *ref = new packet_ref;
           ref->_string = send_entry->_string;
           ref->_next_in_packet = NULL;
           ref->_table_entry = send_entry;

           net_connection::packet_notify *notify = _connection->get_current_write_packet_notify();
           packet_ref_list *strings = &notify->strings;

          if(!strings->_head)
              strings->_head = ref;
          else
              strings->_tail->_next_in_packet = ref;
           strings->_tail = ref;
       }
   }
    
   void read_indexed_string(bit_stream &stream, indexed_string &the_string)
    {
       uint32 index = stream.read_ranged_uint32(0, entry_count - 1);
       if(!stream.read_bool())
       {
           char buffer[indexed_string::max_net_indexed_string_len + 1];
           stream.read_c_str(buffer, indexed_string::max_net_indexed_string_len);
           context *current = context::get_current();
           _remote_strings[index] = indexed_string(current->get_string_table(), buffer);
       }
       the_string = _remote_strings[index];
    }

   void packet_received(packet_ref_list *note)
    {
       packet_ref *walk = note->_head;
       while(walk)
       {
          packet_ref *next = walk->_next_in_packet;
          if(walk->_table_entry->_string == walk->_string)
             walk->_table_entry->_receipt_confirmed = true;
          delete walk;
          walk = next;
       }
   }
   void packet_dropped(packet_ref_list *note)
    {
       packet_ref *walk = note->_head;
       while(walk)
       {
          packet_ref *next = walk->_next_in_packet;
           delete walk;
          walk = next;
       }
   }
};
