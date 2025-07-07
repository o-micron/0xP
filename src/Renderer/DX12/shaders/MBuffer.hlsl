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
    float4x4 viewProjectionMatrix;
    float4x4 inverseViewProjectionMatrix;
    float4x4 inverseViewMatrix;
};

cbuffer MeshConstantBuffer : register(b1)
{
    float4x4 modelMatrix;
};

PSInput VSMain(VSInput input)
{
    PSInput result;

    float4 newPosition = mul(input.position, modelMatrix);

    result.worldPos = newPosition.xyz;

    newPosition = mul(newPosition, viewProjectionMatrix);

    result.position = newPosition;
    result.texcoord = input.texcoord.xy;
    result.normal   = input.normal.xyz;
    result.normal.z *= -1.0f;

    result.viewPos  = inverseViewMatrix[3].xyz;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return float4(input.worldPos, 1.0);
}
