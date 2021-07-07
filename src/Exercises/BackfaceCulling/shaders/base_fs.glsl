#version 410
layout(std140) uniform Modifiers{
    float light_intensity;
    vec3 light_color;
};


layout(location=0) out vec4 vFragColor;
in vec3 vertex_color;
void main() {
    vFragColor.a=1.0;
    vFragColor.rgb=light_intensity*light_color*vertex_color.rgb;
}
