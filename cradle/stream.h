// The cradle library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

/// @defgroup stream_overload Primitive Type Stream Operation Overloads
/// These macros declare the read and write functions for all primitive types.
/// @{
#define IMPLEMENT_OVERLOADED_READ(type)      \
   bool read(type* out_read)         \
   {                                         \
      return read(sizeof(type), out_read);   \
   }

#define IMPLEMENT_OVERLOADED_WRITE(type)        \
   bool write(type in_write)            \
   {                                            \
      return write(sizeof(type), &in_write);    \
   }

#define IMPLEMENT_ENDIAN_OVERLOADED_READ(type)  \
   bool read(type* out_read)            \
   {                                            \
      type temp;                                \
      bool success = read(sizeof(type), out_read); \
      little_endian_to_host(*out_read);   \
      return success;                           \
   }

#define IMPLEMENT_ENDIAN_OVERLOADED_WRITE(type)    \
   bool write(type in_write)               \
   {                                               \
      host_to_little_endian(in_write);  \
      return write(sizeof(type), &in_write);           \
   }

/// @}

/// Base stream class for streaming data across a specific media
class Stream
{
   // Public structs and enumerations...
public:
     /// Status constants for the stream
   enum Status {
      Ok = 0,           ///< Ok!
      IOError,          ///< Read or Write error
      EOS,              ///< End of Stream reached (mostly for reads)
      IllegalCall,      ///< An unsupported operation used.  Always w/ accompanied by AssertWarn
      Closed,           ///< Tried to operate on a closed stream (or detached filter)
      UnknownError      ///< Catchall
   };

   enum Capability {
      StreamWrite    = bit(0), ///< Can this stream write?
      StreamRead     = bit(1), ///< Can this stream read?
      StreamPosition = bit(2)  ///< Can this stream position?
   };

   // Accessible only through inline accessors
private:
   Status m_streamStatus;

   // Derived accessible data modifiers...
protected:
   void setStatus(const Status in_newStatus) { m_streamStatus = in_newStatus; }

public:
    Stream() : m_streamStatus(Closed) {}
   virtual ~Stream() {}

   /// Gets the status of the stream
   Stream::Status getStatus() const { return m_streamStatus; }
   /// Gets a printable string form of the status
   static const char* getStatusString(const Status in_status)
    {
       switch (in_status) {
          case Ok:
             return "StreamOk";
          case IOError:
             return "StreamIOError";
          case EOS:
             return "StreamEOS";
          case IllegalCall:
             return "StreamIllegalCall";
          case Closed:
             return "StreamClosed";
          case UnknownError:
             return "StreamUnknownError";

         default:
          return "Invalid Stream::Status";
       }
    }



   // Derived classes must override these...
protected:
   virtual bool _read(const uint32 in_numBytes,  void* out_pBuffer)      = 0;
   virtual bool _write(const uint32 in_numBytes, const void* in_pBuffer) = 0;

   /*virtual void _write(const String & str);
   virtual void _read(String * str);*/


public:
     /// Checks to see if this stream has the capability of a given function
   virtual bool hasCapability(const Capability) const = 0;

   /// Gets the position in the stream
   virtual uint32  getPosition() const                      = 0;
   /// Sets the position of the stream.  Returns if the new position is valid or not
   virtual bool setPosition(const uint32 in_newPosition) = 0;
   /// Gets the size of the stream
   virtual uint32  getStreamSize() = 0;

   /// Reads a line from the stream.
   /// @param buffer buffer to be read into
   /// @param bufferSize max size of the buffer.  Will not read more than the "bufferSize"
   void readLine(uint8 *buffer, uint32 bufferSize)
    {
       bufferSize--;  // account for NULL terminator
       uint8 *buff = buffer;
       uint8 *buffEnd = buff + bufferSize;
       *buff = '\r';

       // strip off preceding white space
       while ( *buff == '\r' )
       {
          if ( !read(buff) || *buff == '\n' )
          {
             *buff = 0;
             return;
          }
       }

       // read line
       while ( buff != buffEnd && read(++buff) && *buff != '\n' )
       {
          if ( *buff == '\r' )
          {

    #if defined(PLATFORM_MAC_OSX)
          uint32 pushPos = getPosition(); // in case we need to back up.
          if (read(buff)) // feeling free to overwrite the \r as the NULL below will overwrite again...
              if (*buff != '\n') // then push our position back.
                 setPosition(pushPos);
           break; // we're always done after seeing the CR...
    #else
          buff--; // 'erases' the CR of a CRLF
    #endif

          }
       }
       *buff = 0;
    }


   /// writes a line to the stream
   void writeLine(const uint8 *buffer)
    {
       write((uint32) strlen((const char *)buffer), buffer);
       write(2, "\r\n");
    }


   /// Reads a string of maximum 255 characters long
   virtual void readString(char stringBuf[256])
    {
       uint8 len;
       read(&len);
       read(int32(len), stringBuf);
       stringBuf[len] = 0;
    }

   /// Reads a string that could potentially be more than 255 characters long.
   /// @param maxStringLen Maximum length to read.  If the string is longer than maxStringLen, only maxStringLen bytes will be read.
   /// @param stringBuf buffer where data is read into
   
    void readLongString(uint32 maxStringLen, char *stringBuf)
    {
       uint32 len;
       read(&len);
       if(len > maxStringLen)
       {
          m_streamStatus = IOError;
          return;
       }
       read(len, stringBuf);
       stringBuf[len] = 0;
    }

    /// Writes a string to the stream.  This function is slightly unstable.
   /// Only use this if you have a valid string that is not empty.
   /// writeString is safer.
   void writeLongString(uint32 maxStringLen, const char *string)
    {
       uint32 len = (uint32) strlen(string);
       if(len > maxStringLen)
          len = maxStringLen;
       write(len);
       write(len, string);
    }

   /// Writes a string to the stream.
   virtual void writeString(const char *string, int32 maxLen=255)
    {
       uint32 len = string ? uint32(strlen(string)) : 0;
       if(len > maxLen)
          len = maxLen;

       write(uint8(len));
       if(len)
          write(len, string);
    }


/*
   // read/write real strings
   void write(const String & str) { _write(str); }
   void read(String * str) { _read(str); }

   /// Write some raw data onto the stream.
   bool write(const RawData &);
   /// Read some raw data from the stream.
   bool read(RawData *);*/

   // Overloaded write and read ops..
  public:
   bool read(const uint32 in_numBytes,  void* out_pBuffer) {
      return _read(in_numBytes, out_pBuffer);
   }
   bool write(const uint32 in_numBytes, const void* in_pBuffer) {
      return _write(in_numBytes, in_pBuffer);
   }

    IMPLEMENT_OVERLOADED_WRITE(int8)
    IMPLEMENT_OVERLOADED_WRITE(uint8)

    IMPLEMENT_ENDIAN_OVERLOADED_WRITE(int16)
    IMPLEMENT_ENDIAN_OVERLOADED_WRITE(int32)
    IMPLEMENT_ENDIAN_OVERLOADED_WRITE(uint16)
    IMPLEMENT_ENDIAN_OVERLOADED_WRITE(uint32)
    IMPLEMENT_ENDIAN_OVERLOADED_WRITE(float32)
    IMPLEMENT_ENDIAN_OVERLOADED_WRITE(float64)

    IMPLEMENT_OVERLOADED_READ(int8)
    IMPLEMENT_OVERLOADED_READ(uint8)

    IMPLEMENT_ENDIAN_OVERLOADED_READ(int16)
    IMPLEMENT_ENDIAN_OVERLOADED_READ(int32)
    IMPLEMENT_ENDIAN_OVERLOADED_READ(uint16)
    IMPLEMENT_ENDIAN_OVERLOADED_READ(uint32)
    IMPLEMENT_ENDIAN_OVERLOADED_READ(float32)
    IMPLEMENT_ENDIAN_OVERLOADED_READ(float64)

   // We have to do the bool's by hand, since they are different sizes
   //  on different compilers...
   //
   bool read(bool* out_pRead) {
      uint8 translate;
      bool success = read(&translate);
      if (success == false)
         return false;

      *out_pRead = translate != 0;
      return true;
   }
   bool write(const bool& in_rWrite) {
      uint8 translate = in_rWrite ? uint8(1) : uint8(0);
      return write(translate);
   }

   /// Copy the contents of another stream into this one
   bool copyFrom(Stream *other)
    {
       uint8 buffer[1024];
       uint32 numBytes = other->getStreamSize() - other->getPosition();
       while((other->getStatus() != Stream::EOS) && numBytes > 0)
       {
          uint32 numRead = numBytes > sizeof(buffer) ? sizeof(buffer) : numBytes;
          if(! other->read(numRead, buffer))
             return false;

          if(! write(numRead, buffer))
             return false;

          numBytes -= numRead;
       }

       return true;
    }


   /// Write a number of tabs to this stream
   void writeTabs(uint32 count)
   {
      char tab[] = "   ";
      while(count--)
         write(3, (void*)tab);
   }
    bool read(ColorI* pColor)
    {
       bool success = read(&pColor->red);
       success     |= read(&pColor->green);
       success     |= read(&pColor->blue);
       success     |= read(&pColor->alpha);

       return success;
    }

    bool write(const ColorI& rColor)
    {
       bool success = write(rColor.red);
       success     |= write(rColor.green);
       success     |= write(rColor.blue);
       success     |= write(rColor.alpha);

       return success;
    }

};
