// The core library - copyright KAGR LLC, portions copyright GarageGames. The use of this source code is governed by the MIT software license described in the "license.txt" file in this directory.

class byte_buffer : public ref_object
{
protected:
	/// Pointer to our data buffer.
	uint8 *_data_ptr;
	
	/// Length of buffer.
	uint32  _buf_size;
	
public:
	enum {
		default_buffer_size = 1500,
	};
	byte_buffer(const byte_buffer &copy_buf)
    {
        _buf_size = copy_buf._buf_size;
        _data_ptr = (uint8 *) mem_alloc(_buf_size);
        memcpy(_data_ptr, copy_buf._data_ptr, _buf_size);
    }
	byte_buffer(const char *string)
	{
		_buf_size = strlen(string) + 1;
		_data_ptr = (uint8 *) mem_alloc(_buf_size);
		memcpy(_data_ptr, (const uint8 *) string, _buf_size);
	}
	
	/// Create a byte_buffer from a chunk of memory.
	byte_buffer(const uint8 *data_ptr, uint32 buffer_size)
	{
		_buf_size = buffer_size;
		_data_ptr = (uint8 *) mem_alloc(buffer_size);
		memcpy(_data_ptr, data_ptr, buffer_size);
	}
	
	/// Create a byte_buffer of the specified size.
	byte_buffer(uint32 buffer_size = default_buffer_size)
	{
		_buf_size = buffer_size;
		_data_ptr = (uint8 *) mem_alloc(buffer_size);
	}
	
	~byte_buffer()
	{
		mem_free(_data_ptr);
	}
	
	/// Set the byte_buffer to point to a new chunk of memory.
	void set_buffer(uint8 *data_ptr, uint32 buffer_size)
	{
		mem_free(_data_ptr);
		_data_ptr = (uint8 *) mem_alloc(buffer_size);
		_buf_size = buffer_size;
		memcpy(_data_ptr, data_ptr, buffer_size);
	}
	
	/// Attempts to resize the buffer.
	///
	void resize(uint32 new_buffer_size)
	{
		if(_buf_size >= new_buffer_size)
			_buf_size = new_buffer_size;
		else
		{
			_buf_size = new_buffer_size;
			_data_ptr = (uint8 *) mem_realloc(_data_ptr, new_buffer_size);
		}
	}
	
	/// Appends the specified buffer to the end of the byte buffer.
	void append_buffer(const uint8 *data_buffer, uint32 buffer_size)
	{
		uint32 start = _buf_size;
		resize(_buf_size + buffer_size);
		memcpy(_data_ptr + start, data_buffer, buffer_size);
	}
	
	/// Appends the specified byte_buffer to the end of this byte buffer.
	void append_buffer(const byte_buffer &the_buffer)
	{
		return append_buffer(the_buffer.get_buffer(), the_buffer.get_buffer_size());
	}
	
	bool is_equal(const byte_buffer &the_buffer)
	{
		if(the_buffer._buf_size != _buf_size)
			return false;
		return !memcmp(the_buffer._data_ptr, _data_ptr, _buf_size);
	}
	
	uint32 get_buffer_size() const
	{
		return _buf_size;
	}
	
	uint8 *get_buffer()
	{
		return _data_ptr;
	}
	
	const uint8 *get_buffer() const
	{
		return _data_ptr;
	}
	
	/// Clear the buffer.
	void clear()
	{
		memset(_data_ptr, 0, _buf_size);
	}
};

typedef ref_ptr<byte_buffer> byte_buffer_ptr;
template <class stream> static inline bool write(stream &the_stream, const byte_buffer_ptr &buffer)
{
	uint32 len = buffer->get_buffer_size();
	write(the_stream, len);
	return the_stream.write_bytes(buffer->get_buffer(), len);
}

template <class stream> static inline bool read(stream &the_stream, byte_buffer_ptr &buffer)
{
	uint32 len;
	read(the_stream, len);
	buffer = new byte_buffer(len);
	return the_stream.read_bytes(buffer->get_buffer(), len);
}
