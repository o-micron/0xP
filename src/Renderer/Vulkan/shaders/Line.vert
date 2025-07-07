#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform XPCameraUBO
{
	mat4 viewProjectionMatrix;
    mat4 inverseViewProjectionMatrix;
    mat4 inverseViewMatrix;
} cameraUbo;

layout(push_constant) uniform PushConstant
{
	mat4 model;
} pc;

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inNormal;
layout(location = 2) in vec4 inTexcoord;

layout(location = 0) out vec3 _VPosition;
layout(location = 1) out vec3 _VNormal;
layout(location = 2) out vec2 _VTexcoord;
layout(location = 3) out vec3 _ViewPos;

void
main()
{
	gl_Position = cameraUbo.viewProjectionMatrix * pc.model * vec4(inPosition.xyz, 1.0);
    _VPosition  = (pc.model * vec4(inPosition.xyz, 1.0)).xyz;
    _VNormal    = (pc.model * vec4(inNormal.xyz, 0.0)).xyz;
    _VTexcoord  = vec2(inTexcoord.x, inTexcoord.y);
    _ViewPos    = cameraUbo.inverseViewMatrix[3].xyz;
}
