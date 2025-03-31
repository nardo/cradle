// The replicator library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

// bitstream i/o functions for key networkable types: indexed strings and safe_ptrs of net_object and its subclasses

static inline bool write(bit_stream &the_stream, const indexed_string &value)
{
    net_connection *conn = the_stream.get_connection();
    net_string_table *table = 0;
    if(conn && (table = conn->get_string_table()) != NULL)
        table->write_indexed_string(the_stream, value);
    else
        the_stream.write_c_str(value.c_str(), indexed_string::max_net_indexed_string_len);
    return true;
}

static inline bool read(bit_stream &the_stream, indexed_string &value)
{
    net_connection *conn = the_stream.get_connection();
    net_string_table *table = 0;
    if(conn && (table = conn->get_string_table()) != NULL)
        table->read_indexed_string(the_stream, value);
    else
    {
        char buffer[indexed_string::max_net_indexed_string_len+1];
        the_stream.read_c_str(buffer, indexed_string::max_net_indexed_string_len);
        context *current = context::get_current();
        assert(current);
        value = indexed_string(current->get_string_table(), buffer);
    }
    return true;
}

template<class stream, class type> static inline bool write(stream &the_stream, const safe_ptr<type> &value)
{
    replicant_connection *gc = (replicant_connection *) the_stream.get_connection();
    if(!value.is_valid())
    {
        the_stream.write_bool(false);
        return true;
    }
    else
    {
        int32 index = gc->get_replicant_index(value);
        if(the_stream.write_bool(index != -1))
        {
            the_stream.write_integer(index, replicant_connection::replicant_id_bit_size);
            return true;
        }
        else
            return false;
    }
}

template<class stream, class type> static inline bool read(stream &the_stream, safe_ptr<type> &value)
{
    replicant_connection *gc = (replicant_connection *) the_stream.get_connection();
    if(!the_stream.read_bool())
    {
        value = 0;
        return true;
    }
    else
    {
        int32 index = the_stream.read_integer(replicant_connection::replicant_id_bit_size);
        net_object *local_replicant = gc->resolve_replicant(index);
        if(!local_replicant)
        {
            the_stream.raise_error();
            return false;
        }
        value = dynamic_cast<type *>(local_replicant);
        if(value.is_null())
        {
            the_stream.raise_error();
            return false;
        }
        return true;
    }
}

