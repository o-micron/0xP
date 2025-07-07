#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inTexcoord;
layout(location = 1) in vec2 inDumm2;
layout(location = 2) in vec3 inDummy3;

layout(location = 0) out vec4 outFragColor;

layout(set = 0, binding = 0) uniform sampler2D PositionUTexture;
layout(set = 0, binding = 1) uniform sampler2D NormalVTexture;
layout(set = 0, binding = 2) uniform sampler2D AlbedoTexture;
layout(set = 0, binding = 3) uniform sampler2D MetallicRoughnessAmbientObjectIdTexture;

vec4 idToColorVibrant(uint id) {
  float h = fract(float(id + 200) * 0.10344827586);
  return clamp(vec4(
      sin(h * 6.283185 + 0.0) * 0.5 + 0.5,
      sin(h * 6.283185 + 2.094) * 0.5 + 0.5,
      sin(h * 6.283185 + 4.188) * 0.5 + 0.5,
      1.0
  ), vec4(0.0), vec4(1.0));
}

void
main()
{
  vec4 metallicRoughnessAmbientObjectIdSample = texture(MetallicRoughnessAmbientObjectIdTexture, inTexcoord);
  outFragColor = idToColorVibrant(uint(metallicRoughnessAmbientObjectIdSample.w));
}
