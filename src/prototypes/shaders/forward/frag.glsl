#version 410 core

in vec2           _texcoord;

uniform sampler2D channel0;
layout(location = 0) out vec4 frag_color;

void
main()
{
    frag_color = vec4(texture(channel0, _texcoord).xyz, 1.0);
}