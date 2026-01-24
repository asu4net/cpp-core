#include "load_model.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

// @Note: Attrib layout:

/*
   Float arrays:

   attrib.vertices = [
       x0, y0, z0,
       x1, y1, z1,
       x2, y2, z2,
       ...
   ];
   attrib.texcoords = [
        u0, v0,
        u1, v1,
        u2, v2,
        ...
   ];
    attrib.normals = [
        nx0, ny0, nz0,
        nx1, ny1, nz1,
        ...
    ];
*/

Model::Model(std::string_view filename, bool normals_as_colors)
{
    if (filename.empty())
    return;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    std::filesystem::path abs_path = std::filesystem::absolute(filename);
    std::filesystem::path dir_path = abs_path.parent_path();
    m_dirpath = dir_path.string();
    m_name = abs_path.stem().string();

    bool ok = tinyobj::LoadObj(
        &attrib,
        &shapes,
        &materials,
        &warn,
        &err,
        filename.data(),
        dir_path.string().c_str()
    );

    if (!ok)
    {
        dbg_check(false, "Mesh could not be loaded!\n");
        return;
    }

    m_vertices.reserve(attrib.vertices.size() / 3);
    m_indices.reserve(1024);

    u64 it_index = 0; for (const auto& shape: shapes)
    {
        u32 index_offset = static_cast<u32>(m_indices.size());
        u32 index_count = 0;
        
        for (const auto& idx : shape.mesh.indices)
        {
            Vertex& v = m_vertices.emplace_back();

            v.pos.x = attrib.vertices[3 * idx.vertex_index + 0];
            v.pos.y = attrib.vertices[3 * idx.vertex_index + 1];
            v.pos.z = attrib.vertices[3 * idx.vertex_index + 2];

            if (idx.texcoord_index >= 0)
            {
                v.uv.x = attrib.texcoords[2 * idx.texcoord_index + 0];
                v.uv.y = attrib.texcoords[2 * idx.texcoord_index + 1];
            }
            
            v.normal.x = attrib.normals[3 * idx.normal_index + 0];
            v.normal.y = attrib.normals[3 * idx.normal_index + 1];
            v.normal.z = attrib.normals[3 * idx.normal_index + 2];

            if (normals_as_colors)
            {
                v.color.x = v.normal.x;
                v.color.y = v.normal.y;
                v.color.z = v.normal.z;
                v.color.w = 1.f;
            }
            else
            {
               v.color = Color.White;
            }
            
            m_indices.emplace_back(static_cast<u32>(m_indices.size()));
            ++index_count;
        }

        Shape& s = m_shapes.emplace_back();
        s.index_offset = index_offset;
        s.index_count = index_count;
        s.material_index = shape.mesh.material_ids.empty() ? 0u : static_cast<u32>(shape.mesh.material_ids[0]);
        
        if (it_index < materials.size())
        {
            s.name = materials[it_index].name;
            s.textures.ambient = materials[it_index].ambient_texname;
            s.textures.diffuse = materials[it_index].diffuse_texname;
        }

        ++it_index;
    }
    
    m_loaded = true;
}
