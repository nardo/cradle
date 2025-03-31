// The core library - copyright KAGR LLC, portions copyright GarageGames. The use of this source code is governed by the MIT software license described in the "license.txt" file in this directory.

struct context
{
	public:
	context() : _small_block_allocator(&_page_allocator, this), _string_table(&_page_allocator), _frame_allocator(&_page_allocator) {
	} //}, _pile(&_page_allocator) {}
	
	page_allocator &get_page_allocator() { return _page_allocator; }
	small_block_allocator<context> &get_small_block_allocator() { return _small_block_allocator; }
	indexed_string::table &get_string_table() { return _string_table; }
	//pile *get_pile() { return &_pile; }
	
	arena_allocator<> *get_frame_allocator()
	{
		return &_frame_allocator;
	}
    void set_current_on_thread()
    {
        thread_storage_ptr<context>::set(this);
    }
    static context *get_current()
    {
        return thread_storage_ptr<context>::get();
    }
	private:
	//pile _pile;
	page_allocator _page_allocator;
	small_block_allocator<context> _small_block_allocator;
	indexed_string::table _string_table;
	arena_allocator<> _frame_allocator;
};
