#include "../XPShaderTypes.h"

struct VSInput
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 texcoord : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float3 worldPos : WORLDPOS;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float3 viewPos : VIEWPOS;
};

cbuffer CameraConstantBuffer : register(b0)
{
    CameraMatrices cameraMatrices;
};

struct DrawConstants
{
    float4x4 modelMatrix;
};
ConstantBuffer<DrawConstants> myDrawConstants : register(b1, space0);

// Texture2D<float4> tex0 : register(t0);
// SamplerState samp0 : register(s0);

PSInput VSMain(VSInput input)
{
    PSInput result;

    float4 newPosition = float4(input.position.xyz, 1.0);
    result.worldPos = mul(newPosition, myDrawConstants.modelMatrix);
    result.position = mul(mul(newPosition, myDrawConstants.modelMatrix), cameraMatrices.viewProjectionMatrix);
    result.texcoord = input.texcoord.xy;
    result.normal   = input.normal.xyz;
    result.normal.z *= -1.0f;

    result.viewPos  = cameraMatrices.inverseViewMatrix[3].xyz;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return float4(input.worldPos, 1.0);
}
