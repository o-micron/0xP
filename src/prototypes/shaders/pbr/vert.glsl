#version 410 core

layout(location = 0) in vec4 inPositionU;
layout(location = 1) in vec4 inNormalV;

uniform mat4 Model;
uniform uint ObjectId;
uniform vec3 BaseColor;
uniform float Metallic;
uniform float Roughness; 

layout(location = 0) out vec4       _Position;
layout(location = 1) out vec3       _Normal;
layout(location = 2) out vec2       _Texcoord;
layout(location = 3) out float      _Metallic;
layout(location = 4) out float      _Roughness;
layout(location = 5) out vec3       _BaseColor;
layout(location = 6) flat out float _ObjectId;

void
main()
{
    _Position           = Model * vec4(inPositionU.xyz, 1.0);
    _Normal             = mat3(Model) * inNormalV.xyz;
    _ObjectId           = float(ObjectId);
    _Texcoord           = vec2(inPositionU.w, inNormalV.w);
    _Metallic           = Metallic;
    _Roughness          = Roughness;
    _BaseColor          = BaseColor;
    gl_Position         = Projection * View * _Position;
}