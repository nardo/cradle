// The vectrex library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

struct PolyBuilder
{
   static void createRegularPolygon(array<Point> & pnts, int32 sides, float32 radius)
    {
       sides = clamp(sides, 3, 256);
       for(int32 i = 0; i < sides; i++)
       {
          float32 angle = (Float2Pi * i) / sides;
          Point pnt(cos(angle) * radius, sin(angle) * radius);
          pnts.push_back(pnt);
       }
    }

   static void createStarPolygon(array<Point> & pnts, int32 sides, float32 circumRadius, float32 inRadius, float32 radialShift)
    {
       if(circumRadius == inRadius)
       {
          createRegularPolygon(pnts, sides, circumRadius);
          return;
       }

       sides = clamp(sides, 3, 256);
       sides *= 2;

       for(int32 i = 0; i < sides; i++)
       {
          float32 angle;
          float32 radius;

          if(i&1)
          {
             angle = ((Float2Pi * i) / sides) + radialShift;
             radius = inRadius;
          }
          else
          {
             angle = (Float2Pi * i) / sides;
             radius =  circumRadius;
          }
          Point pnt(cos(angle) * radius, sin(angle) * radius);
          pnts.push_back(pnt);
       }
    }


   static void createCirclePolygon(array<Point> & pnts, float32 radius, int32 sides = 32)
    {
       createRegularPolygon(pnts, sides, radius);
    }

   static void createEllipsePolygon(array<Point> & pnts, Point radius, int32 sides = 32)
    {
       sides = clamp(sides, 3, 256);
       for(int32 i = 0; i < sides; i++)
       {
          float32 angle = (Float2Pi * i) / sides;
          Point pnt(cos(angle) * radius.x, sin(angle) * radius.y);
          pnts.push_back(pnt);
       }
    }


};

