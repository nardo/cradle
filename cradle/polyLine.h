// The cradle library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

/**
 * Given a line list will output an offset program to be used in
 * generating a renderable mesh.
 *
 * @param vec     The list of input verts.
 * @param offsets Table of offsets to be applied at the input
 *                verts. For miter joints (between edges with
 *                large angle) this is simply an offset to apply
 *                at each vertex (+offset == left of edge
 *                direction, -offset == right of edge
 *                direction). For bevel corners (those with
 *                small angle between edges) there are two
 *                offsets output. First is the miter offset (to
 *                be added once at the vertex) and a cap offset
 *                (to be applied both +/- at the vertex). The
 *                miter offset generates the shared vertex
 *                between the two extruded line segments, while
 *                the cap +offset generates the vertex on first
 *                extruded segment, -offset generates the vertex
 *                on the second extruded segment.
 * @param corners For miter joints a '0' is output - a single
 *                offset should be consumed per input vert.
 *                Otherwise two offsets should be consumed per
 *                input vert.  If this is '1' then the bevel
 *                corner goes to the left, and if '2' it goes to
 *                the right.  Needed for generating appropriate
 *                winding for triangle strip.
 *
 */
#define NORMALIZE_DISTANCE
static void createPolyLineProgram(const array<Point> &vec, array<Point> &offsets, array<char> &corners, array<float32> &distances, float32 miterLimit)
{
   if(vec.size() < 2)
      return;
   
   // The angles used to determine if a corner should be beveled.
   const float kBevelLeft = miterLimit;
   const float kBevelRight = (180.f - kBevelLeft);

   // This guy loop?
   bool loop = vec[0] == vec[vec.size() - 1];

   float32 totalLen = 0.f;

   // Generate a list of edge vectors and running length:
   array<Point> edgeVector;
   edgeVector.reserve(vec.size());

   distances.reserve(vec.size());
   distances.push_back(0.f);
   
   for(int32 i = 0; i < vec.size() - 1; i++)
   {
      Point e = vec[i+1] - vec[i];

      float32 len = e.len();
      totalLen += len;

      // normalize:
      e *= (1.f / len);

      edgeVector.push_back(e);
      distances.push_back(len + distances[i]);
   }

#if defined(NORMALIZE_DISTANCE)
   // Walk through and normalize distances:
   for(int32 i = 0; i < distances.size(); i++)
      distances[i] /= totalLen;
#endif

   Point lastEdge;
   if(loop)
   {
      // Use last line segment:
      lastEdge = edgeVector.last();
      edgeVector.push_back(edgeVector.first());
   }
   else
   {
      // Use self (always generates miter):
      lastEdge = edgeVector.first();
      edgeVector.push_back(edgeVector.last());
   }
      
   for(int32 i = 0; i < edgeVector.size(); i++)
   {
      // Calculate the miter projection vector for this edge
      Point curEdge = edgeVector[i];
      float32 angle = (180 - curEdge.getAngleBetween(lastEdge )) * 0.5;
   
      lastEdge = curEdge;
      curEdge.rotate(-angle);

      float32 d = 1 / sin(DegToRad(angle));

      // Bevel the corner if angle within range (first edge only candidate
      // if looping):
      bool beveled = ((angle <= kBevelLeft) || (angle >= kBevelRight)) &&
                     ((i != 0) || ((i == 0) && loop));

      // Beveled corner adds an additional offset to generate the top of the
      // triangle joining the two line segments:
      if(0) //beveled)
      {
         // For bevel corners the first offset is from bevel line to shared corner:
         if(angle >= kBevelRight)
         {
            corners.push_back(2);
            d *= -1.f;
         }
         else
            corners.push_back(1);

         // Add miter offset
         offsets.push_back(curEdge * d);

         // Add the cap vector (will be projected left & right):
         d *= tan(DegToRad(angle));
         offsets.push_back(Point(curEdge * d).cross());
      }
      else
      {
         // Add miter offset:
         offsets.push_back(curEdge * d);
         corners.push_back(0);
      }
   }
}

static void extrudePolyLineProgram_TriangleList(const array<Point> &vec, float32 width, const array<Point> &offsets, const array<char> &corners, array<Point> &extruded)
{
   const Point * offs  = &offsets[0];
   width *= 0.5f;

   for(int32 i = 0; i < (vec.size() - 1); i++)
   {
      // Flagged bevel?
      if(0)//corners[i])
      {
         Point offA = *offs++;
         offs++; // SKIP
         Point offB = *offs;

         // Get left/right points from extruded segment end points:
         Point rightA = vec[i] - offA * width;
         Point leftA = vec[i] + offA * width;

         Point rightB = vec[i+1] - offB * width;
         Point leftB = vec[i+1] - offB * width;

         // Push back two tris:
         extruded.push_back(rightA);
         extruded.push_back(leftA);
         extruded.push_back(leftB);

         extruded.push_back(rightA);
         extruded.push_back(leftB);
         extruded.push_back(rightB);
      }
      else
      {
         // Miter corner:
         Point offA = *offs++;
         Point offB = *offs;

         // Get left/right points from extruded segment end points:
         Point rightA = vec[i] - offA * width;
         Point leftA = vec[i] + offA * width;

         Point rightB = vec[i+1] - offB * width;
         Point leftB = vec[i+1] + offB * width;

         // Push back two tris:
         extruded.push_back(rightA);
         extruded.push_back(leftA);
         extruded.push_back(leftB);

         extruded.push_back(rightA);
         extruded.push_back(leftB);
         extruded.push_back(rightB);
      }
   }
}

static void extrudePolyLineProgram_TriangleList(const array<Point> &vec, float32 width, const array<Point> &offsets, const array<char> &corners, const array<float32> &distances, array<Point> &extruded, array<Point> &texCoords)
{
   const Point * offs  = &offsets[0];
   width *= 0.5f;

   for(int32 i = 0; i < (vec.size() - 1); i++)
   {
      // Flagged bevel?
      if(corners[i])
      {
         return;
      }
      else
      {
          // Miter corner:
          Point offA = *offs++;
          Point offB = *offs;
          
          // Get left/right points from extruded segment end points:
          Point A = vec[i];
          Point B = vec[i+1];
          Point rightA = A - offA * width;
          Point leftA = A + offA * width;
          
          Point rightB = B - offB * width;
          Point leftB = B + offB * width;
          
          // Texcoords for same:
          float32 distA = distances[i];
          float32 distB = distances[i+1];
          
          Point texOutA(1.f, distA);
          Point texInA(0.f, distA);
          
          Point texOutB(1.f, distB);
          Point texInB(0.f, distB);
          
          // Push back two tris:
          Point rightVec = rightB - rightA;
          Point leftVec = leftB - leftA;
          Point segVec = vec[i+1] - vec[i];
          float32 segLen = segVec.len();
          
          segVec *= 1 / segLen;
          float32 lDot = leftVec.dot(segVec);
          float32 rDot = rightVec.dot(segVec);
          if(lDot < 0)
          {
              float32 scaleFactor = segLen / (segLen - lDot);
              float32 tex = 0.5 + scaleFactor * 0.5;
              float32 leftWidth = width * scaleFactor;
              leftA = A + offA * leftWidth;
              leftB = B + offB * leftWidth;
              texOutA.x = tex;
              texOutB.x = tex;
          }
          if(rDot < 0)
          {
              float32 scaleFactor = segLen / (segLen - rDot);
              float32 tex = 0.5 - scaleFactor * 0.5;

              float32 rightWidth = width * scaleFactor;
              rightA = A - offA * rightWidth;
              rightB = B - offB * rightWidth;
              texInA.x = tex;
              texInB.x = tex;
          }
              
          extruded.push_back(rightA);
          extruded.push_back(leftA);
          extruded.push_back(leftB);
          texCoords.push_back(texInA);
          texCoords.push_back(texOutA);
          texCoords.push_back(texOutB);

          extruded.push_back(rightA);
          extruded.push_back(leftB);
          extruded.push_back(rightB);

          texCoords.push_back(texInA);
          texCoords.push_back(texOutB);
          texCoords.push_back(texInB);
      }
   }
}

/**
 * Extrude the given polyline program (table of offests + corner
 * types) into a triangle strip.
 *
 * @param vec
 * @param width
 * @param offets
 * @param corners
 * @param extruded
 */
static void extrudePolyLineProgram_TriangleStrip(const array<Point> &vec, float32 width, const array<Point> &offsets, const array<char> &corners, array<Point> &extruded)
{
   const Point * offs  = &offsets[0];
   width *= 0.5f;

   for(int32 i = 0; i < vec.size(); i++)
   {
      // Beveled corner?
      if(corners[i])
      {
         // First offset is miter offset, second is bevel cap offset:
         Point off1 = *offs++;
         Point off2 = *offs++;

         Point bevelPoint = vec[i] + off1 * width;
         Point seg1Point = vec[i] + off2 * width;
         Point seg2Point = vec[i] - off2 * width;

         // Winding depends on direction of bevel corner:
         if(corners[i] == 2) // right
         {
            // Add cap:
            extruded.push_back(bevelPoint);
            extruded.push_back(seg1Point);
            extruded.push_back(seg2Point);

            // Restart strip:
            extruded.push_back(seg2Point);
            extruded.push_back(bevelPoint);
            extruded.push_back(seg2Point);
         }
         else
         {
            // Render cap:
            extruded.push_back(seg1Point);
            extruded.push_back(bevelPoint);
            extruded.push_back(seg2Point);

            // Restart strip:
            extruded.push_back(bevelPoint);
            extruded.push_back(seg2Point);
            extruded.push_back(bevelPoint);
         }
      }
      else
      {
         // Miter corner:
         Point off = *offs++;

         Point a = vec[i] - off * width;  // Right of edge
         Point b = vec[i] + off * width;  // Left of edge

         extruded.push_back(a);
         extruded.push_back(b);
      }
   }
}

/**
 * Extrude the given polyline program (table of offsets + corner
 * types) into a triangle strip.
 *
 * @param vec
 * @param width
 * @param offets
 * @param corners
 * @param extruded
 * @param distances
 * @param texCoords
 */
static void extrudePolyLineProgram_TriangleStrip(const array<Point> &vec, float32 width, const array<Point> &offsets, const array<char> &corners, const array<float32> &distances, array<Point> &extruded, array<Point> &texCoords)
{
   if(vec.size() < 2)
       return;

   const Point * offs  = &offsets[0];
   const float32 * dists = &distances[0];
   width *= 0.5f;

   for(int32 i = 0; i < vec.size(); i++)
   {
      float32 dist = *dists++;

      // tex.x is mapped across the line width to accomidate nx1 textures
      Point texOut(1.f, dist);
      Point texIn(0.f, dist);
      
      // Beveled corner?
      if(corners[i])
      {
         // First offset is miter offset, second is bevel cap offset:
         Point off1 = *offs++;
         Point off2 = *offs++;

         Point bevelPoint = vec[i] + off1 * width;
         Point seg1Point = vec[i] + off2 * width;
         Point seg2Point = vec[i] - off2 * width;

         // Winding depends on direction of bevel corner:
         if(corners[i] == 2) // right
         {
            // Add cap:
            extruded.push_back(bevelPoint);   texCoords.push_back(texIn);
            extruded.push_back(seg1Point);    texCoords.push_back(texOut);
            extruded.push_back(seg2Point);    texCoords.push_back(texOut);

            // Restart strip:
            extruded.push_back(seg2Point);    texCoords.push_back(texOut);
            extruded.push_back(bevelPoint);   texCoords.push_back(texIn);
            extruded.push_back(seg2Point);    texCoords.push_back(texOut);
         }
         else
         {
            // Render cap:
            extruded.push_back(seg1Point);    texCoords.push_back(texIn);
            extruded.push_back(bevelPoint);   texCoords.push_back(texOut);
            extruded.push_back(seg2Point);    texCoords.push_back(texIn);

            // Restart strip:
            extruded.push_back(bevelPoint);   texCoords.push_back(texOut);
            extruded.push_back(seg2Point);    texCoords.push_back(texIn);
            extruded.push_back(bevelPoint);   texCoords.push_back(texOut);
         }
      }
      else
      {
         // Miter corner:
         Point off = *offs++;

         Point a = vec[i] - off * width;  // Right of edge
         Point b = vec[i] + off * width;  // Left of edge

         extruded.push_back(a);    texCoords.push_back(texIn);
         extruded.push_back(b);    texCoords.push_back(texOut);
      }
   }
}


class PolyLine
{
public:
   array<Point>   mVerts;           // Input verts

   array<Point>   mOffsets;         // Offsets to generate joints/caps
   array<char>    mCorners;         // Corner flags used in building output
   array<float32>     mDistances;       // Distances to push out offsets

   array<Point>   mExtruded;        // Output verts (triangle strip)
   array<Point>   mTexCoords;       // Output texcoords

   float32                  mWidth;           // Line width
   float32                  mMiterLimit;
   bool                 mExtrudedTriStrip;

   PolyLine():
    mWidth(0.f),
    mMiterLimit(22.5f),
    mExtrudedTriStrip(true)
 {
 }

    /**
     * Set the source line list to be extruded.
     *
     * @param source
     */
    void setSource(const array<Point> &source, bool closed = false)
    {
       mVerts = source;
       mOffsets.clear();
       mCorners.clear();
       mExtruded.clear();
       mDistances.clear();
       mWidth = 0.f;

       if(closed && mVerts.size() > 2)
          mVerts.push_back(mVerts[0]);

       createPolyLineProgram(mVerts, mOffsets, mCorners, mDistances, mMiterLimit);
    }

    /**
     * Extrudes the polyline into a triangle strip.
     *
     * @param width
     *
     * @return
     */
    const array<Point> & extrudeTriangleStrip(float32 width)
    {
       // Cached?
       if((width == mWidth) && (mExtrudedTriStrip == true))
          return mExtruded;

       mExtruded.clear();
       mTexCoords.clear();
       mWidth = width;
       mExtrudedTriStrip = true;

       extrudePolyLineProgram_TriangleStrip(mVerts, mWidth, mOffsets, mCorners, mDistances, mExtruded, mTexCoords);
       return mExtruded;
    }

    /**
     * Extrudes the polyline into a triangle list.
     *
     * @param width
     *
     * @return
     */
    const array<Point> & extrudeTriangleList(float32 width)
    {
       // Cached?
       if((width == mWidth) && (mExtrudedTriStrip == false))
          return mExtruded;

       mExtruded.clear();
       mTexCoords.clear();
       mWidth = width;
       mExtrudedTriStrip = false;

       extrudePolyLineProgram_TriangleList(mVerts, mWidth, mOffsets, mCorners, mDistances, mExtruded, mTexCoords);
       return mExtruded;
    }
};
