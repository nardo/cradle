// The core library - copyright KAGR LLC, portions copyright GarageGames. The use of this source code is governed by the MIT software license described in the "license.txt" file in this directory.

template <class stream, typename type> static inline bool write(stream &the_stream, const type &value)
{
    assert(is_arithmetic<type>::is_true || is_enum<type>::is_true);
	type temp = value;
	host_to_little_endian(temp);
	return the_stream.write_bytes((byte *) &temp, sizeof(temp)) == sizeof(temp);
}

template <class stream, typename type> static inline bool read(stream &the_stream, type &value)
{
    assert(is_arithmetic<type>::is_true || is_enum<type>::is_true);
	if(the_stream.read_bytes((byte *) &value, sizeof(type)) != sizeof(type))
		return false;

	little_endian_to_host(value);
	return true;
}

template <class stream> static inline bool write(stream &the_stream, const bool &value)
{
    the_stream.write_bool(value);
    return true;
}

template <class stream> static inline bool read(stream &the_stream, bool &value)
{
    value = the_stream.read_bool();
    return true;
}

template <class stream> static inline bool write(stream &the_stream, const string &the_string)
{
    the_stream.write_c_str(the_string.c_str(), string::max_net_string_len);
    return true;
}

template <class stream> static inline bool read(stream &the_stream, string &the_string)
{
    char str_buf[string::max_net_string_len + 1];
    the_stream.read_c_str(str_buf, string::max_net_string_len);
    the_string = string(str_buf);
    return true;
}

template <class stream, typename type, unsigned size> static inline bool write(stream &the_stream, const fixed_array<type, size> &value)
{
    for(unsigned i = 0; i < size; i++)
        write(the_stream, value[i]);
    return true;
}

template <class stream, typename type, unsigned size> static inline bool read(stream &the_stream, fixed_array<type, size> &value)
{
    for(unsigned i = 0; i < size; i++)
        read(the_stream, value[i]);
    return true;
}

template <class stream, typename type, uint32 max_size> static inline bool write(stream &the_stream, const max_sized_array<type, max_size> &value)
{
    uint32 write_size = value.size();
    if(write_size > max_size)
        write_size = max_size;
    the_stream.write_ranged_uint32(write_size, 0, max_size);
    for(uint32 i = 0; i < write_size; i++)
        write(the_stream, value[i]);
    return true;
}

template <class stream, typename type, uint32 max_size> static  inline bool read(stream &the_stream, max_sized_array<type, max_size> &value)
{
    uint32 read_size = the_stream.read_ranged_uint32(0, max_size);
    value.reserve(read_size);
    for(uint32 i = 0; i < read_size; i++)
    {
        value.increment();
        read(the_stream, value.last());
    }
    return true;
}

enum {
    max_array_write_size = 128,
};

template <class stream, typename type> static inline bool write(stream &the_stream, const array<type> &value)
{
    uint32 write_size = value.size();
    assert(write_size <= max_array_write_size);
    if(write_size > max_array_write_size)
        write_size = max_array_write_size;
    the_stream.write_ranged_uint32(write_size, 0, max_array_write_size);
    for(uint32 i = 0; i < write_size; i++)
        write(the_stream, value[i]);
    return true;
}

template <class stream, typename type> static inline bool read(stream &the_stream, array<type> &value)
{
    uint32 read_size = the_stream.read_ranged_uint32(0, max_array_write_size);
    value.reserve(read_size);
    for(uint32 i = 0; i < read_size; i++)
    {
        value.increment();
        read(the_stream, value.last());
    }
    return true;
}

template <class stream, uint32 enum_count> static inline bool write(stream &the_stream, const enumeration<enum_count> &value)
{
	the_stream.write_ranged_uint32(value, 0, enum_count - 1);
	return true;
}

template <class stream, uint32 enum_count> static inline bool read(stream &the_stream, enumeration<enum_count> &value)
{
	value = the_stream.read_ranged_uint32(0, enum_count - 1);
	return true;
}

template <class stream, int32 range_min, int32 range_max> static inline bool write(stream &the_stream, const int_ranged<range_min, range_max> &value)
{
	the_stream.write_ranged_uint32(value._value - range_min, 0, range_max - range_min + 1);
	return true;
}

template <class stream, int32 range_min, int32 range_max> static inline bool read(stream &the_stream,  int_ranged<range_min, range_max> &value)
{
	value._value = range_min + the_stream.read_ranged_uint32(0, range_max - range_min + 1);
	return true;
}

template <class stream, uint32 precision> static inline bool write(stream &the_stream, const unit_float<precision> &value)
{
    the_stream.write_unit_float(value.value, precision);
	return true;
}

template <class stream, uint32 precision> static inline bool read(stream &the_stream, unit_float<precision> &value)
{
    value.value = the_stream.read_unit_float(precision);
	return true;
}

template <class stream, uint32 precision> static inline bool write(stream &the_stream, const signed_unit_float<precision> &value)
{
	the_stream.write_signed_unit_float(value.value, precision);
	return true;
}

template <class stream, uint32 precision> static inline bool read(stream &the_stream, signed_unit_float<precision> &value)
{
    value.value = the_stream.read_signed_unit_float(precision);
	return true;
}
