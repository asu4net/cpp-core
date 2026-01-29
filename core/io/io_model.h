#pragma once

// @Pending:

// Este modelo no es el que realmente querríamos para una app real.

// (para desarrollo mirar io_image.h).

struct IO_Model_VTX {
    Vec3 pos;
    Vec2 uv;
    Vec3 normal;
    Vec4 color;
};

struct IO_Model_Shape {
    struct {
        std::string ambient;
        std::string diffuse;
    } textures;

    std::string name;
    u32 index_offset = 0;
    u32 index_count = 0;
    u32 material_index = 0;
};

using IO_Model_Vertices = std::vector<IO_Model_VTX>;
using IO_Model_Shapes = std::vector<IO_Model_Shape>;
using IO_Model_Elems = std::vector<u32>;

struct IO_Model {
    bool normals_as_colors = false;
    std::string name;
    std::string dirpath;
    IO_Model_Vertices vertices;
    IO_Model_Elems elems;
    IO_Model_Shapes shapes;
};

fn io_model_load(std::string_view filename, IO_Model* model) -> bool;