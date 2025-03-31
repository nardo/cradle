// The cradle library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

class VectorModelInstance;

struct ObjectData
{
   char        name[256];
   Matrix2D    transform;
   bool        visible;

   void read(Stream & stream)
   {
      stream.readString(name);
      transform.read(stream);
      stream.read(&visible);
   }
};

struct ShapeData
{
   float32         strokeWidth;
   float32         strokeMiterLimit;
   ColorI      strokeColor;
   bool        strokeScreen;
   int32         strokeTexture;
   Matrix2D    strokeTexGen;
   bool        strokeTexGenLen;

   ColorI      fillColor;
   int32         fillTexture;
   Matrix2D    fillTexGen;

   void read(Stream & stream)
   {
      stream.read(&strokeWidth);
      stream.read(&strokeMiterLimit);
      stream.read(&strokeColor);
      stream.read(&strokeScreen);
      stream.read(&strokeTexture);
      strokeTexGen.read(stream);
      stream.read(&strokeTexGenLen);
   
      stream.read(&fillColor);
      stream.read(&fillTexture);
      fillTexGen.read(stream);
   }
};

struct TextureData
{
   int32   id;               // ID used by geo's to retrieve this particular texture
   char  textureFile[256]; // Texture file to use
   int32   falloffFunc;      // What function to use when evaluating falloff texture
   int32   falloffDim;       // Falloff texture dimension
   float32   falloffFactor;    // color = falloffFunc(pos, factor)
   bool  falloffCircle;    // when set generates a dimxdim texture, otherwise linear texture generated

   void read(Stream & stream)
   {
      stream.read(&id);
      stream.readString(textureFile);
      stream.read(&falloffFunc);
      stream.read(&falloffDim);
      stream.read(&falloffFactor);
      stream.read(&falloffCircle);
   }
};


static void renderPolyLine(const PolyLine & polyLine, const ColorI & col, bool emitTexCoords)
{
   if(col.alpha)
   {
       A.P.color(col);
       
      //glColor4ub(col.red, col.green, col.blue, col.alpha);
/*
      // TriStrip or TriList?
      glBegin(polyLine.mExtrudedTriStrip ? GL_TRIANGLE_STRIP : GL_TRIANGLES);
      if(emitTexCoords)
      {
         for(int32 i = 0; i < polyLine.mExtruded.size(); i++)
         {
            glTexCoord2f(polyLine.mTexCoords[i].x, polyLine.mTexCoords[i].y);
            glVertex2f(polyLine.mExtruded[i].x, polyLine.mExtruded[i].y);
         }
      }
      else
      {
         for(int32 i = 0; i < polyLine.mExtruded.size(); i++)
            glVertex2f(polyLine.mExtruded[i].x, polyLine.mExtruded[i].y);
      }*/
       A.P.draw_prepare();
       
       glBegin(GL_LINES);
       for(int32 i = 0; i < polyLine.mExtruded.size(); i += 3)
       {
           A.P._glVertex2fXF(polyLine.mExtruded[i].x, polyLine.mExtruded[i].y);
           A.P._glVertex2fXF(polyLine.mExtruded[i+1].x, polyLine.mExtruded[i+1].y);
           A.P._glVertex2fXF(polyLine.mExtruded[i+1].x, polyLine.mExtruded[i+1].y);
           A.P._glVertex2fXF(polyLine.mExtruded[i+2].x, polyLine.mExtruded[i+2].y);
           A.P._glVertex2fXF(polyLine.mExtruded[i+2].x, polyLine.mExtruded[i+2].y);
           A.P._glVertex2fXF(polyLine.mExtruded[i].x, polyLine.mExtruded[i].y);

       }

      glEnd();
   }
}

/**
 * Implies emiting texcoords
 *
 * @param polyLine  The line to render
 * @param col
 * @param texGen    Texgen matrix to use in generating texcoords
 * @param texGenLen When set will generate the texcoords using
 *                  the existing texcoords as input to the
 *                  texgen.
 */
static void renderPolyLine(const PolyLine & polyLine, const ColorI & col, const Matrix2D & texGen, bool texGenLen)
{
   if(col.alpha)
   {
       if(A.P.draw_outline)
       {
           glDisable(GL_TEXTURE_2D);
           float32 numVerts = polyLine.mExtruded.size();
           A.P.draw_prepare();
           
           glBegin(GL_LINES);
           for(int32 i = 0; i < polyLine.mExtruded.size(); i += 3)
           {
               float32 inten = ((i+2) / numVerts);
               glColor4f(inten, inten, inten, 1);
               A.P._glVertex2fXF(polyLine.mExtruded[i].x, polyLine.mExtruded[i].y);
               A.P._glVertex2fXF(polyLine.mExtruded[i+1].x, polyLine.mExtruded[i+1].y);
               A.P._glVertex2fXF(polyLine.mExtruded[i+1].x, polyLine.mExtruded[i+1].y);
               A.P._glVertex2fXF(polyLine.mExtruded[i+2].x, polyLine.mExtruded[i+2].y);
               A.P._glVertex2fXF(polyLine.mExtruded[i+2].x, polyLine.mExtruded[i+2].y);
               A.P._glVertex2fXF(polyLine.mExtruded[i].x, polyLine.mExtruded[i].y);
           }
           glEnd();
           glEnable(GL_TEXTURE_2D);
       }
       else
       {
           glColor4ub(col.red, col.green, col.blue, col.alpha);
           glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
           A.P.set_blend_mode(BlendModeBlend);
           A.P.draw_prepare();
           
           glBegin(polyLine.mExtrudedTriStrip ? GL_TRIANGLE_STRIP : GL_TRIANGLES);
           if(texGenLen)
           {
               for(int32 i = 0; i < polyLine.mExtruded.size(); i++)
               {
                   //Point texCoord = texGen * polyLine.mTexCoords[i];
                   Point texCoord = polyLine.mTexCoords[i];
                   glTexCoord2f(texCoord.x, texCoord.y);
                   A.P._glVertex2fXF(polyLine.mExtruded[i].x, polyLine.mExtruded[i].y);
               }
           }
           else
           {
               for(int32 i = 0; i < polyLine.mExtruded.size(); i++)
               {
                   Point texCoord = texGen * polyLine.mExtruded[i];
                   glTexCoord2f(texCoord.x, texCoord.y);
                   A.P._glVertex2fXF(polyLine.mExtruded[i].x, polyLine.mExtruded[i].y);
               }
           }
           glEnd();
           
           A.P.set_blend_mode(BlendModeNone);
       }
   }
}

//---------------------------------------------------------------------
// Helper to insert a keyframe into an animation. Will create animation if
// it does not exist.  Expects 'time', 'value' and 'transitionType' field to
// be in scope:
//---------------------------------------------------------------------
#define INSERT_KEY(type, fieldAddr) \
   { \
      KeyFrameAnimation<type> * anim = (KeyFrameAnimation<type> *)mAnimations.find(fieldAddr); \
      if(!anim) \
      { \
         anim = new KeyFrameAnimation<type>((type*)fieldAddr); \
         mAnimations.insert(anim); \
      } \
      anim->insertKey(KeyFrame<type>(time, *(type*)value, transitionType)); \
   }

//---------------------------------------------------------------------
// Helper to remove a keyframe from an animation.  If the animation does not
// exist it will 'return false'.  Will return 'true' if it removed the key.
// Expects 'time' field to be in scope:
//---------------------------------------------------------------------
#define REMOVE_KEY(type, fieldAddr) \
   { \
      KeyFrameAnimation<type> * anim = (KeyFrameAnimation<type> *)mAnimations.find(fieldAddr); \
      if(!anim) \
         return false; \
      anim->removeKey(time); \
      return true; \
   }


//---------------------------------------------------------------------
// A hierarchical model format.
//---------------------------------------------------------------------
class VectorModel
{
    enum { // Match AthenaCoreTexture enum:
       kRampFuncLinear = 0,
       kRampFuncCos,
       kRampFuncExp,
       kRampFuncParticle,
       kRampFuncTrail,
       kRampFuncRepair1,
       kRampFuncRepair2,
    };

public:
   float32                           mRotation;        //       "
   ColorI                        mStrokeColor;
   ColorI                        mFillColor;
protected:
   const char *                  mName;            // Name for this model (lookups/etc.)
   Matrix2D                      mTransform;       // Local frame for this model

   Point                       mTranslation;     // Performed in order if manipulating
   Point                       mScale;           // and replaces use of mTransform...

   bool                          mVisible;         // Visibility flag
   bool                          mTransformIdentity;

   VectorModel *                 mParent;          // Parent link
   array<VectorModelInstance *> mInstances;       // Drawable instances of this model
   array<VectorModel*>          mChildren;        // Child models

   VectorPrimitive *             mPrimitive;       // Primitive source for model
   uint32                           mPrimitiveType;   // Type of primitive (avoid rtti)

   // Stroke outline data:
   float32                           mStrokeMiterLimit;
//   ColorI                        mStrokeColor;
   const char *                  mStrokeTextureName;
   TextureHandle                  mStrokeTexture;
   Point                       mStrokeTexTranslation;
   float32                           mStrokeTexRotation;
   Point                       mStrokeTexScale;
   PolyLine *          mStrokeLine;

   // Fill data (generalized geo):
//   ColorI                        mFillColor;
   const char *                  mFillTextureName;
   TextureHandle                  mFillTexture;
   SimpleMesh *        mFillMesh;

   // Transform is built from ScaleRotateTranslate
   Point                       mFillTexTranslation;
   float32                           mFillTexRotation;
public:
    float32                           mStrokeWidth;
    struct TexLookup
    {
        int32 id;
        TextureHandle handle;
    };
    
    typedef array<TexLookup> TextureIDLookupArray;

   // ...
   float32                           mStrokeWidthMax;  
   void setChildStrokeWidthMax(const char * child, float32 widthMax)
    {
       VectorModel * model = getChild(child);
       if(model)
          model->mStrokeWidthMax = widthMax;
    }

   // ...

   Point                       mFillTexScale;

   // 
   InterpolatorSet               mAnimations;

   enum ModelCaps 
   {
      // General Caps
      kHasStroke              = bit(0),   // Can potentially display stroke path
      kHasFill                = bit(1),   // Can potentially display fill geo
      kDynamicGeometry        = bit(2),   // Geometry is dynamic

      // Stroke caps:
      kStrokeProjectedWidth   = bit(3),   // Stroke path width shold be projected onto screen
      kStrokeTexGenLen        = bit(4),   // Apply the stroke texgen along the length of the texcoords

      // Fill caps:
      kFillDynamicTexCoords   = bit(5),   // Texgen fill coords
   };
   bit_set                      mCaps;

   // Fugly load process....
   bool loadObjectData(Stream & stream)
    {
       ObjectData objData;
       objData.read(stream);

       mName = dup_c_str(objData.name);
       setTransform(objData.transform);
       mVisible = objData.visible;

       /////////////////////
       objData.transform.decompose(mTranslation, mScale, mRotation);
       /////////////////////

       return true;
    }
    static TextureHandle lookupLoadTexture(int32 id, TextureIDLookupArray &loadTextures)
    {
        for(uint32 i = 0; i < loadTextures.size(); i++)
            if(loadTextures[i].id == id)
                return loadTextures[i].handle;
        
       return 0;
    }

   bool loadSelf(Stream & stream, int32 type, TextureIDLookupArray &loadTextures)
    {
       // Pull out common objType data:
       loadObjectData(stream);

       switch(type)
       {
          case 'GRUP':   // AthenaCoreGroup
          {
             int32 children;
             stream.read(&children);
             while(children-- > 0)
             {
                stream.read(&type);

                if(!loadChild(stream, type, loadTextures))
                   return false;
             }
             break;
          }

          // AthenaCoreShape/primitve types:
          case 'LINE':   // AthenaCoreLine
          case 'RECT':   // AthenaCoreRect
          case 'CIRC':   // AthenaCoreCircle
          case 'ELIP':   // AthenaCoreEllipse
          case 'STAR':   // AthenaCoreStar
          case 'POLY':   // AthenaCorePolygon
          {
             mPrimitiveType = type;

             /////////////
             //mCaps.set(kDynamicGeometry);
             mCaps.set(kFillDynamicTexCoords);
             mCaps.set(kHasStroke);
             mCaps.set(kHasFill);
             /////////////

             ShapeData shapeData;
             shapeData.read(stream);

             // Set persisted shape data:
             mStrokeWidth = shapeData.strokeWidth;
             mStrokeMiterLimit = shapeData.strokeMiterLimit;
             mStrokeColor = shapeData.strokeColor;
             mCaps.set(kStrokeProjectedWidth, shapeData.strokeScreen);
             shapeData.strokeTexGen.decompose(mStrokeTexTranslation, mStrokeTexScale, mStrokeTexRotation);
             mCaps.set(kStrokeTexGenLen, shapeData.strokeTexGenLen);
             mFillColor = shapeData.fillColor;
             shapeData.fillTexGen.decompose(mFillTexTranslation, mFillTexScale, mFillTexRotation);

             // Build primitive
             mPrimitive = VectorPrimitive::construct(type);
             mPrimitive->read(stream);

             buildShape();

             // Fetch textures:
             if(shapeData.strokeTexture != -1)
                mStrokeTexture = lookupLoadTexture(shapeData.strokeTexture, loadTextures);
             if(shapeData.fillTexture != -1)
                mFillTexture = lookupLoadTexture(shapeData.fillTexture, loadTextures);

             break;
          }

          default:
             assert_msg(0, "Unknown chunk type in model.");
             return false;
       }

       return true;
    }
    
   bool loadChild(Stream & stream, int32 type, TextureIDLookupArray &loadTextures)
    {
       // Handle group stuff.
       VectorModel * child = new VectorModel;
       child->mParent = this;

       if(!child->loadSelf(stream, type, loadTextures))
          return false;

       mChildren.push_back(child);
       return true;
    }



public:
   VectorModel() :
    mName(0),
    mTransform(true),
    mVisible(true),
    mTransformIdentity(true),
    mParent(0),
    mPrimitive(0),
    mPrimitiveType(0),

    mStrokeWidth(1.f),
    mStrokeWidthMax(0.f),
    mStrokeColor(255,255,255,255),
    mStrokeTextureName(0),
    mStrokeMiterLimit(22.5f),
    mStrokeLine(0),

    mFillColor(255,255,255,255),
    mFillTextureName(0),
    mFillMesh(0) {}
    
   ~VectorModel()
    {
       while(mChildren.size())
       {
          delete mChildren.last();
          mChildren.pop_back();
       }

       while(mInstances.size())
       {
          delete mInstances.last();
          mInstances.pop_back();
       }

       delete mPrimitive;
       delete mStrokeLine;
    }


   const char * getName() const
    {
       return mName ? mName : "";
    }

   const Matrix2D & getTransform() const
    {
       return mTransform;
    }

   void setTransform(const Matrix2D & transform)
    {
       mTransform = transform;
       mTransformIdentity = transform.isIdentity();
    }

   void render(Matrix2D * view = 0)
    {
       if(!mVisible)
          return;

       Matrix2D transform;

       mTransformIdentity = false;
       if(!mTransformIdentity)
       {
          // Get transform position
          transform = Matrix2D::fromScaleRotateTranslate(mScale, mRotation, mTranslation);
          if(view)
             transform.mul(*view);

          A.P.modelView.push();
           mat4 m;
           transform.to_mat4_transpose(m);
          A.P.modelView.mul(m);
       }

       // Update geometry:
       if(mCaps.test(kDynamicGeometry) && mPrimitive)
          buildShape();

       // Draw stroke:
       if(mStrokeLine)
       {
          // Get approx. width of stroke in screen space.  This should be
          // evalulated per offset -- probably fine if we only perform
          // uniform scaling...
          float32 strokeWidth = 1.f;
          if(mCaps.test(kStrokeProjectedWidth))
          {
              float32 x, y, vw, vh;
              UserInterface::getViewport(x, y, vw, vh);
              
              strokeWidth = mStrokeWidth * vw/A.UI.canvasWidth;

             // clamp (hack)
             if(mStrokeWidthMax != 0.f && strokeWidth > mStrokeWidthMax)
                strokeWidth = mStrokeWidthMax;
          }
          else
             strokeWidth = mStrokeWidth;

    #if 0
          mStrokeLine->extrudeTriangleStrip(strokeWidth);
    #else
          mStrokeLine->extrudeTriangleList(strokeWidth);
    #endif

          if(mStrokeTexture.is_valid())
          {
             glEnable(GL_TEXTURE_2D);
              glBindTexture(GL_TEXTURE_2D, mStrokeTexture->m_textureID);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

              //renderPolyLine(*mStrokeLine, mStrokeColor, true);
             Matrix2D texGen = Matrix2D::fromScaleRotateTranslate(mStrokeTexScale,
                                                                  mStrokeTexRotation,
                                                                  mStrokeTexTranslation);
             renderPolyLine(*mStrokeLine, mStrokeColor, texGen, mCaps.test(kStrokeTexGenLen));

             glDisable(GL_TEXTURE_2D);
          }
          else
             renderPolyLine(*mStrokeLine, mStrokeColor, false);
       }

       // Draw fill:
       if(mFillMesh)
       {
          if(mFillTexture.is_valid())
          {
             // Update texcoords (may already be updated if regenerating prim):
             if(mCaps.test(kFillDynamicTexCoords))
             {
                Matrix2D texGen(Matrix2D::fromScaleRotateTranslate(mFillTexScale,
                                                                   mFillTexRotation,
                                                                   mFillTexTranslation));
                mFillMesh->applyTexCoords(texGen);
             }

             glEnable(GL_TEXTURE_2D);
              mFillTexture->glSet(0);

             mFillMesh->render(mFillColor, true);

             glDisable(GL_TEXTURE_2D);
          }
          else
             mFillMesh->render(mFillColor, false);
       }

       for(int32 i = 0; i < mChildren.size(); i++)
          mChildren[i]->render();

       if(!mTransformIdentity)
       {
          A.P.modelView.pop();
       }
    }

   void setTime(const float32 time)
    {
       mAnimations.setTime(time);
       for(int32 i = 0; i < mChildren.size(); i++)
          mChildren[i]->setTime(time);
    }

   void advanceTime(const float32 timeStep)
    {
       mAnimations.advanceTime(timeStep);
       for(int32 i = 0; i < mChildren.size(); i++)
          mChildren[i]->advanceTime(timeStep);
    }


   bool isVisible() const
    {
       return mVisible;
    }

   void setVisible(bool value)
    {
       mVisible = value;
    }


   VectorModel * getParent()
    {
       return mParent;
    }

   uint32 numChildren() const
    {
       return mChildren.size();
    }

   VectorModel * getChild(uint32 idx)
    {
       if(idx >= mChildren.size())
          return 0;

       return mChildren[idx];
    }

   VectorModel * getChild(const char * name) const
    {
       for(int32 i = 0; i < mChildren.size(); i++)
       {
          if(!strcasecmp(mChildren[i]->getName(), name))
             return mChildren[i];
       }
       return 0;
    }

   VectorModel * findChild(const char * name) const
    {
       VectorModel * child = getChild(name);
       if(!child)
       {
          for(int32 i = 0; i < mChildren.size() && !child; i++)
             child = mChildren[i]->findChild(name);
       }
       return child;
    }


   VectorModelInstance * createInstance()
    {
       VectorModelInstance * instance = new VectorModelInstance(this);
       mInstances.push_back(instance);
       return instance;
    }

   void destroyInstance(VectorModelInstance * instance)
    {
       if(instance && (instance->mModel == this))
       {
          for(int32 i = 0; i < mInstances.size(); i++)
             if(mInstances[i] == instance)
             {
                mInstances.erase(i);
                delete instance;
                return;
             }
       }
    }

    static void addLoadTexture(TextureData & data, TextureIDLookupArray &loadTextures)
    {
       TextureHandle texHandle;

       // Load or construct this texture?
       if(data.textureFile[0])
       {
          char fullPath[256];
          snprintf(fullPath, sizeof(fullPath), "./images/%s", data.textureFile);
           texHandle = A.TX.loadTexture(fullPath);
       }
       else
       {
          tRampCallback * rampCallback = 0;
          switch(data.falloffFunc)
          {
             case kRampFuncLinear:   rampCallback = linearRampCallback; break;
             case kRampFuncCos:      rampCallback = cosRampCallback; break;
             case kRampFuncExp:      rampCallback = expRampCallback; break;
             case kRampFuncParticle: rampCallback = particleRampCallback; break;
             case kRampFuncTrail:    rampCallback = trailRampCallback; break;
             case kRampFuncRepair1:  rampCallback = repairLineRampCallback; break;
             case kRampFuncRepair2:  rampCallback = repairCircleRampCallback; break;
          }
          assert_msg(rampCallback, "Doh");

          texHandle = createFalloffTextureRGBA(data.falloffDim, data.falloffFactor, rampCallback, data.falloffCircle);
       }

       if(texHandle.is_valid())
       {
           TexLookup t;
           t.id = data.id;
           t.handle = texHandle;
  
           loadTextures.push_back(t);
       }
    }

   // Load model:
   bool load(const char *fileName)
    {
       FileStream s;
       if(!s.open(fileName, "rb"))
           return false;
       return load(s);
   }
    
   bool load(Stream & stream)
    {
       int32 type = -1;
       stream.read(&type);

       if(type != 'SCEN')
          return false;

       // Common AthenaCoreObject stuff:
       loadObjectData(stream);

       // A texture table is needed to lookup textures by id.  Once
       // loaded it is no longer referenced.
       TextureIDLookupArray textureTable;

       // Now follows children -- TextureGroup type is handled immediately.
       int32 numChildren = 0;
       stream.read(&numChildren);

       while(numChildren-- > 0)
       {
          type = -1;
          stream.read(&type);

          if(type == 'TGRP')
          {
             int32 numTextures;
             stream.read(&numTextures);

             // Read in all textures and add to temp load db
             while(numTextures-- > 0)
             {
                stream.read(&type);
                if(type != 'TXTR')
                   return false;

                TextureData textureData;
                textureData.read(stream);

                addLoadTexture(textureData, textureTable);
             }
          }
          else
          {
             // Load normal object/shape
             if(!loadChild(stream, type, textureTable))
                return false;
          }
       }

       return true;
    }

   void buildShape()
    {
       if(!mCaps.test(kHasStroke|kHasFill))
          return;

       array<Point> pnts;
       mPrimitive->buildPath(pnts);

       if(mCaps.test(kHasStroke))
       {
          // Create stroke:
          if(!mStrokeLine)
             mStrokeLine = new PolyLine();

          mStrokeLine->mMiterLimit = mStrokeMiterLimit;
          mStrokeLine->setSource(pnts);
       }

       if(mCaps.test(kHasFill))
       {
          // Create mesh:
          if(!mFillMesh)
             mFillMesh = new SimpleMesh();

          ////////////////////////////
          array<Point> tmp;
          tmp.setSize(pnts.size());
          for(int32 i = 0; i < pnts.size(); i++)
             tmp[i] = Point(pnts[i].x, pnts[i].y);
          ////////////////////////////

          // Grab texgen info and build mesh:
          Matrix2D texGen = Matrix2D::fromScaleRotateTranslate(mFillTexScale, mFillTexRotation, mFillTexTranslation);
          mFillMesh->buildFromPath(tmp, texGen, mPrimitiveType != 'POLY');
       }
    }


   // Retrieve the field for the given track name:
   void * getFieldAddr(uint32 track)
    {
       switch(track)
       {
          case 'pTRA':   return &mTranslation;            break;
          case 'pSCL':   return &mScale;                  break;
          case 'cSTR':   return &mStrokeColor;            break;
          case 'cFIL':   return &mFillColor;              break;
          case 'fROT':   return &mRotation;               break;
          case 'fSTW':   return &mStrokeWidth;            break;
          case 'uSTA':   return &mStrokeColor.alpha;      break;
          case 'uFIA':   return &mFillColor.alpha;        break;
          case 'bVIS':   return &mVisible;                break;
          case 'pFTT':   return &mFillTexTranslation;     break;
          case 'pFTS':   return &mFillTexScale;           break;
          case 'fFTR':   return &mFillTexRotation;        break;
          case 'pSTT':   return &mStrokeTexTranslation;   break;
          case 'pSTS':   return &mStrokeTexScale;         break;
          case 'fSTR':   return &mStrokeTexRotation;      break;
       }

       if(mPrimitive)
          return mPrimitive->getFieldAddr(track);

       return 0;
    }


   uint32 getPrimitiveType() const
    {
       return mPrimitiveType;
    }

   VectorPrimitive * getPrimitive() const
    {
       return mPrimitive;
    }


   // Add/remove key to a named track.
   bool addKey(uint32 track, float32 time, void * value, eTransitionType transitionType)
    {
       void * addr = getFieldAddr(track);
       if(addr)
       {
          switch((track & 0xff000000) >> 24)
          {
             case 'p':   INSERT_KEY(Point, addr); break;
             case 'c':   INSERT_KEY(ColorI, addr);  break;
             case 'f':   INSERT_KEY(float32, addr);     break;
             case 'i':   INSERT_KEY(int32, addr);     break;
             case 'u':   INSERT_KEY(uint8, addr);      break;
             case 'b':   INSERT_KEY(bool, addr);    break;
          }
       }
       return true;
    }

   bool removeKey(uint32 track, float32 time)
    {
       void * addr = getFieldAddr(track);
       if(addr)
       {
          switch((track & 0xff000000) >> 24)
          {
             case 'p':   REMOVE_KEY(Point, addr); break;
             case 'c':   REMOVE_KEY(ColorI, addr);  break;
             case 'f':   REMOVE_KEY(float32, addr);     break;
             case 'i':   REMOVE_KEY(int32, addr);     break;
             case 'u':   REMOVE_KEY(uint8, addr);      break;
             case 'b':   REMOVE_KEY(bool, addr);    break;
          }
       }
       return true;
    }

   // Dynamic access to keyframe animations.
   bool addPointKey(uint32 track, float32 time, Point value, eTransitionType transitionType = kTransitionLinear)
      { return addKey(track, time, &value, transitionType); }
   bool addColorKey(uint32 track, float32 time, ColorI value, eTransitionType transitionType = kTransitionLinear)
      { return addKey(track, time, &value, transitionType); }
   bool addFloatKey(uint32 track, float32 time, float32 value, eTransitionType transitionType = kTransitionLinear)
      { return addKey(track, time, &value, transitionType); }
   bool addIntKey(uint32 track, float32 time, int32 value, eTransitionType transitionType = kTransitionLinear)
      { return addKey(track, time, &value, transitionType); }
   bool addCharKey(uint32 track, float32 time, uint8 value, eTransitionType transitionType = kTransitionLinear)
      { return addKey(track, time, &value, transitionType); }
   bool addBoolKey(uint32 track, float32 time, bool value, eTransitionType transitionType = kTransitionLinear)
      { return addKey(track, time, &value, transitionType); }
};

//---------------------------------------------------------------------
// Instance of a model:
//---------------------------------------------------------------------
class VectorModelInstance
{
   friend class VectorModel;

private:
   VectorModel *  mModel;        // Model this is an instance of
   Matrix2D       mTransform;

   float32            mAnimStart;    // Start time for animation
   float32            mAnimEnd;      // End time for animation (== start for oneshot)
   float32            mAnimCur;      // Curent animation time

   bit_set       mFlags;

   enum InstanceFlags
   {
      // Animation:
      kAnimActive       = bit(0),      // changing anim time
      kAnimGoingUp      = bit(1),      // otherwise going down...
      kAnimPingPong     = bit(2),      // toggling between up/down
      kAnimLoop         = bit(3),      // looping animation
   };
   VectorModelInstance(VectorModel * model):
    mModel(model),
    mTransform(true),
    mAnimStart(0.f),
    mAnimEnd(0.f),
    mAnimCur(0.f)
 {
 }
    ~VectorModelInstance() {}

public:

   const Matrix2D & getTransform() const
    {
       return mTransform;
    }

   void setTransform(const Matrix2D & transform)
    {
       mTransform = transform;
    }


   void render()
    {
       assert_msg(mModel, "Doh");
       mModel->setTime(mAnimCur);
       mModel->render(&mTransform);
    }


   void updateAnim(const float32 timeStep)
    {
       if(!mFlags.test(kAnimActive))
           return;

       // update the current frame:
       float32 step = mFlags.test(kAnimGoingUp) ? timeStep : -timeStep;
       mAnimCur += step;

       if(mFlags.test(kAnimGoingUp))
       {
          if(mAnimCur < mAnimEnd)
             return;

          if(mFlags.test(kAnimPingPong))
          {
             mFlags.clear(kAnimGoingUp);
             mAnimCur = mAnimEnd;
          }
          else if(mFlags.test(kAnimLoop))
          {
             mAnimCur = mAnimStart;
          }
          else
          {
             mFlags.clear(kAnimActive);
             mAnimCur = mAnimEnd;
          }
       }
       else
       {
          if(mAnimCur > mAnimStart)
             return;

          if(mFlags.test(kAnimPingPong))
          {
             mFlags.set(kAnimGoingUp);
             mAnimCur = mAnimStart;
          }
          else if(mFlags.test(kAnimLoop))
          {
             mAnimCur = mAnimEnd;
          }
          else
          {
             mFlags.clear(kAnimActive);
             mAnimCur = mAnimStart;
          }
       }
    }

   void advanceTime(const float32 timeStep)
    {
       updateAnim(timeStep);
    }


   void animateTo(const float32 time)
    {
       mFlags.clear(kAnimPingPong|kAnimLoop);
       mFlags.set(kAnimActive);
       mFlags.set(kAnimGoingUp, time > mAnimCur);

       mAnimStart = mAnimEnd = time;
    }

   void animatePingPong(const float32 start, const float32 end)
    {
       mFlags.clear(kAnimLoop);
       mFlags.set(kAnimGoingUp|kAnimPingPong|kAnimActive);

       mAnimStart = mAnimCur = start;
       mAnimEnd = end;
    }

   void animateLoop(const float32 start, const float32 end)
    {
       mFlags.clear(kAnimPingPong);
       mFlags.set(kAnimGoingUp|kAnimLoop|kAnimActive);

       mAnimStart = mAnimCur = start;
       mAnimEnd = end;
    }

};
