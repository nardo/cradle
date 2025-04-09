// The vectrex library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

struct Rect
{
    Point min, max;
    
    Rect() {}
    Rect(Point p1, Point p2) { set(p1, p2); }
    Rect(float32 x1, float32 y1, float32 x2, float32 y2)
    {
        Point p1(x1, y1);
        Point p2(x2, y2);
        set(p1, p2);
    }
    Point getCenter() { return (max + min) * 0.5; }
    void set(Point p1, Point p2)
    {
        if(p1.x < p2.x)
        {
            min.x = p1.x;
            max.x = p2.x;
        }
        else
        {
            min.x = p2.x;
            max.x = p1.x;
        }
        if(p1.y < p2.y)
        {
            min.y = p1.y;
            max.y = p2.y;
        }
        else
        {
            min.y = p2.y;
            max.y = p1.y;
        }
    }
    bool contains(const Point &p)
    {
        return p.x >= min.x && p.x <= max.x && p.y >= min.y && p.y <= max.y;
    }
    
    void unionPoint(const Point &p)
    {
        if(p.x < min.x)
            min.x = p.x;
        else if(p.x > max.x)
            max.x = p.x;
        if(p.y < min.y)
            min.y = p.y;
        else if(p.y > max.y)
            max.y = p.y;
    }
    
    void unionRect(const Rect &r)
    {
        if(r.min.x < min.x)
            min.x = r.min.x;
        if(r.max.x > max.x)
            max.x = r.max.x;
        if(r.min.y < min.y)
            min.y = r.min.y;
        if(r.max.y > max.y)
            max.y = r.max.y;
    }
    
    bool intersects(const Rect &r)
    {
        return min.x < r.max.x && min.y < r.max.y &&
        max.x > r.min.x && max.y > r.min.y;
    }
    bool intersects(const Point &p)
    {
        return p.x >= min.x && p.x <= max.x && p.y >= min.y && p.y <= max.y;
    }
    
    /// Perform an intersection operation with another box
    /// and store the results in this box.
    void intersect(const Rect& in_rIntersect)
    {
        if(!isEmpty())
        {
            min.setMin(in_rIntersect.min);
            max.setMax(in_rIntersect.max);
        }
        else
            *this = in_rIntersect;
    }
    
    void intersect(const Point in_rIntersect)
    {
        if(!isEmpty())
        {
            min.setMin(in_rIntersect);
            max.setMax(in_rIntersect);
        }
        else
            min = max = in_rIntersect;
    }
    
    bool isEmpty()
    {
        return min.x >= max.x || min.y >= max.y;
    }
    void expand(Point delta) { min -= delta; max += delta; }
    
    Point getExtents()
    {
        return max - min;
    }
    void makeValid()
    {
        set(min, max);
    }
    
    inline Point getClosestPoint(const Point refPt) const
    {
        Point closest;
        if      (refPt.x <= min.x) closest.x = min.x;
        else if (refPt.x >  max.x) closest.x = max.x;
        else                       closest.x = refPt.x;
        
        if      (refPt.y <= min.y) closest.y = min.y;
        else if (refPt.y >  max.y) closest.y = max.y;
        else                       closest.y = refPt.y;
        
        return closest;
    }
    
};
