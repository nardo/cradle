// The core library - copyright KAGR LLC, portions copyright GarageGames. The use of this source code is governed by the MIT software license described in the "license.txt" file in this directory.

// Maintain a reserve of half the public size
struct array_half_reserve_policy
{
   inline static uint32 grow_to(uint32 public_size, uint32 /*array_size*/) {
      return public_size + public_size / 2;
   }
   inline static uint32 shrink_to(uint32 public_size, uint32 array_size) {
      return (public_size < array_size / 2)?
         public_size + public_size / 2: array_size;
   };
};

// Maintain a reserve of half the public size
struct array_half_reserve_grow_policy
{
   inline static uint32 grow_to(uint32 public_size, uint32 /*array_size*/) {
      return public_size + public_size / 2;
   }
   inline static uint32 shrink_to(uint32 public_size, uint32 array_size) {
      return array_size;
   };
};

template<class type, unsigned size> class fixed_array
{
public:
    type _array[size];
    fixed_array() {}
    type &operator[](unsigned index)
    {
        return _array[index];
    }
    
    const type &operator[](unsigned index) const
    {
        return _array[index];
    }
};

template<class type, class policy = array_half_reserve_grow_policy> class array
{
public:
	typedef type value_type;
	typedef type &reference;
	typedef const type &const_reference;
	typedef type *iterator;
	typedef const type *const_iterator;

    // porting code block work in progress
    void setSize(uint32 new_size)
    {
        resize(new_size);
    }
    
	array()
	{
		_public_size = 0;
		_array_size = 0;
		_array = 0;
	}
	
	array(const uint32 initial_reserve)
	{
		_public_size = 0;
		_array_size = initial_reserve;
		_array = initial_reserve ? (type *) mem_alloc(initial_reserve * sizeof(type)) : 0;
	}
	
	array(const array &p)
	{
		_public_size = 0;
		_array_size = 0;
		_array = 0;
		*this = p;
	}
	
	~array()
	{
		destroy(begin(), end());
		mem_free(_array);
	}
	
	uint32 size() const
	{
		return _public_size;
	}
	
	uint32 capacity() const
	{
		return _array_size;
	}
	
	void reserve(uint32 size)
	{
		if(size > _array_size)
			_resize(size);
	}
	
	void resize(uint32 size)
	{
		if(size > _public_size)
		{
			if(size > _array_size)
				_resize(policy::grow_to(size, _array_size));
			construct(end(), end() + (size - _public_size));
			_public_size = size;
		}
		else if(size < _public_size)
		{
			destroy(end() - (_public_size - size), end());
			_public_size = size;
			uint32 new_size = policy::shrink_to(size, _array_size);
			if(new_size != _array_size)
				_resize(new_size);
		}
	}
	
	void compact()
	{
		if(_public_size < _array_size)
			_resize(_public_size);
	}
	
	void clear()
	{
		destroy(begin(), end());
		mem_free(_array);
		_public_size = 0;
		_array_size = 0;
		_array = 0;
	}
	
	bool is_empty()
	{
		return _public_size == 0;
	}
    
    void increment()
    {
        resize(_public_size + 1);
    }
	
	iterator insert(uint32 index, const type &x)
	{
        assert(index <= _public_size);
        increment();
        for(uint32 last = _public_size - 1; last > index; last--)
            _array[last] = _array[last - 1];
        _array[index] = x;
        return _array + index;
	}
	
	iterator insert(iterator itr, const type &x)
	{
		return insert(uint32(itr - _array), x);
	}
	
	iterator insert(uint32 index)
	{
		return insert(index, type());
	}
	
	iterator insert(iterator itr)
	{
		return insert(uint32(itr - _array));
	}
	
	iterator push_front(const type &x)
	{
		return insert(uint32(0), x);
	}
	
	iterator push_back(const type &x)
	{
		if(_public_size + 1 > _array_size)
			_resize(policy::grow_to(_public_size + 1, _array_size));
		iterator ret = construct(_array + _public_size, x);
        _public_size++;
        return ret;
	}
	
	iterator push_front()
	{
		return insert(uint32(0));
	}
	
	iterator push_back()
	{
		return push_back(type());
	}
	
	void erase(uint32 index)
	{
        assert(index < _public_size);
		if(has_trivial_copy<type>::is_true)
		{
			copy(_array + index + 1, end(), _array + index);
			destroy(&last());
            --_public_size;
			uint32 new_size = policy::shrink_to(_public_size, _array_size);
			assert(new_size >= _public_size);
			if(new_size != _array_size)
			{
				_array = (type *) mem_realloc(_array, new_size * sizeof(type));
				_array_size = new_size;
			}
		}
		else
		{
			uint32 new_size = policy::shrink_to(_public_size - 1, _array_size);
			if(new_size != _array_size)
			{
				assert(new_size >= _public_size - 1);
				type *new_array = (type *) mem_alloc(new_size * sizeof(type));
				uninitialized_copy(begin(), _array + index, new_array);
				uninitialized_copy(_array + index + 1, end(), new_array + index);
				destroy(begin(), end());
				mem_free(_array);
				_array = new_array;
				_array_size = new_size;
			}
			else
			{
				copy(_array + index + 1, end(), _array + index);
				destroy(&last());
			}
			_public_size--;
		}
	}
	
	void erase(iterator itr)
	{
		erase(uint32(itr - _array));
	}
	
	void erase_fast(iterator itr)
	{
        assert(itr <= end() - 1);
		if(itr != end() - 1)
			*itr = last();
		pop_back();
	}
	
	void erase_fast(uint32 index)
	{
		erase_fast(&_array[index]);
	}
	
	void pop_front()
	{
		erase(uint32(0));
	}
	
	void pop_back()
	{
        assert(_public_size >= 1);
        
		destroy(&last());
        --_public_size;
		uint32 size = policy::shrink_to(_public_size, _array_size);
		if(size != _array_size)
			_resize(size);
	}
	
	iterator begin()
	{
		return _array;
	}
	
	const_iterator begin() const
	{
		return _array;
	}
	
	iterator end()
	{
		return _array + _public_size;
	}
	
	const_iterator end() const
	{
		return _array + _public_size;
	}
	
	iterator rbegin()
	{
		return end() - 1;
	}
	
	const_iterator rbegin() const
	{
		return end() - 1;
	}
	
	iterator rend()
	{
		return begin() - 1;
	}
	
	const_iterator rend() const
	{
		return begin() - 1;
	}
	
	type &first()
	{
		return _array[0];
	}
	
	const type &first() const
	{
		return _array[0];
	}
	
	type &last()
	{
        assert(_public_size >= 1);
		return _array[_public_size - 1];
	}
	
	const type &last() const
	{
        assert(_public_size >= 1);
		return _array[_public_size - 1];
	}
	
	type &operator[](uint32 index)
	{
        assert(!index || index < _public_size);
		return _array[index];
	}
	
	const type &operator[](uint32 index) const
	{
        assert(index < _public_size);
		return _array[index];
	}
	
	void operator=(const array &p)
	{
        destroy(begin(), end());
        _public_size = 0;
        reserve(p._public_size);
        uninitialized_copy(p.begin(), p.end(), begin());
        _public_size = p._public_size;
    }
    
    typedef int32 (QSORT_CALLBACK *compare_func)(type *a, type *b);
    void sort(compare_func f)
    {
        typedef int (QSORT_CALLBACK *qsort_compare_func)(const void *, const void *);
        qsort(_array, size(), sizeof(type), (qsort_compare_func) f);
    }
protected:
	uint32 _public_size;
	uint32 _array_size;
	type *_array;
	
	void _resize(uint32 size)
	{
		assert(size >= _public_size);
		if(has_trivial_copy<type>::is_true)
			_array = (type *) mem_realloc(_array, size * sizeof(type));
		else
		{
			type *new_array = (type *) mem_alloc(size * sizeof(type));
			uninitialized_copy(begin(), end(), new_array);
			destroy(begin(), end());
			mem_free(_array);
			_array = new_array;
		}
		_array_size = size;
	}
};

template <class T, uint32 max_size> class max_sized_array : public array<T>
{
};

/*template<class type, class policy> struct container_manipulator<array<type, policy> >
{
	static type_record *get_key_type()
	{
		return get_global_type_record<uint32>();
	}
	static type_record *get_value_type()
	{
		return get_global_type_record<type>();
	}
};*/
