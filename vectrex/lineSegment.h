// The vectrex library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

class LineSegment;
typedef array<LineSegment> tSegmentList;
typedef array<Point>       tPointList;
#define kFloatEpsilon 1e-5f
#define kPointEpsilon .725f

class LineSegment
{
    static bool isZero(const float32 val, const float32 epsilon = kFloatEpsilon)
    {
       return (val > -epsilon) && (val < epsilon);
    }

    static bool areNear(const float32 a, const float32 b, const float32 epsilon = kFloatEpsilon)
    {
       return isZero((a-b), epsilon);
    }

    static bool areNear(const Point & a, const Point & b, const float32 epsilon = kPointEpsilon)
    {
       return areNear(a.x, b.x, epsilon) && areNear(a.y, b.y, epsilon);
    }

    // orders p0/p1:
    static bool pointLessThan(const Point & p0, const Point & p1)
    {
       if(p0.x < p1.x)
          return true;

       if(p0.x > p1.x)
          return false;

       if(p0.y < p1.y)
          return true;

       return false;
    }

public:
   Point mP0, mP1;

    LineSegment() {}
   LineSegment(const LineSegment & rhs) : mP0(rhs.mP0), mP1(rhs.mP1) {}
   LineSegment(const Point & p0, const Point & p1)
    :
     mP0(p0),
     mP1(p1){}

   // Segments have an endpoint in common
   bool sharesEndpoint(const LineSegment & other) const
    {
       return areNear(mP0, other.mP0) || areNear(mP0, other.mP1) ||
              areNear(mP1, other.mP0) || areNear(mP1, other.mP1);
    }

   // Segments share both endpoints
   bool sharesEndpoints(const LineSegment & other) const
    {
       return (areNear(mP0, other.mP0) || areNear(mP0, other.mP1)) &&
              (areNear(mP1, other.mP0) || areNear(mP1, other.mP1));
    }

   // If one of the segments is the given point
   bool hasPoint(const Point & pnt) const
    {
       return areNear(mP0, pnt) || areNear(mP1, pnt);
    }

   // Segments parallel
   bool parallel(const LineSegment & other) const
    {
       Point a = mP0 - mP1;
       Point b = other.mP0 - other.mP1;
       a.normalize();
       b.normalize();

       float32 dot = a.dot(b);
       return areNear(fabs(dot), 1.f);
    }

   // Segments are collinear
   bool onLine(const Point & pnt) const
    {
       Point v1 = mP1 - mP0;
       Point v2 = pnt - mP0;

       v1.normalize();
       v2.normalize();

       float32 cval = v1.cross(v2);
       return areNear(cval, 0.f);
    }

   bool collinear(const LineSegment & other) const
    {
       bool result = onLine(other.mP0) && onLine(other.mP1); //&& other.onLine(mP0) && other.onLine(mP1);
       return result;
    }


   // Segments 'overlap' [requires ordered elements]
   bool overlap(const LineSegment & other) const
    {
       if(areNear(mP0, other.mP0) || areNear(mP0, other.mP1) ||
          areNear(mP1, other.mP0) || areNear(mP1, other.mP1))
          return true;

       if(pointLessThan(mP1, other.mP0) ||
          pointLessThan(other.mP1, mP0))
          return false;

       return true;
    }


   // Join together the two segments (assumes collinear + overlap) [requires ordered elements] 
   void join(const LineSegment & other)
    {
       if(pointLessThan(other.mP0, mP0))
          mP0 = other.mP0;
       if(pointLessThan(mP1, other.mP1))
          mP1 = other.mP1;
    }

   // Order so that mP0 <= mP1
   void order()
    {
       if(!pointLessThan(mP0, mP1))
       {
          Point tmp = mP0;
          mP0 = mP1;
          mP1 = tmp;
       }
    }


   enum {
      kLeft = 0,
      kRight,
      kBeyond,
      kBehind, 
      kBetween,
      kOrigin,
      kDestination
   };

   int32 classifyPoint(const Point & pnt, float32 epsilon = 0.001f) const
    {
       Point v1 = mP1 - mP0;
       Point v2 = pnt - mP0;

       float32 cval = v1.cross(v2);

       if(cval > epsilon)
          return kLeft;
       if(cval < -epsilon)
          return kRight;
       if((v1.x*v2.x < epsilon) || (v1.y*v2.y < epsilon))
          return kBehind;
       if(v1.len() < v2.len())
          return kBeyond;
       if(areNear(mP0, pnt, epsilon))
          return kOrigin;
       if(areNear(mP1, pnt, epsilon))
          return kDestination;

       return kBetween;
    }


   // Removes/joins segments which are collinear [requires ordered elements] 
   static void joinCollinearSegments(tSegmentList & segments)
    {
       int32 numSegments = segments.size();
       int32 curSegment = 0;

       while(curSegment < numSegments)
       {
          LineSegment & cur = segments[curSegment];

          for(int32 i = curSegment + 1; i < numSegments; i++)
          {
             LineSegment & itr = segments[i];

             // Join these segments if possible
             if(cur.collinear(itr) && cur.overlap(itr))
             {
                cur.join(itr);
                segments.erase_fast(i);
                numSegments--;
                i = curSegment;
             }
          }

          curSegment++;
       }
    }


   static void removeEmptySegments(tSegmentList & segments)
    {
       for(int32 i = 0; i < segments.size(); i++)
       {
          if(areNear(segments[i].mP0, segments[i].mP1))
          {
             segments.erase_fast(i);
             i--;
          }
       }
    }

   static void removeDuplicateSegments(tSegmentList & segments)
    {
       int32 curSegment = 0;
       while(curSegment < segments.size())
       {
          LineSegment & cur = segments[curSegment];

          for(int32 i = (curSegment+1); i < segments.size(); i++)
          {
             if(cur.sharesEndpoints(segments[i]))
             {
                segments.erase_fast(i);
                i--;
             }
          }
          curSegment++;
       }
    }


   static void fromPointList(tSegmentList & segments, const tPointList & points, bool ordered)
    {
       segments.clear();
       addPointList(segments, points, ordered);
    }

   static void addPointList(tSegmentList & segments, const tPointList & points, bool ordered)
    {
       segments.reserve(segments.size() + (points.size() / 2));

       if(ordered)
       {
          for(int32 i = 0; i < points.size() / 2; i++)
          {
             LineSegment segment(points[i*2], points[i*2+1]);
             segment.order();

             segments.push_back(segment);
          }
       }
       else
       {
          for(int32 i = 0; i < points.size() / 2; i++)
             segments.push_back(LineSegment(points[i*2], points[i*2+1]));
       }
    }

   static void toPointList(const tSegmentList & segments, tPointList & points)
    {
       points.clear();
       points.reserve(segments.size() * 2);
       for(int32 i = 0; i < segments.size(); i++)
       {
          points.push_back(segments[i].mP0);
          points.push_back(segments[i].mP1);
       }
    }


    // Fill's outline with a connected segment list (removing from segments list).  Returns
    // true only when a loop is built.  Will return when no segments can be joined
    // or when a loop is formed. Always pulls at least 1 segment into outline.
static bool fetchConnectedSegments(tSegmentList & segments, tSegmentList & outline)
    {
       if(segments.size() == 0)
          return false;

       outline.clear();
       outline.push_back(segments.last());
       segments.pop_back();

       int32 head = 0;
       int32 tail = 0;

       for(int32 i = 0; i < segments.size(); i++)
       {
          if(segments[i].sharesEndpoint(outline[head]))
          {
             head = outline.size();
             outline.push_back(segments[i]);
             segments.erase_fast(i);

             i = -1;
          }
          else if(segments[i].sharesEndpoint(outline[tail]))
          {
             tail = outline.size();
             outline.push_back(segments[i]);
             segments.erase_fast(i);

             i = -1;
          }

          // loop?
          if(outline.size() > 2 && (outline[tail].sharesEndpoint(outline[head])))
             return true;
       }

       // loop?
       if(outline.size() > 2 && (outline[tail].sharesEndpoint(outline[head])))
          return true;

       return false;
    }

    
    // Create a point list representing a line loop from the given outline.  Returns
    // success if a lineloop was created (may not have consumed all input points)
   static bool createLineLoop(const tSegmentList & outline, tPointList & points) // ?[requires ordered elements]?
    {
       if(outline.size() < 2)
          return false;

       points.reserve(outline.size());

       tSegmentList work = outline;

       LineSegment lastJoined = work.last();
       work.pop_back();

       // Start it off...
       points.push_back(lastJoined.mP0);
       points.push_back(lastJoined.mP1);

       for(int32 i = 0; i < work.size(); i++)
       {
          // Just join in one direction...
          if(work[i].hasPoint(points.last()))
          {
             // join them:
             if(areNear(work[i].mP0, points.last()))
                points.push_back(work[i].mP1);
             else
                points.push_back(work[i].mP0);

             lastJoined = work[i];
             work.erase_fast(i);

             i = -1;
             continue;
          }
       }

       if(areNear(points.first(), points.last()))
       {
          points.last() = points.first();
          return true;
       }
       return false;
    }
};

