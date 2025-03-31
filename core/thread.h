// The core library - copyright KAGR LLC, portions copyright GarageGames. The use of this source code is governed by the MIT software license described in the "license.txt" file in this directory.

/// Platform independent semaphore class.
///
/// The semaphore class wraps OS specific semaphore functionality for thread synchronization.
class semaphore
{
public:
	/// semaphore constructor - initialCount specifies how many wait calls
	/// will be let through before an increment is required.
	semaphore(uint32 initial_count = 0, uint32 maximum_count = 1024)
	{
		#ifdef PLATFORM_WIN32
			_semaphore = CreateSemaphore(NULL, initial_count, maximum_count, NULL);
		//#elif defined(PLATFORM_MAC_OSX)
		//	MPCreateSemaphore(maximum_count, initial_count, &_semaphore);
		#elif defined(PLATFORM_MAC_OSX)
			_semaphore = sem_open("this_is_a_core_semaphore", O_CREAT);
			sem_unlink("this_is_a_core_semaphore");
		#else
			sem_init(&_semaphore, 0, initial_count);
		#endif
	}
	~semaphore()
	{
		#ifdef PLATFORM_WIN32
			CloseHandle(_semaphore);
		#elif defined(PLATFORM_MAC_OSX)
			sem_close(_semaphore);
		#else
			sem_destroy(&_semaphore);
		#endif
	}

	/// Thread calling wait will block as long as the semaphore's count
	/// is zero.  If the semaphore is incremented, one of the waiting threads
	/// will be awakened and the semaphore will decrement.
	void wait()
	{
		#ifdef PLATFORM_WIN32
			WaitForSingleObject(_semaphore, INFINITE);
		#elif defined(PLATFORM_MAC_OSX)
			sem_wait(_semaphore);
		//	MPWaitOnSemaphore(_semaphore, kDurationForever);
		#else
			sem_wait(&_semaphore);
		#endif
	}
	/// Increments the semaphore's internal count.  This will wake
	/// count threads that are waiting on this semaphore.
	void increment(uint32 count = 1)
	{
		#ifdef PLATFORM_WIN32
			ReleaseSemaphore(_semaphore, count, NULL);
		#elif defined(PLATFORM_MAC_OSX)
			sem_post(_semaphore);
		//	for(uint32 i = 0; i < count; i++)
		//		MPSignalSemaphore(_semaphore);
		#else
			for(uint32 i = 0; i < count; i++)
				sem_post(&_semaphore);
		#endif
	}

private:
	#ifdef PLATFORM_WIN32
		HANDLE _semaphore;
	#elif defined(PLATFORM_MAC_OSX)
		sem_t *_semaphore;
	#else
		sem_t _semaphore;
	#endif
};



/// Platform independent thread class.
class thread : public ref_object
{
public:
	/// thread constructor.
	thread()
	{
		_thread_running = false;
	}
	/// thread destructor.
	~thread()
	{
		if(_thread_running)
		{
			#ifdef PLATFORM_WIN32
				CloseHandle(_thread);
			#else
				pthread_cancel(_thread);
			#endif
		}
	}

	/// run function called when thread is started.
	virtual uint32 run()
	{
		return 0;
	}
	
	bool is_running()
	{
		return _thread_running;
	}

	/// starts the thread's main run function.
	void start()
	{
		_thread_running = true;
		#ifdef PLATFORM_WIN32
			_thread = CreateThread(NULL, 0, thread_proc, this, 0, NULL);
			_return_value = 0;		
		#else
			if(pthread_create(&_thread, NULL, thread_proc, this) != 0)
			{
				// handle error
			}
			_return_value = 0;		
		#endif
	}
protected:
	uint32 _return_value; ///< Return value from thread function
	bool _thread_running;
	
#ifdef PLATFORM_WIN32
	HANDLE _thread;
	static DWORD WINAPI thread_proc( LPVOID lp_parameter )
#else
	pthread_t _thread;
	static void *thread_proc(void *lp_parameter)
#endif
	{
		((thread *) lp_parameter)->_return_value = ((thread *) lp_parameter)->run();
		((thread *) lp_parameter)->_thread_running = false;
		return 0;
	}
};

/// Platform independent per-thread storage class.
class thread_storage
{
public:
	/// thread_storage constructor.
	thread_storage()
	{
		#ifdef PLATFORM_WIN32
			_tls_index = TlsAlloc();
		#else
			pthread_key_create(&_thread_key, NULL);
		#endif
	}

	/// thread_storage destructor.
	~thread_storage()
	{
		#ifdef PLATFORM_WIN32
			TlsFree(_tls_index);
		#else
			pthread_key_delete(_thread_key);
		#endif
	}

	/// returns the per-thread stored void pointer for this thread_storage.  The default value is NULL.
	void *get()
	{
		#ifdef PLATFORM_WIN32
			return TlsGetValue(_tls_index);
		#else
			return pthread_getspecific(_thread_key);
		#endif
	}
	/// sets the per-thread stored void pointer for this thread_storage object.
	void set(void *value)
	{
		#ifdef PLATFORM_WIN32
			TlsSetValue(_tls_index, value);
		#else
			pthread_setspecific(_thread_key, value);
		#endif
	}
private:
	#ifdef PLATFORM_WIN32
		DWORD _tls_index;
	#else
		pthread_key_t _thread_key;
	#endif
};

template <class T> struct thread_storage_ptr
{
    static thread_storage *_storage()
    {
        static thread_storage ts;
        return &ts;
    }
    static void set(T *val)
    {
        thread_storage *s = _storage();
        s->set(val);
    }
    static T *get()
    {
        thread_storage *s = _storage();
        return (T*) s->get();
    }
};
