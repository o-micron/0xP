#include <metal_stdlib>

using namespace metal;

#ifdef XP_RUNTIME_COMPILER
    #include "shaders/XPShaderTypes.h"
#else
    #include "../XPShaderTypes.h"
#endif

struct VertexOut
{
    float4 position [[position]];
};

struct FragmentOut
{
    float4 positionU [[color(XPMBufferIndexPositionU)]];
    float4 normalV [[color(XPMBufferIndexNormalV)]];
    float4 albedo [[color(XPMBufferIndexAlbedo)]];
    float4 metallicRoughnessAmbientObjectId [[color(XPMBufferIndexMetallicRoughnessAmbientObjectId)]];
};

vertex VertexOut
vertexShader(LineVertexIn in [[stage_in]],
             constant XPVertexShaderArgumentBuffer& argBuffer [[ buffer(LineVertexIn_VertexShaderArgumentBuffer0Index) ]])
{
    VertexOut out;
    out.position = argBuffer.frameDataPerObject.viewProjectionMatrix * vector_float4(in.position.x, in.position.y, in.position.z, 1.0);
    return out;
}

fragment FragmentOut fragmentShader(VertexOut in [[stage_in]])
{
    FragmentOut out;
    out.positionU = float4(1.0, 0.0, 0.0, 1.0);
    out.normalV = float4(1.0, 0.0, 0.0, 1.0);
    out.albedo = float4(0.0, 1.0, 1.0, 1.0);
    out.metallicRoughnessAmbientObjectId = float4(0.0, 0.0, 0.0, -1.0);
    return out;
}
