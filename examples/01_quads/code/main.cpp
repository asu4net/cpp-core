#include "app.h"

fn main() -> s32 {

    app_init();

    // Counter-clock wise Quad Vertices.
    constexpr f32 QUAD_VTS[] = {
        -0.5f, -0.5f, // Vertex 0. Bottom-left. 
        +0.5f, -0.5f, // Vertex 1. Bottom-right.
        +0.5f, +0.5f, // Vertex 2. Top-right.  
        -0.5f, +0.5f, // Vertex 3. Top-left.
    };
    
    // *** VAO: Vertex Array Object ***
    
    // @Note: The parent object of our geometry.

    u32 vao = 0u;
    glCreateVertexArrays(1, &vao);

    // *** Vertex Attributes ***
    
    // @Note: This will make OpenGL understand the layout of out vertices.

    // Offset in bytes from the start of the vertex.
    s32 offset = 0;

    // 0: POSITION --------------------------------------------
    
    struct {
        s32 Index = 0;
        s32 Count = 2; // Two components: X, Y.
        s32 Size  = sizeof(f32) * Count; // Size in bytes.
    } constexpr Pos;

    // Enables the Attrib index.
    glEnableVertexArrayAttrib(vao, Pos.Index);
    // Explains OpenGL the memory layout of the attribute.
    glVertexArrayAttribFormat(vao, Pos.Index, Pos.Count, GL_FLOAT, false, offset);
    // Increment the offset with the attrib size in bytes.
    offset += Pos.Size;

    // This will be explained later (sorry :p)
    glVertexArrayAttribBinding(vao, Pos.Index, /* vbo binding */ 0u);

    // --------------------------------------------------------

    // *** VBO: Vertex Buffer Object ***
    
    // @Note: Object used to send the vertices to OpenGL.
    u32 vbo = 0u;

    // Total Vertex Size.
    constexpr u64 QUAD_VT_SIZE = Pos.Size;

    // Create the buffer, and pass the raw memory of the vertices.
    glCreateBuffers(1, &vbo);
    glNamedBufferData(vbo, sizeof(QUAD_VTS), QUAD_VTS, GL_STATIC_DRAW);

    // Links the VBO with the VAO, in the index 0
    glVertexArrayVertexBuffer(vao, 0u, vbo, 0u, QUAD_VT_SIZE);
    
    // *** EBO: Element Buffer Object ***

    // @Note: Object used to explain OpenGL how the VAO/VBO indices
    // build the triangles of our geometry.
    u32 ebo = 0u;

    constexpr u32 QUAD_ELEMS[] = {
        0u, 1u, 2u, // Triangle 1
        2u, 3u, 0u, // Triangle 2
    };
    
    glCreateBuffers(1, &ebo);
    glNamedBufferData(ebo, sizeof(QUAD_ELEMS), QUAD_ELEMS, GL_STATIC_DRAW);

    // Links the EBO with the VAO. 
    glVertexArrayElementBuffer(vao, ebo);

    // *** Shader Program ***
    std::string source = os_read_entire_file("shader_flat_color.glsl");
    u32 shader_flat_color = os_create_gl_program(source);

    // *** UBO: Uniform Buffer Object.

    // @Note: This is meant to send no-vertex related data
    // to the shader.
    struct {
        Mat4 projection = Mat.Identity4;
        Mat4 transform = Mat.Identity4;
    } shader_data;

    u32 ubo = 0u;
    glCreateBuffers(1, &ubo);
    glNamedBufferData(ubo, sizeof(shader_data), nullptr, GL_DYNAMIC_DRAW);
    
    // Our quad position.
    Vec3 quad_pos;

    while (app_running())
    {
        glClear(GL_COLOR_BUFFER_BIT);
        
        // We want to see the quad in movement in order
        // to know if we nailed the UBO setup.
        quad_pos += Vec3(F32.Right) * os_delta_time();

        // Now we create a transform matrix with the vector.
        shader_data.transform = Mat4::transpose(Mat4::transform(quad_pos));
        
        // Also an orthographic projection matrix, to get world
        // space coordinates instead of NDC.

        f32 aspect = 16.0f / 9.0f; // This will give us an homogeneus box.
        f32 zoom   = 3.0f;         // The scale of the box.
        f32 nearpl = 0.0f;         // "Back" plane.
        f32 farpl  = 0.1f;         // "Front" plane.
        
        shader_data.projection = Mat4::transpose(Mat4::orthographic(aspect, zoom, nearpl, farpl));

        // Then we pase our struct instance to the UBO and
        // tell OpenGL we're about to use it.
        glNamedBufferSubData(ubo, 0, sizeof(shader_data), &shader_data);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0u, ubo);

        // Tell OpenGL that we are going to use
        // our VAO and our shader in the next
        // draw call.
        glUseProgram(shader_flat_color);
        glBindVertexArray(vao);

        // The draw call.
        glDrawElements(GL_TRIANGLES, /* index_count */ 6, GL_UNSIGNED_INT, nullptr);

        // Swap the front and back buffers.
        os_swap_buffers();
    }

    app_done();
}