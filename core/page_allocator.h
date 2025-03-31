// The core library - copyright KAGR LLC, portions copyright GarageGames. The use of this source code is governed by the MIT software license described in the "license.txt" file in this directory.

// A page_allocator is a memory allocation pool object that allocates directly from the OS virtual memory store. Allocations through a page_allocator are always aligned on page_size boundaries.

class page_allocator
{
	public:
    
	enum {
		page_size = 4096,
        pages_per_zone = 128, // 512kb chunks
        zone_size = page_size * pages_per_zone,
	};
    array<void *> _zones;

	page_allocator()
	{
		_pages_used = 0;
        _free_page_list = 0;
	}
	
	~page_allocator()
	{
		assert(_pages_used == 0);
        _deallocate_zones();
	}

private:
    struct page_header
    {
        page_header *next;
    };
    
    void *_allocate_zone()
    {
        void* data;
#if defined(PLATFORM_MAC_OSX)
        kern_return_t err;
        
        // Allocate directly from mach VM
        err = vm_allocate((vm_map_t) mach_task_self(), (vm_address_t*) &data, zone_size, VM_FLAGS_ANYWHERE);
        assert(err == KERN_SUCCESS);
        if(err != KERN_SUCCESS)
            data = 0;
#elif defined(PLATFORM_WIN32)
        data = VirtualAlloc(0, zone_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#else
        data = mmap(0, zone_size, PROT_READ|PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	if (data == MAP_FAILED)
	{
		data = 0;
		printf("MAP Failed! Errno=%d\n", errno);
	}

#endif
        assert(data != 0);
        _zones.push_back(data);
        return data;
    }
    
    void _deallocate_zones()
    {
        for(uint32 i = 0; i < _zones.size(); i++)
        {
            void *data = _zones[i];
#if defined(PLATFORM_MAC_OSX)
            kern_return_t err;
            err = vm_deallocate((vm_map_t) mach_task_self(), (vm_address_t) data, zone_size);

            assert(err == KERN_SUCCESS);
    #elif defined(PLATFORM_WIN32)
        VirtualFree(data, 0, MEM_DECOMMIT);
#else
            munmap(data, zone_size);
#endif
        }
    }
public:
	// currently we allocate pages one at a time from the OS VM.  A future optimization could allocate these in chunks... I'm not sure there would be any real net benefit, since the first access to a page is going to cause a page fault anyway.
	
	void *allocate_page()
	{
        if(!_free_page_list)
        {
            void *new_zone = _allocate_zone();
            uint8 *zone_walk = (uint8 *) new_zone;
            for(uint32 i = 0; i < pages_per_zone; i++)
            {
                page_header *page = (page_header *) zone_walk;
                page->next = _free_page_list;
                _free_page_list = page;
                zone_walk += page_size;
            }
        }
        page_header *ret = _free_page_list;
        _free_page_list = ret->next;
        _pages_used++;
        return (void *) ret;
	}
	
	void deallocate_page(void *page_ptr)
	{
        page_header *the_page = (page_header *) page_ptr;
        the_page->next = _free_page_list;
        _free_page_list = the_page;
        _pages_used--;
	}
	private:
	uint32 _pages_used;
    page_header *_free_page_list;
};
