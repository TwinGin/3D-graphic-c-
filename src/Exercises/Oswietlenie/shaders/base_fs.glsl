#version 410

layout(location=0) out vec4 vFragColor;
in vec2 v_vertex_texture_coordinates;
in vec3 vertex_normal_in_vs;
in vec3 vertex_position_in_vs;
uniform sampler2D diffuse_map;

layout(std140) uniform Light{
    vec3 position_in_vs;
    vec3 color;
    vec3 a;
    vec3 ambient;
} light;
void main() {
    vFragColor= texture(diffuse_map,v_vertex_texture_coordinates);
    vec3 normal = normalize(vertex_normal_in_vs);
    if(!gl_FrontFacing){
        normal = -normal;
    }
    vec3 light_vector = light.position_in_vs - vertex_position_in_vs;
    vec4 diffuse_color = texture(diffuse_map,v_vertex_texture_coordinates);
    float r =length(light_vector);
    light_vector/=r;
    float attenaution = 1.0/(light.a[0] + light.a[1]*r+light.a[2]*r*r);
    float light_in = max(0.0,dot(normal,light_vector))*attenaution;
    vFragColor.a = diffuse_color.a;
    vFragColor.rgb = diffuse_color.rgb * light.ambient.rgb;
    vFragColor.rgb += light_in * diffuse_color.rgb*light.color;
}
