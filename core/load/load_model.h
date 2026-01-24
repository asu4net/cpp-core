#pragma once

class Model
{
public:
    struct Vertex
    {
        Vec3 pos;
        Vec2 uv;
        Vec3 normal;
        Vec4 color;
    };
    
    // @Note: Shapes are parts of the model who use different materials.
    struct Shape
    {
        struct 
        {
            std::string ambient;
            std::string diffuse;
        } textures;

        std::string name;
        u32 index_offset = 0;
        u32 index_count = 0;
        u32 material_index = 0;
    };
    
    Model(std::string_view filename, bool normals_as_colors = false);
    ~Model() = default;

    Model(const Model&) = delete;
    Model(Model&&) noexcept = delete;

    auto operator=(const Model&) -> Model& = delete;
    auto operator=(Model&&) noexcept -> Model& = delete;
    
    auto loaded() const -> bool { return m_loaded; }
    auto dirpath() const -> std::string_view { return m_dirpath; }

    // @Note: These are non-constant vector references because we want to steal them later, from the mesh class.
    auto name() const -> const std::string& { return m_name; }
    auto vertices() -> std::vector<Vertex>& { return m_vertices; }
    auto indices() -> std::vector<u32>& { return m_indices; }
    auto shapes() -> std::vector<Shape>& { return m_shapes; }

private:
    bool m_loaded = false;
    std::string m_name;
    std::vector<Vertex> m_vertices;
    std::vector<u32> m_indices;
    std::vector<Shape> m_shapes;
    std::string m_dirpath;
};
