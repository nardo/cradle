// The vectrex library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

class SimpleMesh
{
public:
   enum {
      kTriangleList,
      kTriangleStrip,
      kTriangleFan,
   };

   array<Point>   mVerts;
   array<Point>   mTexCoords;
   uint32               mType;

   SimpleMesh() :
      mType(kTriangleList)
   {
   }

   void buildFromPath(array<Point> & path, const Matrix2D & texGen, bool convex = true)
    {
       mVerts.clear();
       mTexCoords.clear();

       if(path.size() <= 2)
          return;

       // Create fan from centroid if regularish polygon
       if(convex)
       {
          mType = kTriangleFan;
          Rect box(path[0], path[0]);
       
          // Get center
          for(int32 i = 1; i < path.size(); i++)
             box.intersect(path[i]);
       
          mVerts.reserve(path.size() + 1);
          mTexCoords.reserve(path.size() + 1);
       
          // Build fan
          mVerts.push_back(box.getCenter());
          mTexCoords.push_back(texGen * box.getCenter());
       
          for(int32 i = 0; i < path.size(); i++)
          {
             mVerts.push_back(path[i]);
             mTexCoords.push_back(texGen * path[i]);
          }
       }
       else
       {
          // Triangulate the polygon -- complex poly not supported..
          mType = kTriangleList;

          Triangulate::Process(path, mVerts);
          mTexCoords.reserve(mVerts.size());

          for(int32 i = 0; i < mVerts.size(); i++)
             mTexCoords.push_back(texGen * mVerts[i] );
       }
    }

   void render(const ColorI & col, bool emitTexCoords = false)
    {
       if(col.alpha && mVerts.size())
       {
          glColor4ub(col.red, col.green, col.blue, col.alpha);
           A.C.draw_prepare();
          switch(mType)
          {
             case SimpleMesh::kTriangleList:  glBegin(GL_TRIANGLES);        break;
             case SimpleMesh::kTriangleStrip: glBegin(GL_TRIANGLE_STRIP);   break;
             default:                         glBegin(GL_TRIANGLE_FAN);     break;
          }

          if(emitTexCoords)
          {
             for(int32 i = 0; i < mVerts.size(); i++)
             {
                glTexCoord2f(mTexCoords[i].x, mTexCoords[i].y);
                 A.C._glVertex2fXF(mVerts[i].x, mVerts[i].y);
             }
          }
          else
          {
             for(int32 i = 0; i < mVerts.size(); i++)
                 A.C._glVertex2fXF(mVerts[i].x, mVerts[i].y);
          }
          glEnd();
       }
    }


   void applyTexCoords(const Matrix2D & texGen)
    {
       mTexCoords.setSize(mVerts.size());
       for(int32 i = 0; i < mVerts.size(); i++)
          mTexCoords[i] = texGen * mVerts[i];
    }

};

