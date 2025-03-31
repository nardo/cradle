// The cradle library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

#define kEpsilon 1e-8f


//---------------------------------------------------------------------
// 3x2 matrix:
//---------------------------------------------------------------------
class Matrix2D
{
    static inline bool areNear(const float32 a, const float32 b, const float32 epsilon = kEpsilon)
    {
       return mIsZero((a-b), epsilon);
    }

    static inline bool areNear(const Point & a, const Point & b, const float32 epsilon = kEpsilon)
    {
       return areNear(a.x, b.x, epsilon) && areNear(a.y, b.y, epsilon);
    }

    static float32 getAngleBetween(const Point & a, const Point & b)
    {
       float32 d = a.x * b.x + a.y * b.y;
       float32 c = a.x * b.y - a.y * b.x;
       return mRadToDeg(mAtan(c, d));
    }


public:
   union {
      struct {
         float32 mE11; float32 mE12;
         float32 mE21; float32 mE22;
         float32 mE31; float32 mE32;
      };
      float32 mE[6];
   };

   Matrix2D() {}
   Matrix2D(bool identity)
   {
      if(identity)
         setIdentity();
   }
   Matrix2D(float32 e11, float32 e12, float32 e21, float32 e22, float32 e31, float32 e32)
   {
      mE11 = e11; mE12 = e12;
      mE21 = e21; mE22 = e22;
      mE31 = e31; mE32 = e32;
   }

   Matrix2D &operator=(const Matrix2D & m)
   {
      for(uint32 i = 0; i < 6; i++)
         mE[i] = m.mE[i];
      return *this;
   }

   float32 & operator[](int32 i) { return mE[i]; }
   const float32 & operator[](const uint32 i) const { return mE[i]; }

   inline Matrix2D & setIdentity()
   {
      mE11 = 1.f; mE12 = 0.f;
      mE21 = 0.f; mE22 = 1.f;
      mE31 = 0.f; mE32 = 0.f;
      return *this;
   }

   inline Point getTranslation() const
   { 
      return Point(mE31, mE32);
   }
   inline void setTranslation(const Point & pos)
   {
      mE31 = pos.x;
      mE32 = pos.y;
   }
   inline void setScale(const Point & scale)
   {
      mE11 = scale.x;
      mE22 = scale.y;
   }
   inline Point getScale() const
   {
      return Point(mE11, mE22);
   }

   inline void setRotation(const float32 theta)
   {
      float32 c = cos(theta);
      float32 s = sin(theta);
      mE11 = c; mE12 = s;
      mE21 = -s; mE22 = c;
   }
   inline void setRotation(const Point & vec)
   {
      Point v = vec;
      v.normalize();
      mE11 = v.x; mE12 = v.y;
      mE21 = v.y; mE22 = v.x;
   }

   inline Matrix2D & translate(const Point & t)
   {
      mE31 += t.x; mE32 += t.y;
      return *this;
   }
   inline Matrix2D & scale(const Point & s)
   {
      mE11 *= s.x; mE12 *= s.y;
      mE21 *= s.x; mE22 *= s.y;
      mE31 *= s.x; mE32 *= s.y;
      return *this;
   }
   inline Matrix2D & scale(const float32 & s)
   {
      mE11 *= s; mE12 *= s;
      mE21 *= s; mE22 *= s;
      mE31 *= s; mE32 *= s;
      return *this;
   }
   inline Matrix2D & scaleInPlace(const Point & s)
   {
      mE11 *= s.x; mE12 *= s.y;
      mE21 *= s.x; mE22 *= s.y;
      return *this;
   }
   inline Matrix2D & scaleInPlace(const float32 & s)
   {
      mE11 *= s; mE12 *= s;
      mE21 *= s; mE22 *= s;
      return *this;
   }
   inline Matrix2D & rotate(const float32 theta)
   {
      Matrix2D rot(true);
      rot.setRotation(theta);
      return mul(rot);
   }

   inline Point xAxis() const
   {
      return Point(mE11, mE12);
   }
   inline Point yAxis() const
   {
      return Point(mE21, mE22);
   }
   inline void setXAxis(const Point & vec)
   {
      mE11 = vec.x;
      mE21 = vec.y;
   }
   inline void setYAxis(const Point & vec)
   {
      mE21 = vec.x;
      mE22 = vec.x;
   }

   float32 expansionX() const
    {
       return mSqrt(mE11 * mE11 + mE12 * mE12);
    }

   float32 expansionY() const
    {
       return mSqrt(mE21 * mE21 + mE22 * mE22);
    }

   void setExpansionX(float32 val)
    {
       float32 exp = expansionX();
       if(!areNear(exp, 0.f))
       {
          float32 coef = val / exp;
          mE11 *= coef;
          mE12 *= coef;
       }
    }

   void setExpansionY(float32 val)
    {
       float32 exp = expansionY();
       if(!areNear(exp, 0.f))
       {
          float32 coef = val / exp;
          mE21 *= coef;
          mE22 *= coef;
       }
    }

   bool isIdentity() const
    {
       return areNear(mE11, 1.f) && areNear(mE12, 0.f) &&
              areNear(mE21, 0.f) && areNear(mE22, 1.f) &&
              areNear(mE31, 0.f) && areNear(mE32, 0.f);
    }
    
   bool isIdentityExact() const
    {
       return (mE11 == 1.f) && (mE12 == 0.f) &&
              (mE21 == 0.f) && (mE22 == 1.f) &&
              (mE31 == 0.f) && (mE32 == 0.f);
    }

    bool isTranslation() const
    {
       return areNear(mE11, 1.f) && areNear(mE12, 0.f) &&
              areNear(mE21, 0.f) && areNear(mE22, 1.f) &&
              (!areNear(mE31, 0.f) || areNear(mE32, 0.f));
    }

   bool isScale() const
    {
       return areNear(mE12, 0.f) && areNear(mE21, 0.f) &&
              areNear(mE31, 0.f) && areNear(mE32, 0.f) &&
             (!areNear(mE11, 1.f) || areNear(mE22, 0.f));
    }

   bool isUniformScale() const
    {
       return !areNear(mE11, 1.f) && areNear(mE11, mE22) &&
              areNear(mE12, 0.f) && areNear(mE21, 1.f) &&
              areNear(mE31, 0.f) && areNear(mE32, 0.f);
    }

   bool isRotation() const
    {
       return !areNear(mE11, mE22) && areNear(mE12, -mE21) &&
              areNear(mE31, 0.f) && areNear(mE32, 0.f) &&
              areNear(mE11*mE11 + mE12*mE12, 1.f);
    }

   bool isScaleTranslation() const
    {
       return areNear(mE11, mE12) &&
              areNear(mE12, 0.f) &&
              areNear(mE21, 0.f);
    }

   Matrix2D inverse() const
    {
       Matrix2D d;

       const float32 determ = det();
       if(!areNear(determ, 0.f))
       {
           const float32 ideterm = 1.f / determ;
           d.mE11 =  mE22 * ideterm;
           d.mE12 = -mE12 * ideterm;
           d.mE21 = -mE21 * ideterm;
           d.mE22 =  mE11 * ideterm;
           d.mE31 = -mE31 * d.mE11 - mE32 * d.mE21;
           d.mE32 = -mE31 * d.mE12 - mE32 * d.mE22;
       }
       else
          d.setIdentity();

       return d;
    }


   float32 trace() const
    {
       return mE11 + mE22;
    }

   float32 det() const
    {
       return mE11 * mE22 - mE12 * mE21;

    }

   float32 descrim2() const
    {
       return mFabs(det());
    }

   float32 descrim() const
    {
       return mSqrt(descrim2());
    }


   Matrix2D & mul(const Matrix2D & a)                     // M * a -> M
    {
       Matrix2D result = *this * a;
       *this = result;
       return *this;
    }

   Matrix2D & mul(const Matrix2D & a, const Matrix2D & b) // a * b -> M
    {
       *this = a * b;
       return *this;
    }

    Point mulV(Point & p) const
   {
      Point ret;
      ret.x = p.x * mE11 + p.y * mE21;
      ret.y = p.x * mE12 + p.y * mE22;
      p = ret;
      return p;
   }
   Point mul(Point & p) const                          // M * p -> p
   {
      Point ret;
      ret.x = p.x * mE11 + p.y * mE21 + mE31;
      ret.y = p.x * mE12 + p.y * mE22 + mE32;
      p = ret;
      return p;
   }
   Rect & mul(Rect & box) const                          // M * box -> box
   {
      mul(box.min);
      mul(box.max);
      box.makeValid();

      return box;
   }
   static Matrix2D fromTranslation(const Point & t)
    {
       Matrix2D mat(true);
       mat.setTranslation(t);
       return mat;
    }

   static Matrix2D fromScale(const Point & s)
    {
       Matrix2D mat(true);
       mat.setScale(s);
       return mat;
    }

   static Matrix2D fromRotation(const float32 & r)
    {
       Matrix2D mat(true);
       mat.setRotation(mDegToRad(r));
       return mat;
    }

   static Matrix2D fromRotation(const Point & r)
    {
       Matrix2D mat(true);
       mat.setRotation(r);
       return mat;
    }

   static Matrix2D fromTranslateScale(const Point & t, const Point & s)
    {
       return Matrix2D(s.x, 0.f,
                       0.f, s.y,
                       t.x * s.x, t.y * s.y);
                    
    }

   static Matrix2D fromTranslateRotate(const Point & t, const float32 & r)
    {
       Matrix2D mat;
       mat.setRotation(r);
       mat.mE31 = t.x * mat.mE11 - t.y * mat.mE12;
       mat.mE32 = t.y * mat.mE11 + t.x * mat.mE12;
       return mat;

    }

   static Matrix2D fromScaleTranslate(const Point & s, const Point & t)
    {
       return Matrix2D(s.x, 0.f,
                       0.f, s.y,
                       t.x, t.y);
    }

   static Matrix2D fromScaleRotateTranslate(const Point & scale, const float32 & rotate, const Point & translate)
    {
       float32 r = mDegToRad(rotate);
       float32 c = mCos(r);
       float32 s = mSin(r);

       return Matrix2D(scale.x * c,  scale.x * s,
                       scale.y * -s, scale.y * c,
                       translate.x,  translate.y);
    }


   void decompose(Point & translation, Point & scale, float32 & rotation) const
    {
       // Translation:
       translation.set(mE31, mE32);

       // Scale:
       scale.x = Point(mE11, mE12).len();
       scale.y = Point(mE21, mE22).len();

       // Rotation:
       if(mE11 != 0.f)
       {
          //rotation = mRadToDeg(mAcos(mE11/scale.x));
          rotation = getAngleBetween(Point(1.f, 0.f), Point(mE11, mE12));
       }
       else
          rotation = 0.f;
    }

   void write(Stream & stream)
    {
       stream.write(mE[0]);
       stream.write(mE[1]);
       stream.write(mE[2]);
       stream.write(mE[3]);
       stream.write(mE[4]);
       stream.write(mE[5]);
    }

   void read(Stream & stream)
    {
       setIdentity();
       stream.read(&mE[0]);
       stream.read(&mE[1]);
       stream.read(&mE[2]);
       stream.read(&mE[3]);
       stream.read(&mE[4]);
       stream.read(&mE[5]);
    }
    Matrix2D operator*(const Matrix2D & m1) const
    {
       Matrix2D ret;

       ret[0] = mE[0] * m1[0] + mE[1] * m1[2];
       ret[1] = mE[0] * m1[1] + mE[1] * m1[3];
       ret[2] = mE[2] * m1[0] + mE[3] * m1[2];
       ret[3] = mE[2] * m1[1] + mE[3] * m1[3];
       ret[4] = mE[4] * m1[0] + mE[5] * m1[2];
       ret[5] = mE[4] * m1[1] + mE[5] * m1[3];
       
       ret[4] += m1[4];
       ret[5] += m1[5];

       return ret;
    }

    Point operator*(const Point & v) const
    {
       Point ret;
       ret.x = v.x * mE11 + v.y * mE21 + mE31;
       ret.y = v.x * mE12 + v.y * mE22 + mE32;
       return ret;
    }

    Point operator/(const Point & p) const
    {
       return inverse() * p;
    }

    bool operator==(const Matrix2D & a) const
    {
       for(uint32 i = 0; i < 6; i++)
          if(a[i] != mE[i])
             return false;
       return true;
    }

    bool operator!=(const Matrix2D & a)
    {
       for(uint32 i = 0; i < 6; i++)
          if(a[i] != mE[i])
             return true;
       return false;
    }
    void to_mat4(mat4 &mx) const
    {
        float32 *m = mx.data();
        
       m[0] = mE11;   m[4] = mE12;   m[ 8] = 0.f;   m[12] = 0.f;
       m[1] = mE21;   m[5] = mE22;   m[ 9] = 0.f;   m[13] = 0.f;
       m[2] = 0.f;        m[6] = 0.f;        m[10] = 1.f;   m[14] = 0.f;
       m[3] = mE31;   m[7] = mE32;   m[11] = 0.f;   m[15] = 1.f;
    }

    void to_mat4_transpose(mat4 &mx) const
    {
        float32 *m = mx.data();
       m[0] = mE11;   m[1] = mE12;   m[ 2] = 0.f;   m[3] = 0.f;
       m[4] = mE21;   m[5] = mE22;   m[ 6] = 0.f;   m[7] = 0.f;
       m[8] = 0.f;        m[9] = 0.f;        m[10] = 1.f;   m[11] = 0.f;
       m[12] = mE31;   m[13] = mE32;   m[14] = 0.f;   m[15] = 1.f;
    }

};

