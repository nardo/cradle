// The vectrex library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

enum {
   kNumCircleSegments   = 32,
   kNumEllipseSegments  = 32,
};


// Describes a base primitive which models are build on.
class VectorPrimitive
{
public:
    virtual ~VectorPrimitive() {}
   virtual void * getFieldAddr(uint32 name) = 0;
   virtual void read(Stream & stream) = 0;
   virtual void buildPath(array<Point> & pnts) = 0;

   static VectorPrimitive * construct(uint32 name)
    {
       switch (name)
       {
          case 'LINE': return new LinePrimitive;    break;
          case 'RECT': return new RectPrimitive;    break;
          case 'CIRC': return new CirclePrimitive;  break;
          case 'ELIP': return new EllipsePrimitive; break;
          case 'STAR': return new StarPrimitive;    break;
          case 'POLY': return new PolygonPrimitive; break;
       }
       return 0;
    }

};

class LinePrimitive : public VectorPrimitive
{
public:
   Point    mStart;
   Point    mEnd;

   void read(Stream & stream)
    {
       stream.read(&mStart.x);
       stream.read(&mStart.y);
       stream.read(&mEnd.x);
       stream.read(&mEnd.y);
    }

   void * getFieldAddr(uint32 name)
    {
       switch(name)
       {
          case 'pSTA':   return &mStart;   break;
          case 'pEND':   return &mEnd;     break;
       }
       return 0;
    }

   void buildPath(array<Point> & pnts)
    {
       pnts.push_back(mStart);
       pnts.push_back(mEnd);
    }

};
class RectPrimitive : public VectorPrimitive
{
public:
   Point    mPos;
   Point    mExtent;

   void read(Stream & stream)
    {
       stream.read(&mPos.x);
       stream.read(&mPos.y);
       stream.read(&mExtent.x);
       stream.read(&mExtent.y);
    }

   void * getFieldAddr(uint32 name)
    {
       switch(name)
       {
          case 'pPOS':   return &mPos;     break;
          case 'pEXT':   return &mExtent;  break;
       }
       return 0;
    }

   void buildPath(array<Point> & pnts)
    {
       pnts.reserve(5);
       pnts.push_back(Point(mPos.x, mPos.y));
       pnts.push_back(Point(mPos.x + mExtent.x, mPos.y));
       pnts.push_back(Point(mPos.x + mExtent.x, mPos.y + mExtent.y));
       pnts.push_back(Point(mPos.x, mPos.y + mExtent.y));
       pnts.push_back(pnts[0]);
    }


};
class CirclePrimitive : public VectorPrimitive
{     
public:
   Point    mCenterPos;
   float32      mRadius;

   void read(Stream & stream)
    {
       stream.read(&mCenterPos.x);
       stream.read(&mCenterPos.y);
       stream.read(&mRadius);
    }

   void * getFieldAddr(uint32 name)
    {
       switch(name)
       {
          case 'pPOS':   return &mCenterPos;  break;
          case 'fRAD':   return &mRadius;     break;
       }
       return 0;
    }

   void buildPath(array<Point> & pnts)
    {
       pnts.reserve(kNumCircleSegments + 1);
       PolyBuilder::createRegularPolygon(pnts, kNumCircleSegments, mRadius);
       pnts.push_back(pnts[0]);
    }

};
class EllipsePrimitive : public VectorPrimitive
{
public:
   Point    mCenterPos;
   Point    mRadiusPos;

   void read(Stream & stream)
    {
       stream.read(&mCenterPos.x);
       stream.read(&mCenterPos.y);
       stream.read(&mRadiusPos.x);
       stream.read(&mRadiusPos.y);
    }

   void * getFieldAddr(uint32 name)
    {
       switch(name)
       {
          case 'pPOS':   return &mCenterPos;  break;
          case 'pRAD':   return &mRadiusPos;  break;
       }
       return 0;
    }

   void buildPath(array<Point> & pnts)
    {
       pnts.reserve(kNumCircleSegments + 1);
       PolyBuilder::createEllipsePolygon(pnts, mRadiusPos, kNumCircleSegments + 1);
       pnts.push_back(pnts[0]);
    }
};
class StarPrimitive : public VectorPrimitive
{
public:
   Point    mCenterPos;
   int32      mSides;
   float32      mCircumRadius;
   float32      mInnerRadius;
   float32      mRadialShift;

   void read(Stream & stream)
    {
       stream.read(&mCenterPos.x);
       stream.read(&mCenterPos.y);
       stream.read(&mSides);
       stream.read(&mCircumRadius);
       stream.read(&mInnerRadius);
       stream.read(&mRadialShift);
    }

   void * getFieldAddr(uint32 name)
    {
       switch(name)
       {
          case 'pPOS':   return &mCenterPos;     break;
          case 'iSID':   return &mSides;         break;
          case 'fRAD':   return &mCircumRadius;  break;
          case 'fIRD':   return &mInnerRadius;   break;
          case 'fRST':   return &mRadialShift;   break;
       }
       return 0;
    }

   void buildPath(array<Point> & pnts)
    {
       pnts.reserve((mCircumRadius == mInnerRadius ? mSides : mSides * 2) + 1);
       PolyBuilder::createStarPolygon(pnts, mSides, mCircumRadius, mInnerRadius, mRadialShift);
       pnts.push_back(pnts[0]);
    }

};
class PolygonPrimitive : public VectorPrimitive
{
public:
   bool                    mClosed;
   array<Point>      mVerts;

   void read(Stream & stream)
    {
       int32 numVerts;

       stream.read(&mClosed);
       stream.read(&numVerts);

       mVerts.reserve(numVerts);
       for(int32 i = 0; i < numVerts; i++)
       {
          Point pnt;
          stream.read(&pnt.x);
          stream.read(&pnt.y);
          mVerts.push_back(pnt);
       }
    }

   void * getFieldAddr(uint32 name)
    {
       return 0;
    }

   void buildPath(array<Point> & pnts)
    {
       pnts.reserve(mVerts.size() + (mClosed ? 1 : 0));
       pnts = mVerts;

       if(mClosed && mVerts.size())
          pnts.push_back(pnts[0]);
    }

};
