// The cradle library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

/// The VoiceEncoder class is an abstract base class for the various
/// voice compression algorithms supported by AthenaCore - initially just the
/// HawkVoice variable bitrate LPC10 codec.  It takes an arbitrary buffer
/// of 16 bit samples at 8KHz and returns a compressed buffer.  The 
/// original buffer is modified to contain any samples that were not used 
/// due to unfilled frame sizing.
class VoiceEncoder : public ref_object
{
    virtual uint32 getSamplesPerFrame() = 0;
    virtual uint32 getMaxCompressedFrameSize() = 0;
    virtual uint32 compressFrame(int16 *samplePtr, uint8 *outputPtr) = 0;
public:
    byte_buffer_ptr compressBuffer(byte_buffer_ptr sampleBuffer)
    {
        uint32 sampleCount = sampleBuffer->get_buffer_size() >> 1;
        uint32 spf = getSamplesPerFrame();
        
        uint32 framesUsed = uint32(sampleCount / spf);
        if(framesUsed)
        {
            uint32 samplesUsed = framesUsed * spf;
            uint32 maxSize = getMaxCompressedFrameSize() * framesUsed;
            byte_buffer_ptr ret = new byte_buffer(maxSize);
            
            uint8 *compressedPtr = ret->get_buffer();
            int16 *samplePtr = (int16 *) sampleBuffer->get_buffer();
            
            uint32 len = 0;
            
            for(uint32 i = 0; i < samplesUsed; i += spf)
                len += compressFrame(samplePtr + i, compressedPtr + len);
            
            ret->resize(len);
            
            uint32 newSize = (sampleCount - samplesUsed) * sizeof(uint16);
            memcpy(samplePtr, samplePtr + samplesUsed, newSize);
            
            sampleBuffer->resize(newSize);
            return ret;
        }
        return NULL;
    }
    
};

/// The VoiceDecoder class is an abstract base class for the various
/// voice decompression algorithms supported by AthenaCore - initially just the
/// HawkVoice variable bitrate LPC10 codec.  It takes a buffer compressed
/// with the appropriate VoiceEncoder analogue and returns the decompressed
/// 16 bit sample buffer.
class VoiceDecoder : public ref_object
{
    virtual uint32 getSamplesPerFrame() = 0;
    virtual uint32 getAvgCompressedFrameSize() = 0;
    
    virtual uint32 decompressFrame(int16 *framePtr, uint8 *inputPtr, uint32 inSize) = 0;
public:
    byte_buffer_ptr decompressBuffer(const byte_buffer &compressedBuffer)
    {
        uint32 spf = getSamplesPerFrame();
        uint32 avgCompressedSize = getAvgCompressedFrameSize();
        uint32 compressedSize = compressedBuffer.get_buffer_size();
        
        // guess the total number of frames:
        uint32 guessFrameCount = (compressedSize / avgCompressedSize) + 1;
        
        byte_buffer_ptr ret = new byte_buffer(spf * sizeof(int16) * guessFrameCount);
        
        uint32 p = 0;
        uint8 *inputPtr = (uint8 *) compressedBuffer.get_buffer();
        uint32 frameCount = 0;
        int16 *samplePtr = (int16 *) ret->get_buffer();
        
        for(uint32 i = 0; i < compressedSize; i += p)
        {
            if(frameCount == guessFrameCount)
            {
                guessFrameCount = frameCount + ( (compressedSize - i) / avgCompressedSize ) + 1;
                ret->resize(spf * sizeof(int16) * guessFrameCount);
                
                samplePtr = (int16 *) ret->get_buffer();
            }
            p = decompressFrame(samplePtr + frameCount * spf, inputPtr + i, compressedSize - i);
            frameCount++;
        }
        ret->resize(frameCount * spf * sizeof(int16));
        return ret;
    }
    
};

/// The LPC10VoiceEncoder class implements the HawkVoice LPC10 codec
/// compressor.
class LPC10VoiceEncoder : public VoiceEncoder
{
    void *encoderState;
    uint32 getSamplesPerFrame()
    {
        return LPC10_SAMPLES_PER_FRAME;
    }
    
    uint32 getMaxCompressedFrameSize()
    {
        return LPC10_ENCODED_FRAME_SIZE;
    }
    
    uint32 compressFrame(int16 *samplePtr, uint8 *outputPtr)
    {
        return vbr_lpc10_encode(samplePtr, outputPtr, (lpc10_encoder_state *) encoderState);
    }
    
public:
    LPC10VoiceEncoder()
    {
        encoderState = create_lpc10_encoder_state();
        init_lpc10_encoder_state((lpc10_encoder_state *) encoderState);
    }
    
    
    ~LPC10VoiceEncoder()
    {
        destroy_lpc10_encoder_state((lpc10_encoder_state *) encoderState);
    }
    
};

/// The LPC10VoiceDecoder class implements the HawkVoice LPC10 codec
/// decompressor.
class LPC10VoiceDecoder : public VoiceDecoder
{
    void *decoderState;
    uint32 getSamplesPerFrame()
    {
        return LPC10_SAMPLES_PER_FRAME;
    }
    
    uint32 getAvgCompressedFrameSize()
    {
        return (LPC10_ENCODED_FRAME_SIZE + 1) >> 1;
    }
    
    
    uint32 decompressFrame(int16 *framePtr, uint8 *inputPtr, uint32 inSize)
    {
        int p;
        vbr_lpc10_decode(inputPtr, inSize, framePtr, (lpc10_decoder_state *) decoderState, &p);
        return (uint32) p;
    }
    
public:
    LPC10VoiceDecoder()
    {
        decoderState = create_lpc10_decoder_state();
        init_lpc10_decoder_state((lpc10_decoder_state *) decoderState);
    }
    
    ~LPC10VoiceDecoder()
    {
        destroy_lpc10_decoder_state((lpc10_decoder_state *) decoderState);
    }
    
};

/// The GSMVoiceEncoder class implements the HawkVoice GSM codec
/// compressor.
class GSMVoiceEncoder : public VoiceEncoder
{
    void *encoderState;
    uint32 getSamplesPerFrame()
    {
        return GSM_SAMPLES_PER_FRAME;
    }
    
    uint32 getMaxCompressedFrameSize()
    {
        return GSM_ENCODED_FRAME_SIZE;
    }
    
    uint32 compressFrame(int16 *samplePtr, uint8 *outputPtr)
    {
        return gsm_encode((struct gsm_state *) encoderState, samplePtr, outputPtr);
    }
    
public:
    GSMVoiceEncoder()
    {
        encoderState = gsm_create();
    }
    
    ~GSMVoiceEncoder()
    {
        gsm_destroy((struct gsm_state *) encoderState);
    }
    
};

/// The GSMVoiceDecoder class implements the HawkVoice GSM codec
/// decompressor.
class GSMVoiceDecoder : public VoiceDecoder
{
    void *decoderState;
    uint32 getSamplesPerFrame()
    {
        return GSM_SAMPLES_PER_FRAME;
    }
    
    uint32 getAvgCompressedFrameSize()
    {
        return GSM_ENCODED_FRAME_SIZE;
    }
    
    uint32 decompressFrame(int16 *framePtr, uint8 *inputPtr, uint32 inSize)
    {
        gsm_decode((struct gsm_state *) decoderState, inputPtr, framePtr);
        return GSM_ENCODED_FRAME_SIZE;
    }
    
public:
    GSMVoiceDecoder()
    {
        decoderState = gsm_create();
    }
    
    ~GSMVoiceDecoder()
    {
        gsm_destroy((struct gsm_state *) decoderState);
    }
};
