// The core library - copyright KAGR LLC, portions copyright GarageGames. The use of this source code is governed by the MIT software license described in the "license.txt" file in this directory.

/// Base class for functor_decl template classes.  The functor objects store the parameters and member function pointer for the invocationof some class member function.  functor is used by the journaling system and RPC mechanism to store a function for later transmission and dispatch, either to a remote host, a journal file, or another thread in the process.

struct functor : public ref_object {
    method_hash_type _functor_hash;
    /// Construct the functor.
    functor() { _functor_hash = 0; }
	/// Destruct the functor.
	virtual ~functor() {}
	/// Reads this functor from a bit_stream.
	virtual void _read(bit_stream &stream) = 0;
	/// Writes this functor to a bit_stream.
	virtual void _write(bit_stream &stream) = 0;
	/// Dispatch the function represented by the functor.
	virtual void dispatch(void *t) = 0;
};

/// functor_decl template class.  This class is specialized based on the member function call signature of the method it represents.  Other specializations hold specific member function pointers and slots for each of the function arguments.
template <class T> 
struct functor_decl : public functor {
	functor_decl() {}
	void set() {}
	void _read(bit_stream &stream) {}
	void _write(bit_stream &stream) {}
	void dispatch(void *t) { }
};

template <class T> 
struct functor_decl<void (T::*)()> : public functor {
	typedef void (T::*func_ptr)();
	func_ptr ptr;
    functor_decl(func_ptr p) : ptr(p) { _functor_hash = hash_method(p); }
	void set() {}
	void _read(bit_stream &stream) {}
	void _write(bit_stream &stream) {}
	void dispatch(void *t) { ((T *)t->*ptr)(); }
};

#define BaseType(X) typename deref<X>::base_type
#define RefType(X) typename deref<X>::reference_type

template <class T, class A>
struct functor_decl<void (T::*)(A)> : public functor {
	typedef void (T::*func_ptr)(A);
	func_ptr ptr;
    BaseType(A) a;
    functor_decl(func_ptr p) : ptr(p) { _functor_hash = hash_method(p); }
	void set(RefType(A) _a) { a = _a; }
	void _read(bit_stream &stream) { read(stream, a); }
	void _write(bit_stream &stream) { write(stream, a); }
	void dispatch(void *t) { (((T *)t)->*ptr)(a); }
};
template <class T, class A, class B>
struct functor_decl<void (T::*)(A,B)>: public functor {
	typedef void (T::*func_ptr)(A,B);
	func_ptr ptr;
    BaseType(A) a;
    BaseType(B) b;
    functor_decl(func_ptr p) : ptr(p) { _functor_hash = hash_method(p); }
	void set(RefType(A) _a, RefType(B) _b) { a = _a; b = _b;}
	void _read(bit_stream &stream) { read(stream, a); read(stream, b); }
	void _write(bit_stream &stream) { write(stream, a); write(stream, b); }
	void dispatch(void *t) { (((T *)t)->*ptr)(a, b); }
};

template <class T, class A, class B, class C>
struct functor_decl<void (T::*)(A,B,C)>: public functor {
	typedef void (T::*func_ptr)(A,B,C);
	func_ptr ptr; BaseType(A) a; BaseType(B) b; BaseType(C) c;
    functor_decl(func_ptr p) : ptr(p) { _functor_hash = hash_method(p); }
	void set(RefType(A) _a, RefType(B) _b, RefType(C) _c) { a = _a; b = _b; c = _c;}
	void _read(bit_stream &stream) { read(stream, a); read(stream, b); read(stream, c); }
	void _write(bit_stream &stream) { write(stream, a); write(stream, b); write(stream, c); }
	void dispatch(void *t) { (((T *)t)->*ptr)(a, b, c); }
};

template <class T, class A, class B, class C, class D>
struct functor_decl<void (T::*)(A,B,C,D)>: public functor {
	typedef void (T::*func_ptr)(A,B,C,D);
	func_ptr ptr; BaseType(A) a; BaseType(B) b; BaseType(C) c; BaseType(D) d;
    functor_decl(func_ptr p) : ptr(p) { _functor_hash = hash_method(p); }
	void set(RefType(A) _a, RefType(B) _b, RefType(C) _c, RefType(D) _d) { a = _a; b = _b; c = _c; d = _d; }
	void _read(bit_stream &stream) { read(stream, a); read(stream, b); read(stream, c); read(stream, d); }
	void _write(bit_stream &stream) { write(stream, a); write(stream, b); write(stream, c); write(stream, d); }
	void dispatch(void *t) { (((T *)t)->*ptr)(a, b, c, d); }
};

template <class T, class A, class B, class C, class D, class E>
struct functor_decl<void (T::*)(A,B,C,D,E)>: public functor {
	typedef void (T::*func_ptr)(A,B,C,D,E);
	func_ptr ptr; BaseType(A) a; BaseType(B) b; BaseType(C) c; BaseType(D) d; BaseType(E) e;
    functor_decl(func_ptr p) : ptr(p) { _functor_hash = hash_method(p); }
	void set(RefType(A) _a, RefType(B) _b, RefType(C) _c, RefType(D) _d, RefType(E) _e) { a = _a; b = _b; c = _c; d = _d; e = _e; }
	void _read(bit_stream &stream) { read(stream, a); read(stream, b); read(stream, c); read(stream, d); read(stream, e); }
	void _write(bit_stream &stream) { write(stream, a); write(stream, b); write(stream, c); write(stream, d); write(stream, e); }
	void dispatch(void *t) { (((T *)t)->*ptr)(a, b, c, d, e); }
};

template <class T, class A, class B, class C, class D, class E, class F>
struct functor_decl<void (T::*)(A,B,C,D,E,F)>: public functor {
	typedef void (T::*func_ptr)(A,B,C,D,E,F);
	func_ptr ptr; BaseType(A) a; BaseType(B) b; BaseType(C) c; BaseType(D) d; BaseType(E) e; BaseType(F) f;
    functor_decl(func_ptr p) : ptr(p) { _functor_hash = hash_method(p); }
	void set(RefType(A) _a, RefType(B) _b, RefType(C) _c, RefType(D) _d, RefType(E) _e, RefType(F) _f) { a = _a; b = _b; c = _c; d = _d; e = _e; f = _f; }
	void _read(bit_stream &stream) { read(stream, a); read(stream, b); read(stream, c); read(stream, d); read(stream, e); read(stream, f); }
	void _write(bit_stream &stream) { write(stream, a); write(stream, b); write(stream, c); write(stream, d); write(stream, e); write(stream, f); }
	void dispatch(void *t) { (((T *)t)->*ptr)(a, b, c, d, e, f); }
};

template <class T, class A, class B, class C, class D, class E, class F, class G>
struct functor_decl<void (T::*)(A,B,C,D,E,F,G)>: public functor {
	typedef void (T::*func_ptr)(A,B,C,D,E,F,G);
	func_ptr ptr; BaseType(A) a; BaseType(B) b; BaseType(C) c; BaseType(D) d; BaseType(E) e; BaseType(F) f; BaseType(G) g;
    functor_decl(func_ptr p) : ptr(p) { _functor_hash = hash_method(p); }
	void set(RefType(A) _a, RefType(B) _b, RefType(C) _c, RefType(D) _d, RefType(E) _e, RefType(F) _f, RefType(G) _g) { a = _a; b = _b; c = _c; d = _d; e = _e; f = _f; g = _g; }
	void _read(bit_stream &stream) { read(stream, a); read(stream, b); read(stream, c); read(stream, d); read(stream, e); read(stream, f); read(stream, g); }
	void _write(bit_stream &stream) { write(stream, a); write(stream, b); write(stream, c); write(stream, d); write(stream, e); write(stream, f); write(stream, g); }
	void dispatch(void *t) { (((T *)t)->*ptr)(a, b, c, d, e, f, g); }
};

template <class T, class A, class B, class C, class D, class E, class F, class G, class H>
struct functor_decl<void (T::*)(A,B,C,D,E,F,G,H)>: public functor {
	typedef void (T::*func_ptr)(A,B,C,D,E,F,G,H);
	func_ptr ptr; BaseType(A) a; BaseType(B) b; BaseType(C) c; BaseType(D) d; BaseType(E) e; BaseType(F) f; BaseType(G) g; BaseType(H) h;
    functor_decl(func_ptr p) : ptr(p) { _functor_hash = hash_method(p); }
	void set(RefType(A) _a, RefType(B) _b, RefType(C) _c, RefType(D) _d, RefType(E) _e, RefType(F) _f, RefType(G) _g, RefType(H) _h) { a = _a; b = _b; c = _c; d = _d; e = _e; f = _f; g = _g; h = _h; }
	void _read(bit_stream &stream) { read(stream, a); read(stream, b); read(stream, c); read(stream, d); read(stream, e); read(stream, f); read(stream, g); read(stream, h); }
	void _write(bit_stream &stream) { write(stream, a); write(stream, b); write(stream, c); write(stream, d); write(stream, e); write(stream, f); write(stream, g); write(stream, h); }
	void dispatch(void *t) { (((T *)t)->*ptr)(a, b, c, d, e, f, g, h); }
};

template <class T, class A, class B, class C, class D, class E, class F, class G, class H, class I>
struct functor_decl<void (T::*)(A,B,C,D,E,F,G,H,I)>: public functor {
	typedef void (T::*func_ptr)(A,B,C,D,E,F,G,H,I);
	func_ptr ptr; BaseType(A) a; BaseType(B) b; BaseType(C) c; BaseType(D) d; BaseType(E) e; BaseType(F) f; BaseType(G) g; BaseType(H) h; BaseType(I) i;
    functor_decl(func_ptr p) : ptr(p) { _functor_hash = hash_method(p); }
	void set(RefType(A) _a, RefType(B) _b, RefType(C) _c, RefType(D) _d, RefType(E) _e, RefType(F) _f, RefType(G) _g, RefType(H) _h, RefType(I) _i) { a = _a; b = _b; c = _c; d = _d; e = _e; f = _f; g = _g; h = _h; i = _i; }
	void _read(bit_stream &stream) { read(stream, a); read(stream, b); read(stream, c); read(stream, d); read(stream, e); read(stream, f); read(stream, g); read(stream, h); read(stream, i); }
	void _write(bit_stream &stream) { write(stream, a); write(stream, b); write(stream, c); write(stream, d); write(stream, e); write(stream, f); write(stream, g); write(stream, h); write(stream, i); }
	void dispatch(void *t) { (((T *)t)->*ptr)(a, b, c, d, e, f, g, h, i); }
};

template <class T, class A, class B, class C, class D, class E, class F, class G, class H, class I, class J>
struct functor_decl<void (T::*)(A,B,C,D,E,F,G,H,I,J)>: public functor {
	typedef void (T::*func_ptr)(A,B,C,D,E,F,G,H,I,J);
    func_ptr ptr; BaseType(A) a; BaseType(B) b; BaseType(C) c; BaseType(D) d; BaseType(E) e; BaseType(F) f; BaseType(G) g; BaseType(H) h; BaseType(I) i; BaseType(J) j;
    functor_decl(func_ptr p) : ptr(p) { _functor_hash = hash_method(p); }
	void set(RefType(A) _a, RefType(B) _b, RefType(C) _c, RefType(D) _d, RefType(E) _e, RefType(F) _f, RefType(G) _g, RefType(H) _h, RefType(I) _i, RefType(J) _j) { a = _a; b = _b; c = _c; d = _d; e = _e; f = _f; g = _g; h = _h; i = _i; j = _j; }
	void _read(bit_stream &stream) { read(stream, a); read(stream, b); read(stream, c); read(stream, d); read(stream, e); read(stream, f); read(stream, g); read(stream, h); read(stream, i); read(stream, j); }
	void _write(bit_stream &stream) { write(stream, a); write(stream, b); write(stream, c); write(stream, d); write(stream, e); write(stream, f); write(stream, g); write(stream, h); write(stream, i); write(stream, j); }
	void dispatch(void *t) { (((T *)t)->*ptr)(a, b, c, d, e, f, g, h, i, j); }
};


class functor_creator
{
public:
    method_hash_type _functor_hash;

	virtual functor *create() = 0;
	virtual ~functor_creator() {}
};

template<typename signature> class functor_creator_decl : public functor_creator
{
	signature f;
public:
    
    functor_creator_decl(signature sig)
	{
		f = sig;
        _functor_hash = hash_method(sig);
	}
	functor *create()
	{
		return new functor_decl<signature>(f);
	}
};
