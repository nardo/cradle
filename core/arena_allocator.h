// The core library - copyright KAGR LLC, portions copyright GarageGames. The use of this source code is governed by the MIT software license described in the "license.txt" file in this directory.

template <uint32 alignment = 8> class arena_allocator
{
public:
	arena_allocator(page_allocator *zone)
	{
		_allocator = zone;

		_current_offset = page_allocator::page_size;
		_current_page = 0;
	}

	~arena_allocator()
	{
		clear();
	}
	
	void clear()
	{
		while(_current_page)
		{
			page *prev = _current_page->prev;
			_allocator->deallocate_page(_current_page);
			_current_page = prev;
		}

		_current_offset = page_allocator::page_size;
	}
	
	void *allocate(uint32 size)
	{
		if(!size)
			return 0;
        assert(size <= maximum_allocation_size);
        
		// align size:
		size = (size + (alignment - 1)) & ~(alignment - 1);
		
		if(_current_offset + size > maximum_allocation_size)
		{
			// need a new page to fit this allocation:
			page *new_page = (page *) _allocator->allocate_page();
			new_page->allocator = this;
			_current_offset = page_header_size + size;
            new_page->prev = _current_page;
            _current_page = new_page;
			return ((uint8 *) new_page) + page_header_size;
		}
		else
		{
			void *ret = ((uint8 *) _current_page) + _current_offset;
			_current_offset += size;
			return ret;
		}
	}
private:
	struct page
	{
		arena_allocator *allocator;
		page *prev;
	};
	uint32 _current_offset;
    page_allocator *_allocator;
	page *_current_page;
	uint32 _alignment;
	
public:
	enum {
		page_header_size = (sizeof(page) + (alignment - 1)) & ~(alignment - 1),
		maximum_allocation_size = page_allocator::page_size - page_header_size,
	};
};
