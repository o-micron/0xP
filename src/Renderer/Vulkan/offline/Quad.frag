#version 450
layout(binding = 0) uniform sampler2D Color;
layout(location = 0) in vec2 inUV;
layout(location = 0) out vec4 FragColor;
void main() {
    FragColor = texture(Color, inUV);
}