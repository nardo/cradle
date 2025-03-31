/// Platform independent Mutual Exclusion implementation
class mutex
{
public:
    /// mutex constructor
    mutex()
    {
        #ifdef PLATFORM_WIN32
            InitializeCriticalSection(&_lock);
        #else
            pthread_mutexattr_t attr;
            pthread_mutexattr_init(&attr);
            #ifdef PLATFORM_MAC_OSX
                pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
            #else
                pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
            #endif
            pthread_mutex_init(&_mutex, &attr);
            pthread_mutexattr_destroy(&attr);
        #endif
    }
    /// mutex destructor
    ~mutex()
    {
        #ifdef PLATFORM_WIN32
            DeleteCriticalSection(&_lock);
        #else
            pthread_mutex_destroy(&_mutex);
        #endif
    }

    /// Locks the mutex.  If another thread already has this mutex locked, this call will block until it is unlocked.  If the lock method is called from a thread that has already locked this mutex, the call will not block and the thread will have to unlock the mutex for as many calls as were made to lock before another thread will be allowed to lock the mutex.
    void lock()
    {
        #ifdef PLATFORM_WIN32
            EnterCriticalSection(&_lock);
        #else
            pthread_mutex_lock(&_mutex);
        #endif
    }

    /// Unlocks the mutex.  The behavior of this method is undefined if called
    /// by a thread that has not previously locked this mutex.
    void unlock()
    {
        #ifdef PLATFORM_WIN32
            LeaveCriticalSection(&_lock);
        #else
            pthread_mutex_unlock(&_mutex);
        #endif
    }
private:
#ifdef PLATFORM_WIN32
    CRITICAL_SECTION _lock;
#else
    pthread_mutex_t _mutex;
#endif
};
