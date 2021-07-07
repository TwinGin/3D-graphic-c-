#version 410

layout(location=0) in  vec4 vertex_position;
layout(location=1) in vec2 vertex_texture_coordinates;
layout(location=2) in vec3 a_vertex_normal;
layout(std140) uniform Transformations{
    mat4 P;
    mat4 VM;
    mat3 N;
};
out vec2 v_vertex_texture_coordinates;
out vec3 vertex_normal_in_vs;
out vec3 vertex_position_in_vs;
void main() {
    v_vertex_texture_coordinates=vertex_texture_coordinates;
    vertex_position_in_vs=(VM * vertex_position).xyz;
    vertex_normal_in_vs = normalize(N*a_vertex_normal);
    gl_Position = P*VM*vertex_position;
}
