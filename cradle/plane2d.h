// The cradle library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

#define kPlane2DEpsilon 0.001f

// Basic 2D hyperplane (line in standard form: Ax + By - C = 0)
class Plane2D
{
public:
  float32 mCoef[3];

  enum {
     kSpanning = 0,
     kInFront,
     kInBack,
     kCoincident
  };

  Plane2D() {}

  Plane2D(const LineSegment & line)
  {
     set(line);
  }

  Plane2D(const Point & start, const Point & end)
  {
     set(start, end);
  }

  Plane2D(const Plane2D & rhs)
  {
     mCoef[0] = rhs.mCoef[0];
     mCoef[1] = rhs.mCoef[1];
     mCoef[2] = rhs.mCoef[2];
  }

  void set(const LineSegment & line)
  {
     set(line.mP0, line.mP1);
  }

  void set(const Point & start, const Point & end)
  {
     float32 dx = end.x - start.x;
     float32 dy = end.y - start.y;

     mCoef[0] = -dy;
     mCoef[1] = dx;
     mCoef[2] = (dy * start.x) - (dx * start.y);
  }

  float32 distanceToPoint(const Point & pnt)
  {
     float32 c = (mCoef[0] * pnt.x) + (mCoef[1] * pnt.y) + mCoef[2];
     float32 d = sqrtf(mCoef[0] * mCoef[0] + mCoef[1] * mCoef[1]);
     return c / d;
  }

  int32 classifyPoint(const Point & pnt, const float32 epsilon = kPlane2DEpsilon) const
  {
     float32 c = (mCoef[0] * pnt.x) + (mCoef[1] * pnt.y) + mCoef[2];
     if(c > epsilon)
        return kInFront;
     else if(c < (-epsilon))
        return kInBack;
     else
        return kSpanning;
  }

  int32 classifyLine(const LineSegment & line)
   {
      int32 a = classifyPoint(line.mP0);
      int32 b = classifyPoint(line.mP1);

      // Point on line:
      if(a == kSpanning)
      {
         if(b == kSpanning)
            return kCoincident;
         else
            return b;
      }
      else if(b == kSpanning)
         return a;

      // Crosses line:
      if(a == kInFront && b == kInBack)
         return kSpanning;
      if(a == kInBack && b == kInFront)
         return kSpanning;

      // On a side:
      return a;
   }

  int32 splitLine(const LineSegment & line, LineSegment & front, LineSegment & back)
   {
      Plane2D p(line);

      float32 crossX = 0.f;
      float32 crossY = 0.f;

      float32 div = mCoef[0] * p.mCoef[1] - mCoef[1] * p.mCoef[0];

      if(div == 0.f)
      {
         if(p.mCoef[0] == 0.f)
            crossX = line.mP0.x;
         if(p.mCoef[1] == 0.f)
            crossY = line.mP0.y;
         if(mCoef[0] == 0.f)
            crossY = -mCoef[1];
         if(mCoef[1] == 0.f)
            crossX = mCoef[2];
      }
      else
      {
         crossX = (-mCoef[2] * p.mCoef[1] + mCoef[1] * p.mCoef[2]) / div;
         crossY = (-mCoef[0] * p.mCoef[2] + mCoef[2] * p.mCoef[0]) / div;
      }

      int32 a = classifyPoint(line.mP0);
      int32 b = classifyPoint(line.mP1);

      // No front or back:
      if(a == kSpanning && b == kSpanning)
         return kCoincident;

      // Split:
      if(a == kInBack && b == kInFront)
      {
         front.mP0.set(crossX, crossY);
         front.mP1.set(line.mP1);
         back.mP0.set(line.mP0);
         back.mP1.set(crossX, crossY);
         return kSpanning;
      }
      else if(a == kInFront && b == kInBack)
      {
         front.mP0.set(line.mP0);
         front.mP1.set(crossX, crossY);
         back.mP0.set(crossX, crossY);
         back.mP1.set(line.mP1);
         return kSpanning;
      }

      // One side or other (just set front or back):
      if(a == kSpanning)
      {
         if(b == kInFront)
            front = line;
         else
            back = line;
         return b;
      }
      else
      {
         if(a == kInFront)
            front = line;
         else
            back = line;
         return a;
      }
   }


  static void unitTest()
   {
      LineSegment planeLine(Point(0,0), Point(10,10));
      LineSegment testLine(Point(0,10), Point(10,0));

      Plane2D plane(planeLine);

      LineSegment front, back;
      int32 classify = plane.splitLine(testLine, front, back);

      logf(cradle, ("Plane2D:"));
      logf(cradle, (" - coef(%f, %f, %f)", plane.mCoef[0], plane.mCoef[1], plane.mCoef[2]));
      logf(cradle, (" - fromLine (%f, %f) -> (%f, %f)", planeLine.mP0.x, planeLine.mP0.y, planeLine.mP1.x, planeLine.mP1.y));
      logf(cradle, (" - testLine (%f, %f) -> (%f, %f)", testLine.mP0.x, testLine.mP0.y, testLine.mP1.x, testLine.mP1.y));
      logf(cradle, (" - split: %d", classify));
      logf(cradle, ("  > front (%f, %f) -> (%f, %f)", front.mP0.x, front.mP0.y, front.mP1.x, front.mP1.y));
      logf(cradle, ("  > back (%f, %f) -> (%f, %f)", back.mP0.x, back.mP0.y, back.mP1.x, back.mP1.y));
   }

};
