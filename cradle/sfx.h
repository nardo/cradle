// The cradle library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

class SFXObject;
typedef ref_ptr<SFXObject> SFXHandle;

struct SFXProfile
{
    const char *fileName;
    bool        isRelative;
    float32         gainScale;
    bool        isLooping;
    float32         fullGainDistance;
    float32         zeroGainDistance;
};

struct SFXBufferArray
{
    array<ALuint> buffers;
    bool singleProfile;
    SFXProfile *profiles;
};

struct SFXManager
{
    enum {
        NumSources = 16,
    };
    ALCdevice *gDevice;
    ALCcontext *gContext;
    bool valid;
    
    ALuint gSources[NumSources];
    Point mListenerPosition;
    Point mListenerVelocity;
    float32 mMaxDistance;
    
    array<SFXBufferArray> buffers;
    array<ALuint> gVoiceFreeBuffers;
    
    array<SFXHandle> gPlayList;
    
    SFXManager()
    {
        gDevice = NULL;
        gContext = NULL;
        valid = false;
        
        mMaxDistance = 500;
    }
    SFXBufferArray *allocBuffers(uint32 bufferCount, SFXProfile *profiles, bool singleProfile)
    {
#ifndef AC_DEDICATED
        if(!valid)
            return NULL;
        buffers.increment();
        SFXBufferArray *ret = &buffers.last();
        ret->buffers.setSize(bufferCount);
        ret->profiles = profiles;
        ret->singleProfile = singleProfile;
        alGenBuffers(bufferCount, &ret->buffers[0]);
        return ret;
#endif
    }
    
    void shutdown()
    {
#ifndef AC_DEDICATED
        if(!valid)
            return;
        
        for(uint32 i = 0; i < buffers.size(); i++)
            alDeleteBuffers(buffers[i].buffers.size(), &buffers[i].buffers[0]);

        alcMakeContextCurrent(NULL);
        alcDestroyContext(A.sfx.gContext);
        alcCloseDevice(A.sfx.gDevice);
#endif
    }

    void init()
    {
#ifndef AC_DEDICATED
        ALint error;
#if defined ( PLATFORM_WIN32 )
        gDevice = alcOpenDevice((ALubyte *) "DirectSound3D");
#else
        gDevice = alcOpenDevice(NULL);
#endif
        if(!gDevice)
        {
            logf(cradle, ("Failed to intitialize OpenAL."));
            return;
        }
        
        static int contextData[][2] =
        {
            {ALC_FREQUENCY, 11025},
            {0,0} // Indicate end of list...
        };
        
        gContext = alcCreateContext(gDevice, (ALCint*)contextData);
        alcMakeContextCurrent(gContext);
        
        error = alGetError();
        
        gVoiceFreeBuffers.setSize(32);
        alGenBuffers(32, &gVoiceFreeBuffers[0]);
        
        error = alGetError();
        
        alDistanceModel(AL_NONE);
        error = alGetError();
        
        // load up all the sound buffers
        //if(error != AL_NO_ERROR)
        //   return;
        
        alGenSources(NumSources, gSources);
        
        valid = true;
#endif
    }

    void process()
    {
#ifndef AC_DEDICATED
        if(!A.sfx.valid)
            return;
        
        // ok, so we have a list of currently "playing" sounds, which is
        // unbounded in length, but only the top NumSources actually have sources
        // associtated with them.  Sounds are prioritized on a 0-1 scale
        // based on type and distance.
        // Each time through the main loop, newly played sounds are placed
        // on the process list.  When SFXProcess is called, any finished sounds
        // are retired from the list, and then it prioritizes and sorts all
        // the remaining sounds.  For any sounds from 0 to NumSources that don't
        // have a current source, they grab one of the sources not used by the other
        // top sounds.  At this point, any sound that is not looping, and is
        // not in the active top list is retired.
        
        // ok, look through all the currently playing sources and see which
        // ones need to be retired:
        
        bool sourceActive[NumSources];
        for(int32 i = 0; i < NumSources; i++)
        {
            ALint state;
            unqueueBuffers(i);
            alGetSourcei(gSources[i], AL_SOURCE_STATE, &state);
            sourceActive[i] = state != AL_STOPPED && state != AL_INITIAL;
        }
        for(int32 i = 0; i < gPlayList.size(); )
        {
            SFXHandle &s = gPlayList[i];
            
            if(s->mSourceIndex != -1 && !sourceActive[s->mSourceIndex])
            {
                // this sound was playing; now it is stopped,
                // so remove it from the list.
                s->mSourceIndex = -1;
                gPlayList.erase_fast(i);
            }
            else
            {
                // compute a priority for this sound.
                if(!s->mProfile->isRelative)
                    s->mPriority = (500 - (s->mPosition - mListenerPosition).len()) / 500.0f;
                else
                    s->mPriority = 1.0;
                i++;
            }
        }
        // now, bubble sort all the sounds up the list:
        // we choose bubble sort, because the list should
        // have a lot of frame-to-frame coherency, making the
        // sort most often O(n)
        for(int32 i = 1; i < gPlayList.size(); i++)
        {
            float32 priority = gPlayList[i]->mPriority;
            for(int32 j = i - 1; j >= 0; j--)
            {
                if(priority > gPlayList[j]->mPriority)
                {
                    SFXHandle temp = gPlayList[j];
                    gPlayList[j] = gPlayList[j+1];
                    gPlayList[j+1] = temp;
                }
            }
        }
        // last, release any sources and get rid of non-looping sounds
        // outside our max sound limit
        for(int32 i = NumSources; i < gPlayList.size(); )
        {
            SFXHandle &s = gPlayList[i];
            if(s->mSourceIndex != -1)
            {
                sourceActive[s->mSourceIndex] = false;
                s->mSourceIndex = -1;
            }
            if(!s->mProfile->isLooping)
                gPlayList.erase_fast(i);
            else
                i++;
        }
        // now assign sources to all our sounds that need them:
        int32 firstFree = 0;
        int32 max = NumSources;
        if(max > gPlayList.size())
            max = gPlayList.size();
        
        for(int32 i = 0; i < max; i++)
        {
            SFXHandle &s = gPlayList[i];
            if(s->mSourceIndex == -1)
            {
                while(sourceActive[firstFree])
                    firstFree++;
                s->mSourceIndex = firstFree;
                sourceActive[firstFree] = true;
                s->playOnSource();
            }
            else
            {
                // for other sources, just attenuate the gain.
                s->updateGain();
            }
        }
#endif
    }
    
    void unqueueBuffers(int32 sourceIndex)
    {
        // free up any played buffers from this source.
        if(sourceIndex != -1)
        {
            ALint processed;
            alGetError();
            
            alGetSourcei(gSources[sourceIndex], AL_BUFFERS_PROCESSED, &processed);
            
            while(processed)
            {
                ALuint buffer;
                alSourceUnqueueBuffers(gSources[sourceIndex], 1, &buffer);
                if(alGetError() != AL_NO_ERROR)
                    return;
                
                //logf(cradle, ("unqueued buffer %d\n", buffer));
                processed--;
                
                // ok, this is a lame solution - but the way OpenAL should work is...
                // you should only be able to unqueue buffers that you queued - duh!
                // otherwise it's a bitch to manage sources that can either be streamed
                // or already loaded.
                uint32 i;
                bool found = false;
                
                for(uint32 j = 0; j < buffers.size(); j++)
                {
                    for(i = 0 ; i < buffers[j].buffers.size(); i++)
                    {
                        if(buffer == buffers[j].buffers[i])
                        {
                            found = true;
                            break;
                        }
                    }
                    if(found)
                        break;
                }
                if(!found)
                    gVoiceFreeBuffers.push_back(buffer);
            }
        }
    }

    static bool loadWav(const char *fileBuffer, ALuint buffer)
    {
        ALsizei freq;
        ALenum   format;
        ALboolean loop;
        SDL_AudioSpec spec;
        uint8 *audio_buf;
        uint32 audio_len;
        
        
        if(!SDL_LoadWAV(fileBuffer, &spec, &audio_buf, &audio_len))
        {
            logf(cradle, ("Failure (1) loading sound file '%s'", fileBuffer));
            return false;
        }
        switch(spec.format)
        {
            case SDL_AUDIO_S8:
            case SDL_AUDIO_U8:
                format = AL_FORMAT_MONO8;
                break;
            case SDL_AUDIO_S16:
                format = AL_FORMAT_MONO16;
                break;
            default:
                logf(cradle, ("Failure (1.5) loading sound file '%s - unknown format %d'", fileBuffer, spec.format));
                return false;
        }
        
        alBufferData(buffer, format, audio_buf, audio_len, spec.freq);
        SDL_free(audio_buf);
        if(alGetError() != AL_NO_ERROR)
        {
            logf(cradle, ("Failure (2) loading sound file '%s'", fileBuffer));
            return false;
        }
        return true;
    }
    void setListenerParams(Point position, Point velocity)
    {
#ifndef AC_DEDICATED
        if(!valid)
            return;
        
        mListenerPosition = position;
        mListenerVelocity = velocity;
        alListener3f(AL_POSITION, position.x, position.y, -mMaxDistance/2);
#endif
    }
    

} sfx;

class SFXObject : public ref_object
{
    friend class SFXManager;
    
    ALuint mBuffer;
    Point mPosition;
    Point mVelocity;
    byte_buffer_ptr mInitialBuffer;
    SFXProfile *mProfile;
    float32 mGain;
    int32 mSourceIndex;
    float32 mPriority;
    void playOnSource()
    {
#ifndef AC_DEDICATED
        ALuint source = A.sfx.gSources[mSourceIndex];
        alSourceStop(source);
        A.sfx.unqueueBuffers(mSourceIndex);
        
        if(mInitialBuffer.is_valid())
        {
            if(!A.sfx.gVoiceFreeBuffers.size())
                return;
            
            ALuint buffer = A.sfx.gVoiceFreeBuffers.first();
            A.sfx.gVoiceFreeBuffers.pop_front();
            
            alBufferData(buffer, AL_FORMAT_MONO16, mInitialBuffer->get_buffer(),
                         mInitialBuffer->get_buffer_size(), 8000);
            alSourceQueueBuffers(source, 1, &buffer);
        }
        else
            alSourcei(source, AL_BUFFER, mBuffer);
        
        alSourcei(source, AL_LOOPING, mProfile->isLooping);
#ifdef PLATFORM_MAC_OSX
        // This is a workaround for the broken OS X implementation of AL_NONE distance model.
        alSourcef(source, AL_REFERENCE_DISTANCE,9000);
        alSourcef(source, AL_ROLLOFF_FACTOR,1);
        alSourcef(source, AL_MAX_DISTANCE, 10000);
#endif
        updateMovementParams();
        
        updateGain();
        alSourcePlay(source);
#endif
    }
    
    
    void updateGain()
    {
#ifndef AC_DEDICATED
        ALuint source = A.sfx.gSources[mSourceIndex];
        float32 gain = mGain;
        
        if(!mProfile->isRelative)
        {
            float32 distance = (A.sfx.mListenerPosition - mPosition).len();
            if(distance > mProfile->fullGainDistance)
            {
                if(distance < mProfile->zeroGainDistance)
                    gain *= 1 - (distance - mProfile->fullGainDistance) /
                    (mProfile->zeroGainDistance - mProfile->fullGainDistance);
                else
                    gain = 0.0f;
            }
            else
                gain = 1.0f;
        }
        else
            gain = 1.0f;
        
        alSourcef(source, AL_GAIN, gain * mProfile->gainScale);
#endif
    }
    
    void updateMovementParams()
    {
#ifndef AC_DEDICATED
        ALuint source = A.sfx.gSources[mSourceIndex];
        if(mProfile->isRelative)
        {
            alSourcei(source, AL_SOURCE_RELATIVE, true);
            alSource3f(source, AL_POSITION, 0, 0, 0);
            //alSource3f(source, AL_VELOCITY, 0, 0, 0);
        }
        else
        {
            alSourcei(source, AL_SOURCE_RELATIVE, false);
            alSource3f(source, AL_POSITION, mPosition.x, mPosition.y, 0);
            //alSource3f(source, AL_VELOCITY, mVelocity.x, mVelocity.y, 0);
        }
#endif
    }
    
public:
    SFXObject(SFXProfile *profile, byte_buffer_ptr samples, ALuint buffer, float32 gain, Point position, Point velocity)
    {
        mBuffer = buffer;
        mProfile = profile;
        mGain = gain;
        mPosition = position;
        mVelocity = velocity;
        mSourceIndex = -1;
        mPriority = 0;
        mInitialBuffer = samples;
    }
    
    ~SFXObject()
    {
        
    }
    
    void setGain(float32 gain)
    {
        if(!A.sfx.valid)
            return;
        
        mGain = gain;
        if(mSourceIndex != -1)
            updateGain();
    }
    
    void play()
    {
#ifndef AC_DEDICATED
        if(!A.sfx.valid)
            return;
        
        if(mSourceIndex != -1)
            return;
        else
        {
            // see if it's on the play list:
            for(int32 i = 0; i < A.sfx.gPlayList.size(); i++)
                if(this == A.sfx.gPlayList[i].get_pointer())
                    return;
            A.sfx.gPlayList.push_back(this);
        }
#endif
    }
    
    void stop()
    {
#ifndef AC_DEDICATED
        if(!A.sfx.valid)
            return;
        
        // remove from the play list, if this sound is playing:
        if(mSourceIndex != -1)
        {
            alSourceStop(A.sfx.gSources[mSourceIndex]);
            mSourceIndex = -1;
        }
        for(int32 i = 0; i < A.sfx.gPlayList.size(); i++)
        {
            if(A.sfx.gPlayList[i].get_pointer() == this)
            {
                A.sfx.gPlayList.erase(i);
                return;
            }
        }
#endif
    }
    
    void setMovementParams(Point position, Point velocity)
    {
        if(!A.sfx.valid)
            return;
        
        mPosition = position;
        mVelocity = velocity;
        if(mSourceIndex != -1)
            updateMovementParams();
    }
    
    void queueBuffer(byte_buffer_ptr b)
    {
#ifndef AC_DEDICATED
        if(!A.sfx.valid)
            return;
        
        if(!b->get_buffer_size())
            return;
        
        mInitialBuffer = b;
        if(mSourceIndex != -1)
        {
            if(!A.sfx.gVoiceFreeBuffers.size())
                return;
            
            ALuint source = A.sfx.gSources[mSourceIndex];
            ALuint buffer = A.sfx.gVoiceFreeBuffers.first();
            A.sfx.gVoiceFreeBuffers.pop_front();
            
            int16 max = 0;
            int16 *ptr = (int16 *) b->get_buffer();
            uint32 count = b->get_buffer_size() / 2;
            while(count--)
            {
                if(max < *ptr)
                    max = *ptr;
                ptr++;
            }
            
            //logf(cradle, ("queued buffer %d - %d max %d len\n", buffer, max, mInitialBuffer->getBufferSize()));
            alBufferData(buffer, AL_FORMAT_MONO16, mInitialBuffer->get_buffer(),
                         mInitialBuffer->get_buffer_size(), 8000);
            alSourceQueueBuffers(source, 1, &buffer);
            
            ALint state;
            alGetSourcei(mSourceIndex, AL_SOURCE_STATE, &state);
            if(state == AL_STOPPED)
                alSourcePlay(mSourceIndex);
        }
        else
            play();
#endif
    }
    
    bool isPlaying() { return mSourceIndex != -1; }
    
    static bool startRecording()
    {
        return false;
    }
    
    static void captureSamples(byte_buffer_ptr sampleBuffer)
    {
    }
    
    
    static void stopRecording()
    {
    }
    
    
    static void setMaxDistance(float32 maxDistance);
    

    static ref_ptr<SFXObject> play(uint32 profileIndex, float32 gain = 1.0f)
    {
        assert_msg(A.sfx.buffers.size() > 0, "no buffers!");
        SFXBufferArray &buf = A.sfx.buffers[0];
        assert_msg(profileIndex < buf.buffers.size(), "profile out of range!");
        assert_msg(buf.singleProfile == false, "SFXObject::play requires a profile per sound.");
        
        ref_ptr<SFXObject> ret = new SFXObject(buf.profiles + profileIndex, NULL, buf.buffers[profileIndex], gain, Point(), Point());
        ret->play();
        return ret;
    }
    
    static ref_ptr<SFXObject> play(uint32 profileIndex, Point position, Point velocity, float32 gain = 1.0f)
    {
        assert_msg(A.sfx.buffers.size() > 0, "no buffers!");
        SFXBufferArray &buf = A.sfx.buffers[0];
        assert_msg(profileIndex < buf.buffers.size(), "profile out of range!");
        assert_msg(buf.singleProfile == false, "SFXObject::play requires a profile per sound.");

        ref_ptr<SFXObject> ret = new SFXObject(buf.profiles + profileIndex, NULL,  buf.buffers[profileIndex], gain, position, velocity);
        ret->play();
        return ret;
    }
    
    static ref_ptr<SFXObject> playRecordedBuffer(byte_buffer_ptr b, float32 gain = 1.0f)
    {
        ref_ptr<SFXObject> ret = new SFXObject(0, b, 0, gain, Point(), Point());
        ret->play();
        return ret;
    }

    static ref_ptr<SFXObject> playVoice(uint32 voicePack, uint32 voiceIndex, float32 gain = 1.0)
    {
        assert_msg(A.sfx.buffers.size() > voicePack, "no buffers!");
        SFXBufferArray &buf = A.sfx.buffers[voicePack + 1];
        assert_msg(voiceIndex < buf.buffers.size(), "voice out of range!");
        SFXProfile *prof;
        if(buf.singleProfile)
            prof = buf.profiles;
        else
            prof = buf.profiles + voiceIndex;

        ref_ptr<SFXObject> ret = new SFXObject(prof, NULL, buf.buffers[voiceIndex], gain, Point(), Point());
        ret->play();
        return ret;
    }
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif
