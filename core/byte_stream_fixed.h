// The core library - copyright KAGR LLC, portions copyright GarageGames. The use of this source code is governed by the MIT software license described in the "license.txt" file in this directory.

class byte_stream_fixed
{
public:
	uint32 _position;
	uint32 _buffer_size;
	byte *_buffer;

	byte_stream_fixed(byte *buffer = 0, uint32 buffer_size = 0)
	{
		set_buffer(buffer, buffer_size);
	}

	void set_position(uint32 byte_pos)
	{
		_position = byte_pos;
		if(_position > _buffer_size)
			_position = _buffer_size;
	}

	const uint32 get_position() const
	{
		return _position;
	}
	
	const uint32 get_buffer_size() const
	{
		return _buffer_size;
	}
	
	byte *get_buffer()
	{
		return _buffer;
	}
	
	const byte *get_buffer() const
	{
		return _buffer;
	}
	
	void set_buffer(byte *buffer, uint32 buffer_size)
	{
		_buffer = buffer;
		_buffer_size = buffer_size;
		_position = 0;
	}

	uint32 read_bytes(byte *dest_ptr, uint32 read_byte_count)
	{
		if(read_byte_count + _position > _buffer_size)
			read_byte_count = _buffer_size - _position;

		memcpy(dest_ptr, _buffer + _position, read_byte_count);
		_position += read_byte_count;
		return read_byte_count;
	}

	uint32 write_bytes(const byte *src_ptr, uint32 write_byte_count)
	{
		if(write_byte_count + _position > _buffer_size)
			write_byte_count = _buffer_size - _position;

		memcpy(_buffer + _position, src_ptr, write_byte_count);
		_position += write_byte_count;
		return write_byte_count;
	}
};
