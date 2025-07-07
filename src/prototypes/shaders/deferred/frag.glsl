#version 410 core

in vec3   _CamPosition;
in float  _CamNear;
in vec2   _Texcoord;
in float  _CamFar;
in float  _Time;

uniform sampler2D   channel0;                 // FragPosition
uniform sampler2D   channel1;                 // FragNormal
uniform sampler2D   channel2;                 // FragAlbedo
uniform sampler2D   channel3;                 // MetallicRoughnessAmbientBloomFragColor
uniform sampler2D   channel4;                 // UVObjectIDFragColor
uniform samplerCube texSky;
uniform samplerCube texIrradiance;
uniform samplerCube texPrefilter;
uniform sampler2D   texBrdf;
uniform sampler2D   texDepth;

out vec4 FragColorChannel0;
out vec4 FragColorChannel1;
out vec4 FragColorChannel2;
out vec4 FragColorChannel3;

#define PI        3.14159265359
#define LOD       4.0
#define EXPOSURE  3.0
#define GAMMA     2.2

const vec3 ColorIndices[27] = vec3[27](
        vec3(1.0, 1.0, 1.0),
        vec3(0.0, 0.0, 0.0),
        vec3(1.0, 0.0, 0.0),
        vec3(0.0, 1.0, 0.0),
        vec3(0.0, 0.0, 1.0),
        vec3(1.0, 1.0, 0.0),
        vec3(0.0, 1.0, 1.0),
        vec3(1.0, 0.0, 1.0),
        vec3(0.5, 0.5, 0.5),
        vec3(1.0, 0.5, 0.5),
        vec3(0.5, 1.0, 0.5),
        vec3(0.5, 0.5, 1.0),
        vec3(0.0, 0.5, 0.5),
        vec3(0.5, 0.0, 0.5),
        vec3(0.5, 0.5, 0.0),
        vec3(1.0, 1.0, 0.5),
        vec3(0.5, 1.0, 1.0),
        vec3(1.0, 0.5, 1.0),
        vec3(1.0, 0.0, 0.5),
        vec3(0.5, 1.0, 0.0),
        vec3(0.0, 0.5, 1.0),
        vec3(0.0, 1.0, 0.5),
        vec3(0.5, 0.0, 1.0),
        vec3(1.0, 0.5, 0.0),
        vec3(0.0, 0.0, 0.5),
        vec3(0.5, 0.0, 0.0),
        vec3(0.0, 0.5, 0.0)
);

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}  
// ----------------------------------------------------------------------------

float LinearizeDepth(in float depth) {
  float z = depth;
  return (2.0 * _CamNear) / (_CamFar + _CamNear - z * (_CamFar - _CamNear));
}

vec4 mainImageChannel0(vec3 position, float depth) {
    vec3 Albedo = texture(channel2, _Texcoord).rgb;
    vec4 MetallicRoughnessAmbientBloomFragColor = texture(channel3, _Texcoord);
    // vec4 UVObjectIDFragColor = texture(channel4, _Texcoord);
    float Metallic = MetallicRoughnessAmbientBloomFragColor.r;
    float Roughness = MetallicRoughnessAmbientBloomFragColor.g;
    if(Metallic == 0.0 && Roughness == 0.0) {
        return vec4(Albedo, 1.0);
    } else {
        vec3 Normal = texture(channel1, _Texcoord).rgb;

        vec3 V = normalize(_CamPosition - position);
        vec3 R = reflect(-V, Normal);

        vec3 F0             = vec3(0.04);
        F0                  = mix(F0, Albedo, Metallic);
        vec3 F              = fresnelSchlickRoughness(max(dot(Normal, V), 0.0), F0, Roughness);
        vec3 kS             = F;
        vec3 kD             = 1.0 - kS;
        kD                 *= 1.0 - Metallic;
        vec3 irradiance     = texture(texIrradiance, Normal).rgb;
        vec3 diffuse        = irradiance * Albedo;

        // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
        vec3 prefilteredColor   = textureLod(texPrefilter, R,  Roughness * LOD).rgb;
        vec2 brdf               = texture(texBrdf, vec2(max(dot(Normal, V), 0.0), Roughness)).rg;
        vec3 specular           = prefilteredColor * (F * brdf.x + brdf.y);
        vec3 ambient            = (kD * diffuse + specular); //* ao;

        vec3 color = ambient;

        // HDR tonemapping
        color = color / (color + vec3(1.0));
        // gamma correct
        color = pow(color, vec3(1.0/GAMMA));
        // ColorIndices[uint(mod(UVObjectIDFragColor.b, 9.0))]
        return vec4(color, 1.0);
    }
}

float rand(vec2 co){
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec4 mainImageChannel1(vec3 position, float depth) {
    vec4 UVObjectIDFragColor = texture(channel4, _Texcoord);
    return vec4(ColorIndices[int(mod(UVObjectIDFragColor.b, 27.0))], 1.0);
}

vec4 mainImageChannel2(vec3 position, float depth) {
    return vec4(0.0, 1.0, 0.0, 1.0);
}

vec4 mainImageChannel3(vec3 position, float depth) {
    return vec4(0.0, 0.0, 1.0, 1.0);
}

void
main()
{
    float depth = LinearizeDepth(texture(texDepth, _Texcoord).r);
    vec3 Position = texture(channel0, _Texcoord).rgb;
    if(depth < 0.99) {
        FragColorChannel0 = mainImageChannel0(Position, depth);
        FragColorChannel1 = mainImageChannel1(Position, depth);
        FragColorChannel2 = mainImageChannel2(Position, depth);
        FragColorChannel3 = mainImageChannel3(Position, depth);
    }
}