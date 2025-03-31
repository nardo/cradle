// The cradle library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

class Timer
{
   uint32 mPeriod;
   uint32 mCurrentCounter;
public:
   Timer(uint32 period = 0)
   {
      mCurrentCounter = mPeriod = period;
   }

   bool update(uint32 timeDelta)
   {
      if(!mCurrentCounter)
         return false;

      if(timeDelta >= mCurrentCounter)
      {
         mCurrentCounter = 0;
         return true;
      }
      mCurrentCounter -= timeDelta;
      return false;
   }
    uint32 getPeriod() const
    {
        return mPeriod;
    }
   uint32 getCurrent() const
   {
      return mCurrentCounter;
   }

   float32 getFraction()
   {
      if(!mPeriod)
         return 1;
      return 1 - (mCurrentCounter / float32(mPeriod));
   }

   void reset()
   {
      mCurrentCounter = mPeriod;
   }

   void reset(uint32 newCounter, uint32 newPeriod = 0)
   {
      if(!newPeriod)
         newPeriod = newCounter;
      mCurrentCounter = newCounter;
      mPeriod = newPeriod;
   }
};
