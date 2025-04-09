// The vectrex library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

enum eTransitionType
{
    kTransitionLinear = 0,

    kTransitionEaseInQuad,
    kTransitionEaseOutQuad,
    kTransitionEaseInOutQuad,

    kTransitionEaseInCubic,
    kTransitionEaseOutCubic,
    kTransitionEaseInOutCubic,

    kTransitionEaseInQuart,
    kTransitionEaseOutQuart,
    kTransitionEaseInOutQuart,

    kTransitionEaseInQuint,
    kTransitionEaseOutQuint,
    kTransitionEaseInOutQuint,

    kTransitionEaseInSine,
    kTransitionEaseOutSine,
    kTransitionEaseInOutSine,

    kTransitionEaseInExpo,
    kTransitionEaseOutExpo,
    kTransitionEaseInOutExpo,

    kTransitionEaseInCirc,
    kTransitionEaseOutCirc,
    kTransitionEaseInOutCirc,

    kTransitionEaseInBack,
    kTransitionEaseOutBack,
    kTransitionEaseInOutBack,
};

//////////////////////////////////////////////////
//////////////////////////////////////////////////
template <typename T>
static T TweenLinear(const T & begin, const T & end, float time)
{
    T delta = end - begin;
    return begin + (delta * time);
}
template <>
static bool TweenLinear<bool>(const bool & begin, const bool & end, float time)
{
    return (time < 1.f) ? begin : end;
}
template <>
static ColorI TweenLinear<ColorI>(const ColorI & begin, const ColorI & end, float time)
{
    float32 r = end.red - begin.red;
    float32 g = end.green - begin.green;
    float32 b = end.blue - begin.blue;
    float32 a = end.alpha - begin.alpha;
    
	return ColorI(begin.red + r * time, begin.green + g * time, begin.blue + b * time, begin.alpha + a * time);
}

//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////
template <typename T>
static T TweenEaseInQuad(const T & begin, const T & end, float time)
{
    T delta = end - begin;
    return begin + (delta * (time * time));
}
template <typename T>
static T TweenEaseOutQuad(T & begin, T & end, float time)
{
    T delta = end - begin;
    return begin + (delta * -(time * (time - 2.f)));
}
template <typename T>
static T TweenEaseInOutQuad(T & begin, T & end, float time)
{
    T delta = end - begin;
    time *= 2.f;
    if(time < 1.f)
        return begin + (delta * ((time * time) * 0.5f));
    else
        return begin + (delta * (((time - 1.f) * (time - 3.f) - 1.f) * -0.5f));
}

//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////
template <typename T>
static T TweenEaseInCubic(const T & begin, const T & end, float time)
{
    T delta = end - begin;
    return begin + (delta * (time * time * time));
}
template <typename T>
static T TweenEaseOutCubic(T & begin, T & end, float time)
{
    T delta = end - begin;
    time -= 1.f;
    return begin + (delta * ((time * time * time) + 1.f));
}

template <typename T>
static T TweenEaseInOutCubic(T & begin, T & end, float time)
{
    T delta = end - begin;
    time *= 2.f;
    if(time < 1.f)
        return begin + (delta * ((time * time * time) * 0.5f));
    else
    {
        time -= 2.f;
        return begin + (delta * (((time * time * time) + 2.f) * 0.5f));
    }
}

//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////
template <typename T>
static T TweenEaseInQuart(const T & begin, const T & end, float time)
{
    T delta = end - begin;
    return begin + (delta * (time * time * time * time));
}
template <typename T>
static T TweenEaseOutQuart(T & begin, T & end, float time)
{
    T delta = end - begin;
    time -= 1.f;
    return begin + (delta * -((time * time * time * time) - 1.f));
}

template <typename T>
static T TweenEaseInOutQuart(T & begin, T & end, float time)
{
    T delta = end - begin;
    time *= 2.f;
    if(time < 1.f)
        return begin + (delta * ((time * time * time * time) * 0.5f));
    else
    {
        time -= 2.f;
        return begin + (delta * (((time * time * time * time) - 2.f) * -0.5f));
    }
}

//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////
template <typename T>
static T TweenEaseInQuint(const T & begin, const T & end, float time)
{
    T delta = end - begin;
    return begin + (delta * (time * time * time * time * time));
}
template <typename T>
static T TweenEaseOutQuint(T & begin, T & end, float time)
{
    T delta = end - begin;
    time -= 1.f;
    return begin + (delta * ((time * time * time * time * time) + 1.f));
}

template <typename T>
static T TweenEaseInOutQuint(T & begin, T & end, float time)
{
    T delta = end - begin;
    time *= 2.f;
    if(time < 1.f)
        return begin + (delta * ((time * time * time * time * time) * 0.5f));
    else
    {
        time -= 2.f;
        return begin + (delta * (((time * time * time * time * time) + 2.f) * 0.5f));
    }
}

//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////
template <typename T>
static T TweenEaseInSine(const T & begin, const T & end, float time)
{
    T delta = end - begin;
    return begin + (delta + (delta * -mCos(time * (FloatPi / 2.f))));
}
template <typename T>
static T TweenEaseOutSine(T & begin, T & end, float time)
{
    T delta = end - begin;
    return begin + (delta * mSin(time * (FloatPi / 2.f)));
}

template <typename T>
static T TweenEaseInOutSine(T & begin, T & end, float time)
{
    T delta = end - begin;
    return begin + (delta * ((mCos(FloatPi * time) - 1.f) * -0.5f));
}

//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////
template <typename T>
static T TweenEaseInExpo(const T & begin, const T & end, float time)
{
    if(time == 0.f)
        return begin;
    else
    {
        T delta = end - begin;
        return begin + (delta * mPow(2.f, 10.f * (time - 1.f)));
    }
}

template <typename T>
static T TweenEaseOutExpo(T & begin, T & end, float time)
{
    if(time == 1.f)
        return end;
    else
    {
        T delta = end - begin;
        return begin + (delta * (-mPow(2.f, -10.f * time) + 1.f));
    }
}

template <typename T>
static T TweenEaseInOutExpo(T & begin, T & end, float time)
{
    if(time == 0.f)
        return begin;
    else if(time == 1.f)
        return end;
    else
    {
        T delta = end - begin;
        time *= 2.f;
        if(time < 1.f)
            return begin + (delta * (mPow(2.f, 10.f * (time - 1.f)) * 0.5f));
        else
            return begin + (delta * ((-mPow(2.f, -10.f * (time - 1.f)) + 2.f) * 0.5f));
    }
}

//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////
template <typename T>
static T TweenEaseInCirc(const T & begin, const T & end, float time)
{
    T delta = end - begin;
    return begin + (delta * -(mSqrt(1.f - (time * time)) - 1.f));
}

template <typename T>
static T TweenEaseOutCirc(T & begin, T & end, float time)
{
    T delta = end - begin;
    time -= 1.f;
    return begin + (delta * mSqrt(1.f - (time * time)));
}

template <typename T>
static T TweenEaseInOutCirc(T & begin, T & end, float time)
{
    T delta = end - begin;
    time *= 2.f;
    if(time < 1.f)
        return begin + (delta * ((mSqrt(1.f - (time * time)) - 1.f) * -0.5f));
    else
    {
        time -= 2.f;
        return begin + (delta * ((mSqrt(1.f - (time * time)) + 1.f) * 0.5f));
    }
}

//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////
template <typename T>
static T TweenEaseInBack(const T & begin, const T & end, float time, float s)
{
    T delta = end - begin;
    return begin + (delta * (time * time * ((s + 1.f) * time - s)));
}

template <typename T>
static T TweenEaseOutBack(T & begin, T & end, float time, float s)
{
    T delta = end - begin;
    time -= 1.f;
    return begin + (delta * ((time * time * ((s + 1.f) * time + s) + 1.f)));
}

template <typename T>
static T TweenEaseInOutBack(T & begin, T & end, float time, float s)
{
    T delta = end - begin;
    time *= 2.f;
    s *= 1.525f;
    if(time < 1.f)
        return begin + (delta * ((time * time * ((s + 1.f) * time - s)) * 0.5f));
    else
    {
        time -= 2.f;
        return begin + (delta * (((time * time * ((s + 1.f) * time + s)) + 2.f) * 0.5f));
    }
}

//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////
template <typename T>
static T Tween(eTransitionType transitionType, float time, const T & begin, const T & end)
{
    switch(transitionType)
    {
        default:
        case kTransitionLinear:             return TweenLinear(begin, end, time);

        case kTransitionEaseInQuad:         return TweenEaseInQuad(begin, end, time); 
        case kTransitionEaseOutQuad:        return TweenEaseOutQuad(begin, end, time); 
        case kTransitionEaseInOutQuad:      return TweenEaseInOutQuad(begin, end, time); 
                                            
        case kTransitionEaseInCubic:        return TweenEaseInCubic(begin, end, time); 
        case kTransitionEaseOutCubic:       return TweenEaseOutCubic(begin, end, time); 
        case kTransitionEaseInOutCubic:     return TweenEaseInOutCubic(begin, end, time); 
                                            
        case kTransitionEaseInQuart:        return TweenEaseInQuart(begin, end, time);     
        case kTransitionEaseOutQuart:       return TweenEaseOutQuart(begin, end, time);    
        case kTransitionEaseInOutQuart:     return TweenEaseInOutQuart(begin, end, time);  
                                            
        case kTransitionEaseInQuint:        return TweenEaseInQuint(begin, end, time);     
        case kTransitionEaseOutQuint:       return TweenEaseOutQuint(begin, end, time);    
        case kTransitionEaseInOutQuint:     return TweenEaseInOutQuint(begin, end, time);  
                                            
		  case kTransitionEaseInSine:         return TweenEaseInSine(begin, end, time);
        case kTransitionEaseOutSine:        return TweenEaseOutSine(begin, end, time); 
        case kTransitionEaseInOutSine:      return TweenEaseInOutSine(begin, end, time); 
                                            
        case kTransitionEaseInExpo:         return TweenEaseInExpo(begin, end, time);     
        case kTransitionEaseOutExpo:        return TweenEaseOutExpo(begin, end, time);    
        case kTransitionEaseInOutExpo:      return TweenEaseInOutExpo(begin, end, time);  
                                            
        case kTransitionEaseInCirc:         return TweenEaseInCirc(begin, end, time);     
        case kTransitionEaseOutCirc:        return TweenEaseOutCirc(begin, end, time);    
        case kTransitionEaseInOutCirc:      return TweenEaseInOutCirc(begin, end, time);  
                                            
        case kTransitionEaseInBack:         return TweenEaseInBack(begin, end, time, 1.70158f);     
        case kTransitionEaseOutBack:        return TweenEaseOutBack(begin, end, time, 1.70158f);      
        case kTransitionEaseInOutBack:      return TweenEaseInOutBack(begin, end, time, 1.70158f);      
    }
}
 
template <>
static inline ColorI Tween<ColorI>(eTransitionType transitionType, float time, const ColorI & begin, const ColorI & end)
{
	return TweenLinear(begin, end, time);
}

template <>
static inline Color Tween<Color>(eTransitionType transitionType, float time, const Color & begin, const Color & end)
{
	return TweenLinear(begin, end, time);
}

//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////
template <typename T>
static T Tween(eTransitionType transitionType, float time, const T & begin, const T & end, float duration)
{
    return Tween(transitionType, time / duration, begin, end);
}

class Interpolator
{
public:
   enum {
      kActive             = bit(0),   // Time is stepped, Update() may be invoked
      kClampBegin         = bit(1),   // Invoke Update() only when nextTime >= mBeginTime
      kClampEnd           = bit(2),   // Invoke Update() only when curTime <= mEndTime
   };

   Interpolator() :
      mCurTime(0.f),
      mBeginTime(0.f),
      mEndTime(1.f),
      mFlags(0)
   {
   }                   

   Interpolator(float32 beginTime, float32 endTime, bool active = true) :
      mCurTime(0.f),
      mBeginTime(beginTime),
      mEndTime(endTime),
      mFlags(kClampBegin|kClampEnd)
   {
      mFlags.set(kActive, active);
   }

   virtual void advanceTime(float32 step)
    {
       if(!mFlags.test(kActive))
          return;
       
       float32 curTime = mCurTime;
       float32 nextTime = mCurTime + step;
       
       mCurTime = nextTime;
       
       if(mFlags.test(kClampBegin))
       {
          // Update() only when nextTime >= mBeginTime
          if(nextTime < mBeginTime)
             return;
       }

       if(mFlags.test(kClampEnd))
       {
          // Update() only when curTime <= mEndTime
          if(curTime > mEndTime)
             return;
       }
       
       update();
    }

   virtual void setTime(float32 time)
    {
       if(!mFlags.test(kActive))
          return;

       mCurTime = time;
       update();
    }

   virtual void update() {}
   virtual void * getHandle() const { return 0; }
   
   float32 getCurTime() const { return mCurTime; }
   void setCurTime(float32 value) { mCurTime = value; }
   
   float32 getBeginTime() const { return mBeginTime; }
   void setBeginTime(float32 value) { mBeginTime = value; }
   
   float32 getEndTime() const { return mEndTime; }
   void setEndTime(float32 value) { mEndTime = value; }
   
   bool getActive() const { return mFlags.test(kActive); }
   void setActive(bool value) { mFlags.set(kActive, value); }
   
   bool isFinished() const { return mCurTime >= mEndTime; }

   // Helper to clamp time [0, 1]
   float32 getNormalizedTime() const
   {
      // Check edge conditions:
      if(mCurTime <= mBeginTime)
         return 0.f;
      if(mCurTime >= mEndTime)
         return 1.f;          
      if(mBeginTime == mEndTime)
         return 1.f;
      
      return (mCurTime - mBeginTime) / (mEndTime - mBeginTime);
   }

private:
    float32        mCurTime;
    float32        mBeginTime;
    float32        mEndTime;
    bit_set   mFlags;
};

class InterpolatorSet : public Interpolator
{
public:
    virtual void advanceTime(float32 step) const
    {
       for(int32 i = (mTransitions.size() - 1); i >= 0; i--)
          mTransitions[i]->advanceTime(step);
    }

    virtual void setTime(float32 time) const
    {
       for(int32 i = (mTransitions.size() - 1); i >= 0; i--)
          mTransitions[i]->setTime(time);
    }

    enum {
       kInsertDestroyExisting    = bit(0),      // Delete any existing transitions (by handle)
    };

    void * insert(Interpolator * value, int32 insertFlags = kInsertDestroyExisting)
    {
       void *handle = value->getHandle();

       // Delete any interpolator already in list which matches handle:
       if(insertFlags & kInsertDestroyExisting)
       {
          for(int32 i = 0; i < mTransitions.size(); i++)
          {
             if(handle == mTransitions[i]->getHandle())
             {
                delete mTransitions[i];
                mTransitions.erase_fast(i);
             }
          }
       }
       
       mTransitions.push_back(value);
       return handle;
    }

    void remove(void * handle)
    {
       for(int32 i = 0; i < mTransitions.size(); i++)
          if(handle == mTransitions[i]->getHandle())
             mTransitions.erase_fast(i);
    }

    void destroy(void * handle)
    {
       for(int32 i = 0; i < mTransitions.size(); i++)
          if(mTransitions[i]->getHandle() == handle)
          {
             delete mTransitions[i];
             mTransitions.erase_fast(i);
          }
    }

    Interpolator * find(void *handle)
    {
       for(int32 i = (mTransitions.size() - 1); i >= 0; i--)
          if(mTransitions[i]->getHandle() == handle)
             return mTransitions[i];
       return 0;
    }

    void removeAll()
    {
       mTransitions.clear();
    }

    void destroyAll()
    {
       for(int32 i = 0; i < mTransitions.size(); i++)
          delete mTransitions[i];
       mTransitions.clear();
    }

    void removeInactive()
    {
       for(int32 i = 0; i < mTransitions.size(); i++)
          if(!mTransitions[i]->getActive())
             mTransitions.erase_fast(i);
    }

    void destroyInactive()
    {
       for(int32 i = 0; i < mTransitions.size(); i++)
       {
          if(!mTransitions[i]->getActive())
          {
             delete mTransitions[i];
             mTransitions.erase_fast(i);
          }
       }
    }

    void removeFinished()
    {
       for(int32 i = 0; i < mTransitions.size(); i++)
          if(mTransitions[i]->isFinished())
             mTransitions.erase_fast(i);
    }

    void destroyFinished()
    {
       for(int32 i = 0; i < mTransitions.size(); i++)
       {
          if(mTransitions[i]->isFinished())
          {
             delete mTransitions[i];
             mTransitions.erase_fast(i);
          }
       }
    }

private:
    array<Interpolator*>    mTransitions;
};

// Interoplate a type vlue.  The mOutput field should point to the value which
// is set in the 'update()' method.  The mOutput ptr must be valid for the lifetime
// of this type.
template <typename T>
class ValueInterpolator : public Interpolator
{
public:

    ValueInterpolator(const T & beginValue, const T & endValue, void * output, float32 duration, float32 delay = 0.f, eTransitionType transitionType = kTransitionLinear) :
        Interpolator(delay, delay + duration),
        mBeginValue(beginValue),
        mEndValue(endValue),
        mOutput(output),
        mTransitionType(transitionType)
    {
    }

    const T & getBeginValue() const { return mBeginValue; }
    const T & getEndValue() const { return mEndValue; }
    eTransitionType getTransitionType() const { return mTransitionType; }

    virtual void update()
    {
        if(mOutput)
            *static_cast<T*>(mOutput) = Tween(mTransitionType, getNormalizedTime(), mBeginValue, mEndValue);
    }
    virtual void * getHandle() const
    {
        return mOutput;
    }

protected:
    T                   mBeginValue;
    T                   mEndValue;
    void *              mOutput;
    eTransitionType     mTransitionType;
};
