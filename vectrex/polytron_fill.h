// The vectrex library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

// polytron_fill objects are instanced from polytrons for fill triangle rendering

template <class vertex_type> class polytron_fill
{
public:
    array<vertex_type> vertices;
    array<uint32> indices;
    array<polytron::primitive> tri_lists;
    template<class vt> void fill_vertex(vt &v, const vec2 &p)
    {
        v.p.x = p.x;
        v.p.y = p.y;
    }

    void fill_vertex(vec2 &v, const vec2 &p)
    {
        v = p;
    }

    void set(polytron &p)
    {
        vertices.clear();
        indices.clear();
        tri_lists.clear();
        if(!p.fill_triangles.size())
            return;
        
        const uint32 invalid_index = 0xFFFFFFFF;
        
        // first pass identifies which vertices are referenced at all by the polytron's fill triangles:
        array<uint32> vertex_remap;
        uint32 used_vertex_count = 0;
        vertex_remap.resize(p.vertices.size());
        for(uint32 i = 0; i < p.vertices.size(); i++)
            vertex_remap[i] = invalid_index;
        
        tri_lists.resize(p.fill_triangles.size());
        for(uint32 ft = 0; ft < p.fill_triangles.size(); ft++)
        {
            polytron::primitive &tris = p.fill_triangles[ft];
            tri_lists[ft] = tris;
            for(uint32 i = 0; i < tris.vertex_count; i++)
            {
                uint32 index_index = tris.index_start + i;
                uint32 vertex_index = p.indices[index_index];
                if(vertex_remap[vertex_index] == invalid_index)
                    vertex_remap[vertex_index] = used_vertex_count++;
            }
        }
        vertices.resize(used_vertex_count);
        // now copy over the vertex data
        for(uint32 i = 0; i < p.vertices.size(); i++)
            if(vertex_remap[i] != invalid_index)
                fill_vertex(vertices[vertex_remap[i]], p.vertices[i]);

        // and now the indices...
        polytron::primitive &t = tri_lists.last();
        uint32 total_index_count = t.index_start + t.vertex_count;
        indices.reserve(total_index_count);
        for(uint32 i = 0; i < total_index_count; i++)
        {
            uint32 remap_index = vertex_remap[p.indices[i]];
            indices.push_back(remap_index);
        }
    }
    
    void render_debug()
    {
        A.C.enable_vertex_array(&vertices[0], vertices.size());

        A.C.colorf(0.33, 0.33, 0.33, 1);
        A.C.draw_array_triangles_indexed(&indices[0], indices.size());
        
        A.C.colorf(0.66, 0.66, 0.66, 1);
        for(uint32 i = 0; i < indices.size(); i += 3)
            A.C.draw_array_line_loop_indexed(&indices[i], 3);
        A.C.disable_vertex_array(&vertices[0]);

    }
};
