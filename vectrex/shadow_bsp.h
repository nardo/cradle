
// what do we want out of the shadow bsp tree?
// the goal is to process the shadow-casting lights as well as the occluders in the scene to produce a set of wall edge segments with the occlusion level of each segment as well as the mask of which lights affect that segment, as well as region poly soups (the floor areas if you will), that are identified also by occlusion level and light mask.
// since shadow-casting lights are a future addition, we'll start with just the occlustion level. Lighting and occlusion level will require some thought :-).
// occlusion level is a uint32 that runs from 0 (no occlusion) to the user-spcecified max_occlusion.
// each shadow-casting edge is added with an occlusion increment that adds to the occlusion level of anything behind the new edge from the observer position.


struct shadow_bsp
{
    
    struct strip_segment
    {
        strip_segment *next; // next segment in the strip
        strip_segment *prev; // previous segment in the strip;
        uint32 p0;  // index of the first point in the polytron's vertex list
        uint32 p1;
        uint32 split_p; // index of the split point or invalid_vertex if the edge is not split
        strip_segment * c0; // index of the child edge from p0 to split_p
        strip_segment * c1; // index of the child edge from split_p to p1
        uint32 occlusion_level;
    };

    struct node
    {
        Plane2D plane;
        uint32 pv0; // plane vertex 0
        uint32 pv1; // plane vertex 1
        node *front;
        node *back;
        strip_segment *segment;
        node *next_process;
        
        void set_plane(array<vec2> &verts, uint32 v0, uint32 v1)
        {
            pv0 = v0;
            pv1 = v1;
            plane.set(verts[v0], verts[v1]);
        }
    };
    
    enum {
        invalid_vertex = 0xFFFFFFFF,
    };
    
    arena_allocator<> _allocator;
    node *tree_root;
    node *process_list;
    polytron &p;
    
    node *_alloc_nodes(uint32 node_count)
    {
        node *ret = (node *) _allocator.allocate(node_count * sizeof(node));
        for(uint32 i = 0; i < node_count; i++)
        {
            ret[i].front = ret[i].back = ret[i].next_process = 0;
            ret[i].segment = 0;
        }
        return ret;
    }
    
    static node *no_occlusion() { static node n; return &n; }
    static node *full_occlusion() { static node n; return &n; }

    strip_segment *_alloc_segment(uint32 p0, uint32 p1)
    {
        strip_segment *new_seg = (strip_segment *) _allocator.allocate(sizeof(strip_segment));
        new_seg->next = new_seg->prev = new_seg->c0 = new_seg->c1 = 0;
        new_seg->split_p = invalid_vertex;
        new_seg->p0 = p0;
        new_seg->p1 = p1;
        new_seg->occlusion_level = 0;
        return new_seg;
    }

    uint32 max_occlusion;
    uint32 observer_vert;
    strip_segment **outlines;

    shadow_bsp(context &_c, polytron &_p) : p(_p), _allocator(&_c.get_page_allocator()) { tree_root = 0; process_list = 0;  }
    
    enum segment_classification
    {
        on_plane,
        in_front,
        in_back,
        front_to_back,
        back_to_front,
    };
    
    segment_classification classify_and_intersect(node *n, uint32 v0, uint32 v1, uint32 &intersect_vert)
    {
        Plane2D::plane_test_result r0 = n->plane.classifyPoint(p.vertices[v0]);
        Plane2D::plane_test_result r1 = n->plane.classifyPoint(p.vertices[v1]);
        if(r0 == Plane2D::kSpanning) // r0 is on the plane
        {
            if(r1 == Plane2D::kSpanning)
                return on_plane;
            else if(r1 == Plane2D::kInFront)
                return in_front;
            else
                return in_back;
        }
        else if(r1 == Plane2D::kSpanning)
        {
            if(r0 == Plane2D::kInFront)
                return in_front;
            else
                return in_back;
        }
        else if(r0 == r1)
            return (r0 == Plane2D::kInFront) ? in_front : in_back;
        
        // otherwise, segment crosses the plane, so compute the intersect point
        
        Plane2D seg_plane(p.vertices[v0], p.vertices[v1]);
        float32 crossX = 0.f;
        float32 crossY = 0.f;
        float32 div = n->plane.mCoef[0] * seg_plane.mCoef[1] - n->plane.mCoef[1] * seg_plane.mCoef[0];
        if(div == 0.f)
        {
           if(seg_plane.mCoef[0] == 0.f)
              crossX = p.vertices[v0].x;
           if(seg_plane.mCoef[1] == 0.f)
              crossY = p.vertices[v0].y;
           if(n->plane.mCoef[0] == 0.f)
              crossY = -n->plane.mCoef[1];
           if(n->plane.mCoef[1] == 0.f)
              crossX = n->plane.mCoef[2];
        }
        else
        {
           crossX = (-n->plane.mCoef[2] * seg_plane.mCoef[1] + n->plane.mCoef[1] * seg_plane.mCoef[2]) / div;
           crossY = (-n->plane.mCoef[0] * seg_plane.mCoef[2] + n->plane.mCoef[2] * seg_plane.mCoef[0]) / div;
        }
        intersect_vert = p.vertices.size();
        p.vertices.increment();
        p.vertices[intersect_vert].set(crossX, crossY);
        if(r0 == Plane2D::kInFront)
            return front_to_back;
        else
            return back_to_front;
    }
    
    // setup is called to load up the bsp tree
    // this will first load up the strip tree segments with the border_line_strips from the polytron.
    // we preload the segments tree one for one with the border line strips, so the segments list matches the original inputs at the end of the process (ie the first n segments are the original border outlines where n = p.border_line_strips.size()
    void setup(vec2 observer, float32 left, float32 right, float32 top, float32 bottom)
    {
        _allocator.clear();
        
        max_occlusion = 1;
        array<vec2> &verts = p.vertices;
        uint32 num_strips = p.border_line_strips.size();
        
        outlines = (strip_segment **) _allocator.allocate(num_strips * sizeof(strip_segment *));
        for(uint32 i = 0; i < num_strips; i++)
            _setup_strip(i, p.border_line_strips[i].index_start, p.border_line_strips[i].vertex_count);
        // insert the vertices for the four initial shadow planes:
        observer_vert = p.vertices.size();
        verts.resize(observer_vert + 5);
        verts[observer_vert] = observer;
        verts[observer_vert + 1].set(left, top);
        verts[observer_vert + 2].set(right, top);
        verts[observer_vert + 3].set(right, bottom);
        verts[observer_vert + 4].set(left, bottom);
        // insert the four shadow planes
        tree_root = _alloc_nodes(4);

        tree_root[0].back = full_occlusion();
        tree_root[0].front = tree_root + 1;
        tree_root[0].set_plane(verts, observer_vert + 1, observer_vert + 2);
        tree_root[1].back = full_occlusion();
        tree_root[1].front = tree_root + 2;
        tree_root[1].set_plane(verts, observer_vert + 2, observer_vert + 3);
        tree_root[2].back = full_occlusion();
        tree_root[2].front = tree_root + 3;
        tree_root[2].set_plane(verts, observer_vert + 3, observer_vert + 4);
        tree_root[3].back = full_occlusion();
        tree_root[3].front = no_occlusion();
        tree_root[3].set_plane(verts, observer_vert + 4, observer_vert + 1);

        // now add all the strips we set up into the tree
        for(uint32 i = 0; i < num_strips; i++)
        {
            strip_segment *walk = outlines[i];
            while(walk)
            {
                strip_segment *next = walk->next;
                _add_occlusion_segment(tree_root, walk);
                walk = next;
            }
        }
        for(node *walk = process_list; walk; walk = walk->next_process)
        {
            _clip_segment_recurse(walk->segment, walk->front);
        }
    }
    
    void _add_occlusion_segment(node * &tree, strip_segment *s)
    {
        assert(s->c0 == 0);
        assert(s->c1 == 0);
        assert(s->split_p == invalid_vertex);

        // see if the segment is back-facing:
        Plane2D seg_plane(p.vertices[s->p0], p.vertices[s->p1]);
        Plane2D::plane_test_result r0 = seg_plane.classifyPoint(p.vertices[observer_vert]);
        if(r0 == Plane2D::kInBack)
        {
            s->occlusion_level = max_occlusion + 1;
            return;
        }
        
        // if this segment is fully occluded, mark it as such and return
        if(tree == full_occlusion() || s->occlusion_level == max_occlusion)
        {
            s->occlusion_level = max_occlusion;
            return;
        }
        // if this segment lands fully on a no_occlusion leaf, construct a shadow volume and return the new node
        if(tree == no_occlusion())
        {
            // construct a shadow volume for this segment
            tree = _alloc_nodes(3);
            tree[0].front = no_occlusion();
            tree[0].set_plane(p.vertices, s->p0, s->p1);
            tree[0].back = tree + 1;

            // set up this segment for later processing -- it'll need to get clipped down the front tree of the new node, once the full tree is formed
            tree[0].segment = s;
            tree[0].next_process = process_list;
            process_list = tree;
            
            tree[1].set_plane(p.vertices, observer_vert, s->p0);
            tree[1].front = tree + 2;
            tree[1].back = no_occlusion();

            tree[2].set_plane(p.vertices, observer_vert, s->p1);
            tree[2].front = no_occlusion();
            tree[2].back = full_occlusion();
            return;
        }
        
        // otherwise there is a tree split node here, so classify this new segment:
        uint32 intersect_vert;
        segment_classification result = classify_and_intersect(tree, s->p0, s->p1, intersect_vert);
        
        if(result == in_front)
        {
            // the segment is entirely on the front side
            _add_occlusion_segment(tree->front, s);
            return;
        }
        if(result == in_back)
        {
            _add_occlusion_segment(tree->back, s);
            return;
        }
        if(result == on_plane)
        {
            // we need to pass this segment down both sides of the tree
            _add_occlusion_segment(tree->front, s);
            
            // the segment may have been split on the first pass, so we need to loop through all the partial segments for passing it down the other side of the tree.
            _add_occlusion_segment_tree_back(tree->back, s);
            return;
        }
        // otherwise we have an intersection
        // add new sub-segments
        _split_segment(s, intersect_vert);
        
        if(result == front_to_back)
        {
            _add_occlusion_segment(tree->front, s->c0);
            _add_occlusion_segment(tree->back, s->c1);
        }
        else
        {
            _add_occlusion_segment(tree->back, s->c0);
            _add_occlusion_segment(tree->front, s->c1);
        }
    }
    
    void _split_segment(strip_segment *seg, uint32 intersect_vert)
    {
        assert(seg->c0 == 0);
        assert(seg->c1 == 0);
        assert(seg->split_p == invalid_vertex);
        seg->split_p = intersect_vert;
        seg->c0 = _alloc_segment(seg->p0, intersect_vert);
        seg->c1 = _alloc_segment(intersect_vert, seg->p1);
        seg->c0->occlusion_level = seg->occlusion_level;
        seg->c1->occlusion_level = seg->occlusion_level;
    }
    
    void _add_occlusion_segment_tree_back(node *&n, strip_segment *s)
    {
        return;
        if(s->split_p == invalid_vertex)
            _add_occlusion_segment(n, s);
        else
        {
            _add_occlusion_segment_tree_back(n, s->c0);
            _add_occlusion_segment_tree_back(n, s->c1);
        }
    }
    
    void _clip_segment_recurse(strip_segment *s, node *n)
    {
        if(s->split_p != invalid_vertex)
        {
            _clip_segment_recurse(s->c0, n);
            _clip_segment_recurse(s->c1, n);
            return;
        }
        // ok, we're down to a simple segment...
        if(n == no_occlusion())
            return;
        
        if(n == full_occlusion())
        {
            s->occlusion_level = max_occlusion;
            return;
        }
        // otherwise, this is not a leaf...
        uint32 intersect_vert;
        segment_classification result = classify_and_intersect(n, s->p0, s->p1, intersect_vert);
        
        switch(result)
        {
            case on_plane:
                // recurse down both sides of the tree
                _clip_segment_recurse(s, n->front);
                _clip_segment_recurse(s, n->back);
                return;
            case in_front:
                _clip_segment_recurse(s, n->front);
                return;
            case in_back:
                _clip_segment_recurse(s, n->back);
                return;
            case front_to_back:
                _split_segment(s, intersect_vert);
                _clip_segment_recurse(s->c0, n->front);
                _clip_segment_recurse(s->c1, n->back);
                return;
            case back_to_front:
                _split_segment(s, intersect_vert);
                _clip_segment_recurse(s->c0, n->back);
                _clip_segment_recurse(s->c1, n->front);
                return;
        }
    }
    
    
    void _setup_strip(uint32 seg_index, uint32 index_start, uint32 vert_count)
    {
        outlines[seg_index] = 0;
        strip_segment **walk = &outlines[seg_index];
        strip_segment *prev = 0;
        while(vert_count >= 2)
        {
            strip_segment *new_seg = _alloc_segment(p.indices[index_start], p.indices[index_start+1]);
            new_seg->prev = prev;
            *walk = new_seg;
            prev = new_seg;
            walk = &new_seg->next;
            vert_count--;
            index_start++;
        }
    }
    
    void draw_line(vec2 &p0, vec2 &p1)
    {
        glLineWidth(4);
        A.C.draw_line(p0, p1);
        vec2 eD = p1 - p0;
        eD.normalize();
        vec2 n(eD.y, -eD.x);
        glLineWidth(1);
        A.C.draw_line(p1, p1 - eD * 10 + n * 10);
        A.C.draw_line(p1, p1 - eD * 10 - n * 10);
    }
    void recurse_render_seg(strip_segment *s)
    {
        if(s->split_p == invalid_vertex)
        {
            // this is a leaf of the segment tree, render it...
            vec2 p0 = p.vertices[s->p0];
            vec2 p1 = p.vertices[s->p1];
            vec2 o = p.vertices[observer_vert];
            if(s->occlusion_level == max_occlusion)
            {
                A.C.colorf(1,1,0);
                draw_line(p0, p1);
            }
            else if(s->occlusion_level == max_occlusion + 1)
            {
                A.C.colorf(1,0,0);
                draw_line(p0, p1);
            }
            else
            {
                A.C.colorf(0,1,1);
                draw_line(p0, p1);

                A.C.draw_line(o, p0);
                A.C.draw_line(o, p1);
            }
        }
        else
        {
            recurse_render_seg(s->c0);
            recurse_render_seg(s->c1);
        }
    }
    
    void render_test()
    {
        vec2 box_offset(8,8);
        for(uint32 i = 0; i < p.border_line_strips.size(); i++)
        {
            strip_segment *segs = outlines[i];
            uint32 seg_index = 0;
            while(segs)
            {
                recurse_render_seg(segs);
                vec2 v = p.vertices[segs->p0];
                A.C.colorf(1,1,1);
                UserInterface::drawStringf(v.x - 4, v.y - 4, 15, "%d.%d", i, seg_index);

                seg_index++;

                segs = segs->next;
            }
        }
    }
};
