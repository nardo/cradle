// The vectrex library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

/// The polytron class -- encapsulates poly soup - composed of a set of fill primitives (ie triangle lists) and perimiter border polygons that can either be either open or closed.
/// polytron_outline objects are instanced from polytrons for perimeter beam rendering, polytron_fill objects are instanced from polytrons for fill triangle rendering

class polytron
{
    public:
    enum primitive_type
    {
        line_strip,
        triangle_list,
    };
    struct primitive
    {
        uint32 index_start;
        uint32 vertex_count;
        float32 length; // length of line strips
        
        primitive()
        {
            index_start = vertex_count = 0;
            length = 0;
        }
    };
    struct index_prop
    {
        vec2 n;
        vec2 v;
        float t;
    };
    
    primitive fill_triangles;
    array<primitive> border_line_strips;
    array<uint32> indices;
    array<index_prop> index_properties;
    array<vec2> vertices;
    Rect root_bounds;
        
    void clear()
    {
        border_line_strips.clear();
        indices.clear();
        index_properties.clear();
        vertices.clear();
        fill_triangles.vertex_count = 0;
    }
    
    void convex_poly(const vec2 *bounds, uint32 vertex_count, bool closed = true)
    {
        assert(vertex_count > 1);
        clear();
        
        // simple shapes will have two primitives - one triangle list for the fill, and one either open or closed polygon for the perimiter.
        border_line_strips.resize(1);
        vertices.resize(vertex_count);
        
        fill_triangles.index_start = 0;
        fill_triangles.vertex_count = 3 * (vertex_count - 2);
        border_line_strips[0].index_start = fill_triangles.vertex_count;
        border_line_strips[0].vertex_count = vertex_count;
        if(closed)
            border_line_strips[0].vertex_count++;
        
        uint32 index_count = fill_triangles.vertex_count + border_line_strips[0].vertex_count;
        indices.resize(index_count);
        index_properties.resize(index_count);
        
        uint32 index_base = 0;
        
        for(uint32 i = 0; i < vertex_count - 2; i++)
        {
            // for now, a simple triangle fan
            indices[index_base] = 0;
            indices[index_base + 1] = i + 1;
            indices[index_base + 2] = i + 2;
            index_base += 3;
        }
        for(uint32 i = 0; i < vertex_count; i++)
        {
            vertices[i] = bounds[i];
            indices[index_base] = i;
            index_base++;
        }
        if(closed)
            indices[index_base] = indices[ border_line_strips[0].index_start];
        root_bounds.min = root_bounds.max = bounds[0];
        for(uint32 i = 1; i < vertex_count; i++)
            root_bounds.unionPoint(bounds[i]);
        compute_perimeters_and_normals();
    }
    
    void _recurse_count_tree_polys(PolygonWithHolesTree &tree, uint32 &poly_count, uint32 &poly_index_count)
    {
        poly_count += 1 + tree.holes.size();
        poly_index_count += tree.vertexIndices.size() + 1;
        for(uint32 i = 0; i < tree.holes.size(); i++)
            poly_index_count += tree.holes[i].vertexIndices.size() + 1;
        //for(uint32 i = 0; i < tree.childrenPolygons.size(); i++)
        //    _recurse_count_tree_polys(tree.childrenPolygons[i], poly_count, poly_index_count);
    }
    
    void _next_border_from_index_list(poly_tree::index_list &poly, uint32 &border_index, uint32 &index_base, bool hole)
    {
        border_line_strips[border_index].index_start = index_base;
        border_line_strips[border_index].vertex_count = poly.size() + 1;

        if(hole)
        {
            for(uint32 i = poly.size(); i--; )
                indices[index_base++] = poly[i];
            indices[index_base++] = poly[poly.size() - 1];
        }
        else
        {
            for(uint32 i = 0; i < poly.size(); i++)
                indices[index_base++] = poly[i];
            indices[index_base++] = poly[0];
        }
        border_index++;
    }
    
    void _recurse_build_tree_polys(PolygonWithHolesTree &tree, uint32 &border_index, uint32 &index_base)
    {
        _next_border_from_index_list(tree.vertexIndices, border_index, index_base, false);
        for(uint32 i = 0; i < tree.holes.size(); i++)
            _next_border_from_index_list(tree.holes[i].vertexIndices, border_index, index_base, true);
        //for(uint32 i = 0; i < tree.childrenPolygons.size(); i++)
        //    _recurse_build_tree_polys(tree.childrenPolygons[i], prim_index, index_base);
    }
    
    void poly_tree(poly_tree &tree)
    {
        clear();
        tree.compute_trees_and_tris();
        poly_tree::holey_tree_list &trees = tree.holey_trees;
        poly_tree::tri_lists &tris = tree.triangles;
        poly_tree::vertex_list &verts = tree.vertices;

        assert(verts.size() > 0);
        vertices.resize(verts.size());
        root_bounds.min = root_bounds.max = Point(verts[0][0], verts[0][1]);
        vertices[0].set(verts[0][0], verts[0][1]);
        for(uint32 i = 1; i < vertices.size(); i++)
        {
            Point p(verts[i][0], verts[i][1]);
            vertices[i] = p;
            root_bounds.unionPoint(p);
        }
        uint32 triangle_count = 0;
        for(uint32 i = 0; i < tris.size(); i++)
            triangle_count += tris[i].size();
        uint32 poly_count = 0;
        uint32 poly_index_count = 0;
        for(uint32 i = 0; i < trees.size(); i++)
            _recurse_count_tree_polys(trees[i], poly_count, poly_index_count);
        border_line_strips.resize(poly_count);
        
        uint32 index_count = triangle_count * 3 + poly_index_count;
        indices.resize(index_count);
        index_properties.resize(index_count);
        
        fill_triangles.index_start = 0;
        fill_triangles.vertex_count = triangle_count * 3;
        uint32 index_base = 0;
        for(uint32 i = 0; i < tris.size(); i++)
        {
            for(uint32 j = 0; j < tris[i].size(); j++)
            {
                glm::ivec3 &tri = tris[i][j];
                indices[index_base++] = tri[0];
                indices[index_base++] = tri[1];
                indices[index_base++] = tri[2];
            }
        }
        uint32 border_index = 0;
        for(uint32 i = 0; i < trees.size(); i++)
            _recurse_build_tree_polys(trees[i], border_index, index_base);
        compute_perimeters_and_normals();
    }
    
    void convex_poly(const array<vec2> &bounds, bool closed = true)
    {
        convex_poly(&bounds[0], bounds.size(), closed);
    }
    
    void rect(const Rect &bounds)
    {
        vec2 verts[4];
        verts[0] = bounds.min;
        verts[1].set(bounds.max.x, bounds.min.y);
        verts[2] = bounds.max;
        verts[3].set(bounds.min.x, bounds.max.y);
        convex_poly(verts, 4, true);
    }
    
    void compute_perimeters_and_normals()
    {
        for(uint32 i = 0; i < border_line_strips.size(); i++)
        {
            primitive &p = border_line_strips[i];
            
            // first calculate the perimeter of this primitive
            float32 perimeter = 0;
            uint32 start_index = indices[p.index_start];
            uint32 end_index_index = p.index_start + p.vertex_count - 1;
            uint32 end_index = indices[end_index_index];

            bool loop = start_index == end_index;
            vec2 prev_vert(vertices[start_index].x, vertices[start_index].y);
            
            // start at the second vertex
            uint32 j;
            for(j = 1; j < p.vertex_count; j++)
            {
                uint32 index_index = p.index_start + j;
                uint32 cur_index = indices[index_index];
                vec2 cur_vert(vertices[cur_index].x, vertices[cur_index].y);
                // stuff the vector from prev to current into the normal for this vertex, and the length of that vector into the t value
                vec2 edge_vec = cur_vert - prev_vert;
                float32 len = edge_vec.len();
                if(len)
                    edge_vec *= 1 / len;
                else
                    edge_vec.set(1, 0);
                
                index_properties[index_index].v = edge_vec;
                index_properties[index_index].t = len;
                perimeter += len;
                prev_vert = cur_vert;
            }
            
            // now fixup the first vertex and prepare to calc the actual normals - if this is a loop, we set the first delta to the one we computed for the last. If we're not looping, the first vertex gets the same delta as the second -- ie. colinear.
            
            // also, in the next step, we caclulate the vertex normals walking back from the end of the poly.
            // if this is a loop, we start at the end vertex - 1 and stuff the last vertex's normal in from the first. Otherwise we start at the end vertex and copy the delta (same as how the beginning is handled)
            
            vec2 next_v = index_properties[end_index_index].v;

            if(loop)
            {
                index_properties[p.index_start].v = next_v;
                index_properties[p.index_start].t = index_properties[end_index_index].t;
                j = p.vertex_count - 1;
            }
            else
            {
                uint32 next_index = p.index_start + 1;
                index_properties[p.index_start].v = index_properties[next_index].v;
                index_properties[p.index_start].t = index_properties[next_index].t;
                j = p.vertex_count;
            }

            while(j--)
            {
                uint32 index_index = p.index_start + j;
                vec2 cur_v = index_properties[index_index].v;

                // at this point next_v and cur_v have the edge unit vector
                vec2 next_n(-next_v.y, next_v.x);
                vec2 cur_n(-cur_v.y, cur_v.x);
                
                // ok, now we have two unit edge vectors and two unit normals...
                // compute the intersection point of the two lines.
                // that intersection point will be the "normal" offset for that vertex

                // line1 is defined by next_n, next_n + next_v
                // line2 is defined by cur_n, cur_n + cur_v
                // a = next_n
                // b = next_n + next_v
                // c = cur_n
                // d = cur_n + cur_v
                // a1 = b.y - a.y = next_v.y
                // b1 = a.x - b.x = -next_v.x
                // c1 = a1 * a.x + b1 * a.y = next_v.y * next_n.x + (-next_v.x * next_n.y)
                // a2 = d.y - c.y = cur_v.y
                // b2 = c.x - d.x = -cur_v.x
                // c2 = a2 * c.x + b2 * c.y = cur_v.y * cur_n.x + -cur_v.x * cur_n.y
                // determinant = a1 * b2 - a2 * b1 = next_v.y * (-cur_v.x) + (cur_v.y * next_v.x);
                // if(determinant == 0) // lines are parallel -- n = cur_n
                // else
                // n.x * determinant = b2*c1 - b1*c2 = -cur_v.x * c1 + next_v.x * c2
                // n.y * determinant = a1*c2 - a2*c1 = next_v.y * c2 - cur_v.y * c1
                float32 determinant = cur_v.y * next_v.x - next_v.y * cur_v.x;
                if(mFabs(determinant) < __EQUAL_CONST_F)
                    index_properties[index_index].n = cur_n;
                else
                {
                    float32 c1 = next_v.y * next_n.x - next_v.x * next_n.y;
                    float32 c2 = cur_v.y * cur_n.x - cur_v.x * cur_n.y;
                    float32 inv_d = 1 / determinant;
                    index_properties[index_index].n.x = (next_v.x * c2 - cur_v.x * c1) * inv_d;
                    index_properties[index_index].n.y = (next_v.y * c2 - cur_v.y * c1) * inv_d;
                }
                next_v = cur_v;
            }
            // now fix up the looped primitives
            if(loop)
                index_properties[p.index_start + p.vertex_count - 1].n = index_properties[p.index_start].n;
            
            // finally, compute correct 0..1 t values for the polygon
            if(perimeter)
            {
                p.length = perimeter;
                float32 inv_perimiter = 1 / perimeter;
                float32 distance_sum = 0;
                index_properties[p.index_start].t = 0;
                for(uint32 j = 1; j < p.vertex_count; j++)
                {
                    uint32 index_index = p.index_start + j;
                    distance_sum += index_properties[index_index].t;
                    index_properties[index_index].t = distance_sum * inv_perimiter;
                }
            }
        }
    }
    
    void render_debug()
    {
        // draws in four steps:
        // first fills all the triangle lists with dark gray
        // second draws all triangle list outlines in light gray
        // third draws all open and closed polygons in wide yellow
        // fourth draws vertex normals and t values
        
        A.C.enable_vertex_array(&vertices[0], vertices.size());

        A.C.colorf(0.33, 0.33, 0.33, 1);
        A.C.draw_array_triangles_indexed(&indices[fill_triangles.index_start], fill_triangles.vertex_count);
        
        A.C.colorf(0.66, 0.66, 0.66, 1);
        for(uint32 i = 0; i < fill_triangles.vertex_count; i += 3)
            A.C.draw_array_line_loop_indexed(&indices[fill_triangles.index_start + i], 3);
        
        glLineWidth(3);
        A.C.colorf(1, 1, 0, 1);
        for(uint32 i = 0; i < border_line_strips.size(); i++)
        {
            primitive &p = border_line_strips[i];
            A.C.draw_array_line_strip_indexed(&indices[p.index_start], p.vertex_count);
        }
        glLineWidth(1);
        A.C.disable_vertex_array(&vertices[0]);

        A.C.colorf(0,1,1,1);
        for(uint32 i = 0; i < border_line_strips.size(); i++)
        {
            primitive &p = border_line_strips[i];
            for(uint32 i = 0; i < p.vertex_count; i++)
            {
                uint32 index_index = p.index_start + i;
                uint32 index = indices[index_index];
                vec2 pos(vertices[index].x, vertices[index].y);
                vec2 n = index_properties[index_index].n;
                vec2 end_pos = pos + n * 20;
                
                A.C.draw_line(pos, end_pos);
                if(i != p.vertex_count - 1)
                {
                    index_index++;
                    index = indices[index_index];
                    pos = vertices[index] + index_properties[index_index].n * 20;
                    A.C.draw_line(pos, end_pos);
                }
            }
        }
    }
};
