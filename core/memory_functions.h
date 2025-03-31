// The core library - copyright KAGR LLC, portions copyright GarageGames. The use of this source code is governed by the MIT software license described in the "license.txt" file in this directory.
#define CORE_DEBUG_MEMORY
//#define CORE_HEAP_CHECK_ON_ALL_CALLS

#ifndef CORE_DEBUG_MEMORY
static inline void *mem_alloc(size_t size)
{
	return malloc(size);
}

static inline void mem_free(void *ptr)
{
	free(ptr);
}

static inline void *mem_realloc(void *ptr, size_t size)
{
	return realloc(ptr, size);
}

#else

class mutex;

static mutex &mem_mutex()
{
    static mutex m;
    return m;
}

enum {
    mem_guard_size = 12,
};

struct mem_header
{
    mem_header *_next;
    mem_header *_prev;
    const char *_file;
    size_t _size;
    uint32 _line;
    uint8 _guard[mem_guard_size];
};

static void mem_get_alloc_guards(const uint8 **pre_guard, const uint8 **post_guard)
{
    static const uint8 pre_guard_str[mem_guard_size+1] = "G0odAlloCate";
    static const uint8 post_guard_str[mem_guard_size+1] = "EndoG0odAloc";
    *pre_guard = pre_guard_str;
    *post_guard = post_guard_str;
}

static void mem_get_free_guards(const uint8 **pre_guard, const uint8 **post_guard)
{
    static const uint8 pre_guard_str[mem_guard_size+1] = "FreedPreGard";
    static const uint8 post_guard_str[mem_guard_size+1] = "p0stFReeGurd";
    *pre_guard = pre_guard_str;
    *post_guard = post_guard_str;
}

// format of each allocation is:
// mem_header + 16-byte align + alloc_size + 12-byte tail guard

static mem_header *get_alloc_list()
{
    static mem_header alloc_list = { &alloc_list, &alloc_list, 0, 0, 0, {0,0,0} };
    return &alloc_list;
}

static inline size_t mem_header_size()
{
    // 16-byte align mem_header
    size_t size = (sizeof(mem_header) + 0xF) & ~0xF;
    return size;
};

static mem_header *mem_get_alloc_ptr(void *ptr)
{
    uint8 *byte_ptr = reinterpret_cast<uint8 *>(ptr);
    return reinterpret_cast<mem_header *>(byte_ptr - mem_header_size());
}

static void mem_validate_guards(mem_header *ptr, const uint8 *pre_guard, const uint8 *post_guard)
{
    size_t header_size = mem_header_size();
    size_t pre_guard_size = header_size - sizeof(mem_header) + mem_guard_size;
    size_t index = 0;
    for(size_t i = 0; i < pre_guard_size; i++)
    {
        assert(ptr->_guard[i] == pre_guard[index]);
        index++;
    }
    const uint8 *post_ptr = reinterpret_cast<uint8 *>(ptr) + header_size + ptr->_size;
    for(size_t i = 0; i < mem_guard_size; i++)
        assert(post_ptr[i] == post_guard[i]);
}

static void mem_write_guards(mem_header *ptr, const uint8 *pre_guard, const uint8 *post_guard)
{
    size_t header_size = mem_header_size();
    size_t pre_guard_size = header_size - sizeof(mem_header) + mem_guard_size;
    size_t index = 0;
    for(size_t i = 0; i < pre_guard_size; i++)
    {
        ptr->_guard[i] = pre_guard[index];
        index++;
    }
    uint8 *post_ptr = reinterpret_cast<uint8 *>(ptr) + header_size + ptr->_size;
    for(size_t i = 0; i < mem_guard_size; i++)
        post_ptr[i] = post_guard[i];
}

static void mem_check_heap()
{
    const uint8 *pre_guard;
    const uint8 *post_guard;
    mem_get_alloc_guards(&pre_guard, &post_guard);

    mem_mutex().lock();
    mem_header *alloc_list = get_alloc_list();
    for(mem_header *walk = alloc_list->_next; walk != alloc_list; walk = walk->_next)
        mem_validate_guards(walk, pre_guard, post_guard);
    mem_mutex().unlock();
}

static inline size_t mem_alloc_size_with_guards(size_t size)
{
    return mem_header_size() + size + mem_guard_size;
}

static inline void *mem_alloc_r(size_t size, const char *file, uint32 line)
{
#ifdef CORE_HEAP_CHECK_ON_ALL_CALLS
    mem_check_heap();
#endif
    size_t alloc_size = mem_alloc_size_with_guards(size);
    void *alloc_ptr = malloc(alloc_size);
    mem_header *ptr = reinterpret_cast<mem_header *>(alloc_ptr);
    ptr->_file = file;
    ptr->_line = line;
    ptr->_size = size;
    
    const uint8 *pre_guard;
    const uint8 *post_guard;
    mem_get_alloc_guards(&pre_guard, &post_guard);
    mem_write_guards(ptr, pre_guard, post_guard);
    
    mem_mutex().lock();
    mem_header *alloc_list = get_alloc_list();
    ptr->_next = alloc_list->_next;
    ptr->_prev = alloc_list;
    ptr->_next->_prev = ptr;
    alloc_list->_next = ptr;
    mem_mutex().unlock();
    
    size_t header_size = mem_header_size();
    return reinterpret_cast<void *>(reinterpret_cast<uint8 *>(ptr) + header_size);
}

static inline void mem_free_r(void *alloc_ptr)
{
    if(!alloc_ptr)
        return;
    
#ifdef CORE_HEAP_CHECK_ON_ALL_CALLS
    mem_check_heap();
#endif
    mem_header *ptr = mem_get_alloc_ptr(alloc_ptr);

    const uint8 *pre_guard;
    const uint8 *post_guard;
    mem_get_alloc_guards(&pre_guard, &post_guard);
    mem_validate_guards(ptr, pre_guard, post_guard);

    mem_mutex().lock();
    mem_get_free_guards(&pre_guard, &post_guard);
    mem_write_guards(ptr, pre_guard, post_guard);

    ptr->_prev->_next = ptr->_next;
    ptr->_next->_prev = ptr->_prev;
    mem_mutex().unlock();
    free(ptr);
}

static inline void *mem_realloc_r(void *alloc_ptr, size_t new_size, const char *file, uint32 line)
{
#ifdef CORE_HEAP_CHECK_ON_ALL_CALLS
    mem_check_heap();
#endif
    if(!alloc_ptr)
        return mem_alloc_r(new_size, file, line);
    mem_header *ptr = mem_get_alloc_ptr(alloc_ptr);
    size_t copy_size = ptr->_size;
    if(new_size < copy_size)
        copy_size = new_size;
    void *new_alloc = mem_alloc_r(new_size, file, line);
    memcpy(new_alloc, alloc_ptr, copy_size);
    mem_free_r(alloc_ptr);
    return new_alloc;
}

#define mem_alloc(x) mem_alloc_r(x, __FILE__, __LINE__)
#define mem_realloc(x, y) mem_realloc_r(x, y, __FILE__, __LINE__)
#define mem_free mem_free_r

void* FN_CDECL operator new(size_t size)
{
    return mem_alloc_r(size, 0, 0);
}

void* FN_CDECL operator new[](size_t size)
{
    return mem_alloc_r(size, 0, 0);
}


void* FN_CDECL operator new(size_t size, const char *file, const uint32 line)
{
    return mem_alloc_r(size, file, line);
}

void* FN_CDECL operator new[](size_t size, const char *file, const uint32 line)
{
    return mem_alloc_r(size, file, line);
}

void  FN_CDECL operator delete(void* ptr)
{
    return mem_free_r(ptr);
}

void  FN_CDECL operator delete[](void* ptr, const char *file)
{
    return mem_free_r(ptr);
}

#define placenew(x) new(x)
#ifdef new
#undef new
#endif
#define new new(__FILE__, __LINE__)

#endif

static inline char *dup_c_str(const char *the_string)
{
    uint32 len = strlen(the_string);
    char *ret = (char *) mem_alloc(len + 1);
    strcpy(ret, the_string);
    return ret;
}
