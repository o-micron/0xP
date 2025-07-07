#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 _VPosition;
layout(location = 1) in vec3 _VNormal;
layout(location = 2) in vec2 _VTexcoord;
layout(location = 3) in vec3 _ViewPos;

layout(location = 0) out vec4 FragColor;

void
main()
{
	FragColor = vec4(_VPosition, 1.0);
}
