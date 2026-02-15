#ifdef VERTEX_SHADER

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_uv; // 1. Get the UV attribute.

layout(std140, binding = 0) uniform Shader_Data {
  mat4 u_projection;
  mat4 u_transform;
  int u_tex_unit; // 2. Get the texture unit.
};

// 3. Pass the data to the fragment.
out vec2 v_uv;
out flat int v_tex_unit; 

void main() {
    gl_Position = u_projection * u_transform * vec4(a_pos.x, a_pos.y, 0.0, 1.0);
    // 4. Set the data.
    v_uv = a_uv; 
    v_tex_unit = u_tex_unit;
}
#endif

#ifdef FRAGMENT_SHADER

// 1. Retrieve the data from the vertex.
in vec2 v_uv;
in flat int v_tex_unit; 

layout(location = 0) out vec4 o_col;

#define MAX_TEXTURES 32

// 3. Alse retrieve the texture samplers from out program.
uniform sampler2D u_samplers[MAX_TEXTURES];

void main() {
  // 4. Get the current pixel color, associated
  // with our vertex, using the uv coordinates
  // and the texture unit.
  o_col = texture(u_samplers[v_tex_unit], v_uv);
}

#endif