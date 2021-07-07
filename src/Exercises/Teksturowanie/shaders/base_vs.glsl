#version 410

layout(location=0) in  vec4 vertex_position;
layout(location=1) in vec2 vertex_texture_coordinates;

layout(std140) uniform Transformations{
    mat4 P;
    mat4 V;
};
out vec2 v_vertex_texture_coordinates;
void main() {
    v_vertex_texture_coordinates=vertex_texture_coordinates;
    gl_Position = P*V*vertex_position;
}
