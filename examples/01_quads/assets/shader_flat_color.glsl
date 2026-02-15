#ifdef VERTEX_SHADER
// The Position Vertex Attribute.
layout(location = 0) in vec2 a_pos;

// The Uniform Buffer Object data.
layout(std140, binding = 0) uniform Shader_Data {
  mat4 u_projection;
  mat4 u_transform;
};

void main() {
    // We pass it to the gl_Position vertex shader output.
    gl_Position = u_projection * u_transform * vec4(a_pos.x, a_pos.y, 0.0, 1.0);
}

#endif

#ifdef FRAGMENT_SHADER
// The color fragment shader output.
layout(location = 0) out vec4 o_col;

void main() {
  // We just hardcode white here.
  o_col = vec4(1.0, 1.0, 1.0, 1.0);
}
#endif