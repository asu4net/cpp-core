#include "gl_quad.h"
#include "app.h"

#if GAME_GL

struct {

    struct {
        u32 vao = 0u;
        u32 vbo = 0u;
        u32 ebo = 0u;
        u32 ubo = 0u;
        u32 pgm = 0u;
    } objects;

    struct {
        Mat4 projection = Mat.Identity4;
        Mat4 transform = Mat.Identity4;
    } shader_data;

} internal gl_quad;

fn gl_quad_buffer_init() -> void {

    auto& [vao, vbo, ebo, ubo, pgm] = gl_quad.objects;

    constexpr f32 QUAD_VTS[] = {
        -0.5f, -0.5f, // Vertex 0. Bottom-left. 
        +0.5f, -0.5f, // Vertex 1. Bottom-right.
        +0.5f, +0.5f, // Vertex 2. Top-right.  
        -0.5f, +0.5f, // Vertex 3. Top-left.
    };
    
    // *** VAO ***
    glCreateVertexArrays(1, &vao);

    // *** Vertex Attributes ***
    
    i32 offset = 0;

    // 0: POSITION --------------------------------------------
    
    struct {
        i32 Index = 0;
        i32 Count = 2; 
        i32 Size  = sizeof(f32) * Count;
    } constexpr Pos;

    glEnableVertexArrayAttrib(vao, Pos.Index);
    glVertexArrayAttribFormat(vao, Pos.Index, Pos.Count, GL_FLOAT, false, offset);
    offset += Pos.Size;

    glVertexArrayAttribBinding(vao, Pos.Index, /* vbo binding */ 0u);

    // --------------------------------------------------------
    
    // *** VBO ***
    constexpr u64 QUAD_VT_SIZE = Pos.Size;
    glCreateBuffers(1, &vbo);
    glNamedBufferData(vbo, sizeof(QUAD_VTS), QUAD_VTS, GL_STATIC_DRAW);
    glVertexArrayVertexBuffer(vao, 0u, vbo, 0u, QUAD_VT_SIZE);
    
    // *** EBO ***
    constexpr u32 QUAD_ELEMS[] = {
        0u, 1u, 2u, // Triangle 1
        2u, 3u, 0u, // Triangle 2
    };
    glCreateBuffers(1, &ebo);
    glNamedBufferData(ebo, sizeof(QUAD_ELEMS), QUAD_ELEMS, GL_STATIC_DRAW);
    glVertexArrayElementBuffer(vao, ebo);

    // *** UBO ***
    glCreateBuffers(1, &ubo);
    glNamedBufferData(ubo, sizeof(gl_quad.shader_data), nullptr, GL_DYNAMIC_DRAW);

    // *** Program ***
    std::string source = os_read_entire_file("shader_flat_color.glsl");
    pgm = os_create_gl_program(source);

    f32 aspect = 16.0f / 9.0f; 
    f32 zoom = 3.0f;         
    f32 nearpl = 0.0f;      
    f32 farpl = 0.1f;      
    gl_quad.shader_data.projection = Mat4::transpose(Mat4::orthographic(aspect, zoom, nearpl, farpl));
}

fn gl_quad_buffer_done() -> void {

    auto& [vao, vbo, ebo, ubo, pgm] = gl_quad.objects;
    glDeleteProgram(pgm);
    glDeleteBuffers(1, &ubo);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
    gl_quad = {};
}

fn gl_quad_draw(const Vec3& pos, const Vec3& rot, const Vec3& scl) -> void {
    auto& [vao, vbo, ebo, ubo, pgm] = gl_quad.objects;
    auto& shader_data = gl_quad.shader_data;

    shader_data.transform = Mat4::transpose(Mat4::transform(pos, rot, scl));
    glNamedBufferSubData(ubo, 0, sizeof(shader_data), &shader_data);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0u, ubo);
    glUseProgram(pgm);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, /* index_count */ 6, GL_UNSIGNED_INT, nullptr);
}

#endif