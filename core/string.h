// The core library - copyright KAGR LLC, portions copyright GarageGames. The use of this source code is governed by the MIT software license described in the "license.txt" file in this directory.

class string
{
protected:
	struct string_record
	{
		uint32 ref_count;
		uint32 len;
		char data[1];
	};

	static string_record *_get_null_string_record()
	{
		static string_record null_string = { 0x7FFFFFFF, 0,  { 0 } };
		return &null_string;
	}
	
	string_record *_string_record;
	void _dec_ref()
	{
		if(!--_string_record->ref_count && _string_record != _get_null_string_record())
			mem_free(_string_record);
	}
public:
    enum {
        max_net_string_len = 255,
    };
	string()
	{
		_string_record = _get_null_string_record();
		_string_record->ref_count++;
	}
	
	~string()
	{
		_dec_ref();
	}
	string(const char *string_data)
	{
		_string_record = _get_null_string_record();
		_string_record->ref_count++;
		set(string_data);
	}
	
	string(const char *string_data, uint32 len)
	{
		_string_record = _get_null_string_record();
		_string_record->ref_count++;
		set(string_data, len);		
	}
	
	string(const unsigned char *data, uint32 len)
	{
		_string_record = _get_null_string_record();
		_string_record->ref_count++;
		set(data, len);		
	}
	
	string(const string &the_string)
	{
		_string_record = the_string._string_record;
		_string_record->ref_count++;
	}
	
	string& operator=(const string &the_string)
	{
		_dec_ref();
		_string_record = the_string._string_record;
		_string_record->ref_count++;
		return *this;
	}
	void set(const char *string_data)
	{
		uint32 len = strlen(string_data);
		set(string_data, len);
	}
	string operator[](int index)
	{
		if(index < 0 || index >= _string_record->len)
			return string();
		else
			return string(_string_record->data + index, 1);
	}
	string operator[](string &s_index)
	{
		int index = atoi(s_index.c_str());
		if(index < 0 || index >= _string_record->len)
			return string();
		else
			return string(_string_record->data + index, 1);
	}
	void set(const char *string_data, uint32 len)
	{
		reserve(len);
		memcpy(_string_record->data, string_data, len);
	}
	void reserve(uint32 len)
	{
		_dec_ref();
		_string_record = (string_record *) mem_alloc(sizeof(string_record) + len);
		_string_record->len = len;
		_string_record->data[len] = 0; // ensure null termination after buffer length
		_string_record->ref_count = 1;

	}

	void set(const unsigned char *string_data, uint32 len)
	{
		set((const char *) string_data, len);
	}
	
	operator const char *() const
	{
		return _string_record->data;
	}
	const char *c_str() const 
	{
		return _string_record->data;
	}
	uint8 *data()
	{
		return (uint8 *) _string_record->data;
	}
	const uint8 *data() const
	{
		return (uint8 *) _string_record->data;
	}
	uint32 len() const
	{
		return _string_record->len;
	}
};

static inline string format_string(const char *fmt, ...)
{
	formatted_string_buffer buf;
	
	va_list args;
	va_start(args, fmt);
	uint32 len = buf.format(fmt, args);
	
	string return_value;
	return_value.set(buf.c_str(), len);
	
	return return_value;
}
