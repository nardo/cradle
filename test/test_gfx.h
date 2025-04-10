static std::vector<std::vector<glm::ivec3> > triangulate(std::vector<glm::dvec2>&& vertices, const std::vector<std::vector<int> >& indices) {
    std::vector<PolygonWithHolesTree> trees =
            buildPolygonAreaTrees(
                    buildPolygonTrees(vertices,
                            decomposePolygonGraph(vertices,
                                    insertSteinerVerticesForPolygons(vertices, indices))));
    std::vector<std::vector<glm::ivec3> > triangles;
    for(const decomposition::PolygonWithHoles& polygon : trees) {
        triangles.push_back(triangulatePolygonWithHoles(vertices, polygon));
    }
    return triangles;
}

class TestGFXUserInterface : public TestUserInterface
{
    typedef TestUserInterface Parent;
public:
    polytron test_vectron;
    poly_tree test_poly_tree;
    float32 cur_width;
    
    TestGFXUserInterface()
    {
        cur_width = 1;
#if 0
        enum {
            num_verts = 58,
            num_polys = 12,
        };
        
        static float32 vert_data[num_verts][2] =
        {
            {174.0, 129.0},
            {116.0, 433.0},
            {608.0, 421.0},
            {539.0, 130.0},
            
            {282.0, 324.0},
            {252.0, 399.0},
            {188.0, 354.0},
            
            {471.0, 339.0},
            {436.36, 399.0},
            {327.0, 344.0},
            {434.0, 282.0},
            
            {505.0, 408.0},
            {602.0, 86.0},
            {527.0, 317.0},
            {504.0, 247.0},
            {483.0, 199.0},
            {441.0, 187.0},
            {499.0, 161.0},
            {570.0, 370.0},
            
            {343.0, 212.0},
            {327.0, 303.0},
            {297.0, 410.0},
            {318.0, 164.0},
            {359.0, 157.0},
            {380.0, 248.0},
            
            {216.0, 242.0},
            {174.36, 323.0},
            {207.0, 153.0},
            {266.0, 237.0},
            
            {174.0, 129.0},
            {116.0, 433.0},
            {608.0, 421.0},
            {539.0, 130.0},
            
            {282.0, 324.0},
            {252.0, 399.0},
            {188.0, 354.0},
            
            {471.0, 339.0},
            {436.36, 399.0},
            {327.0, 344.0},
            {434.0, 282.0},
            
            {505.0, 408.0},
            {523.0, 363.0},
            {527.0, 317.0},
            {504.0, 247.0},
            {483.0, 199.0},
            {441.0, 187.0},
            {499.0, 161.0},
            {570.0, 370.0},
            
            {343.0, 212.0},
            {327.0, 303.0},
            {297.0, 410.0},
            {318.0, 164.0},
            {359.0, 157.0},
            {380.0, 248.0},
            
            {216.0, 242.0},
            {174.36, 323.0},
            {207.0, 153.0},
            {266.0, 237.0}
        };
        static uint32 poly_data[num_polys][9] = {
            {4, 0, 1, 2, 3 },
            {3, 4, 5, 6 },
            {4, 7, 8, 9, 10 },
            {8, 11, 12, 13, 14, 15, 16, 17, 18 },
            {6, 19, 20, 21, 22, 23, 24 },
            {4, 25, 26, 27, 28 },
            {4, 29, 30, 31, 32 },
            {3, 33, 34, 35 },
            {4, 36, 37, 38, 39 },
            {8, 40, 41, 42, 43, 44, 45, 46, 47 },
            {6, 48, 49, 50, 51, 52, 52 },
            {4, 53, 54, 55, 56 }
        };
#else
        enum {
            num_verts = 13,
            num_polys = 2,
        };
        static float32 vert_data[num_verts][2] = {
            {100, 100 },
            { 500, 50 },
            { 600, 100 },
            { 500, 200 },
            { 600, 350 },
            { 500, 500 },
            { 400, 350 },
            { 100, 500 },
            { 150, 200 },
            { 200, 200 },
            { 200, 350 },
            { 350, 350 },
            { 350, 200 },
        };

        static uint32 poly_data[num_polys][10] = {
            {9, 0, 1, 2, 3, 4, 5, 6, 7, 8 },
            {4, 9, 10, 11, 12 }
        };

#endif
        uint32 index_offset = test_poly_tree.add_vertices(&vert_data[0][0], num_verts);
        for(uint32 i = 0; i < num_polys; i++)
            test_poly_tree.add_indexed_poly(&poly_data[i][1], poly_data[i][0]);
#if 0
        test_vectron.poly_tree(test_poly_tree);

#else
        vec2 verts[5];
        verts[0].set(100,100);
        verts[1].set(500,50);
        verts[2].set(480,300);
        verts[3].set(250, 450);
        verts[4].set(72, 400);
        test_vectron.convex_poly(verts, 5, false);
#endif
    }
    void render()
    {
        test_vectron.render_debug();
        polytron_outline<RenderVertex2CT> o;
        o.set(test_vectron, cur_width, cur_width);
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

        Parent::render();
    }
    void onKeyDown(uint32 key)
    {
        if(key == '=')
            cur_width += 10;
        else if(key == '-')
            cur_width -= 10;
        else
            Parent::onKeyDown(key);
    }
    
} UITestGFX;


