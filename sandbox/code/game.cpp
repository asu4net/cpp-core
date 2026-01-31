#include "app.h"

fn main() -> i32 {

    app_init();

    // Counter-clock wise Quad Vertices.
    constexpr f32 QUAD_VTS[] = {
        -0.5f, -0.5f,  0.0f, 0.0f, // Vertex 0. Bottom-left. 
        +0.5f, -0.5f,  1.0f, 0.0f, // Vertex 1. Bottom-right.
        +0.5f, +0.5f,  1.0f, 1.0f, // Vertex 2. Top-right.  
        -0.5f, +0.5f,  0.0f, 1.0f, // Vertex 3. Top-left.
    };
    
    // *** VAO: Vertex Array Object ***
    
    // @Note: The parent object of our geometry.

    u32 vao = 0u;
    glCreateVertexArrays(1, &vao);

    // *** Vertex Attributes ***
    
    // @Note: This will make OpenGL understand the layout of out vertices.

    // Offset in bytes from the start of the vertex.
    i32 offset = 0;

    // 0: POSITION --------------------------------------------
    
    struct {
        i32 Index = 0;
        i32 Count = 2; // Two components: X, Y.
        i32 Size  = sizeof(f32) * Count; // Size in bytes.
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

    // 1: UV --------------------------------------------------
    
    struct {
        i32 Index = 1;
        i32 Count = 2; // Two components: X, Y.
        i32 Size  = sizeof(f32) * Count; // Size in bytes.
    } constexpr UV;

    glEnableVertexArrayAttrib(vao, UV.Index);
    glVertexArrayAttribFormat(vao, UV.Index, UV.Count, GL_FLOAT, false, offset);
    offset += UV.Size;
    glVertexArrayAttribBinding(vao, UV.Index, /* vbo binding */ 0u);

    // --------------------------------------------------------
    
    // *** VBO: Vertex Buffer Object ***
    
    // @Note: Object used to send the vertices to OpenGL.
    u32 vbo = 0u;

    // Total Vertex Size.
    constexpr u64 QUAD_VT_SIZE = Pos.Size + UV.Size;

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
    std::string source = os_read_entire_file("shader_texture.glsl");
    u32 shader_flat_color = os_create_gl_program(source);

    // *** UBO: Uniform Buffer Object.

    // @Note: This is meant to send no-vertex related data
    // to the shader.
    struct {
        Mat4 projection = Mat.Identity4;
        Mat4 transform = Mat.Identity4;
        i32 tex_unit = 0;
    } shader_data;

    u32 ubo = 0u;
    glCreateBuffers(1, &ubo);
    glNamedBufferData(ubo, sizeof(shader_data), nullptr, GL_DYNAMIC_DRAW);
    
    // Our quad position.
    Vec3 quad_pos;

    // *** Texture ***
    
    // Handcraft a 4 pixel image.
    u8 image_pixels[16] = {
        042u, 045u, 121u, 255u, // Some blue.
        232u, 005u, 103u, 255u, // Some red.
        255u, 142u, 104u, 255u, // Some orange.
        244u, 234u, 188u, 255u, // Some yellow.
    };    

    IO_Image image;
    image.data     = image_pixels;
    image.channels = 4; // RGBA
    image.width    = 2; // Pixel Width.
    image.height   = 2; // Pixel Height.

    // Create the texture object.
    u32 tex = 0u;
    glCreateTextures(GL_TEXTURE_2D, 1, &tex);
    
    // Check if as RGB or RGBA.
    i32 storage_format = image.channels == 4 ? GL_RGBA8 
                       : image.channels == 3 ? GL_RGB8 : 0;

    // Reserve the storage.    
    glTextureStorage2D(tex, 1, storage_format, image.width, image.height);

    // Texture config.
    glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Check if as RGB or RGBA. (Again :S)
    i32 data_format = image.channels == 4 ? GL_RGBA 
                    : image.channels == 3 ? GL_RGB : 0;
    
    // Send the texture data to the gpu.
    glTextureSubImage2D(tex, 0, 0, 0, image.width, image.height, data_format, GL_UNSIGNED_BYTE, image.data);

    while (app_running())
    {
        glClear(GL_COLOR_BUFFER_BIT);
        
        glBindTextureUnit(0, tex);

        // We want to see the quad in movement in order
        // to know if we nailed the UBO setup.
        //quad_pos += Vec3(F32.Right) * os_delta_time();

        // Now we create a transform matrix with the vector.
        shader_data.transform = Mat4::transpose(Mat4::transform(quad_pos, F32.Zero, Vec3(F32.One) * 2.0f));
        
        // Also an orthographic projection matrix, to get world
        // space coordinates instead of NDC.

        f32 aspect = 16.0f / 9.0f; // This will give us an homogeneus box.
        f32 zoom   = 3.0f;         // The scale of the box.
        f32 nearpl = 0.0f;         // "Back" plane.
        f32 farpl  = 0.1f;         // "Front" plane.
        
        shader_data.projection = Mat4::transpose(Mat4::orthographic(aspect, zoom, nearpl, farpl));
        shader_data.tex_unit = 0;

        // Then we pase our struct instance to the UBO and
        // tell OpenGL we're about to use it.
        glNamedBufferSubData(ubo, 0, sizeof(shader_data), &shader_data);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0u, ubo);

        // Tell OpenGL that we are going to use
        // our VAO and our shader in the next
        // draw call.
        glUseProgram(shader_flat_color);
        
        // Create the sampler array.
        i32 samplers[32];
        for (i32 i = 0; i < 32; ++i) {
            samplers[i] = i;
        }

        // Pass the sampler array to the gpu via single uniform. 
        GLint location = glGetUniformLocation(shader_flat_color, "u_samplers");
        glUniform1iv(location, 32, samplers);

        glBindVertexArray(vao);

        // The draw call.
        glDrawElements(GL_TRIANGLES, /* index_count */ 6, GL_UNSIGNED_INT, nullptr);

        // Swap the front and back buffers.
        os_swap_buffers();
    }

    app_done();
}
