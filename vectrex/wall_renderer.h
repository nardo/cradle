// The vectrex library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

/// The wall_renderer class -- manages the prep and render of walls, including the generation of a line-of-sight and light/shadow BSP tree for objects that occlude.


struct wall_renderer
{
    poly_tree tree;
    polytron_fill<RenderVertex3T> fill;
    polytron wallytron;
    
    // extruded vertex and index data for the walls
    array<RenderVertex3C> wall_verts;
    array<RenderVertex2CT> outline_quad_verts;
    array<uint32> upper_wall_quads;
    array<uint32> lower_wall_quads;
    Rect bounds;
    bool have_big_poly;
    enum
    {
        BarrierTextureWorldSize = 1024,
        BarrierOutlineTextureWorldSize = 256,
    };
    
    wall_renderer()
    {
        have_big_poly = false;
    }
    
    void reset()
    {
        tree.clear();
        have_big_poly = false;
    }
    
    static bool is_clockwise(array<vec2> &vec)
    {
        uint32 edge_count = vec.size() - 1;
        float32 edge_sum = 0;
        for(uint32 i = 0; i < edge_count; i++)
        {
            float32 edge_calc = (vec[i+1].x - vec[i].x) * (vec[i].y + vec[i+1].y);
            edge_sum += edge_calc;
        }
        return edge_sum >= 0;
    }
    
    static void reverse_winding(array<vec2> &poly)
    {
        uint32 half = poly.size() / 2;
        for(uint32 i = 0; i < half; i++)
        {
            vec2 swap_vec = poly[i];
            uint32 swap_index = poly.size() - 1 - i;
            poly[i] = poly[swap_index];
            poly[swap_index] = swap_vec;
        }
    }
    

    void add_void(array<vec2> &void_vec, bool loop)
    {
        // this is a void... now for the fun stuff...
        // if this is the first void we're getting, then we need to set up the poly_tree special-like. Basically if the level is constructed using voids, then one giant polygon will get added first, followed by the subtractive voids, followed by all of the additive barrier makers. Good clean fun. The only issue is, we don't know at first how big to make the level-sized poly... so we'll zero out the verts and fix it up once all the barriers are over.
        if(tree.vertices.size() == 0)
        {
            have_big_poly = true;
            float32 giant_poly[4][2] =
            {
                {0,0 },
                {0,0 },
                {0,0 },
                {0,0 },
            };
            tree.add_vertices(&giant_poly[0][0], 4);
            uint32 indices[5] = { 0, 1, 2, 3, 0 };
            tree.add_indexed_poly(indices, 5);
        }
        // now add the void.
        if(!loop)
            void_vec.push_back(void_vec[0]);
        
        if(!is_clockwise(void_vec))
            reverse_winding(void_vec);
        tree.add_poly(&void_vec[0], void_vec.size());
    }
    
    void add_barriers(array<vec2> &barriers_vec, bool loop, float32 width)
    {
        polytron pt;
        if(loop)
        {
            barriers_vec.pop_back();
            if(!is_clockwise(barriers_vec))
                reverse_winding(barriers_vec);

            pt.convex_poly(barriers_vec, true);
            // we are only using this polytron to compute the border
            polytron::primitive &poly = pt.border_line_strips[0];
            for(uint32 i = 0; i < poly.vertex_count; i++)
            {
                uint32 index_index = poly.index_start + i;
                pt.vertices[pt.indices[index_index]] += pt.index_properties[index_index].n * width * 0.5;
            }
            uint32 offset = tree.add_vertices(&pt.vertices[0], pt.vertices.size());
            tree.add_indexed_poly(&pt.indices[poly.index_start], poly.vertex_count, offset);
        }
        else
        {
            pt.convex_poly(barriers_vec, false);
            polytron_outline<vec2> b;
            b.set(pt, width * 0.5, width * 0.5);
            
            // first, add all the vertices:
            uint32 index_offset = tree.add_vertices(&b.vertices[0], b.vertices.size());
            array<uint32> indices;
            
            // now loop through the beams
            for(uint32 bi = 0; bi < b.beams.size(); bi++)
            {
                indices.clear();
                polytron_outline<vec2>::beam &beam = b.beams[bi];
                indices.reserve(beam.index_count);
                uint32 prev_index = 0xFFFFFFFF;
                for(uint32 i = 0; i < beam.index_count; i += 4)
                {
                    uint32 io = beam.index_start + i;
                    uint32 out0 = b.indices[io + 1];
                    uint32 out1 = b.indices[io + 2];
                    if(out0 != prev_index)
                        indices.push_back(out0);
                    indices.push_back(out1);
                    prev_index = out1;
                }
                for(uint32 i = beam.index_count; i;)
                {
                    i -= 4;
                    uint32 io = beam.index_start + i;
                    uint32 in0 = b.indices[io];
                    uint32 in1 = b.indices[io + 3];
                    if(in1 != prev_index)
                        indices.push_back(in1);
                    indices.push_back(in0);
                    prev_index = in0;
                }
                tree.add_indexed_poly(&indices[0], indices.size(), index_offset);
            }
        }
    }
    
    /*void animate_texture_offet(float32 delta_tex_y)
    {
        for(uint32 v = 0; v < outline_verts.size(); v++ )
        {
            RenderVertex2CT &vert = outline_verts[v];
            vert.t.y += delta_tex_y;
        }
    }*/
    
    void construct(float32 x_expand, float32 y_expand, float32 top_z)
    {
        // ok - all of the barriers and possibly some voids are set up in the tree.
        
        uint32 bounds_vert_start = 0;
        if(have_big_poly)
            bounds_vert_start = 4;
        Point p(tree.vertices[bounds_vert_start][0], tree.vertices[bounds_vert_start][1]);
        bounds.set(p,p);
        // loop through all the rest of the verts in the poly_tree
        for(uint32 i = bounds_vert_start + 1; i < tree.vertices.size(); i++)
        {
            p.set(tree.vertices[i][0], tree.vertices[i][1]);
            bounds.unionPoint(p);
        }
        
        // expand the level bounds by the margins (so players can't see the edges
        bounds.min.x -= x_expand;
        bounds.max.x += x_expand;
        bounds.min.y -= y_expand;
        bounds.max.y += y_expand;
        
        if(have_big_poly)
        {
            // Before we build a polytron from the whole tree, we may need to fix up the first polygon -- but only if voids were added!
            glm::dvec2 *v = &tree.vertices[0];
            v[0][0] = bounds.min.x;
            v[0][1] = bounds.min.y;
            v[1][0] = bounds.max.x;
            v[1][1] = bounds.min.y;
            v[2][0] = bounds.max.x;
            v[2][1] = bounds.max.y;
            v[3][0] = bounds.min.x;
            v[3][1] = bounds.max.y;
        }
        
        wallytron.poly_tree(tree);
        fill.set(wallytron);
        for(uint32 i = 0; i < fill.vertices.size(); i++)
        {
            RenderVertex3T &v = fill.vertices[i];
            v.t.x = v.p.x / 512.0f;
            v.t.y = v.p.y / 512.0f;
            v.p.z = top_z;
        }
    }
    
    void construct_walls_and_outlines(vec2 center_pos, float32 top_z, float32 outline_z, float32 bottom_z, float32 outline_width)
    {
        uint32 save_vertex_count = wallytron.vertices.size();
        radial_visibility_clipper c(A.G._context, wallytron, 1);
        c.setup_polytron_strips(center_pos);
        c.clip_process();

        // now build the walls:
        uint32 edge_count = c.clipped_edge_list.size();
        
        // for each clipped edge, we have 6 wall vertices, 4 upper quad indices, 4 lower quad indices, and 4 strip quad vertices
        wall_verts.resize(0);
        upper_wall_quads.resize(0);
        lower_wall_quads.resize(0);
        wall_verts.reserve(edge_count * 6);
        upper_wall_quads.reserve(edge_count * 4);
        lower_wall_quads.reserve(edge_count * 4);
        outline_quad_verts.resize(edge_count * 4);

        ColorI wall_top(128,128,255,255);
        ColorI wall_outline(96,96,192,255);
        ColorI wall_bottom(32,32,64,0);

        ColorI wall_top_shadow(64,64,128,255);
        ColorI wall_outline_shadow(48,48,96,255);
        ColorI wall_bottom_shadow(16,16,32,0);
        
        ColorI outline_inner(0, 0, 255, 255);
        ColorI outline_outer(255, 0, 0, 0);
        ColorI outline_inner_shadow(0, 0, 128, 255);
        ColorI outline_outer_shadow(128, 0, 0, 0);
        static uint32 outline_winding[4] = { 0, 1, 2, 3 };
        static uint32 outline_winding_flipped[4] = { 0, 3, 2, 1 };
        
        for(uint32 i = 0; i < c.clipped_edge_list.size(); i++)
        {
            radial_visibility_clipper::edge *e = c.clipped_edge_list[i];
            float32 prim_len = wallytron.border_line_strips[e->poly_index].length;

            bool in_shadow = e->cur_occlusion != 0;
            
            // only add the walls that are front-facing:
            if(!e->flipped)
            {
                uint32 start_index = wall_verts.size();
                wall_verts.resize(start_index + 6);
                
                RenderVertex3C *out_verts = &wall_verts[start_index];
                vec2 vert0 = wallytron.vertices[e->v0];
                vec2 vert1 = wallytron.vertices[e->v1];
                out_verts[0].p.set(vert0.x, vert0.y, bottom_z);
                out_verts[1].p.set(vert0.x, vert0.y, outline_z);
                out_verts[2].p.set(vert0.x, vert0.y, top_z);
                out_verts[3].p.set(vert1.x, vert1.y, bottom_z);
                out_verts[4].p.set(vert1.x, vert1.y, outline_z);
                out_verts[5].p.set(vert1.x, vert1.y, top_z);
                
                if(!in_shadow)
                {
                    out_verts[0].c = out_verts[3].c = wall_bottom;
                    out_verts[1].c = out_verts[4].c = wall_outline;
                    out_verts[2].c = out_verts[5].c = wall_top;
                }
                else
                {
                    out_verts[0].c = out_verts[3].c = wall_bottom_shadow;
                    out_verts[1].c = out_verts[4].c = wall_outline_shadow;
                    out_verts[2].c = out_verts[5].c = wall_top_shadow;
                }
                upper_wall_quads.push_back(start_index + 1);
                upper_wall_quads.push_back(start_index + 2);
                upper_wall_quads.push_back(start_index + 5);
                upper_wall_quads.push_back(start_index + 4);
                lower_wall_quads.push_back(start_index);
                lower_wall_quads.push_back(start_index + 1);
                lower_wall_quads.push_back(start_index + 4);
                lower_wall_quads.push_back(start_index + 3);
            }
            // now add the border quads:
            RenderVertex2CT *seg = &outline_quad_verts[i * 4];
            uint32 *o = e->flipped ? outline_winding_flipped : outline_winding;
            
            seg[o[0]].p = wallytron.vertices[e->v0];
            seg[o[1]].p = wallytron.vertices[e->v1];
            seg[o[2]].p = seg[o[1]].p + e->n1 * outline_width;
            seg[o[3]].p = seg[o[0]].p + e->n0 * outline_width;
            float32 t0y = e->t0 * prim_len / float32(BarrierOutlineTextureWorldSize);
            float32 t1y = e->t1 * prim_len / float32(BarrierOutlineTextureWorldSize);

            if(in_shadow)
            {
                seg[o[0]].c = seg[o[1]].c = outline_inner_shadow;
                seg[o[2]].c = seg[o[3]].c = outline_outer_shadow;
            }
            else
            {
                seg[o[0]].c = seg[o[1]].c = outline_inner;
                seg[o[2]].c = seg[o[3]].c = outline_outer;
            }
            seg[o[0]].t.set(0, t0y);
            seg[o[1]].t.set(0, t1y);
            seg[o[2]].t.set(1, t1y);
            seg[o[3]].t.set(1, t0y);
        }
        /*
        
        // set up the barrier outline vert color and tex coords. We'll animate the tex coords in the idle method
        for(uint32 b = 0; b < outline.beams.size(); b++)
        {
            polytron_outline<RenderVertex2CT>::beam &beam = outline.beams[b];
            for(uint32 v = beam.vertex_start; v < beam.vertex_start + beam.vertex_count; v++ )
            {
                RenderVertex2CT &vert = outline.vertices[v];
                float32 alpha = 1 - vert.t.x;
                float32 d = vert.t.y * beam.length / float32(BarrierOutlineTextureWorldSize);
                vert.t.y = d;
                Color c(1-alpha, 0, alpha);
                vert.c.set(c, alpha);
            }
        }*/
        
        wallytron.vertices.resize(save_vertex_count);
    }
    
    
    void construct_shadow_volume(vec2 center_pos)
    {
        radial_visibility_clipper c(A.G._context, wallytron, 1);
        c.setup_polytron_strips(center_pos);
        
        //c.render_edges();
        c.clip_process();
        c.render_clipped_edges();
    }
    
    void render(vec2 center_pos, bool render_tex = true, bool test_z = true)
    {
        if(test_z)
        {
            glDepthMask(GL_TRUE);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
        }
        // test code to render the barrier polytron fill
        //barrier_polytron.render_debug();
        A.C.set_blend_mode(BlendModeNone);
        
        if(render_tex)
        {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D,
                          A.GR.circuitboardTexture->m_textureID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                            GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                            GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                            GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                            GL_REPEAT);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        }
        else
        {
            A.C.colorf(0.75, 0.75, 0.75);
        }

        A.C.enable_vertex_array(&fill.vertices[0], fill.vertices.size());
        for(uint32 i = 0; i < fill.tri_lists.size(); i++)
        {
            A.C.draw_array_triangles_indexed(&fill.indices[fill.tri_lists[i].index_start], fill.tri_lists[i].vertex_count);
        }
        A.C.disable_vertex_array(&fill.vertices[0]);
        if(render_tex)
            glDisable(GL_TEXTURE_2D);

        A.C.set_blend_mode(BlendModeBlend);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        
        // we render the uppers first with z-buffer writes
        // then the lowers without z writes
        // then the polytron_outline
        A.C.enable_vertex_array(&wall_verts[0], wall_verts.size());
        A.C.draw_array_quads_indexed(&upper_wall_quads[0], upper_wall_quads.size());
        glDepthMask(GL_FALSE);
        A.C.draw_array_quads_indexed(&lower_wall_quads[0], lower_wall_quads.size());

        glDepthMask(GL_TRUE);
        A.C.disable_vertex_array(&wall_verts[0]);

        glDisable(GL_CULL_FACE);
        A.C.set_blend_mode(BlendModeNone);

#if 1
        A.C.set_blend_mode(BlendModeBlend);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,
                      A.GR.goalZoneTexture->m_textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                        GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                        GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                        GL_REPEAT);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        A.C.enable_vertex_array(&outline_quad_verts[0], outline_quad_verts.size());
        A.C.draw_array_quads(0, outline_quad_verts.size());
        A.C.disable_vertex_array(&outline_quad_verts[0]);
        glDisable(GL_TEXTURE_2D);
        A.C.set_blend_mode(BlendModeNone);
#endif
        if(test_z)
        {
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
            glDepthFunc(GL_ALWAYS);
        }

        /*for(uint32 outline_index = 0; outline_index < barrier_outlines.size(); outline_index++)
        {
            polytron_outline<RenderVertex2CT> &o = barrier_outlines[outline_index];

            for (uint32 i = 0; i < o.vertices.size(); i++)
            {
                RenderVertex2CT &v = o.vertices[i];
                Color c(v.t.x, v.t.y, 0);
                v.c.set(c, 0.5);
            }
            A.C.set_blend_mode(BlendModeBlend);
            A.C.enable_vertex_array(&o.vertices[0], o.vertices.size());
            for(uint32 b = 0; b < o.beams.size(); b++)
            {
                A.C.draw_array_quads_indexed(&o.indices[o.beams[b].index_start], o.beams[b].index_count);
            }
            A.C.disable_vertex_array(&o.vertices[0]);
            A.C.set_blend_mode(BlendModeNone);
        }*/

        Rect r(center_pos, center_pos);
        r.expand(vec2(50,50));
        A.C.colorf(1,1,1);
        A.C.draw_line(r.min, r.max);
        A.C.draw_line(vec2(r.min.x, r.max.y), vec2(r.max.x, r.min.y));
    }

};

