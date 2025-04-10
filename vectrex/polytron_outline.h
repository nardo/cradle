/// The vectrex library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

/// The polytron_outline class -- polytron_outline objects are instanced from polytrons, extruding a beam of the specified width. Changing the width regenerates the outline from the source polytron

template<class vertex_type> struct polytron_outline
{
    float32 out_width; // extrude beam out from the perimeter
    float32 in_width; // extrude beam in from the perimeter
    
    array<vertex_type> vertices;
    array<uint32> indices;
    
    struct beam
    {
        float32 length;
        // a beam is a set of indexed quads - one quad for each edge/segment of the beam
        uint32 index_start;
        uint32 index_count;
        
        // we also track the overall count of verices used on each beam -- this is so outside code can fix up texture coords on a per-beam basis (the t values run 0..1 as a fraction of its beam's length.
        uint32 vertex_start;
        uint32 vertex_count;
    };
    array<beam> beams;
    
    // that should be all we need...
    
    polytron_outline()
    {
        in_width = out_width = 0;
    }
    
    template<class vt> void fill_vertex(vt &v, const vec2 &p, float32 tex, float32 t)
    {
        v.p = p;
        v.t.x = tex;
        v.t.y = t;
    }
    
    template<class vt> vec2 get_vertex(vt &v)
    {
        return v.p;
    }
    
    void fill_vertex(vec2 &v, const vec2 &p, float32 tex, float32 t)
    {
        v = p;
    }

    vec2 get_vertex(vec2 &v)
    {
        return v;
    }

    void set_width(polytron &p, float32 in_w, float32 out_w)
    {
        if(in_w != in_width || out_w != out_width)
            set(p, in_w, out_w);
    }
    
    void set(polytron &p, float32 in_w, float32 out_w)
    {
        vertices.clear();
        indices.clear();
        in_width = in_w;
        out_width = out_w;
        if(in_width == 0 && out_width == 0)
            return;
        
        // we construct a tri_list beam for each border line strip in the polytron. Each edge of a border becomes a quad with the vertex normals extruded out by out_width and extruded in by in_width. If the vertex type includes texture coordinates, texture coordinates are generated - t.y runs from 0 to 1, where 0 is the inside edge of the beam and 1 is the outside edge. t.x runs from 0 to 1, along the full length of the line.
        // where it gets a little tricky is where an edge is short enough that the extrusion of the inner or the outer "crosses". In this case, we de-extrude the beam on whichever side got crossed.
        // we'll take a rough guess at the size of the vertex and index buffers, so that we're not doing a bunch of allocation as we build the beams
        // the common case will have 2 vertices for each vertex in the polytron plus 2 vertices for each beam, since the start/end vertex of each loop will be emitted twice. Add in a little extra in case any beam crossings happen, as those will emit 4 additional vertices per beam crossing.
        vertices.reserve(uint32((p.vertices.size() + p.border_line_strips.size()) * 2.1f));

        beams.resize(p.border_line_strips.size());

        // the index count is more predictable - four indices per segment in each beam unless one is dropped
        uint32 index_count = 0;
        for(uint32 b = 0; b < beams.size(); b++)
        {
            polytron::primitive &s = p.border_line_strips[b];
            assert(s.vertex_count > 1);
            index_count += (s.vertex_count - 1) * 4;
        }
        indices.reserve(index_count);
        
        // now build the beams.
        for(uint32 b = 0; b < beams.size(); b++)
        {
            beams[b].vertex_start = vertices.size();
            beams[b].index_start = indices.size();
            beams[b].index_count = 0;
            polytron::primitive &s = p.border_line_strips[b];
            beams[b].length = s.length;
            
            // first, add all the default case vertices. We add two vertices for each vertex in the strip, the first extruded out by the vertex normal * out_width and the other extruded in (ie -normal * in_width).
            
            for(uint32 i = 0; i < s.vertex_count; i++)
            {
                uint32 index_index = s.index_start + i;
                vec2 &n = p.index_properties[index_index].n;
                vec2 &v = p.vertices[p.indices[index_index]];
                vec2 out_point = v + n * out_width;
                vec2 in_point = v - n * in_width;
                vertices.increment();
                fill_vertex(vertices.last(), in_point, 0, p.index_properties[index_index].t);
                vertices.increment();
                fill_vertex(vertices.last(), out_point, 1, p.index_properties[index_index].t);
            }
            
            uint32 vs = beams[b].vertex_start;
            
            // now loop through all the segments - emit the simple ones and add fixups where the wires cross.
            uint32 index_index = s.index_start;

            for(uint32 i = 1; i < s.vertex_count; i++)
            {
                // this edge runs from vertex i - 1 to vertex i;
                const polytron::index_prop &prop0 = p.index_properties[index_index];
                const vec2 &v0 = p.vertices[p.indices[index_index]];

                index_index++;
                const polytron::index_prop &prop1 = p.index_properties[index_index];
                const vec2 &v1 = p.vertices[p.indices[index_index]];

                vertex_type *dvs = &vertices[vs];
                
                vec2 in_vec = get_vertex(dvs[2]) - get_vertex(dvs[0]);
                vec2 out_vec = get_vertex(dvs[3]) - get_vertex(dvs[1]);
                
                vec2 seg_vec = prop1.v;
                
                float32 in_dot = in_vec.dot(seg_vec);
                float32 out_dot = out_vec.dot(seg_vec);

                if(in_dot >= 0 && out_dot >= 0)
                {
                    // easy case - push back the four default verts
                    indices.push_back(vs);
                    indices.push_back(vs+1);
                    indices.push_back(vs+3);
                    indices.push_back(vs+2);
                    beams[b].index_count += 4;
                }
                else if(in_dot >= 0)
                {
                    // out_dot < 0
                    float32 seg_len = seg_vec.len();
                    float32 scale_factor = seg_len / (seg_len - out_dot / seg_len);
                    float32 tex = 0.5 + scale_factor * 0.5;
                    float32 new_out_width = out_width * scale_factor;
                    vec2 out0 = v0 + prop0.n * new_out_width;
                    vec2 out1 = v1 + prop1.n * new_out_width;
                    uint32 vo = vertices.size();
                    vertices.resize(vo+2);
                    fill_vertex(vertices[vo], out0, tex, prop0.t);
                    fill_vertex(vertices[vo+1], out1, tex, prop1.t);
                    indices.push_back(vs);
                    indices.push_back(vo);
                    indices.push_back(vo+1);
                    indices.push_back(vs+2);
                    beams[b].index_count += 4;
                }
                else if(out_dot >= 0)
                {
                    // in_dot < 0
                    float32 seg_len = seg_vec.len();
                    float32 scale_factor = seg_len / (seg_len - in_dot / seg_len);
                    float32 tex = 0.5 - scale_factor * 0.5;
                    float32 new_in_width = in_width * scale_factor;
                    vec2 in0 = v0 - prop0.n * new_in_width;
                    vec2 in1 = v1 - prop1.n * new_in_width;
                    uint32 vi = vertices.size();
                    vertices.resize(vi+2);
                    fill_vertex(vertices[vi], in0, tex, prop0.t);
                    fill_vertex(vertices[vi+1], in1, tex, prop1.t);
                    indices.push_back(vi);
                    indices.push_back(vs+1);
                    indices.push_back(vs+3);
                    indices.push_back(vi+1);
                    beams[b].index_count += 4;
                }
                vs += 2;
            }
            beams[b].vertex_count = vertices.size() - beams[b].vertex_start;
        }
    }
};
