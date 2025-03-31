
#define ENABLE_JOURNALING
#define ENABLE_BIG_JOURNALS
// the ENABLE_BIG_JOURNALS flag is primarily used to debug errors in journal block read/writes -- as well as if there is code that executes in a non-deterministic way - i.e. calling block-recording functions in a different order on journal playback vs recording.

class journal : public ref_object, public functor_callable
{
    enum {
        journal_entry_begin_token = 0x1234,
        journal_entry_end_token = 0x5678,
        journal_entry_block_start_token = 0x7F,
        journal_entry_block_end_token = 0xF7,
        block_name_max_len = 32,
        journal_write_buffer_size = 10000,
    };
    FILE *_journal_file;
    bit_stream _read_stream;
    bit_stream _write_stream;
    uint32 _write_position;
    uint32 _read_break_bit_pos;
    uint32 _break_block_index;
    uint32 _block_index;
    uint8 *_playback_data;
    uint8 _record_data[journal_write_buffer_size];
    
public:
    enum mode {
        inactive,
        record,
        playback,
    };
    
protected:
    mode _current_mode;
    bool _inside_entrypoint;
    
    void check_read_position()
    {
        if(_read_stream.was_error_detected() || _read_stream.get_bit_position() >= _read_break_bit_pos)
            os_debug_break();
    }
    void flush_write_stream()
    {
        if(_write_stream.get_byte_position() == 0)
            return;
        uint32 total_bits = (_write_position << 3) | _write_stream.get_bit_position();
        
        // write the new bit total to the beginning of the file
        fseek(_journal_file, 0, SEEK_SET);
        uint32 write_bits = total_bits;
        host_to_little_endian(write_bits);
        fwrite(&write_bits, 1, sizeof(uint32), _journal_file);
        
        // now pick up where we left off...
        fseek(_journal_file, _write_position, SEEK_SET);
        uint32 bytes_to_write = _write_stream.get_byte_position();
        if(bytes_to_write)
        {
            fwrite(_record_data, 1, bytes_to_write, _journal_file);
            fflush(_journal_file);
            
            // now adjust the write bitstream
            if(total_bits & 7)
            {
                _record_data[0] = _record_data[bytes_to_write];
                _write_stream.set_bit_position(total_bits & 7);
                _write_position += bytes_to_write;
            }
            else
            {
                _write_position += bytes_to_write;
                _write_stream.set_bit_position(0);
            }
        }
    }
public:
    journal() :
        _write_stream(_record_data, sizeof(_record_data))
    {
        _inside_entrypoint = false;
        _current_mode = inactive;
        _read_break_bit_pos = 0;
        _journal_file = NULL;
        _write_position = 0;
        _break_block_index = 0;
        _block_index = 0;
        _playback_data = 0;
    }
    
    ~journal()
    {
        if(_playback_data)
            delete[] _playback_data;
    }
    
    bit_stream &get_read_stream() { return _read_stream; }
    bit_stream &get_write_stream() { return _write_stream; }

    void _call(callable_ref &the_functor)
    {
        if(_current_mode == playback)
            return;
        
        assert(_inside_entrypoint == false); // journals are not reentrant
        callable_record *method = find_callable(the_functor->_functor_hash);
        assert(method != 0);
        
        if(_current_mode == record)
        {
#ifdef ENABLE_BIG_JOURNALS
            uint16 token;
            token = journal_entry_begin_token;
            write(_write_stream, token);
#endif
            _write_stream.write_ranged_uint32(method->index, 0, _methods.size() - 1);
            the_functor->_write(_write_stream);
#ifdef ENABLE_BIG_JOURNALS
            token = journal_entry_end_token;
            write(_write_stream, token);
#endif
            flush_write_stream();
        }
        _inside_entrypoint = true;
        the_functor->dispatch(this);
        _inside_entrypoint = false;
    }
    
    void process_next_entry()
    {
        if(_current_mode != playback)
            return;
#ifdef ENABLE_BIG_JOURNALS
        uint16 token;
        read(_read_stream, token);
        assert(token == journal_entry_begin_token);
#endif
        uint32 entry_index = _read_stream.read_ranged_uint32(0, _methods.size() - 1);
        callable_record &m = _methods[entry_index];
        functor *func = m.creator->create();
        func->_read(_read_stream);
#ifdef ENABLE_BIG_JOURNALS
        read(_read_stream, token);
        assert(token == journal_entry_end_token);
#endif
        check_read_position();
        _inside_entrypoint = true;
        func->dispatch(this);
        _inside_entrypoint = false;
        delete func;
    }
    
    mode get_current_mode() { return _current_mode; }
    bool is_in_entrypoint() { return _inside_entrypoint; }
    void start_record(const char *file_name)
    {
        _journal_file = fopen(file_name, "wb");
        _current_mode = record;
        _write_position = sizeof(uint32);
    }
    
    void start_playback(const char *file_name)
    {
        FILE *the_journal = fopen(file_name, "rb");
        if(!the_journal)
            return;
        fseek(the_journal, 0, SEEK_END);
        uint32 file_size = ftell(the_journal);
        fseek(the_journal, 0, SEEK_SET);
        
        _playback_data = new uint8[file_size];
        fread(_playback_data, 1, file_size, the_journal);
        fclose(the_journal);

        _read_stream.set_buffer(_playback_data, 0, file_size * 8);
        uint32 bit_count;
        read(_read_stream, bit_count);
        _read_stream.set_stream_bit_size(bit_count);

        if(!_read_break_bit_pos || _read_break_bit_pos > bit_count)
            _read_break_bit_pos = bit_count;
        _current_mode = playback;
    }
    
    void begin_block(const char *block_name, bool write_block)
    {
        if(write_block)
        {
            logf(journal, ("write block %s", block_name));
            uint8 block_tok = journal_entry_block_start_token;
            assert(strlen(block_name) <= block_name_max_len);
            write(_write_stream, block_tok);
            _write_stream.write_c_str(block_name, block_name_max_len);
        }
        else
        {
            logf(journal, ("read block %s", block_name));
            // see if we want to break on this block playback
            _block_index++;
            if(_break_block_index && _block_index >= _break_block_index)
                os_debug_break();
            uint8 block_tok;
            read(_read_stream, block_tok);
            assert(block_tok == journal_entry_block_start_token);
            char buffer[block_name_max_len + 1];
            _read_stream.read_c_str(buffer, block_name_max_len);
            assert(strcmp(block_name, buffer) == 0);
        }
    }

    void end_block(const char *block_name, bool write_block)
    {
        if(write_block)
        {
            uint8 block_tok = journal_entry_block_end_token;
            write(_write_stream, block_tok);
            flush_write_stream();
        }
        else
        {
            uint8 block_tok;
            read(_read_stream, block_tok);
            assert(block_tok == journal_entry_block_end_token);
            check_read_position();
        }
    }
    
    void set_current_on_thread()
    {
        thread_storage_ptr<journal>::set(this);
    }
    static journal *get_current()
    {
        return thread_storage_ptr<journal>::get();
    }
};

class journal_token
{
    bool _writing;
    const char *_block_name;
    journal *_journal;
public:
    journal_token(journal *the_journal, const char *block_name, bool writing)
    {
        _journal = the_journal;
        _writing = writing;
        _block_name = block_name;
        _journal->begin_block(_block_name, _writing);
    }
    ~journal_token()
    {
        _journal->end_block(_block_name, _writing);
    }
};

#define journal_read_block(block_type, x) \
{ \
    journal *the_journal = journal::get_current(); \
    if(the_journal && the_journal->get_current_mode() == journal::playback && the_journal->is_in_entrypoint()) { \
        journal_token dummy_token(the_journal, #block_type, false); \
        { x } \
    }\
}

#define journal_read(x) read(the_journal->get_read_stream(), x);
#define journal_read_bytes(buffer, len) the_journal->get_read_stream().read_bytes(buffer, len)

#define journal_write_block(block_type, x) \
{ \
    journal *the_journal = journal::get_current(); \
    if(the_journal && the_journal->get_current_mode() == journal::record && the_journal->is_in_entrypoint()) { \
        journal_token dummy_token(the_journal, #block_type, true); \
        { x } \
    }\
}

#define journal_write(x) write(the_journal->get_write_stream(), x);
#define journal_write_bytes(buffer, len) the_journal->get_write_stream().write_bytes(buffer, len)
