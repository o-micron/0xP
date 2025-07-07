#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform XPCameraUBO
{
	mat4 viewProjectionMatrix;
    mat4 inverseViewProjectionMatrix;
    mat4 inverseViewMatrix;
} cameraUbo;

layout(push_constant, std430) uniform PushConstant
{
	mat4 model;
    uint objectId;
    float unused00;
    float unused01;
    float unused10;
    vec4 unused1;
    vec4 unused2;
    vec4 unused3;
} pc;

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inNormal;
layout(location = 2) in vec4 inTexcoord;

layout(location = 0) out vec4 outLocalPositionU;
layout(location = 1) out vec4 outLocalNormalV;
layout(location = 2) out vec4 outWorldPositionU;
layout(location = 3) out vec4 outWorldNormalV;
layout(location = 4) out vec4 outAlbedo;
layout(location = 5) out float outMetallic;
layout(location = 6) out float outRoughness;
layout(location = 7) out float outAmbient;
layout(location = 8) out flat uint outFlags;
layout(location = 9) out flat uint outObjectId;

void
main()
{
	gl_Position = cameraUbo.viewProjectionMatrix * pc.model * vec4(inPosition.xyz, 1.0);

    outLocalPositionU = vec4(inPosition.xyz, inTexcoord.x);
    outLocalNormalV = vec4(inNormal.xyz, inTexcoord.y);
    
    outWorldPositionU.xyz = (pc.model * vec4(inPosition.xyz, 1.0)).xyz;
    outWorldPositionU.w = inTexcoord.x;
    outWorldNormalV.xyz = (pc.model * vec4(inNormal.xyz, 0.0)).xyz;
    outWorldNormalV.w = inTexcoord.y;

    outAlbedo = vec4(1.0, 1.0, 1.0, 1.0);
    outMetallic = 0.001;
    outRoughness = 0.6;
    outAmbient = 1.0;
    outFlags = 0;
    outObjectId = pc.objectId + 1;

    // _VPosition  = (pc.model * vec4(inPosition.xyz, 1.0)).xyz;
    // _VNormal    = (pc.model * vec4(inNormal.xyz, 0.0)).xyz;
    // _VTexcoord  = vec2(inTexcoord.x, inTexcoord.y);
    // _ViewPos    = cameraUbo.inverseViewMatrix[3].xyz;
}
