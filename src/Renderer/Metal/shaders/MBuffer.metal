#include <metal_stdlib>

using namespace metal;

#ifdef XP_RUNTIME_COMPILER
    #include "shaders/XPShaderTypes.h"
#else
    #include "../XPShaderTypes.h"
#endif

struct VertexOut
{
    float4  position [[position]];
    float3  viewPos [[flat]];
    float2  texcoord;
};

vertex VertexOut
vertexShader(MBufferVertexIn in [[stage_in]],
             constant XPVertexShaderArgumentBuffer& argBuffer [[ buffer(MBufferVertexIn_VertexShaderArgumentBuffer0Index) ]])
{
    VertexOut out;

    out.position  = vector_float4(in.position.xyz, 1.0);
    out.texcoord  = in.texcoord.xy;
    out.viewPos   = argBuffer.frameDataPerObject.inverseViewMatrix[3].xyz;

    return out;
}

float4 idToColorVibrant(uint id) {
  float h = fract(float(id + 200) * 0.10344827586);
  return clamp(float4(
      sin(h * 6.283185 + 0.0) * 0.5 + 0.5,
      sin(h * 6.283185 + 2.094) * 0.5 + 0.5,
      sin(h * 6.283185 + 4.188) * 0.5 + 0.5,
      1.0
  ), 0.0, 1.0);
}

fragment float4 fragmentShader(VertexOut in [[stage_in]],
                               texture2d<float> positionUTexture [[texture(XPMBufferIndexPositionU)]],
                               texture2d<float> normalVTexture [[texture(XPMBufferIndexNormalV)]],
                               texture2d<float> albedoTexture [[texture(XPMBufferIndexAlbedo)]],
                               texture2d<float> metallicRoughnessAmbientObjectIdTexture [[texture(XPMBufferIndexMetallicRoughnessAmbientObjectId)]])
{
  constexpr sampler textureSampler (mag_filter::linear, min_filter::linear);
  const float4 metallicRoughnessAmbientObjectIdSample = metallicRoughnessAmbientObjectIdTexture.sample(textureSampler, in.texcoord);
  float4 color = idToColorVibrant(uint(metallicRoughnessAmbientObjectIdSample.w));
  return color;
}
