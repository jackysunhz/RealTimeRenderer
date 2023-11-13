#version 450

//layout (location = 0) in vec3 outColor;

layout (location = 0) out vec4 fragColor;

layout(push_constant) uniform Push{
    vec2 offset;
    vec3 color;
} push;


void main(){
    fragColor = vec4(push.color, 1.0);
}