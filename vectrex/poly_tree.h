// The vectrex library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

/// The poly_tree class -- encapsulates decomposition_library facilities for management of complex polygons

struct poly_tree
{
    typedef std::vector<int> index_list;
    typedef std::vector<glm::dvec2> vertex_list;
    typedef std::vector<PolygonWithHolesTree> holey_tree_list;
    typedef std::vector<glm::ivec3> tri_list;
    typedef std::vector<tri_list> tri_lists;

    std::vector<index_list> indices;
    tri_lists triangles;
    vertex_list vertices;
    holey_tree_list holey_trees;

    void clear()
    {
        vertices.clear();
        indices.clear();
        holey_trees.clear();
        triangles.clear();
    }
    
    void add_poly(vec2 *verts, uint32 vertex_count)
    {
        indices.resize(indices.size() + 1);
        std::vector<int> &new_indices = indices.back();
        new_indices.resize(vertex_count);
        uint32 vertex_start = vertices.size();
        vertices.resize(vertex_count + vertex_start);
        for(uint32 i = 0; i < vertex_count; i++)
        {
            new_indices[i] = vertex_start + i;
            vertices[vertex_start + i] = glm::dvec2(verts[i].x, verts[i].y);
        }
    }
    
    uint32 add_vertices(vec2 *verts, uint32 vertex_count)
    {
        return add_vertices(&verts[0].x, vertex_count);
    }
    
    uint32 add_vertices(float32 *verts, uint32 vertex_count)
    {
        uint32 vertex_start = vertices.size();
        vertices.resize(vertex_count + vertex_start);
        for(uint32 i = 0; i < vertex_count; i++)
        {
            vertices[vertex_start + i] = glm::dvec2(verts[0], verts[1]);
            verts += 2;
        }
        return vertex_start;
    }
    void add_indexed_poly(uint32 *_indices, uint32 index_count, uint32 index_offset = 0)
    {
        indices.resize(indices.size() + 1);
        std::vector<int> &new_indices = indices.back();
        new_indices.resize(index_count);
        for(uint32 i = 0; i < index_count; i++)
            new_indices[i] = index_offset + _indices[i];
    }
    
    void compute_trees_and_tris()
    {
        if(holey_trees.size() == 0)
        {
            holey_trees = buildPolygonAreaTrees(buildPolygonTrees(vertices, decomposePolygonGraph(vertices, insertSteinerVerticesForPolygons(vertices, indices))));
            triangles.resize(holey_trees.size());
            for(uint32 i = 0; i < holey_trees.size(); i++)
                triangles[i] = triangulatePolygonWithHoles(vertices, holey_trees[i]);
        }
    }
};

