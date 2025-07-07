#version 410 core

uniform sampler2D channel0;

layout(location = 0) out vec4 FragColorChannel0;

vec4
objectEdgeFilter(float steps, float threshold, vec3 edgeColor, sampler2D channel, sampler2D channel0, vec2 texcoord, vec2 resolution)
{
    vec3 s  = vec3(0.0, 0.0, 0.0);
    vec3 p0 = texelFetch(channel, ivec2(texcoord * resolution), 0).rgb;
    for (float i = -steps; i <= steps; i++) {
        for (float j = -steps; j <= steps; j++) { s += texelFetch(channel, ivec2(texcoord * resolution) + ivec2(i, j), 0).rgb; }
    }
    float count = (steps * 2.0 + 1.0) * (steps * 2.0 + 1.0);
    if (abs((p0.r * count) - s.r) < threshold && abs((p0.g * count) - s.g) < threshold && abs((p0.b * count) - s.b) < threshold)
        return vec4(texture(channel0, texcoord).rgb, 1.0);
    else
        return vec4(edgeColor, 1.0);
}

void
main()
{
    FragColorChannel0 = objectEdgeFilter(1, 1, vec3(1,1,1), channel0, channel0, vec2(1,1), vec2(1,1));
}