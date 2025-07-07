#version 410 core

layout(location = 0) in vec2   _Texcoord;
layout(location = 1) in vec2   _Resolution;
layout(location = 2) in float  _Time;

uniform sampler2D channel0;
uniform sampler2D channel1;
uniform sampler2D channel2;
uniform sampler2D channel3;

layout(location = 0) out vec4 FragColorChannel0;

void
main()
{
    FragColorChannel0 = vec4(texture(channel0, _Texcoord).xyz, 1.0);
}