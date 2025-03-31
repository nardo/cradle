// The core library - copyright KAGR LLC, portions copyright GarageGames. The use of this source code is governed by the MIT software license described in the "license.txt" file in this directory.

/// stream is a virtual base class for managing bit-level streaming of data.  Subclasses are responsible for refilling the _start and _end
///
class net_connection;

class bit_stream
{
	public:
	typedef uint64 bit_position;
    enum
    {
        default_max_c_str_len = 255,
    };

    bit_stream(byte *buffer = 0, uint32 buffer_size = 0) { _init(buffer, 0, buffer_size * 8); _connection = 0; }
    bit_stream(byte_buffer &b) { _init(b.get_buffer(), 0, b.get_buffer_size() * 8); _connection = 0; }
	virtual ~bit_stream() {}
	
	void set_buffer(byte *buffer, uint32 bit_start, uint32 bit_end) { _init(buffer, bit_start, bit_end); }
	byte *get_buffer() { return _base_ptr; }	
	bool is_dirty() {return _is_dirty;}
	void raise_error() { _error_detected = true; }
	bool was_error_detected() { return _error_detected; }
	
	bit_position get_bit_position() { return _bit_num; }
	void set_bit_position(bit_position pos) { _bit_num = uint32(pos); }
	bit_position get_stream_bit_size() { return _bit_end; }
	bit_position get_bit_space_available() {
		return get_stream_bit_size() - get_bit_position();
	}
    void set_connection(net_connection *c) { _connection = c; }
    net_connection *get_connection() { return _connection; }

	virtual bool is_full()
	{
		return _bit_num == _bit_end;
	}
	
	void advance_to_next_byte() {
		_bit_num = ((_bit_num + 7) >> 3) << 3;
	}
    
    void zero_to_byte_boundary() {
        if(_bit_num & 0x7)
            write_integer(0, 8 - (_bit_num & 0x7));
    }
	virtual uint32 get_next_byte_position() { return (_bit_num + 7) >> 3; }
	virtual uint32 get_byte_position() { return _bit_num >> 3; }
	virtual void set_byte_position(uint32 byte_pos) { _bit_num = byte_pos << 3; }
	virtual uint32 get_stream_byte_size() { return (_bit_end + 7) >> 3; }
	void set_stream_byte_size(uint32 byte_size) { _bit_end = byte_size * 8; }
    void set_stream_bit_size(uint32 bit_size) { _bit_end = bit_size; }
	
	static void copy_bits(byte *dest_ptr, uint32 dest_bit, const byte *source_ptr, uint32 source_bit, uint32 bit_count) { _copy_bits(dest_ptr, dest_bit, source_ptr, source_bit, bit_count); }

	uint32 write_bits(const byte *source_ptr, uint32 source_bit_num, uint32 write_bit_count) { return _write_bits(source_ptr, source_bit_num, write_bit_count); }
	uint32 read_bits(byte *dest_ptr, uint32 dest_bit_num, uint32 read_bit_count) { return _read_bits(dest_ptr, dest_bit_num, read_bit_count); }
	
	uint32 write_bytes(const byte *source_ptr, uint32 write_byte_count) { return _write_bits(source_ptr, 0, write_byte_count << 3) >> 3; }
	uint32 read_bytes(byte *dest_ptr, uint32 read_byte_count) { return _read_bits(dest_ptr, 0, read_byte_count << 3) >> 3; }
	
	bool write_bool(bool value)
	{
		uint8 bool_val = value;
		_write_bits(&bool_val, 0, 1);
		return value;
	}
	
	bool read_bool()
	{
		uint8 dest = 0;
		_read_bits(&dest, 0, 1);
		return bool(dest);
	}
	
    void write_c_str(const char *c_str, uint32 max_string_len = default_max_c_str_len)
    {
        if(!c_str)
            c_str = "";
        // for now, just write out the bytes...
        // we'll add the huffman coder and the substring matcher
        // later...
        uint32 l = 0;
        while((l < max_string_len) && c_str[l])
            l++;
        write_ranged_uint32(l, 0, max_string_len);
        write_bytes((const uint8 *) c_str, l);
    }
    
    void read_c_str(char *c_str, uint32 max_string_len = default_max_c_str_len)
    {
        // c_str must point to a buffer at least max_string_len+1 bytes in size
        uint32 l = read_ranged_uint32(0, max_string_len);
        read_bytes((uint8 *) c_str, l);
        c_str[l] = 0;
    }
    
	uint32 read_integer(uint32 bit_width) { return _read_integer(bit_width); }
	void write_integer(uint32 value, uint32 bit_width) { _write_integer(value, bit_width); }

	/// Writes an unsigned integer value in the range range_start to range_end inclusive.
	void write_ranged_uint32(uint32 value, uint32 range_start, uint32 range_end)
	{
		assert(value >= range_start && value <= range_end);
		assert(range_end >= range_start);
		
		uint32 range_size = range_end - range_start + 1;
		uint32 range_bits = get_next_binary_log(range_size);
		
		write_integer(value - range_start, range_bits);
	}

	/// Reads an unsigned integer value in the range range_start to range_end inclusive.
	uint32  read_ranged_uint32(uint32 range_start, uint32 range_end)
	{
		assert(range_end >= range_start);
		
		uint32 range_size = range_end - range_start + 1;
		uint32 range_bits = get_next_binary_log(range_size);
		
		uint32 val = read_integer(range_bits);
		return val + range_start;
	}
	void write_unit_float(float32 value, uint32 precision)
    {
        write_integer(uint32(value * ((1 << precision) - 1)), precision);
    }
    
    float32 read_unit_float(uint32 precision)
    {
        return read_integer(precision) / float32((1 << precision) - 1);
    }
    
    void write_signed_unit_float(float32 value, uint32 precision)
    {
        write_integer(uint32(( (value + 1)*.5 ) * ((1 << precision) - 1) ), precision);
    }
    
    float32 read_signed_unit_float(uint32 precision)
    {
        return (read_integer(precision) / float32((1 << precision) - 1)) * 2.0f - 1.0f;
    }

	virtual bool more(uint32 /*bits_requested*/) { return false; }
	
private:
    void _init(byte *buffer, uint32 bit_start, uint32 bit_end)
	{
		_base_ptr = buffer;
		_bit_num = bit_start;
		_bit_end = bit_end;
		_is_dirty = false;
		_error_detected = false;
	}
	
	static void _copy_bits(byte *dest_ptr, uint32 dest_bit, const byte *source_ptr, uint32 source_bit, uint32 bit_count)
	{
		assert(bit_count != 0); // this should be checked by any calling code
		dest_ptr += dest_bit >> 3;
		source_ptr += source_bit >> 3;
		dest_bit &= 7;
		source_bit &= 7;
		uint32 last_write_bit = dest_bit + bit_count - 1;
		byte *last_write_byte = dest_ptr + (last_write_bit >> 3);
		
		// check if this is byte aligned first:
		if(source_bit == dest_bit)
		{
			// preload the first byte:
			byte mask = (1 << source_bit) - 1;
			byte buffer = (*dest_ptr & mask) | (*source_ptr++ & ~mask);
			
			while(dest_ptr != last_write_byte)
			{
				*dest_ptr++ = buffer;
				buffer = *source_ptr++;
			}
			mask = uint8((1 << ((last_write_bit & 7) + 1)) - 1);
			*dest_ptr = (buffer & mask) | (*dest_ptr & ~mask);
		}
		else
		{
			uint32 slider = *source_ptr++;
			uint32 up_shift;
			if(source_bit < dest_bit)
			{
				up_shift = dest_bit - source_bit;
				if(dest_bit + bit_count <= 8)
				{
					byte mask = ((1 << bit_count) - 1) << dest_bit;
					 *dest_ptr = (*dest_ptr & ~mask) | (uint8(slider << up_shift) & mask);
					return;
				}
				// otherwise, lets spit out the first dest byte:
				byte mask = (1 << dest_bit) - 1;
				*dest_ptr = (*dest_ptr & mask) | ((slider << up_shift) & ~mask);
				dest_ptr++;
				slider >>= 8 - up_shift;
			}
			else // source_bit > dest_bit
			{
				uint32 down_shift = source_bit - dest_bit;
				byte mask = (1 << dest_bit) - 1;
				slider = (*dest_ptr & mask) | ((slider >> down_shift) & ~mask);
				up_shift = 8 - down_shift;
			}
			while(dest_ptr != last_write_byte)
			{
				slider |= uint32(*source_ptr++) << up_shift;
				*dest_ptr++ = uint8(slider);
				slider >>= 8;
			}
			uint32 last_write_bit_in_byte = last_write_bit & 7;
			// see if we need to read one more source byte:
			if(last_write_bit_in_byte >= up_shift)
				slider |= uint32(*source_ptr) << up_shift;
			byte mask = uint8((1 << (last_write_bit_in_byte + 1)) - 1);
			*dest_ptr = (slider & mask) | (*dest_ptr & ~mask);
		}
	}
	
	uint32 _write_bits(const byte *source_ptr, uint32 source_bit_num, uint32 write_bit_count)
	{
		if(!write_bit_count)
			return 0;
		
		uint32 bits_written = 0;
		_is_dirty = true;
		
		for(;;)
		{
			uint32 bits_remaining = _bit_end - _bit_num;
			if(!bits_remaining)
			{
				if(!more(write_bit_count))
				{
					_error_detected = true;
					return bits_written;
				}
				else
					bits_remaining = _bit_end - _bit_num;
			}
			
			if(write_bit_count <= bits_remaining)
			{
				copy_bits(_base_ptr, _bit_num, source_ptr, source_bit_num, write_bit_count);
				_bit_num += write_bit_count;
				return bits_written + write_bit_count;
			}
			else
			{
				copy_bits(_base_ptr,_bit_num, source_ptr, source_bit_num, bits_remaining);
				_bit_num += bits_remaining;
				source_bit_num += bits_remaining;
				bits_written += bits_remaining;
				write_bit_count -= bits_remaining;
			}
		}
	}
	
	uint32 _read_bits(byte *dest_ptr, uint32 dest_bit_num, uint32 read_bit_count)
	{
		if(!read_bit_count)
			return 0;
		
		uint32 bits_read = 0;
		for(;;)
		{
			uint32 bits_remaining = _bit_end - _bit_num;
			if(!bits_remaining)
			{
				if(!more(read_bit_count))
				{
					_error_detected = true;
					return bits_read;
				}
				else
					bits_remaining = _bit_end - _bit_num;
			}
			
			if(read_bit_count <= bits_remaining)
			{
				copy_bits(dest_ptr, dest_bit_num, (const byte *) _base_ptr, _bit_num, read_bit_count);
				_bit_num += read_bit_count;
				return bits_read + read_bit_count;
			}
			else
			{
				copy_bits(dest_ptr, dest_bit_num, (const byte *) _base_ptr, _bit_num, bits_remaining);
				_bit_num += bits_remaining;
				dest_bit_num += bits_remaining;
				bits_read += bits_remaining;
				read_bit_count -= bits_remaining;
			}
		}
	}

	uint32 _read_integer(uint32 bit_width)
	{
		uint32 ret = 0;
		_read_bits((byte *) &ret, 0, bit_width);
		little_endian_to_host(ret);
		return ret;
	}

	void _write_integer(uint32 value, uint32 bit_width)
	{
		host_to_little_endian(value);
		_write_bits((byte *) &value, 0, bit_width);
	}
    byte *_base_ptr;
     uint32 _bit_num;
    uint32 _bit_end; // when _bit_num == _bit_end, no more can be read/written, and the stream will ask for more data.
    bool _is_dirty;
    bool _error_detected;
    net_connection *_connection;
};

static void stream_test()
{
	struct internal
	{
		static void print_bits(const byte *bits_ptr, uint32 bit_start, uint32 bit_count)
		{
			char *array = new char[bit_count + 1];
			for(uint32 i = 0; i < bit_count; i++)
			{
				const byte *ptr = bits_ptr + ((i + bit_start) >> 3);
				array[i] = "-X"[(*ptr >> ((i + bit_start) & 7)) & 1];
			}
			array[bit_count] = 0;
			printf("%s\n", array);
			delete[] array;
		}
		static void test_range(int start, int count)
		{
			const byte bits[] = { 0xFF, 0xAA, 0x55, 0xCC, 0xFF, 0 };
			uint32 bits_size = strlen((const char *) bits);
			
			printf("Bit buffer copy %d, %d\n", start, count);
			internal::print_bits(bits, 0, bits_size * 8);
			internal::print_bits(bits, start, count);
			byte *buf = new byte[strlen((const char *) bits)];
			printf("0       1       2       3       4       5       6       7       \n");
			printf("0123456701234567012345670123456701234567012345670123456701234567\n");
			for(uint32 i = 0;i < 17; i++)
			{
				bit_stream::copy_bits(buf, 0, bits, 0, bits_size * 8);
				bit_stream::copy_bits(buf, i, bits, start, count);
				internal::print_bits(buf, 0, bits_size * 8);
			}			
		}
	};
	internal::test_range(0,8);
	internal::test_range(12,7);	
	internal::test_range(12,17);	
}
