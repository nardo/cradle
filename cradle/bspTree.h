// The cradle library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

   // Simple Node in a BSP tree:
   class BSPNode
   {
   public:
      Plane2D        mPartition;
      tSegmentList   mLines;
      BSPNode *      mFront;
      BSPNode *      mBack;

      BSPNode() :
         mFront(0),
         mBack(0)
      {
      }

      ~BSPNode()
      {
         delete mFront;
         delete mBack;
      }
   };

   // Simple 2D BSP Tree:
   class BSPTree
   {
   protected:
      BSPNode * mRoot;
       /**
        * Construct a node from first line in list and recursively
        * construct/classify the remaining lines.
        *
        * @param tree   Node to construct tree into
        * @param lines  List of line segments consumed
        */
      static void buildTree(BSPNode * tree, tSegmentList & lines)
       {
          if(lines.is_empty())
             return;

          LineSegment curLine = lines.last();
          lines.pop_back();

          tree->mPartition.set(curLine);
          tree->mLines.push_back(curLine);

          tSegmentList frontList;
          tSegmentList backList;

          // Classify all remaining lines:
          while(!lines.is_empty())
          {
             LineSegment line = lines.last();
             lines.pop_back();

             int32 result = tree->mPartition.classifyLine(line);

             if(result == Plane2D::kInFront)
                frontList.push_back(line);
             else if(result == Plane2D::kInBack)
                backList.push_back(line);
             else if(result == Plane2D::kSpanning)
             {
                LineSegment front;
                LineSegment back;

                int32 splitResult = tree->mPartition.splitLine(line, front, back);
                if(splitResult == Plane2D::kSpanning)
                {
                   frontList.push_back(front);
                   backList.push_back(back);
                }
                else
                {
                   // ... oops
                   assert_msg(0, "DOH");
                }
             }
             else if(result == Plane2D::kCoincident)
                tree->mLines.push_back(line);
          }

          if(!frontList.is_empty())
          {
             tree->mFront = new BSPNode();
             buildTree(tree->mFront, frontList);
          }

          if(!backList.is_empty())
          {
             tree->mBack = new BSPNode();
             buildTree(tree->mBack, backList);
          }
       }

       struct QueryInfo
       {
           Point position;
           float32 distance;
       };
      static void getSortedLines_BackToFront(QueryInfo &q, BSPNode * tree, tSegmentList & lines)
       {
          if(!tree)
             return;

          float32 dist = tree->mPartition.distanceToPoint(q.position);

          if(dist > kPlane2DEpsilon)
          {
             if(dist < q.distance)
             {
                getSortedLines_BackToFront(q, tree->mBack, lines);
                for(int32 i = 0; i < tree->mLines.size(); i++)
                   lines.push_back(tree->mLines[i]);
             }
             getSortedLines_BackToFront(q, tree->mFront, lines);
          }
          else if(dist < -kPlane2DEpsilon)
          {
             if(-dist < q.distance)
             {
                getSortedLines_BackToFront(q, tree->mFront, lines);
                for(int32 i = 0; i < tree->mLines.size(); i++)
                   lines.push_back(tree->mLines[i]);
             }
             getSortedLines_BackToFront(q, tree->mBack, lines);
          }
          else
          {
             // On partition plane:
             getSortedLines_BackToFront(q, tree->mFront, lines);
             getSortedLines_BackToFront(q, tree->mBack, lines);
          }
       }

      void getSortedLines_FrontToBack(QueryInfo &q, BSPNode * tree, tSegmentList & lines)
       {
          if(!tree)
             return;

          float32 dist = tree->mPartition.distanceToPoint(q.position);

          if(dist < -kPlane2DEpsilon)
          {
             getSortedLines_FrontToBack(q, tree->mBack, lines);

             if(-dist < q.distance)
             {
                for(int32 i = 0; i < tree->mLines.size(); i++)
                   lines.push_back(tree->mLines[i]);
                getSortedLines_FrontToBack(q, tree->mFront, lines);
             }
          }
          else if(dist > kPlane2DEpsilon)
          {
             getSortedLines_FrontToBack(q, tree->mFront, lines);

             if(dist < q.distance)
             {
                for(int32 i = 0; i < tree->mLines.size(); i++)
                   lines.push_back(tree->mLines[i]);
                getSortedLines_FrontToBack(q, tree->mBack, lines);
             }
          }
          else
          {
             // On partition plane:
             getSortedLines_FrontToBack(q, tree->mFront, lines);
             getSortedLines_FrontToBack(q, tree->mBack, lines);
          }
       }
       

   public:
      BSPTree() :
         mRoot(0)
      {
      }

      ~BSPTree()
      {
         delete mRoot;
      }

      void buildTree(tSegmentList & lines)
       {
          delete mRoot;
          mRoot = 0;

          if(!lines.is_empty())
          {
             mRoot = new BSPNode();
             buildTree(mRoot, lines);
          }
       }


      void getSortedLines(const Point & eye, tSegmentList & lines, bool backToFront = true, float32 maxDistance = max_value_float32)
       {
          QueryInfo q;
          q.position = eye;
          q.distance = maxDistance;

          lines.clear();
          if(backToFront)
             getSortedLines_BackToFront(q, mRoot, lines);
          else
             getSortedLines_FrontToBack(q, mRoot, lines);
       }

      static void unitTest()
       {
          BSPTree tree;

          tSegmentList lines;
          lines.push_back(LineSegment(Point(0,0), Point(10,10)));
          lines.push_back(LineSegment(Point(0,10), Point(10,0)));
          lines.push_back(LineSegment(Point(0,0), Point(10,0)));
          lines.push_back(LineSegment(Point(10,0), Point(10,10)));
          lines.push_back(LineSegment(Point(10,10), Point(0,10)));
          lines.push_back(LineSegment(Point(0,10), Point(0,0)));

          logf(cradle, ("BSPTree:"));
          for(int32 i = 0; i < lines.size(); i++)
              logf(cradle, ("- lines[%d]: (%f, %f) -> (%f, %f)", i, lines[i].mP0.x, lines[i].mP0.y, lines[i].mP1.x, lines[i].mP1.y));

          tree.buildTree(lines);
          Point eye(2.5, 5);

          tree.getSortedLines(eye, lines, true);
          logf(cradle, ("Sort BackToFront: eyePos: (%f, %f)", eye.x, eye.y));
          for(int32 i = 0; i < lines.size(); i++)
              logf(cradle, (" - sorted[%d]: (%f, %f) -> (%f, %f)", i, lines[i].mP0.x, lines[i].mP0.y, lines[i].mP1.x, lines[i].mP1.y));

          tree.getSortedLines(eye, lines, false);
          logf(cradle, ("Sort FrontToBack: eyePos: (%f, %f)", eye.x, eye.y));
          for(int32 i = 0; i < lines.size(); i++)
              logf(cradle, (" - sorted[%d]: (%f, %f) -> (%f, %f)", i, lines[i].mP0.x, lines[i].mP0.y, lines[i].mP1.x, lines[i].mP1.y));
       }


   };
