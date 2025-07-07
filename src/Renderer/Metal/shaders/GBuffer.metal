#include <metal_stdlib>

using namespace metal;

#define XP_SHADER_VERTEX_SHADER_FUNCTION "vertexShader"
#define XP_SHADER_FRAGMENT_SHADER_FUNCTION "fragmentShader"
#define XP_SHADER_COMPUTE_SHADER_FUNCTION ""

#ifdef XP_RUNTIME_COMPILER
    #include "shaders/XPShaderTypes.h"
#else
    #include "../XPShaderTypes.h"
#endif

float grid(float3 pos, float3 axis, float size);

struct VertexOut
{
    float4  position [[position]];
    float4  localPositionU;
    float4  localNormalV;
    float4  worldPositionU;
    float4  worldNormalV;
    float4  albedo;
    float   metallic;
    float   roughness;
    float   ambient;
    uint    flags [[flat]];
    uint    objectId [[flat]];
};

struct FragmentOut
{
    float4 positionU [[color(XPMBufferIndexPositionU)]];
    float4 normalV [[color(XPMBufferIndexNormalV)]];
    float4 albedo [[color(XPMBufferIndexAlbedo)]];
    float4 metallicRoughnessAmbientObjectId [[color(XPMBufferIndexMetallicRoughnessAmbientObjectId)]];
};

vertex VertexOut
vertexShader(GBufferVertexIn in [[stage_in]],
             constant XPVertexShaderArgumentBuffer& argBuffer [[ buffer(GBufferVertexIn_VertexShaderArgumentBuffer0Index) ]])
{
    VertexOut out;
    
    out.position = argBuffer.frameDataPerObject.viewProjectionMatrix * argBuffer.frameDataPerObject.modelMatrix * vector_float4(in.position.x, in.position.y, in.position.z, 1.0);
    
    out.localPositionU.xyz = in.position.xyz;
    out.localPositionU.w = in.texcoord.x;
    out.localNormalV.xyz = in.normal.xyz;
    out.localNormalV.w = in.texcoord.y;
    
    out.worldPositionU.xyz = (argBuffer.frameDataPerObject.modelMatrix * vector_float4(in.position.x, in.position.y, in.position.z, 1.0)).xyz;
    out.worldPositionU.w = in.texcoord.x;
    out.worldNormalV.xyz = (argBuffer.frameDataPerObject.modelMatrix * vector_float4(in.normal.xyz, 0.0)).xyz;
    out.worldNormalV.w = in.texcoord.y;
    
    out.albedo = argBuffer.frameDataPerObject.albedo;
    out.metallic = argBuffer.frameDataPerObject.metallic;
    out.roughness = argBuffer.frameDataPerObject.roughness;
    out.ambient = 1.0;
    out.flags = argBuffer.frameDataPerObject.flags;
    out.objectId = argBuffer.frameDataPerObject.objectId + 1;
    
    return out;
}

fragment FragmentOut fragmentShader(VertexOut in [[stage_in]],
                               texture2d<float> albedoTexture [[ texture(XPGBufferIndexAlbedo) ]],
                               texture2d<float> normalTexture [[ texture(XPGBufferIndexNormal) ]],
                               texture2d<float> metallicTexture [[ texture(XPGBufferIndexMetallic) ]],
                               texture2d<float> roughnessTexture [[ texture(XPGBufferIndexRoughness) ]],
                               texture2d<float> ambientTexture [[ texture(XPGBufferIndexAmbient) ]])
{
    FragmentOut out;
    
    out.positionU                           = in.worldPositionU;
    out.normalV                             = in.worldNormalV;
    out.albedo                              = in.localPositionU;
    out.metallicRoughnessAmbientObjectId.x  = in.metallic;    
    out.metallicRoughnessAmbientObjectId.y  = in.roughness;
    out.metallicRoughnessAmbientObjectId.z  = in.ambient;
    out.metallicRoughnessAmbientObjectId.w  = in.objectId;

    return out;
}
