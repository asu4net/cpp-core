#include "app.h"

fn main() -> s32 {

    app_init();

    // 1. Here we added the UVs per each vertex.
    constexpr f32 QUAD_VTS[] = {
        -0.5f, -0.5f,  0.0f, 0.0f, 
        +0.5f, -0.5f,  1.0f, 0.0f,
        +0.5f, +0.5f,  1.0f, 1.0f,
        -0.5f, +0.5f,  0.0f, 1.0f,
    };
    
    // *** VAO: Vertex Array Object ***

    u32 vao = 0u;
    glCreateVertexArrays(1, &vao);

    // *** Vertex Attributes ***
    
    s32 offset = 0;

    // 0: POSITION --------------------------------------------
    
    struct {
        s32 Index = 0;
        s32 Count = 2;
        s32 Size  = sizeof(f32) * Count;
    } constexpr Pos;

    glEnableVertexArrayAttrib(vao, Pos.Index);
    glVertexArrayAttribFormat(vao, Pos.Index, Pos.Count, GL_FLOAT, false, offset);
    offset += Pos.Size;
    glVertexArrayAttribBinding(vao, Pos.Index, /* vbo binding */ 0u);

    // --------------------------------------------------------

    // 2. Then we create a vertex attribute for the UVs.

    // 1: UV --------------------------------------------------
    
    struct {
        s32 Index = 1;
        s32 Count = 2; // Two components: U, V.
        s32 Size  = sizeof(f32) * Count;
    } constexpr UV;

    glEnableVertexArrayAttrib(vao, UV.Index);
    glVertexArrayAttribFormat(vao, UV.Index, UV.Count, GL_FLOAT, false, offset);
    offset += UV.Size;
    glVertexArrayAttribBinding(vao, UV.Index, /* vbo binding */ 0u);

    // --------------------------------------------------------
    
    // *** VBO: Vertex Buffer Object ***
    
    u32 vbo = 0u;

    // 3. Add the UV attr size to the quad vertex size.
    constexpr u64 QUAD_VT_SIZE = Pos.Size + UV.Size;

    glCreateBuffers(1, &vbo);
    glNamedBufferData(vbo, sizeof(QUAD_VTS), QUAD_VTS, GL_STATIC_DRAW);
    glVertexArrayVertexBuffer(vao, 0u, vbo, 0u, QUAD_VT_SIZE);
    
    // *** EBO: Element Buffer Object ***

    u32 ebo = 0u;

    constexpr u32 QUAD_ELEMS[] = {
        0u, 1u, 2u, // Triangle 1
        2u, 3u, 0u, // Triangle 2
    };
    
    glCreateBuffers(1, &ebo);
    glNamedBufferData(ebo, sizeof(QUAD_ELEMS), QUAD_ELEMS, GL_STATIC_DRAW);
    glVertexArrayElementBuffer(vao, ebo);

    // *** Shader Program ***
    std::string source = os_read_entire_file("shader_texture.glsl");
    u32 shader_flat_color = os_create_gl_program(source);

    // *** UBO: Uniform Buffer Object.
    struct {
        Mat4 projection = Mat.Identity4;
        Mat4 transform = Mat.Identity4;
        s32 tex_unit = 0; // 4. Declare the texture unit as shader input data. 
    } shader_data;

    u32 ubo = 0u;
    glCreateBuffers(1, &ubo);
    glNamedBufferData(ubo, sizeof(shader_data), nullptr, GL_DYNAMIC_DRAW);
    
    // Our quad position.
    Vec3 quad_pos;

    // 4. Create the texture.

    // *** Texture ***
    
    // Handcraft a 4 pixel image.
    //u8 image_pixels[16] = {
    //    042u, 045u, 121u, 255u, // Some blue.
    //    232u, 005u, 103u, 255u, // Some red.
    //    255u, 142u, 104u, 255u, // Some orange.
    //    244u, 234u, 188u, 255u, // Some yellow.
    //};    

    IO_Image image;
    io_image_load("hello_kitty.png", &image);
    //image.data     = image_pixels;
    //image.channels = 4; // RGBA
    //image.width    = 2; // Pixel Width.
    //image.height   = 2; // Pixel Height.

    // Create the texture object.
    u32 tex = 0u;
    glCreateTextures(GL_TEXTURE_2D, 1, &tex);
    
    // Check if as RGB or RGBA.
    s32 storage_format = image.channels == 4 ? GL_RGBA8 
                       : image.channels == 3 ? GL_RGB8 : 0;

    // Reserve the storage.    
    glTextureStorage2D(tex, 1, storage_format, image.width, image.height);

    // Texture config.
    glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Check if as RGB or RGBA. (Again :S)
    s32 data_format = image.channels == 4 ? GL_RGBA 
                    : image.channels == 3 ? GL_RGB : 0;
    
    // Send the texture data to the gpu.
    glTextureSubImage2D(tex, 0, 0, 0, image.width, image.height, data_format, GL_UNSIGNED_BYTE, image.data);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Which means...
    //final.rgb = src.rgb * src.a + dst.rgb * (1 - src.a)
    //final.a   = src.a

    while (app_running())
    {
        glClear(GL_COLOR_BUFFER_BIT);
        
        // 5. Bind the texture to the unit 0.
        glBindTextureUnit(0, tex);

        shader_data.transform = Mat4::transpose(Mat4::transform(quad_pos, F32.Zero, Vec3(F32.One) * 2.0f));
        
        f32 aspect = 16.0f / 9.0f; 
        f32 zoom   = 3.0f;        
        f32 nearpl = 0.0f;       
        f32 farpl  = 0.1f;      
        
        shader_data.projection = Mat4::transpose(Mat4::orthographic(aspect, zoom, nearpl, farpl));
        shader_data.tex_unit = 0; // 6. Pass the texture unit to the shader.

        glNamedBufferSubData(ubo, 0, sizeof(shader_data), &shader_data);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0u, ubo);
        glUseProgram(shader_flat_color);
        
        // 7. Create the sampler array.
        s32 samplers[32];
        for (s32 i = 0; i < 32; ++i) {
            samplers[i] = i;
        }

        // 8. Pass the sampler array to the gpu via single uniform. 
        GLint location = glGetUniformLocation(shader_flat_color, "u_samplers");
        glUniform1iv(location, 32, samplers);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, /* index_count */ 6, GL_UNSIGNED_INT, nullptr);

        os_swap_buffers();
    }

    app_done();
}