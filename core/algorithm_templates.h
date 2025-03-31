// The core library - copyright KAGR LLC, portions copyright GarageGames. The use of this source code is governed by the MIT software license described in the "license.txt" file in this directory.

// if min/max are #define'd, undef them
#undef min
#undef max
template<class type> static inline type min(type a,type b) { return (a < b)? a: b; }
template<class type> static inline type max(type a,type b) { return (a > b)? a: b; }

template<class type> static inline type clamp(type a,type min,type max) { return (a < min)? min:(a>max)?max: a;}
template<class type> static inline type clamp_min(type a,type min) { return (a < min)? min: a; }
template<class type> static inline type clamp_max(type a,type max) { return (a > max)? max: a; }

template<typename type> static inline bool less(const type &a, const type &b)
{
	return a < b;
}

template<typename type> static inline void swap(type &a, type &b)
{
	type temp = a;
	a = b;
	b = temp;
}

template<typename type> static inline void swap(type *start, type *end, type *dest)
{
	while(start != end)
		swap(*start++, *dest++);
}

//namespace internal {
	template<typename src_type,typename dst_type> static inline dst_type copy_aux(src_type src,src_type end,dst_type dst,false_type)
	{
		while (src != end)
			*dst++ = *src++;
		return dst;
	}

	template<typename src_type,typename dst_type> static inline dst_type copy_aux(src_type src,src_type end,dst_type dst,true_type)
	{
        memmove(dst, src, (uintptr_t)end - (uintptr_t)src);
		return dst + (end - src);
	}
//} // Private

/// Copy from [src,end) to dst, going forwards.
/// Source and destination may overlap.  This function works for all iterators
/// and memmove is used for real pointers.
template<typename src_ptr_type,typename dst_ptr_type> static inline dst_ptr_type copy(src_ptr_type src,src_ptr_type end,dst_ptr_type dst)
{
	typedef typename non_qualified_type<typename type_traits<src_ptr_type>::value_type>::value_type src_data_type;
	typedef typename non_qualified_type<typename type_traits<dst_ptr_type>::value_type>::value_type dst_data_type;
	typename bool_type<
		is_pointer<src_ptr_type>::is_true &&
		is_pointer<dst_ptr_type>::is_true &&
		is_same<src_data_type,dst_data_type>::is_true &&
		has_trivial_assign<src_data_type>::is_true
	>::value_type is_trivial;
	return /*internal::*/copy_aux(src,end,dst,is_trivial);
}

template<typename T,typename U>
static inline U copy(T src,unsigned int count,U dst)
{
   return copy(src,src + count,dst);
}

//namespace internal {
	template<typename src_type,typename dst_type>
	static inline void ucopy_is_trivial(src_type src,src_type end,dst_type dst,false_type)
	{
		while (src != end)
			construct(dst++,*src++);
	}

	template<typename src_type,typename dst_type>
	static inline void ucopy_is_trivial(src_type src,src_type end,dst_type dst,true_type)
	{
	   memmove(dst, src, (uintptr_t)end - (uintptr_t)src);
	}
//}

template<typename src_ptr_type,typename dst_ptr_type>
static inline void uninitialized_copy(src_ptr_type src,src_ptr_type end,dst_ptr_type dst)
{
	typedef typename non_qualified_type<typename type_traits<src_ptr_type>::value_type>::value_type src_data_type;
	typedef typename non_qualified_type<typename type_traits<dst_ptr_type>::value_type>::value_type dst_data_type;
	typename bool_type<
		is_pointer<src_ptr_type>::is_true &&
		is_pointer<dst_ptr_type>::is_true &&
		is_same<src_data_type,dst_data_type>::is_true &&
		has_trivial_assign<src_data_type>::is_true
	>::value_type is_trivial;
	/*internal::*/ucopy_is_trivial(src,end,dst,is_trivial);
}

