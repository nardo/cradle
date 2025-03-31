// The core library - copyright KAGR LLC, portions copyright GarageGames. The use of this source code is governed by the MIT software license described in the "license.txt" file in this directory.

static inline void endian_swap(uint16 &value)
{
	value = (value << 8) | (value >> 8);
}

static inline void endian_swap(uint32 &value)
{
	value = ((value << 24) & 0xFF000000) | ((value << 8) & 0xFF0000) | ((value >> 8) & 0xFF00) | ((value >> 24) & 0xFF);
}

static inline void endian_swap(uint64 &value)
{
	uint32 *src_ptr = (uint32 *) &value;
	endian_swap(src_ptr[0]);
	endian_swap(src_ptr[1]);
	uint32 temp = src_ptr[0];
	src_ptr[0] = src_ptr[1];
	src_ptr[1] = temp;
}

static inline void endian_swap(char8) {}
static inline void endian_swap(int8) {}
static inline void endian_swap(uint8) {}
static inline void endian_swap(bool) {}

static inline void endian_swap(int16 &value) { endian_swap(reinterpret_cast<uint16&>(value)); }
static inline void endian_swap(int32 &value) { endian_swap(reinterpret_cast<uint32&>(value)); }
static inline void endian_swap(float32 &value) { endian_swap(reinterpret_cast<uint32&>(value)); }
static inline void endian_swap(int64 &value) { endian_swap(reinterpret_cast<uint64&>(value)); }
static inline void endian_swap(float64 &value) { endian_swap(reinterpret_cast<uint64&>(value)); }

#ifdef LITTLE_ENDIAN
template<class type> static inline void host_to_little_endian(type& /*value*/) { }
template<class type> static inline void host_to_big_endian(type& value) { endian_swap(value); }
template<class type> static inline void little_endian_to_host(type& /*value*/) { }
template<class type> static inline void big_endian_to_host(type& value) { endian_swap(value); }
#else
template<class type> inline void host_to_little_endian(type& value) { endian_swap(value); }
template<class type> inline void host_to_big_endian(type& /*value*/) { }
template<class type> inline void little_endian_to_host(type& value) { endian_swap(value); }
template<class type> inline void big_endian_to_host(type& /*value*/) { }
#endif

static inline uint32 buffer_to_uint32(uint8 buffer[4])
{
	return (uint32(buffer[0]) << 24) | (uint32(buffer[1]) << 16) | (uint32(buffer[2]) << 8) | buffer[3];
}
