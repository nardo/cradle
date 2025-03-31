struct callable_record
{
    functor_creator *creator;
    uint32 index;
};

typedef ref_ptr<functor> callable_ref;

struct functor_callable
{
    array<callable_record> _methods;
    template <typename signature> uint32 register_callable(signature the_method)
    {
        callable_record the_record;
        the_record.creator = new functor_creator_decl<signature>(the_method);
        for(uint32 i = 0; i < _methods.size(); i++)
        {
            assert(_methods[i].creator->_functor_hash != the_record.creator->_functor_hash);
        }
        the_record.index = _methods.size();
        _methods.push_back(the_record);
        return the_record.index;
    }
    
    callable_record *find_callable(method_hash_type m)
    {
        for(uint32 index = 0; index < _methods.size(); index++)
            if(_methods[index].creator->_functor_hash == m)
                return &_methods[index];
        return 0;
    }
    
    virtual void _call(callable_ref &the_functor) = 0;
    
    void call_prepared(callable_ref &the_functor) { _call(the_functor); }
    
    template <class T>
    void call(void (T::*method)())
    {
        callable_ref f = new functor_decl<void (T::*)()>(method);
        _call(f);
    }

    template <class T>
    static callable_ref call_prepare(void (T::*method)())
    {
        callable_ref f = new functor_decl<void (T::*)()>(method);
        return f;
    }

    template <class T, class A>
    void call(void (T::*method)(A), const RefType(A) arg1)
    {
        functor_decl<void (T::*)(A)> *f = new functor_decl<void (T::*)(A)>(method);
        f->set(arg1);
        callable_ref ref = f;
        _call(ref);
    }

    template <class T, class A>
    static callable_ref call_prepare(void (T::*method)(A), const RefType(A) arg1)
    {
        functor_decl<void (T::*)(A)> *f = new functor_decl<void (T::*)(A)>(method);
        f->set(arg1);
        callable_ref ref = f;
        return ref;
    }

    template <class T, class A, class B>
    void call(void (T::*method)(A,B), const RefType(A) arg1, const RefType(B) arg2)
    {
        functor_decl<void (T::*)(A,B)> *f = new functor_decl<void (T::*)(A,B)>(method);
        f->set(arg1,arg2);
        callable_ref ref = f;
        _call(ref);
    }
    
    template <class T, class A, class B>
    static callable_ref call_prepare(void (T::*method)(A,B), const RefType(A) arg1, const RefType(B) arg2)
    {
        functor_decl<void (T::*)(A,B)> *f = new functor_decl<void (T::*)(A,B)>(method);
        f->set(arg1,arg2);
        callable_ref ref = f;
        return ref;
    }
    
    template <class T, class A, class B, class C>
    void call(void (T::*method)(A,B,C), const RefType(A) arg1, const RefType(B) arg2, const RefType(C) arg3)
    {
        functor_decl<void (T::*)(A,B,C)> *f = new functor_decl<void (T::*)(A,B,C)>(method);
        f->set(arg1,arg2,arg3);
        callable_ref ref = f;
        _call(ref);
    }
    
    template <class T, class A, class B, class C>
    static callable_ref call_prepare(void (T::*method)(A,B,C), const RefType(A) arg1, const RefType(B) arg2, const RefType(C) arg3)
    {
        functor_decl<void (T::*)(A,B,C)> *f = new functor_decl<void (T::*)(A,B,C)>(method);
        f->set(arg1,arg2,arg3);
        callable_ref ref = f;
        return ref;
    }

    template <class T, class A, class B, class C, class D>
    void call(void (T::*method)(A,B,C,D), const RefType(A) arg1, const RefType(B) arg2, const RefType(C) arg3, const RefType(D) arg4)
    {
        functor_decl<void (T::*)(A,B,C,D)> *f = new functor_decl<void (T::*)(A,B,C,D)>(method);
        f->set(arg1,arg2,arg3,arg4);
        callable_ref ref = f;
        _call(ref);
    }
    
    template <class T, class A, class B, class C, class D>
    static callable_ref call_prepare(void (T::*method)(A,B,C,D), const RefType(A) arg1, const RefType(B) arg2, const RefType(C) arg3, const RefType(D) arg4)
    {
        functor_decl<void (T::*)(A,B,C,D)> *f = new functor_decl<void (T::*)(A,B,C,D)>(method);
        f->set(arg1,arg2,arg3,arg4);
        callable_ref ref = f;
        return ref;
    }

    template <class T, class A, class B, class C, class D, class E>
    void call(void (T::*method)(A,B,C,D,E), const RefType(A) arg1, const RefType(B) arg2, const RefType(C) arg3, const RefType(D) arg4, const RefType(E) arg5)
    {
        functor_decl<void (T::*)(A,B,C,D,E)> *f = new functor_decl<void (T::*)(A,B,C,D,E)>(method);
        f->set(arg1,arg2,arg3,arg4,arg5);
        callable_ref ref = f;
        _call(ref);
    }
    
    template <class T, class A, class B, class C, class D, class E>
    static callable_ref call_prepare(void (T::*method)(A,B,C,D,E), const RefType(A) arg1, const RefType(B) arg2, const RefType(C) arg3, const RefType(D) arg4, const RefType(E) arg5)
    {
        functor_decl<void (T::*)(A,B,C,D,E)> *f = new functor_decl<void (T::*)(A,B,C,D,E)>(method);
        f->set(arg1,arg2,arg3,arg4,arg5);
        callable_ref ref = f;
        return ref;
    }
    
    template <class T, class A, class B, class C, class D, class E, class F>
    void call(void (T::*method)(A,B,C,D,E,F), const RefType(A) arg1, const RefType(B) arg2, const RefType(C) arg3, const RefType(D) arg4, const RefType(E) arg5, const RefType(F) arg6)
    {
        functor_decl<void (T::*)(A,B,C,D,E,F)> *f = new functor_decl<void (T::*)(A,B,C,D,E,F)>(method);
        f->set(arg1,arg2,arg3,arg4,arg5,arg6);
        callable_ref ref = f;
        _call(ref);
    }
    
    template <class T, class A, class B, class C, class D, class E, class F>
    static callable_ref call_prepare(void (T::*method)(A,B,C,D,E,F), const RefType(A) arg1, const RefType(B) arg2, const RefType(C) arg3, const RefType(D) arg4, const RefType(E) arg5, RefType(F) arg6)
    {
        functor_decl<void (T::*)(A,B,C,D,E,F)> *f = new functor_decl<void (T::*)(A,B,C,D,E,F)>(method);
        f->set(arg1,arg2,arg3,arg4,arg5,arg6);
        callable_ref ref = f;
        return ref;
    }
    
    template <class T, class A, class B, class C, class D, class E, class F, class G>
    void call(void (T::*method)(A,B,C,D,E,F,G), const RefType(A) arg1, const RefType(B) arg2, const RefType(C) arg3, const RefType(D) arg4, const RefType(E) arg5, const RefType(F) arg6,RefType(G) arg7)
    {
        functor_decl<void (T::*)(A,B,C,D,E,F,G)> *f = new functor_decl<void (T::*)(A,B,C,D,E,F,G)>(method);
        f->set(arg1,arg2,arg3,arg4,arg5,arg6,arg7);
        callable_ref ref = f;
        _call(ref);
    }
    
    template <class T, class A, class B, class C, class D, class E, class F, class G>
    static callable_ref call_prepare(void (T::*method)(A,B,C,D,E,F,G), const RefType(A) arg1, const RefType(B) arg2, const RefType(C) arg3, const RefType(D) arg4, const RefType(E) arg5, RefType(F) arg6, RefType(G) arg7)
    {
        functor_decl<void (T::*)(A,B,C,D,E,F,G)> *f = new functor_decl<void (T::*)(A,B,C,D,E,F,G)>(method);
        f->set(arg1,arg2,arg3,arg4,arg5,arg6,arg7);
        callable_ref ref = f;
        return ref;
    }
    
    template <class T, class A, class B, class C, class D, class E, class F, class G, class H>
    void call(void (T::*method)(A,B,C,D,E,F,G,H), const RefType(A) arg1, const RefType(B) arg2, const RefType(C) arg3, const RefType(D) arg4, const RefType(E) arg5, const RefType(F) arg6,RefType(G) arg7,RefType(H) arg8)
    {
        functor_decl<void (T::*)(A,B,C,D,E,F,G,H)> *f = new functor_decl<void (T::*)(A,B,C,D,E,F,G,H)>(method);
        f->set(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8);
        callable_ref ref = f;
        _call(ref);
    }

    template <class T, class A, class B, class C, class D, class E, class F, class G, class H>
    static callable_ref call_prepare(void (T::*method)(A,B,C,D,E,F,G,H), const RefType(A) arg1, const RefType(B) arg2, const RefType(C) arg3, const RefType(D) arg4, const RefType(E) arg5, RefType(F) arg6, RefType(G) arg7, RefType(H) arg8)
    {
        functor_decl<void (T::*)(A,B,C,D,E,F,G,H)> *f = new functor_decl<void (T::*)(A,B,C,D,E,F,G,H)>(method);
        f->set(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8);
        callable_ref ref = f;
        return ref;
    }
    
    template <class T, class A, class B, class C, class D, class E, class F, class G, class H, class I>
    void call(void (T::*method)(A,B,C,D,E,F,G,H,I), const RefType(A) arg1, const RefType(B) arg2, const RefType(C) arg3, const RefType(D) arg4, const RefType(E) arg5, const RefType(F) arg6,RefType(G) arg7,RefType(H) arg8,RefType(I) arg9)
    {
        functor_decl<void (T::*)(A,B,C,D,E,F,G,H,I)> *f = new functor_decl<void (T::*)(A,B,C,D,E,F,G,H,I)>(method);
        f->set(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9);
        callable_ref ref = f;
        _call(ref);
    }

    template <class T, class A, class B, class C, class D, class E, class F, class G, class H,class I>
    static callable_ref call_prepare(void (T::*method)(A,B,C,D,E,F,G,H,I), const RefType(A) arg1, const RefType(B) arg2, const RefType(C) arg3, const RefType(D) arg4, const RefType(E) arg5, RefType(F) arg6, RefType(G) arg7, RefType(H) arg8, RefType(I) arg9)
    {
        functor_decl<void (T::*)(A,B,C,D,E,F,G,H,I)> *f = new functor_decl<void (T::*)(A,B,C,D,E,F,G,H,I)>(method);
        f->set(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9);
        callable_ref ref = f;
        return ref;
    }
    

};

