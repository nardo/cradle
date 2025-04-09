// The vectrex library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

// a polygon/contour and a series of triangles.
typedef Point PointType;
typedef array<PointType> PointVector;
#define EPSILON 0.0000000001f

class Triangulate
{
public:

  // triangulate a contour/polygon, places results in vector as series of triangles.
  static bool Process(const PointVector &contour, PointVector &result)
    {
       /* allocate and initialize list of Vertices in polygon */
       int32 n = contour.size();
       if( n < 3 )
          return false;

       // Don't want a closed contour...
       if(contour[0] == contour[n-1])
          n--;

       int *V = new int[n];
       
       /* we want a counter-clockwise polygon in V */
       if( 0.0f < Area(contour) )
          for(int32 v=0; v<n; v++)
             V[v] = v;
       else
          for(int32 v=0; v<n; v++)
             V[v] = (n-1)-v;

       int32 nv = n;
       
       /*  remove nv-2 Vertices, creating 1 triangle every time */
       int32 count = 2*nv;   /* error detection */

       for(int32 m=0, v=nv-1; nv>2; )
       {
          /* if we loop, it is probably a non-simple polygon */
          if (0 >= (count--))
          {
             //** Triangulate: ERROR - probable bad polygon!
             delete [] V;
             return false;
          }

        /* three consecutive vertices in current polygon, <u,v,w> */
        int u = v  ; if (nv <= u) u = 0;     /* previous */
        v = u+1; if (nv <= v) v = 0;         /* new v    */
        int w = v+1; if (nv <= w) w = 0;     /* next     */
       
        if ( Snip(contour,u,v,w,nv,V) )
        {
          int32 a,b,c,s,t;

          /* true names of the vertices */
          a = V[u]; b = V[v]; c = V[w];

          /* output Triangle */
          result.push_back( contour[a] );
          result.push_back( contour[b] );
          result.push_back( contour[c] );

          m++;

          /* remove v from remaining polygon */
          for(s=v,t=v+1;t<nv;s++,t++) V[s] = V[t]; nv--;

          /* resest error detection counter */
          count = 2*nv;
        }
      }

      delete [] V;

      return true;
    }

  // compute area of a contour/polygon
  static float32 Area(const PointVector &contour)
    {
      int32 n = contour.size();
      float32 a = 0.f;

      for(int32 p=n-1,q=0; q<n; p=q++)
         a += contour[p].x * contour[q].y - contour[q].x * contour[p].y;

      return a * 0.5f;
    }


  // decide if point Px/Py is inside triangle defined by
  // (Ax,Ay) (Bx,By) (Cx,Cy)
  static bool InsideTriangle(PointType a, PointType b, PointType c, PointType p)
    {
      float32 ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
      float32 cCROSSap, bCROSScp, aCROSSbp;

      ax = c.x - b.x;  ay = c.y - b.y;
      bx = a.x - c.x;  by = a.y - c.y;
      cx = b.x - a.x;  cy = b.y - a.y;
      apx= p.x - a.x;  apy= p.y - a.y;
      bpx= p.x - b.x;  bpy= p.y - b.y;
      cpx= p.x - c.x;  cpy= p.y - c.y;

      aCROSSbp = ax*bpy - ay*bpx;
      cCROSSap = cx*apy - cy*apx;
      bCROSScp = bx*cpy - by*cpx;

      return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
    };



private:
  static bool Snip(const PointVector &contour,int u,int v,int w,int n,int *V)
    {
      PointType a, b, c, p;

      a.x = contour[V[u]].x;
      a.y = contour[V[u]].y;

      b.x = contour[V[v]].x;
      b.y = contour[V[v]].y;

      c.x = contour[V[w]].x;
      c.y = contour[V[w]].y;

      if( EPSILON > (((b.x-a.x)*(c.y-a.y)) - ((b.y-a.y)*(c.x-a.x))) )
         return false;

      for(int32 i=0;i<n;i++)
      {
        if( (i == u) || (i == v) || (i == w) )
           continue;

        p.x = contour[V[i]].x;
        p.y = contour[V[i]].y;

        if(InsideTriangle(a, b, c, p))
           return false;
      }

      return true;
    }


};
