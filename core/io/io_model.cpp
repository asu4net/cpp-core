#include "io_model.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

fn io_model_load(std::string_view filename, IO_Model* model) -> bool {
    if (filename.empty()) {
        return false;
    }

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    std::filesystem::path abs_path = std::filesystem::absolute(filename);
    std::filesystem::path dir_path = abs_path.parent_path();
    model->dirpath = dir_path.string();
    model->name = abs_path.stem().string();

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
        logf(false, "Error! Mesh could not be loaded!\n");
        return false;
    }

    model->vertices.reserve(attrib.vertices.size() / 3);
    model->elems.reserve(1024);

    u64 it_index = 0; for (const auto& shape: shapes)
    {
        u32 index_offset = static_cast<u32>(model->elems.size());
        u32 index_count = 0;
        
        for (const auto& idx : shape.mesh.indices)
        {
            IO_Model_VTX& v = model->vertices.emplace_back();

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

            if (model->normals_as_colors)
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
            
            model->elems.emplace_back(static_cast<u32>(model->elems.size()));
            ++index_count;
        }

        IO_Model_Shape& s = model->shapes.emplace_back();
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
    
    return true;
}