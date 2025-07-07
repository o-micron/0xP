#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform XPCameraUBO
{
	mat4 viewProjectionMatrix;
    mat4 inverseViewProjectionMatrix;
    mat4 inverseViewMatrix;
} cameraUbo;

layout(location = 0) in vec4 inLocalPositionU;
layout(location = 1) in vec4 inLocalNormalV;
layout(location = 2) in vec4 inWorldPositionU;
layout(location = 3) in vec4 inWorldNormalV;
layout(location = 4) in vec4 inAlbedo;
layout(location = 5) in float inMetallic;
layout(location = 6) in float inRoughness;
layout(location = 7) in float inAmbient;
layout(location = 8) in flat uint inFlags;
layout(location = 9) in flat uint inObjectId;

layout(location = 0) out vec4 outPositionU;
layout(location = 1) out vec4 outNormalV;
layout(location = 2) out vec4 outAlbedo;
layout(location = 3) out vec4 outMetallicRoughnessAmbientObjectId;

void
main()
{
    outPositionU                           = inWorldPositionU;
    outNormalV                             = inWorldNormalV;
    outAlbedo                              = inLocalPositionU;
    outMetallicRoughnessAmbientObjectId.x  = inMetallic;    
    outMetallicRoughnessAmbientObjectId.y  = inRoughness;
    outMetallicRoughnessAmbientObjectId.z  = inAmbient;
    outMetallicRoughnessAmbientObjectId.w  = inObjectId;
}
