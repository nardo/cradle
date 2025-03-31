// The core library - copyright KAGR LLC, portions copyright GarageGames. The use of this source code is governed by the MIT software license described in the "license.txt" file in this directory.

#define CRADLE_ENABLE_LOGGING

class log_consumer
{
    log_consumer *_next; ///< Next log_consumer in the global linked list of log consumers.
public:
    static log_consumer **linked_list()
    {
        static log_consumer *the_list = 0;
        return &the_list;
    }
   /// Constructor adds this log_consumer to the global linked list.
    log_consumer()
    {
        log_consumer **list = linked_list();
        _next = *list;
        *list = this;
    }

   /// Destructor removes this log_consumer from the global linked list, and updates the log flags.
   virtual ~log_consumer()
    {
       for(log_consumer **list = linked_list(); *list; list = &(*list)->_next)
       {
           if(*list == this)
           {
               *list = _next;
               return;
           }
       }
   }

    /// Returns the next log_consumer in the linked list.
    log_consumer *next() { return _next; }

   /// Writes a string to this instance of log_consumer.
   virtual void log_c_str(const char *string)
   {
       fprintf(stderr, "%s\n", string);
       fflush(stderr);
   }
};

// default stdout log consumer class
class stdout_log_consumer : public log_consumer
{
public:
    void log_c_str(const char *string)
    {
        printf("%s\n", string);
    }
};

// file log consumer class
class file_log_consumer : public log_consumer
{
private:
    FILE *f;
public:
    file_log_consumer(const char *header, const char* logFile)
    {
        f = fopen(logFile, "w");
        log_c_str(header);
    }
    
    ~file_log_consumer()
    {
        if(f)
            fclose(f);
    }
    
    void log_c_str(const char *string)
    {
        if(f)
        {
            fprintf(f, "%s\n", string);
            fflush(f);
        }
    }
};


struct log_type
{
    log_type *_next;
    
    static log_type **linked_list()
    {
       static log_type *the_list = 0;
       return &the_list;
    }
    bool _enabled;
    const char *_type_name;
    static log_type **current()
    {
        static log_type *the_type = 0;
        return &the_type;
    }

#ifdef CRADLE_ENABLE_LOGGING
    static void set_current(log_type *the_type)
    {
        log_type **cur = current();
        *cur = the_type;
    }
   static log_type *find(const char *name, log_type *storage)
    {
       log_type **list = linked_list();
       for(log_type *walk = *list; walk; walk = walk->_next)
          if(!strcmp(walk->_type_name, name))
             return walk;
       
       storage->_next = *list;
       storage->_enabled = false;
       storage->_type_name = name;
       *list = storage;
       return storage;
   }
#endif
};
#ifdef CRADLE_ENABLE_LOGGING

struct log_type_ref
{
    log_type _storage;
    log_type *_log_type;
    log_type_ref(const char *name)
    {
        _log_type = log_type::find(name, &_storage);
    }
};

///   LogConnectionProtocol,
///   LogNetConnection,
///   LogEventConnection,
///   LogGhostConnection,
///   LogNetInterface,
///   LogPlatform,


/// Logs a printf-style variable argument message of the specified type to the currently active log_consumers.
#define logf(log_type_token, message) { static log_type_ref the_ref(#log_type_token); if(the_ref._log_type->_enabled) { log_type::set_current(the_ref._log_type); _logprintf message ; log_type::set_current(NULL); } }

#define log_enable(log_type_token, enabled) { static log_type_ref the_ref(#log_type_token); the_ref._log_type->_enabled = enabled; }

#define log_block(log_type_token, code) { static log_type_ref the_ref(#log_type_token); if(the_ref._log_type->_enabled) { log_type::set_current(the_ref._log_type); { code } log_type::set_current(NULL); } }

#else
#define logf(logType, message) {  }
#define log_enable(logType, enabled) { }
#define log_block(logType, code) { }
#endif

static const char *&_log_prefix()
{
    static const char* _log_prefix = "LOG";
    return _log_prefix;
}

static void _logprintf(const char *format, ...)
{
    char buffer[4096];
    uint32 bufferStart = 0;
    const char *prefix = _log_prefix();
    const char *cur_type_name = "";
    log_type *cur_type = *log_type::current();
    if(cur_type)
        cur_type_name = cur_type->_type_name;
    bufferStart = snprintf(buffer, sizeof(buffer), "%s:%s ", prefix, cur_type_name);
    
    va_list s;
    va_start( s, format );
    vsnprintf(buffer + bufferStart, sizeof(buffer) - bufferStart, format, s);
    va_end(s);
    
    for(log_consumer *walk = *log_consumer::linked_list(); walk; walk = walk->next())
        walk->log_c_str(buffer);
}

