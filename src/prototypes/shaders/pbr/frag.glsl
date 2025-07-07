#version 410 core

in vec4        _Position;
in vec3        _Normal;
in vec2        _Texcoord;
in float       _Metallic;
in float       _Roughness;
in vec3        _BaseColor;
flat in float  _ObjectId;

#pragma gui
uniform sampler2D   texAlbedo;
#pragma gui
uniform sampler2D   texMetallic;
#pragma gui
uniform sampler2D   texNormal;
#pragma gui
uniform sampler2D   texRoughness;

out vec4 PositionFragColor;
out vec4 NormalFragColor;
out vec4 AlbedoFragColor;
out vec4 MetallicRoughnessAmbientBloomFragColor;
out vec4 UVObjectIDFragColor;

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(texNormal, _Texcoord).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(_Position.xyz);
    vec3 Q2  = dFdy(_Position.xyz);
    vec2 st1 = dFdx(_Texcoord);
    vec2 st2 = dFdy(_Texcoord);

    vec3 N   = normalize(_Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void
main()
{
    PositionFragColor   = vec4(_Position.xyz, 1.0);
    NormalFragColor     = vec4(_Normal, 1.0);
    AlbedoFragColor     = vec4(pow(texture(texAlbedo, _Texcoord).rgb * _BaseColor, vec3(2.2)), 1.0);
    UVObjectIDFragColor = vec4(_Texcoord, _ObjectId, 1.0);
    // will it fetch ? :D
    MetallicRoughnessAmbientBloomFragColor.ba = vec2(1.0, 1.0);
    if (_Metallic == 0.0) {
        MetallicRoughnessAmbientBloomFragColor.r = pow(texture(texMetallic, _Texcoord).r, 2.2);
    } else {
        MetallicRoughnessAmbientBloomFragColor.r = pow(_Metallic, 2.2);
    }
    if (_Roughness == 0.0) {
        MetallicRoughnessAmbientBloomFragColor.g = pow(texture(texRoughness, _Texcoord).g, 2.2);
    } else {
        MetallicRoughnessAmbientBloomFragColor.g = pow(_Roughness, 2.2);
    }
}