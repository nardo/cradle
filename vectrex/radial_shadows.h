// radial_visibility_clipper computes the visibility level of geometry segments and areas of a top-down 2D scene. The clipper functions by converting the geometry into a polar coordinate representation - each (x,y) vertex is transformed into (radius, d) from a central "observer" vertex position. Each occluder primitive/edge can specify an additive "occlusion level" - each point in the space has an occlusion level that is the sum of the occlusion levels of the edges crossed by a ray from the observer to that point, up to the max_occlusion specified for the clipper.
// when adding polygons to the clipper, a winding order may be specified so that backfacing edges don't add to the occlusion level of geometry beyond back edges from the observer pov.
// radial_visibility_clipper uses a polytron as its backing store for vertex data. New vertices (intersections, etc) are added to the polytron's vertex list. polygon edge adjacency is preserved - for each primitive added to the clipper, an output polygon will be produced with additional edge data specifying occlusion level for the edge segment.
// the radial clipper process works as follows:
// primitives are added edge-by-edge. Each vertex creates a "ray" - theta of that ray in radians, the unit vector from the observer along that ray, and the normal vector to the ray. rays are shared between adjacent edges, as well as new edges that are clipped against that ray. Each edge tracks the ray and radius for its beginning and ending vertex. When an added edge crosses 0 radians, that edge is clipped, and both segments are added to the overall edge list, so that all edges in the edge list begin and end in the range of 0...2pi radians. Primitive winding direction is also processed in this step, such that each edge's p0 ray theta is <= its p1 ray theta.
// After all geometry is added to the clipper, the edge list is sorted by p0 ray theta, at which point the edge processing loop begins.
// The edge processing loop does a circular sweep through the edges starting at theta=0 to theta=2pi. At each step, edges are added or removed from the active process list, and the active list is bubble sorted by current radius value for that edge. The initial implementation assumes that edges do not intersect.

struct radial_visibility_clipper
{
    arena_allocator<> _allocator;
    polytron &p;
    uint32 max_occlusion;
    uint32 observer_vertex_index;
    vec2 observer;
    
    enum {
        invalid_vertex = 0xFFFFFFFF,
    };

    struct ray
    {
        vec2 v; // ray vector
        vec2 n; // ray normal
        float32 theta; // angle of ray in radians
        uint32 sort_index; // the index of this ray in the sorted ray list. rays with the same theta will have the same sort_index.
        uint32 vi;
    };
    
    struct edge
    {
        uint32 v0; // index of the first vertex in the poltyron's vertex list
        uint32 v1; // index of the second vertex
        vec2 n0; // outline "normal" at v0
        vec2 n1; // outline "normal" at v1
        float32 t0; // original "t" value from the outline
        float32 t1;
        ray *ray0; // ray from the observer point through vertex 0
        ray *ray1; // ray through vertex 1
        float32 r0; // radius along ray0 for v0
        float32 r1; // radius along ray1 for v1
        vec2 v; // unit vector in the direction from v0 to v1
        vec2 n; // unit edge normal
        float32 l; // length of edge
        bool new_edge; // is this edge new in the process list?
        
        uint32 occlusion_level; // how much this edge adds to the occlusion of what's behind it.
        bool flipped; // true if this edge is backface "flipped" - because ray0->theta always needs to be <= ray1->theta;

        float32 cur_r; // radius of the intersection point between this edge and the current process ray
        uint32 cur_occlusion; // current occlusion level of this edge
        uint32 poly_index;

    };
    
    array<edge *> edge_list;
    array<edge *> clipped_edge_list;
    array<ray *> ray_list;
    ray zero_theta_ray;
    ray two_pi_theta_ray;
    
    radial_visibility_clipper(context &_c, polytron &_p, uint32 _max_occlusion) : p(_p), _allocator(&_c.get_page_allocator()), max_occlusion(_max_occlusion)
    {
        observer_vertex_index = invalid_vertex;
        zero_theta_ray.theta = 0;
        zero_theta_ray.vi = 0;
        zero_theta_ray.v.set(1, 0);
        zero_theta_ray.n.set(0, 1);
        two_pi_theta_ray = zero_theta_ray;
        two_pi_theta_ray.theta = Float2Pi;
    }
    
    void setup_polytron_strips(const vec2 &_observer)
    {
        _allocator.clear();
        array<vec2> &verts = p.vertices;
        observer = _observer;
        observer_vertex_index = verts.size();
        verts.push_back(observer);
        edge_list.clear();
        ray_list.clear();
        ray_list.push_back(&zero_theta_ray);
        ray_list.push_back(&two_pi_theta_ray);
        
        uint32 num_strips = p.border_line_strips.size();
        uint32 total_source_edges = 0;
        
        for(uint32 i = 0; i < num_strips; i++)
            total_source_edges += p.border_line_strips[i].vertex_count - 1;
        
        // reserve a little extra to accommodate split edges
        edge_list.reserve(uint32(total_source_edges * 1.1f));
        ray_list.reserve(total_source_edges);

        for(uint32 i = 0; i < num_strips; i++)
            _add_strip_edges(i, p.border_line_strips[i].index_start, p.border_line_strips[i].vertex_count);
        edge_list.sort(edge_sort_compare);
        ray_list.sort(ray_sort_compare);
        
        uint32 ray_index = 0;
        float32 cur_theta = 0;
        for(uint32 i = 0; i < ray_list.size(); i++)
        {
            ray *r = ray_list[i];
            if(r->theta != cur_theta)
            {
                ray_index++;
                cur_theta = r->theta;
            }
            r->sort_index = ray_index;
        }
    }
    
    static int32 QSORT_CALLBACK edge_sort_compare(edge **a, edge **b)
    {
        float32 theta_compare = (*a)->ray0->theta - (*b)->ray0->theta;
        return theta_compare < 0 ? -1 : (theta_compare > 0 ? 1 : 0);
    }
    
    static int32 QSORT_CALLBACK ray_sort_compare(ray **a, ray **b)
    {
        float32 theta_compare = (*a)->theta - (*b)->theta;
        return theta_compare < 0 ? -1 : (theta_compare > 0 ? 1 : 0);
    }
    
    void _add_strip_edges(uint32 poly_index, uint32 index_start, uint32 vertex_count)
    {
        ray *ray0;
        float32 r0;
        uint32 v0 = p.indices[index_start];
        vec2 n0 = p.index_properties[index_start].n;
        float t0 = p.index_properties[index_start].t;
        _alloc_ray(v0, ray0, r0);
        
        for(uint32 edge = 1; edge < vertex_count; edge++)
        {
            uint32 index_index = index_start + edge;
            ray *ray1;
            float32 r1;
            uint32 v1 = p.indices[index_index];
            vec2 n1 = p.index_properties[index_index].n;
            float t1 = p.index_properties[index_index].t;
            _alloc_ray(v1, ray1, r1);
            _add_edge(v0, ray0, r0, n0, t0, v1, ray1, r1, n1, t1, 1, poly_index);
            v0 = v1;
            r0 = r1;
            n0 = n1;
            t0 = t1;
            ray0 = ray1;
        }
    }
    
    float32 _compute_edge_ray_intersect(vec2 &relative_vert0, vec2 &relative_vert1, ray *ray, float32 &r, vec2 &relative_intersect)
    {
        float32 d0 = relative_vert0.dot(ray->n);
        float32 d1 = relative_vert1.dot(ray->n);
        
        float32 t = (-d0)/(d1 - d0);
        relative_intersect = relative_vert0 + (relative_vert1 - relative_vert0) * t;
        r = relative_intersect.dot(ray->v);
        return t;
    }
    
    void _add_edge(uint32 v0, ray *ray0, float32 r0, vec2 n0, float32 t0, uint32 v1, ray *ray1, float32 r1, vec2 n1, float32 t1, uint32 occlusion_level, uint32 poly_index)
    {
        vec2 vert0 = p.vertices[v0] - observer;
        vec2 vert1 = p.vertices[v1] - observer;
        
        vec2 e_vec = vert1 - vert0;
        float32 len = e_vec.len();
        if(!len)
            e_vec.set(1,0);
        else
            e_vec *= 1/len;
        vec2 e_normal(-e_vec.y, e_vec.x);
        bool flipped = e_normal.dot(vert1) >= 0;
        
        if(flipped)
        {
            swap(vert0, vert1);
            swap(v0, v1);
            swap(ray0, ray1);
            swap(r0, r1);
            swap(n0, n1);
            swap(t0, t1);
            e_vec = -e_vec;
            e_normal = -e_normal;
        }
        if(ray0->theta > ray1->theta)
        {
            // this edge crosses the 0 radians mark, so clip it into two edges.
            float32 clip_r;
            vec2 clip_vert;
            float32 t = _compute_edge_ray_intersect(vert0, vert1, &zero_theta_ray, clip_r, clip_vert);
            uint32 clip_v = p.vertices.size();
            p.vertices.push_back(clip_vert + observer);
            float32 clip_t = t0 + t * (t1 - t0);
            vec2 clip_n = zero_theta_ray.v;
            if(!flipped)
                clip_n = -clip_n;
            
            edge_list.push_back(_alloc_edge(v0, ray0, r0, n0, t0, clip_v, &two_pi_theta_ray, clip_r, clip_n, clip_t, len * t, e_vec, e_normal, flipped, occlusion_level, poly_index));
            edge_list.push_back(_alloc_edge(clip_v, &zero_theta_ray, clip_r, clip_n, clip_t, v1, ray1, r1, n1, t1, len * (1-t), e_vec, e_normal, flipped, occlusion_level, poly_index));
        }
        else
            edge_list.push_back(_alloc_edge(v0, ray0, r0, n0, t0, v1, ray1, r1, n1, t1, len, e_vec, e_normal, flipped, occlusion_level, poly_index));
    }
    
    edge *_alloc_edge(uint32 v0, ray *ray0, float32 r0, vec2 &n0, float t0, uint32 v1, ray *ray1, float r1, vec2 &n1, float t1, float32 len, const vec2 &e_vec, const vec2 &e_normal, bool flipped, uint32 occlusion_level, uint32 poly_index)
    {
        edge *ret = (edge *) _allocator.allocate(sizeof(edge));

        ret->v0 = v0;
        ret->ray0 = ray0;
        ret->r0 = r0;
        ret->v1 = v1;
        ret->n0 = n0;
        ret->n1 = n1;
        ret->t0 = t0;
        ret->t1 = t1;
        ret->ray1 = ray1;
        ret->r1 = r1;
        ret->l = len;
        ret->v = e_vec;
        ret->n = e_normal;
        
        ret->flipped = flipped;
        ret->occlusion_level = occlusion_level;
        
        ret->cur_r = r0;
        ret->cur_occlusion = 0;

        ret->poly_index = poly_index;
        return ret;
    }
    
    void _alloc_ray(uint32 vertex_index, ray* &the_ray, float32 &radius)
    {
        vec2 vert = p.vertices[vertex_index] - observer;
        the_ray = (ray *) _allocator.allocate(sizeof(ray));
        the_ray->theta = atan2(vert.y, vert.x);
        if(the_ray->theta < 0)
            the_ray->theta += Float2Pi;
        radius = vert.len();
        the_ray->vi = vertex_index;
        the_ray->v = vert;
        the_ray->v *= 1 / radius;
        the_ray->n.set(-the_ray->v.y, the_ray->v.x);
        ray_list.push_back(the_ray);
    }

    void clip_process()
    {
        clipped_edge_list.clear();
        clipped_edge_list.reserve(edge_list.size() * 1.5);
        array<edge *> process_list;

        if(!ray_list.size())
            return;
        uint32 next_edge_index = 0;
        for(uint32 i = 0;;)
        {
            _process_next_ray(process_list, i, next_edge_index);
            i++;
            while(i < ray_list.size() && ray_list[i]->sort_index == ray_list[i-1]->sort_index)
                i++;
            if(i == ray_list.size())
                break;
        }
        
        /*for(uint32 i = 0; i < edge_list.size(); i++)
            _process_next_edge(process_list, edge_list[i]);*/
    }
    
    bool _edge_closer(edge *e1, edge *e2, ray *r)
    {
        if(e1->cur_r < e2->cur_r)
            return true;
        if(e1->cur_r > e2->cur_r)
            return false;
        return r->v.dot(e1->v) < r->v.dot(e2->v);
    }
    
    void _process_next_ray(array<edge *> &process_list, uint32 ray_index, uint32 &next_edge_index)
    {
        ray *r = ray_list[ray_index];
        //printf("Ray %d (%g %g)\n", r->sort_index, r->v.x, r->v.y);

        uint32 clipped_edges_start_size = clipped_edge_list.size();
        
        // advance the process_list to the new ray...
        // first, see if any edges terminate on this ray and emit, or update the current radius of each active edge to the intersection point with the new ray
        for(uint32 i = 0; i < process_list.size();)
        {
            edge *e = process_list[i];
            if(e->ray1->sort_index == r->sort_index)
            {
                e->cur_r = e->r1;
                clipped_edge_list.push_back(e);
                process_list.erase(i);
            }
            else
            {
                _update_edge_radius(e, r);
                i++;
            }
        }
        // then add any new edges at this ray to the process list
        while(next_edge_index < edge_list.size() && edge_list[next_edge_index]->ray0->sort_index == r->sort_index)
        {
            edge *new_edge = edge_list[next_edge_index];
            if(new_edge->ray1->sort_index > r->sort_index)
            {
                //printf("Add edge ");
                //_print_edge(next_edge_index, new_edge);
                new_edge->new_edge = true;
                process_list.push_back(new_edge);
            }
            next_edge_index++;
        }

        // now bubble sort the process list
        if(process_list.size() > 1)
        {
            uint32 i = process_list.size() - 2;
            for(;;) {
                for(uint32 j = i + 1; j < process_list.size(); j++)
                {
                    if(_edge_closer(process_list[j], process_list[j-1], r))
                    {
                        edge *temp = process_list[j];
                        process_list[j] = process_list[j-1];
                        process_list[j-1] = temp;
                    }
                    else
                        break;
                }
                if(i == 0)
                    break;
                i--;
            }
        }
        
        // update the cur_occlusion of every edge in the process list. If cur_occlusion changes and the edge is not new, split and emit the edge at the ray.
        uint32 cur_occlusion = 0;
        for(uint32 i = 0; i < process_list.size(); i++)
        {
            edge *e = process_list[i];
            if(!e->new_edge && e->cur_occlusion != cur_occlusion)
            {
                // the occlusion level of this edge has changed
                uint32 intersect_vert = p.vertices.size();
                vec2 intersect = observer + r->v * e->cur_r;
                p.vertices.push_back(intersect);
                vec2 &vert0 = p.vertices[e->v0];
                vec2 &vert1 = p.vertices[e->v1];
                float32 clip_t = (intersect - vert0).dot(e->v) / (vert1 - vert0).dot(e->v);
                vec2 clip_n = r->v;
                if(!e->flipped)
                    clip_n = -clip_n;
                float32 mag_scale = abs(clip_n.dot(e->n));
                if(mag_scale != 0)
                    clip_n /= mag_scale;
                edge *emit_edge = _alloc_edge(e->v0, e->ray0, e->r0, e->n0, e->t0, intersect_vert, r, e->cur_r, clip_n, clip_t, 0, e->v, e->n, e->flipped, e->occlusion_level, e->poly_index);
                emit_edge->cur_occlusion = e->cur_occlusion;
                clipped_edge_list.push_back(emit_edge);
                e->v0 = intersect_vert;
                e->n0 = clip_n;
                e->t0 = clip_t;
                e->ray0 = r;
                e->r0 = e->cur_r;
            }
            e->new_edge = false;
            e->cur_occlusion = cur_occlusion;
            cur_occlusion += e->occlusion_level;
        }
        /*
        //printf("Emit Edges:\n");
        for(uint32 i = clipped_edges_start_size; i < clipped_edge_list.size(); i++)
            _print_edge(i, clipped_edge_list[i]);
        //printf("Process List:\n");
        for(uint32 i = 0; i < process_list.size(); i++)
            _print_edge(i, process_list[i]);*/
    }

    void _print_edge(uint32 i, edge *e)
    {
        vec2 v0 = p.vertices[e->v0];
        vec2 v1 = p.vertices[e->v1];
        
        //printf("%d: (%g %g) -> (%g %g) r: %g o: %d\n", i, v0.x, v0.y, v1.x, v1.y, e->cur_r, e->cur_occlusion);
    }
    
    void _update_edge_radius(edge *e, ray *r)
    {
        vec2 relative_vert0 = p.vertices[e->v0] - observer;
        vec2 relative_vert1 = p.vertices[e->v1] - observer;
        float32 d0 = relative_vert0.dot(r->n);
        float32 d1 = relative_vert1.dot(r->n);
        float32 t = 0;
        if(d1 != d0)
            t = (-d0)/(d1 - d0);
        if(t < 0)
            t = 0;
        if(t > 1)
            t = 1;
        vec2 relative_intersect = relative_vert0 + (relative_vert1 - relative_vert0) * t;
        e->cur_r = relative_intersect.dot(r->v);
    }
    

    /*
    void _advance_process_list(array<edge *> &process_list, uint32 index_start, uint32 occlusion_level_sub, ray *new_ray)
    {
        for(uint32 i = index_start; i < process_list.size();)
        {
            edge *pe = process_list[i];
            if(pe->cur_ray->theta < new_ray->theta)
            {
                // see if this edge is terminated:
                if(pe->ray1->theta <= new_ray->theta)
                {
                    process_list.erase(i);
                    clipped_edge_list.push_back(pe);
                    _advance_process_list(process_list, i, occlusion_level_sub + pe->occlusion_level, pe->ray1);
                    continue;
                }
                // otherwise, compute a new cur_r for the new ray for this edge and go on to the next edge
                vec2 vert0 = p.vertices[pe->v0] - observer;
                vec2 vert1 = p.vertices[pe->v1] - observer;
                vec2 relative_intersect;
                _compute_edge_ray_intersect(vert0, vert1, new_ray, pe->cur_r, relative_intersect);
                pe->cur_ray = new_ray;
                
                if(occlusion_level_sub)
                {
                    // the occlusion level changed at this intersection, so allocate and emit a new edge, and adjust the start of this edge to the intersection point.
                    uint32 intersect_vert = p.vertices.size();
                    p.vertices.push_back(relative_intersect + observer);
                    edge *emit_edge = _alloc_edge(pe->v0, pe->ray0, pe->r0, intersect_vert, new_ray, pe->cur_r, pe->l, pe->v, pe->n, pe->flipped, pe->occlusion_level);
                    emit_edge->cur_occlusion = pe->cur_occlusion;
                    clipped_edge_list.push_back(emit_edge);
                    pe->v0 = intersect_vert;
                    pe->r0 = pe->cur_r;
                    pe->ray0 = new_ray;
                    pe->cur_occlusion -= occlusion_level_sub;
                }
            }
            i++;
        }
    }

    void _process_next_edge(array<edge *> &process_list, edge *e)
    {
        // the process_list contains the list of active edges at cur_theta, so first, we advance all the edges in the process list to the ray0 of the new edge.
        _advance_process_list(process_list, 0, 0, e->ray0);
        
        // at this point, the process list should contain all of the active edges at e->ray0, sorted by radius, so bubble insert the new edge. For any edges behind the new edge, emit the fragment and adjust the cur_occlusion
        for(uint32 i = 0; i < process_list.size(); i++)
        {
            edge *pe = process_list[i];
            if(pe->cur_r > e->r0)
            {
                process_list.insert(i, e);
                for(uint32 j = i+1; j < process_list.size(); j++)
                {
                    pe = process_list[j];
                    vec2 intersect = e->ray0->v * pe->cur_r + observer;
                    uint32 intersect_vert = p.vertices.size();
                    p.vertices.push_back(intersect);
                    edge *emit_edge = _alloc_edge(pe->v0, pe->ray0, pe->r0, intersect_vert, e->ray0, pe->cur_r, pe->l, pe->v, pe->n, pe->flipped, pe->occlusion_level);
                    emit_edge->cur_occlusion = pe->cur_occlusion;
                    clipped_edge_list.push_back(emit_edge);
                    pe->v0 = intersect_vert;
                    pe->r0 = pe->cur_r;
                    pe->ray0 = e->ray0;
                    pe->cur_occlusion += e->occlusion_level;
                }
                return;
            }
        }
        // we didn't find an insertion point, so push the new edge on the back of the process list
        process_list.push_back(e);
    }*/

    void draw_ray(ray *r, float32 rad, float32 frac)
    {
        vec2 vert = observer + r->v * rad;
        A.C.draw_line(observer, vert);
        A.C.draw_line(vert, vert - r->v * 10 + r->n * 4);
        A.C.draw_line(vert, vert - r->v * 10 - r->n * 4);
        vec2 text_pos = observer + r->v * rad * frac;
        
        A.C.draw_line(text_pos, text_pos - r->v * 10);
        A.C.draw_line(text_pos, text_pos + r->n * 10);

        A.C.colorf(1,1,0);
        UserInterface::drawStringf(text_pos.x - 4 + r->n.x * 20, text_pos.y - 4 + r->n.y * 20, 15, "%d.%d", r->sort_index, r->vi);
    }
    
    void render_edges()
    {
        uint32 total_edges = edge_list.size();
        
        for(uint32 i = 0; i < edge_list.size(); i++)
        {
            edge *e = edge_list[i];
            float32 gray_val = 1 - (0.75 * i / total_edges);
            Color edge_color(gray_val, gray_val, gray_val);
            vec2 vert0 = p.vertices[e->v0];
            vec2 vert1 = p.vertices[e->v1];
            vec2 e_center = vert0 + e->v * e->l * 0.5;
            
            glLineWidth(4);
            A.C.color(edge_color);
            A.C.draw_line(vert0, vert1);
            if(e->flipped)
                A.C.draw_circle(e_center, 8);
            
            glLineWidth(1);

            
            A.C.draw_line(vert1, vert1 - e->v * 10 + e->n * 3);
            A.C.draw_line(vert1, vert1 - e->v * 10 - e->n * 3);
            float32 add_amt = e->flipped ? 0.2 : 0;
            A.C.color(edge_color);
            draw_ray(e->ray0, e->r0, 0.6 + add_amt);
            A.C.color(edge_color);
            draw_ray(e->ray1, e->r1, 0.3 + add_amt);
            A.C.colorf(1,1,1);
            A.C.draw_line(e_center, e_center + e->n * 10);
            UserInterface::drawStringf(e_center.x - 4 + e->n.x * 20, e_center.y - 4 + e->n.y * 20, 15, "%d", i);
        }
    }

    void render_clipped_edges()
    {
        float32 max_occlusion = 0;
        for(uint32 i = 0; i < clipped_edge_list.size(); i++)
            if(clipped_edge_list[i]->cur_occlusion > max_occlusion)
                max_occlusion = clipped_edge_list[i]->cur_occlusion;
        for(uint32 i = 0; i < clipped_edge_list.size(); i++)
        {
            edge *e = clipped_edge_list[i];
            float32 color_v = 1;
            if(e->cur_occlusion)
            {
                if(e->cur_occlusion == 1)
                    color_v = 0.6;
                else
                {
                    float32 occ_frac = float32(e->cur_occlusion) / max_occlusion;
                    color_v = 0.4 * (1 - occ_frac);
                }
            }
            Color edge_color(color_v, color_v, color_v);
            vec2 vert0 = p.vertices[e->v0];
            vec2 vert1 = p.vertices[e->v1];
            vec2 e_vec = vert1 - vert0;
            float32 e_len = e_vec.len();
            e_vec *= 1/e_len;
            vec2 e_n(-e_vec.y, e_vec.x);
            vec2 e_center = vert0 + e_vec * e_len * 0.5;
            
            glLineWidth(4);
            A.C.color(edge_color);
            A.C.draw_line(vert0, vert1);
            if(e->flipped)
                A.C.draw_circle(e_center, 8);
            glLineWidth(1);
            A.C.draw_line(vert0, vert0 + e->n0 * 8);
            A.C.draw_line(vert1, vert1 + e->n1 * 8);
            A.C.draw_line(vert1 + e->n1 * 8, vert0 + e->n0 * 8);

            float32 add_amt = e->flipped ? 0.2 : 0;
            A.C.color(edge_color);
            draw_ray(e->ray0, e->r0, 0.6 + add_amt);
            A.C.color(edge_color);
            draw_ray(e->ray1, e->r1, 0.3 + add_amt);
            
            A.C.colorf(1,1,1);
            A.C.draw_line(e_center, e_center + e->n * 10);
            UserInterface::drawStringf(e_center.x - 4 + e->n.x * 20, e_center.y - 4 + e->n.y * 20, 15, "%d", i);

            /*
            A.C.draw_line(vert1, vert1 - e_vec * 10 + e_n * 3);
            A.C.draw_line(vert1, vert1 - e_vec * 10 - e_n * 3);
            A.C.color(edge_color * 0.5 * alpha_mod );
            A.C.draw_line(observer, vert0 + e_vec * 2);
            A.C.draw_line(observer, vert1 - e_vec * 2);
            A.C.colorf(1,1,1);
            UserInterface::drawStringf(e_center.x - 4, e_center.y - 4, 15, "%d", i);
             */
        }
    }
    

};

