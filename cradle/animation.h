// The cradle library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

template <typename T>
class KeyFrame
{
private:
   T                 mValue;              // Value at this keyframe
   float32               mTime;               // Time of this keyframe
   eTransitionType   mTransitionType;     // How to animate to the next keyframe

public:
   KeyFrame() : mTime(0.f),
    mTransitionType(kTransitionLinear) {}

    KeyFrame(float32 time, const T & value, eTransitionType transition = kTransitionLinear) : mValue(value), mTime(time), mTransitionType(transition) {}

   float32 getTime() const { return mTime; }
   const T & getValue() const { return mValue; }
   eTransitionType getTransitionType() const { return mTransitionType; }
};

//---------------------------------------------------------------------
template <typename T>
class KeyFrameAnimation : public Interpolator
{
public:
   typedef KeyFrame<T> KeyType;

    KeyFrameAnimation() : mOutput(0), mCurrentKey(0) {}
    
    KeyFrameAnimation(T * output) : mOutput(output), mCurrentKey(0) { setActive(true); }

   virtual void update()
    {
       float32 time = getCurTime();

       if(mKeys.size() > 0)
       {
          if(time <= mKeys.first().getTime())
          {
             // At Start:
             *mOutput = mKeys.first().getValue();
             mCurrentKey = 0;
          }
          else if(time >= mKeys.last().getTime())
          {
             // At End:
             *mOutput = mKeys.last().getValue();
             mCurrentKey = mKeys.size() - 1;
          }
          else
          {
             int32 current = mCurrentKey;
             
             // Search back:
             while(time < mKeys[current].getTime())
                current--;
             
             // Search forward:
             while(time > mKeys[current + 1].getTime())
                current++;
             
             mCurrentKey = current;
             int32 next = current + 1;
             
             float32 curTime = mKeys[current].getTime();
             float32 nextTime = mKeys[next].getTime();
             
             const T& curValue = mKeys[current].getValue();
             const T& nextValue = mKeys[next].getValue();

             // Evaluate at time:
             float32 normalizedTime = (time - curTime) / (nextTime - curTime);
             *mOutput = Tween(mKeys[current].getTransitionType(), normalizedTime, curValue, nextValue);
          }
       }
    }

   virtual void * getHandle() const { return mOutput; }

   float32 getAnimLength() const
    {
       if(mKeys.size())
          return mKeys.last().getTime();
       return 0.f;
    }

   void insertKey(const KeyType & key)
    {
       float32 insertTime = key.getTime();

       int32 idx;
       for(idx = 0; idx < mKeys.size(); idx++)
       {
          float32 curTime = mKeys[idx].getTime();

          // beyond:
          if(curTime > insertTime)
             break;

          // replace current key:
          if(curTime == insertTime)
          {
             mKeys[idx] = key;
             return;
          }
       }

       if(idx == mKeys.size())
          mKeys.push_back(key);
       else
       {
          mKeys.insert(idx);
          mKeys[idx] = key;
       }
    }


   void removeKey(float32 time)
    {
       for(int32 idx = 0; idx < mKeys.size(); idx++)
          if(mKeys[idx].getTime() == time)
          {
             mKeys.erase(idx);
             return;
          }
    }

private:
   array<KeyType>   mKeys;               // Table of keyframes
   T *               mOutput;             // What value is being updated
   int32               mCurrentKey;         // Current key for lookup purpose
};

