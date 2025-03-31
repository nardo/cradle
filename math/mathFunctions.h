// The math library - copyright KAGR LLC, portions copyright Team Nutshell. This library is derived largely from nml - the C++ Nutshell Mathematics Library for Computer Graphics (https://team-nutshell.github.io/nml/). The use of this sorce code is governed by the MIT software license described in the "license.txt" file in this directory.

static inline bool IsPow2(uint32 value)
{
    return (((value & (value - 1)) == 0) && value);
}

#define __EQUAL_CONST_F float32(0.000001)                            ///< Constant float epsilon used for float32 comparisons

static inline float32 DegToRad(float32 d)
{
   return float32((d * FloatPi) / float32(180));
}

static inline float32 RadToDeg(float32 r)
{
   return float32((r * 180.0) / FloatPi);
}

static inline float64 DegToRad(float64 d)
{
   return (d * FloatPi) / float64(180);
}

static inline float64 RadToDeg(float64 r)
{
   return (r * 180.0) / FloatPi;
}

static float lerp(const float a, const float b, const float interpolationValue)
{
    return a + interpolationValue * (b - a);
}

#define PI 3.1415926535897932384626433832795f

static float toDeg(const float radians)
{
    return radians * (180.0f / PI);
}

static float toRad(const float degrees)
{
    return degrees * (PI / 180.0f);
}

static inline bool mIsZero(const float32 val, const float32 epsilon = 0.0001f )
{
   return (val > -epsilon) && (val < epsilon);
}

static inline float32 mFloor(const float32 val)
{
   return (float32) floor(val);
}

static inline float32 mCeil(const float32 val)
{
   return (float32) ceil(val);
}

static inline float32 mFabs(const float32 val)
{
   return (float32) fabs(val);
}

static inline float64 mFabs(const float64 val)
{
   return fabs(val);
}

static inline float32 mFmod(const float32 val, const float32 mod)
{
   return fmod(val, mod);
}

static inline int32 mAbs(const int32 val)
{
   return abs(val);
}

static inline int32 mClamp(int32 val, int32 low, int32 high)
{
   return std::max(std::min(val, high), low);
}

static inline float32 mClampF(float32 val, float32 low, float32 high)
{
   return (float32) std::max(std::min(val, high), low);
}
/*
static inline int32 mMulDiv(int32 a, int32 b, int32 c)
{
   return m_mulDivS32(a, b, c);
}

static inline uint32 mMulDiv(int32 a, int32 b, uint32 c)
{
   return m_mulDivU32(a, b, c);
}*/


static inline float32 mSin(const float32 angle)
{
   return (float32) sin(angle);
}

static inline float32 mCos(const float32 angle)
{
   return (float32) cos(angle);
}

static inline float32 mTan(const float32 angle)
{
   return (float32) tan(angle);
}

static inline float32 mAsin(const float32 val)
{
   return (float32) asin(val);
}

static inline float32 mAcos(const float32 val)
{
   return (float32) acos(val);
}

static inline float32 mAtan(const float32 x, const float32 y)
{
   return (float32) atan2(x, y);
}

static inline void mSinCos(const float32 angle, float32 &s, float32 &c)
{
   s = mSin(angle);
   c = mCos(angle);
}

static inline float32 mTanh(const float32 angle)
{
   return (float32) tanh(angle);
}

static inline float32 mSqrt(const float32 val)
{
   return (float32) sqrt(val);
}

static inline float64 mSqrt(const float64 val)
{
   return (float64) sqrt(val);
}

static inline float32 mPow(const float32 x, const float32 y)
{
   return (float32) pow(x, y);
}

static inline float32 mLog(const float32 val)
{
   return (float32) log(val);
}

static inline float32 mExp(const float32 val)
{
   return (float32) exp(val);
}

static inline float64 mSin(const float64 angle)
{
   return (float64) sin(angle);
}

static inline float64 mCos(const float64 angle)
{
   return (float64) cos(angle);
}

static inline float64 mTan(const float64 angle)
{
   return (float64) tan(angle);
}

static inline float64 mAsin(const float64 val)
{
   return (float64) asin(val);
}

static inline float64 mAcos(const float64 val)
{
   return (float64) acos(val);
}

static inline float64 mAtan(const float64 x, const float64 y)
{
   return (float64) atan2(x, y);
}

static inline void mSinCos(const float64 angle, float64 &sin, float64 &cos)
{
   sin = mSin(angle);
   cos = mCos(angle);
}

static inline float64 mTanh(const float64 angle)
{
   return (float64) tanh(angle);
}

static inline float64 mPow(const float64 x, const float64 y)
{
   return (float64) pow(x, y);
}

static inline float64 mLog(const float64 val)
{
   return (float64) log(val);
}

static inline float64 mFabsD(const float64 val)
{
   return (float64) fabs(val);
}

static inline float64 mFmodD(const float64 val, const float64 mod)
{
   return (float64) fmod(val, mod);
}

static inline float64 mSqrtD(const float64 val)
{
   return (float64) sqrt(val);
}

static inline float64 mFloorD(const float64 val)
{
   return (float64) floor(val);
}

static inline float64 mCeilD(const float64 val)
{
   return (float64) ceil(val);
}

static inline bool isEqual(float32 a, float32 b)
{
   return mFabs(a - b) < __EQUAL_CONST_F;
}

static inline bool isZero(float32 a)
{
   return mFabs(a) < __EQUAL_CONST_F;
}

//--------------------------------------
static inline float32 mDegToRad(float32 d)
{
   return((d * FloatPi) / 180.0f);
}

static inline float32 mRadToDeg(float32 r)
{
   return((r * 180.0f) / FloatPi);
}

static inline float64 mDegToRad(float64 d)
{
   return (d * FloatPi) / 180.0;
}

static inline float64 mRadToDeg(float64 r)
{
   return (r * 180.0) / FloatPi;
}


