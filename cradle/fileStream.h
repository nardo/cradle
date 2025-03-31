// The cradle library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

class FileStream : public Stream
{
    FILE *mFile;
public:
    FileStream() { mFile = NULL; }
    virtual ~FileStream() { close(); }


   // mandatory methods from Stream base class...
   virtual bool hasCapability(const Capability i_cap) const
    {
       return(0 != (uint32(i_cap) & mStreamCaps));
    }

   virtual uint32  getPosition() const
    {
        if(!mFile)
            return 0;
        long curpos;
        curpos = ftell(mFile);
        return (uint32) curpos;
    }
     
   virtual bool setPosition(const uint32 i_newPosition)
    {
       if(!mFile)
           return false;
       fseek(mFile, i_newPosition, SEEK_SET);
       return true;
    }

   virtual uint32  getStreamSize()
    {
       if(!mFile)
           return 0;
       long curpos = ftell(mFile);
       fseek(mFile, 0, SEEK_END);
       long size = ftell(mFile);
       fseek(mFile, curpos, SEEK_SET);
       return (uint32) size;
    }


   // additional methods needed for a file stream...
   virtual bool open(const string &inFileName, const char *mode)
    {
       if(mFile)
       {
           fclose(mFile);
           mFile = NULL;
       }
       mFile = fopen(inFileName.c_str(), mode);
       
       return(mFile != NULL);
    }

   virtual void close()
    {
       if (mFile != NULL )
       {
           fclose(mFile);
           mFile = 0;
       }
    }


   bool flush()
    {
        if(mFile)
            fflush(mFile);
        return (mFile != NULL);
    }


protected:
   // more mandatory methods from Stream base class...
   virtual bool _read(const uint32 i_numBytes, void *o_pBuffer)
       {
           if(!mFile)
               return false;
           size_t bytes_read = fread(o_pBuffer,1, i_numBytes, mFile);
           return bytes_read == i_numBytes;
       }
   virtual bool _write(const uint32 i_numBytes, const void* i_pBuffer)
       {
           if(!mFile)
               return false;
       size_t bytes_written = fwrite(i_pBuffer, 1, i_numBytes, mFile);
       return bytes_written == i_numBytes;
       }

   uint32  mStreamCaps;                   // dependent on access mode
};
