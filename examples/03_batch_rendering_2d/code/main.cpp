#include "app.h"

u32 shader = 0;
u32 vao = 0u;
u32 vbo = 0u;
u32 ebo = 0u;
u32 ubo = 0u;

struct Sprite_Vertex {
    Vec4 pos;
    Vec2 uv;
    Vec4 tint;
    s32 tex_unit;
};

struct {
    static constexpr s32 verts_per_sprite = 4;
    static constexpr s32 elems_per_sprite = 6;
    static constexpr s32 max = 1; // Max sprites per drawcall.

    Sprite_Vertex data[max * verts_per_sprite] = {};
    s32 count = 0; // Current sprites.
} sprite_batch;

struct {
    Mat4 projection = Mat.Identity4;
} global_shader_data;

u32 white_tex = 0;

struct {
    static constexpr s32 max = 32;
    u32 data[max] = {};
    s32 count = 0;
} tex_array;

// ----------------------------------------------------------

fn create_tex(const IO_Image& image) -> u32;

fn frame_init() -> void;
fn draw_sprite(u32 tex, Vec3 pos, Vec3 rot, Vec3 scl, Vec4 tint) -> void;
fn frame_done() -> void;

fn flush() -> void;
fn give_tex_unit(u32 tex) -> s32;

fn frame_init() -> void {
    sprite_batch.count = 0;
    tex_array.count = 0;

    // Create the white_tex if wasn't.
    if (!white_tex) {
        white_tex = create_tex(*io_image_white());
    }

    // Assign white tex to index 0.
    give_tex_unit(white_tex);    
}

fn flush() -> void {
    frame_done();
    frame_init();
};

fn frame_done() -> void {

    for (s32 i = 0; i < tex_array.count; ++i) {
        glBindTextureUnit(i, tex_array.data[i]);
    }
    
    glUseProgram(shader);

    glNamedBufferSubData(ubo, 0, sizeof(global_shader_data), &global_shader_data);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0u, ubo);

    s32 samplers[32];
    for (s32 i = 0; i < 32; ++i) {
        samplers[i] = i;
    }
    GLint location = glGetUniformLocation(shader, "u_samplers");
    glUniform1iv(location, 32, samplers);

    glNamedBufferSubData(vbo, 0, sizeof(Sprite_Vertex) * sprite_batch.verts_per_sprite * sprite_batch.count, sprite_batch.data);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, /* index_count */ sprite_batch.elems_per_sprite * sprite_batch.count, GL_UNSIGNED_INT, nullptr);
}

fn give_tex_unit(u32 tex) -> s32 {

    // Invalid tex, use white.
    if (tex == 0) {
        tex = white_tex;
    }

    // Check if we filled all the tex slots in this frame.
    if (tex_array.count >= tex_array.max) {
        flush();
    }
    
    s32 tex_unit = 0;

    // Search the tex.
    for (; tex_unit < tex_array.count; ++tex_unit) {
        if (tex_array.data[tex_unit] == tex) {
            return tex_unit; // Found.
        }
    }
    // Not found. Add to last unit.
    tex_array.data[tex_unit] = tex;
    ++tex_array.count;

    return tex_unit;
}

fn draw_sprite(u32 tex, Vec4 tint, Vec3 pos, Vec3 rot = F32.Zero, Vec3 scl = F32.One) -> void {

    if (sprite_batch.count == sprite_batch.max) {
        flush();
    }
    
    for (s32 i = 0; i < sprite_batch.verts_per_sprite; ++i) {

        constexpr Vec4 sprite_verts[] = {
            {-0.5f, -0.5f, 0.0f, 1.0f},
            {+0.5f, -0.5f, 0.0f, 1.0f},
            {+0.5f, +0.5f, 0.0f, 1.0f},
            {-0.5f, +0.5f, 0.0f, 1.0f},
        };

        constexpr Vec2 sprite_uvs[] = {
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {1.0f, 1.0f},
            {0.0f, 1.0f},
        };

        Sprite_Vertex& vertex = sprite_batch.data[sprite_batch.count * sprite_batch.verts_per_sprite + i];
        vertex.pos      = sprite_verts[i] * Mat4::transpose(Mat4::transform(pos, rot, scl));
        vertex.uv       = sprite_uvs[i];

        vertex.tint     = tint;
        vertex.tex_unit = give_tex_unit(tex);
    }

    ++sprite_batch.count;
}

fn create_tex(const IO_Image& image) -> u32 {
    // Create the texture object.
    u32 tex = 0u;
    glCreateTextures(GL_TEXTURE_2D, 1, &tex);
    
    // Check if as RGB or RGBA.
    s32 storage_format = image.channels == sprite_batch.verts_per_sprite ? GL_RGBA8 
                       : image.channels == 3 ? GL_RGB8 : 0;

    // Reserve the storage.    
    glTextureStorage2D(tex, 1, storage_format, image.width, image.height);

    // Texture config.
    glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Check if as RGB or RGBA. (Again :S)
    s32 data_format = image.channels == sprite_batch.verts_per_sprite ? GL_RGBA 
                    : image.channels == 3 ? GL_RGB : 0;
    
    // Send the texture data to the gpu.
    glTextureSubImage2D(tex, 0, 0, 0, image.width, image.height, data_format, GL_UNSIGNED_BYTE, image.data);

    return tex;
}

fn main() -> s32 {

    app_init();

    // *** VAO: Vertex Array Object ***
    glCreateVertexArrays(1, &vao);

    // *** Vertex Attributes ***
    s32 offset = 0;

    constexpr Data_Type sprite_attrs[] = {
        Data_Type::Float4, // Position.
        Data_Type::Float2, // UVs.
        Data_Type::Float4, // Tint Color.
        Data_Type::Int,    // Texture Unit.
    };

    s32 index = 0;
    for (Data_Type attr: sprite_attrs) {
        checkf(os_is_gl_attribute(attr), "This type does not count as gl attribute!");
        glEnableVertexArrayAttrib(vao, index);
        if (is_integer_type(attr)) {
            glVertexArrayAttribIFormat(vao, index, get_count(attr), os_to_gl(attr), offset);
        } else {
            glVertexArrayAttribFormat(vao, index, get_count(attr), os_to_gl(attr), false, offset);
        }
        glVertexArrayAttribBinding(vao, index, /* vbo binding */ 0u);
        offset += get_size(attr);
        ++index;
    }

    // *** VBO: Vertex Buffer Object ***
    glCreateBuffers(1, &vbo);
    glNamedBufferData(vbo, sizeof(sprite_batch.data), nullptr, GL_DYNAMIC_DRAW);
    glVertexArrayVertexBuffer(vao, 0u, vbo, 0u, sizeof(Sprite_Vertex));
    
    // *** EBO: Element Buffer Object ***
    constexpr s32 max_elems = sprite_batch.max * sprite_batch.verts_per_sprite * sprite_batch.elems_per_sprite;

    u32 all_sprite_elems[max_elems];

    // Sprite 0:  0, 1, 2, 2, 3, 0
    // Sprite 1:  4, 5, 6, 6, 7, 4
    // Sprite 2:  8, 9, 10, 10, 11, 8
    // etc...

    for (s32 i = 0; i < sprite_batch.max; ++i) {
        s32 elem_offset  = i * sprite_batch.elems_per_sprite;
        s32 vert_offset  = i * sprite_batch.verts_per_sprite;
        // Triangle 1
        all_sprite_elems[elem_offset + 0] = vert_offset + 0;
        all_sprite_elems[elem_offset + 1] = vert_offset + 1;
        all_sprite_elems[elem_offset + 2] = vert_offset + 2;
        // Triangle 2
        all_sprite_elems[elem_offset + 3] = vert_offset + 2;
        all_sprite_elems[elem_offset + 4] = vert_offset + 3;
        all_sprite_elems[elem_offset + 5] = vert_offset + 0;
    }

    glCreateBuffers(1, &ebo);
    glNamedBufferData(ebo, sizeof(u32) * max_elems, all_sprite_elems, GL_STATIC_DRAW);
    glVertexArrayElementBuffer(vao, ebo);

    // *** Shader Program ***
    std::string source = os_read_entire_file("shader_sprite.glsl");
    shader = os_create_gl_program(source);

    // *** UBO: Uniform Buffer Object ***
    glCreateBuffers(1, &ubo);
    glNamedBufferData(ubo, sizeof(global_shader_data), nullptr, GL_DYNAMIC_DRAW);
    
    // Our quad position.
    Vec3 quad_pos;

    IO_Image image;
    io_image_load("hello_kitty.png", &image);
    u32 hello_kitty_tex = create_tex(image);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (app_running())
    {
        glClear(GL_COLOR_BUFFER_BIT);
        
        f32 aspect = 16.0f / 9.0f; 
        f32 zoom   = 9.0f;        
        f32 nearpl = 0.0f;       
        f32 farpl  = 0.1f;      
        
        global_shader_data.projection = Mat4::transpose(Mat4::orthographic(aspect, zoom, nearpl, farpl));

        constexpr s32 max = 900000;
        static Vec3* positions = new Vec3[max];
        static Vec4* colors = new Vec4[max];
        static s32 count = 0;
        static f32 timer = 0.0f;

        timer += os_delta_time();

        if (timer >= 0.001f) {
            if (count < max) {
                Vec2 pos = Vec2::random({ -16.f, 9.f }, { 16.f, -9.f });
                positions[count] = { pos.x, pos.y, 0.0f };
                colors[count] = Vec4::random(Color.Black, Color.White);
                count++;
            }
            timer = 0.0f;
        }

        frame_init();
        for (s32 i = 0; i < count; ++i) {
            draw_sprite(hello_kitty_tex, colors[i], positions[i]);
        }
        frame_done();

        imgui_frame_init();

        static f32 time = 0.0f;
        time += os_delta_time();

        ImGui::Begin("Debug");
        ImGui::Text("FPS: %f", os_av_fps());
        ImGui::Text("Hello Kitty Count: %i", count);
        ImGui::Text("Time: %f", time);
        ImGui::End();

        imgui_frame_done();


        os_swap_buffers();
    }

    app_done();
}