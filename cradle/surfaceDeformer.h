// The cradle library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

class SurfaceDeformer
{
private:
   struct Effect
   {
      enum {
         kRipple,       // z = amplitude * sin(r * period + phaseShift)
         kPinch
      };

      enum {
         kActive        = (1<<0),   // Only does stuff when active
         kFullscreen    = (1<<1),   // Effect is a fullscreen.  Distance/position are within [0,1]
         kRampDistance  = (1<<2),   // Linearly ramp the effect over maxDistance
         kRampTime      = (1<<3),   // Linearly ramp the effect over time
         kRampAmplitude = (1<<4),   // Linearly ramp amplitude to 0.f over time
      };

      Point       mPos;             // 'world' position of the effect
      Point       mLocalPos;        // transformed local position ([0,1],[0,1])

      uint32         mType;
      uint32         mFlags;
      float32         mCurrentTime;
      float32         mLifetime;

      // Effect params:
      float32         mAmplitude;
      float32         mOrigAmplitude;
      float32         mFrequency;
      float32         mMaxDistance;        // max distance effect alters verts
      float32         mLocalMaxDistance;   // transformed max distance
   };
   
   array<Effect>  mEffects;         // Can potentially have a number of active effects
   int32                  mXDim;            // Number of vertical slices in mesh
   int32                  mYDim;            // Number of horizontal slices in mesh
    RenderVertex3T       *mVerts;   // The verts in unit space [0,1] (0,0 == bottom left)
    GLuint                 *mIndices;  // Triangle indices for the screen
   Point                mPos;             // 'world' position
   Point                mExtent;          // 'world' extent
    uint32                  mWidth;
    uint32                  mHeight;
    Point mTexCoordScale;
    
    
   void buildMesh()
    {
       delete [] mVerts;
       delete [] mIndices;

       uint32 numVerts = mXDim * mYDim;
       mVerts = new RenderVertex3T[numVerts];
       mIndices = new GLuint[(mXDim - 1) * (mYDim - 1) * 6]; // two triangles per grid rect

       RenderVertex3T * cur = mVerts;

       float32 xMax = float32(mXDim - 1);
       float32 yMax = float32(mYDim - 1);

       for(uint32 y = 0; y < mYDim; y++)
       {
           for(uint32 x = 0; x < mXDim; x++)
           {
               // [0,1]
               cur->p.x = float32(x) / xMax;
               cur->p.y = float32(y) / yMax;
               cur->p.z = -1.f;
               
               // [0,1]
               cur->t.x = cur->p.x * mTexCoordScale.x;
               cur->t.y = mTexCoordScale.y - cur->p.y * mTexCoordScale.y;
               
               cur++;
           }
       }
       GLuint *cur_idx = mIndices;
       
       for(uint32 y = 0; y < mYDim - 1; y++)
       {
           for(uint32 x = 0; x < mXDim - 1; x++)
           {
               uint32 vi = y * mXDim + x;
               cur_idx[0] = vi;
               cur_idx[1] = vi + 1;
               cur_idx[2] = vi + mXDim;
               cur_idx[3] = vi + mXDim;
               cur_idx[4] = vi + 1;
               cur_idx[5] = vi + mXDim + 1;
               cur_idx += 6;
           }
       }
    }

public:
   SurfaceDeformer() :
    mWidth(0),
    mHeight(0),
    mXDim(16),
    mYDim(16),
    mVerts(0),
    mIndices(0),
    mTexCoordScale(1,1),
    mPos(0.f, 0.f),
    mExtent(1.f, 1.f) {}
    
   ~SurfaceDeformer()
    {
       delete [] mVerts;
       delete [] mIndices;
    }

    void updateScreenExtent(uint32 _width, uint32 _height)
    {
        if(mWidth != _width || mHeight != _height)
        {
            mWidth = _width;
            mHeight = _height;
            
            if(!IsPow2(mWidth) || !IsPow2(mHeight))
               mTexCoordScale.set(mWidth, mHeight);
            else
               mTexCoordScale.set(1, 1);

            int32 kNumRows = mWidth / 10;
            init(kNumRows, (kNumRows * mHeight) / mWidth);
        }
    }

   void init(int32 xDim, int32 yDim)
    {
       mXDim = xDim;
       mYDim = yDim;

       buildMesh();
    }

   void render()
    {
       A.P.draw_triangles_indexed(mVerts, mXDim * mYDim, mIndices, (mXDim - 1) * (mYDim - 1) * 6);
    }


   void setPosition(const Point & pos) { mPos = pos; }
   void setExtent(const Point & extent) { mExtent = extent; }
   void update(float32 timeStep)
    {
          // Update effect local settings:
          for(int32 i = 0; i < mEffects.size(); i++)
          {
             Effect & effect = mEffects[i];
             if((effect.mFlags & Effect::kActive) == 0)
                continue;

             if((effect.mFlags & Effect::kRampAmplitude) && (effect.mLifetime != 0.f))
                effect.mAmplitude = effect.mOrigAmplitude * (1.f - (effect.mCurrentTime / effect.mLifetime));

             if(effect.mFlags & Effect::kFullscreen)
                continue;

              Point _p = effect.mPos - mPos;
              _p.x /= mExtent.x;
              _p.y /= mExtent.y;
              
             effect.mLocalPos = Point(0.5f, 0.5f) + _p;
             effect.mLocalMaxDistance = effect.mMaxDistance / mExtent.x;
          }

          RenderVertex3T *vert = &mVerts[0];

          float32 xStep = 1.f / float32(mXDim - 1);
          float32 yStep = 1.f / float32(mYDim - 1);

          // Walk all verts:
          for(int32 y = 0; y < mYDim; y++)
          {
             for(int32 x = 0; x < mXDim; x++)
             {
                // Reset this vert:
                vert->p.x = float32(x) * xStep;
                vert->p.y = float32(y) * yStep;
                 vert->p.z = -1; //0.f;
                 vec2 texCoord;
                 texCoord.x = vert->p.x * mTexCoordScale.x;
                 texCoord.y = vert->p.y * mTexCoordScale.y;

                // Apply each effect:
                for(int32 i = 0; i < mEffects.size(); i++)
                {
                   Effect & effect = mEffects[i];
                   if((effect.mFlags & Effect::kActive) == 0)
                      continue;

                   Point effectOffset = effect.mLocalPos - Point(vert->p.x, vert->p.y);
                   float32 effectDistanceSqr = effectOffset.lenSquared();

                   // Not within max distance?
                   if((effect.mLocalMaxDistance != 0.f) && (effectDistanceSqr > (effect.mLocalMaxDistance * effect.mLocalMaxDistance)))
                      continue;

                   float32 effectDistance = sqrt(effectDistanceSqr);
                   float32 val = 0.f;

                   // Calculate the value:
                   switch(effect.mType)
                   {
                      case Effect::kPinch:
                         val = effect.mAmplitude;
                         break;

                      case Effect::kRipple:
                         val = sin(effect.mFrequency * effectDistance - effect.mCurrentTime * 10.f) * effect.mAmplitude;
                         break;
                   }

                   // Want to ramp the attenuation:
                   if((effect.mFlags & Effect::kRampDistance) && (effect.mLocalMaxDistance != 0.f))
                      val *= 1.f - (effectDistance / effect.mLocalMaxDistance);
                   if((effect.mFlags & Effect::kRampTime) && (effect.mLifetime != 0.f))
                      val *= 1.f - (effect.mCurrentTime / effect.mLifetime);

                   // Apply
                   switch(effect.mType)
                   {
                      case Effect::kPinch:
                         texCoord.x += mTexCoordScale.x * effectOffset.x * val;
                         texCoord.y += mTexCoordScale.y * effectOffset.y * val;
                         break;

                      case Effect::kRipple:
                         vert->p.z += val;
                         break;
                   }
                }
                 texCoord.y = mTexCoordScale.y - texCoord.y;
                 vert->t = texCoord;

                vert++;
             }
          }

          // Update the timers:
          for(int32 i = 0; i < mEffects.size();)
          {
             Effect & effect = mEffects[i];
             effect.mCurrentTime += timeStep;

             if(effect.mCurrentTime >= effect.mLifetime)
                mEffects.erase_fast(i);
             else
                i++;
          }
    }


   // erm...
   void addScreenTeleport()
    {
       Effect e;
       e.mType = Effect::kRipple;
       e.mFlags = Effect::kFullscreen|Effect::kActive|Effect::kRampAmplitude;
       e.mLocalPos.set(0.5, 0.5);
       e.mOrigAmplitude = 150.f;
       e.mAmplitude = 150.f;
       e.mCurrentTime = 0.f;
       e.mLifetime = 1.f;
       e.mFrequency = 20.f;
       e.mLocalMaxDistance = 0.f;
    
       mEffects.push_back(e);
    }

   void addBurstExplosion(const Point & pos, float32 maxDistance)
    {
       Effect e;
       e.mType = Effect::kPinch;
       e.mFlags = Effect::kActive|Effect::kRampTime|Effect::kRampDistance;
       e.mPos = pos;
       e.mAmplitude = 1.5f;
       e.mCurrentTime = 0.f;
       e.mLifetime = 0.5f;
       e.mMaxDistance = maxDistance;

       mEffects.push_back(e);
    }

   void addShipExplosion(const Point & pos, float32 maxDistance)
    {
       Effect e;
       e.mType = Effect::kPinch;
       e.mFlags = Effect::kActive|Effect::kRampTime|Effect::kRampDistance;
       e.mPos = pos;
       e.mAmplitude = 0.25f;
       e.mCurrentTime = 0.f;
       e.mLifetime = 1.f;
       e.mMaxDistance = maxDistance;

       mEffects.push_back(e);
    }

};
