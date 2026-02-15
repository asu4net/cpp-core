#ifdef VERTEX_SHADER

layout(location = 0) in vec4 a_pos;
layout(location = 1) in vec2 a_uv; 
layout(location = 2) in vec4 a_tint;
layout(location = 3) in int a_tex_unit;

layout(std140, binding = 0) uniform Shader_Data {
  mat4 u_projection;
};

out vec2 v_uv;
out vec4 v_tint;
out flat int v_tex_unit; 

void main() {
    gl_Position = u_projection * vec4(a_pos.x, a_pos.y, a_pos.z, a_pos.w);

    v_uv = a_uv; 
    v_tint = a_tint; 
    v_tex_unit = a_tex_unit;
}
#endif

#ifdef FRAGMENT_SHADER

// 1. Retrieve the data from the vertex.
in vec2 v_uv;
in vec4 v_tint;
in flat int v_tex_unit; 

layout(location = 0) out vec4 o_col;

#define MAX_TEXTURES 32

uniform sampler2D u_samplers[MAX_TEXTURES];

void main() {
  o_col = texture(u_samplers[v_tex_unit], v_uv) * v_tint;
}

#endif