#version 410 core

uniform sampler2D channel0;

layout(location = 0) out vec4 FragColorChannel0;

vec2
barrelDistortion(vec2 coord, float amt)
{
    vec2  cc   = coord - 0.5;
    float dist = dot(cc, cc);
    return coord + cc * dist * amt;
}
float
sat(float t)
{
    return clamp(t, 0.0, 1.0);
}
float
linterp(float t)
{
    return sat(1.0 - abs(2.0 * t - 1.0));
}
float
remap(float t, float a, float b)
{
    return sat((t - a) / (b - a));
}
vec3
spectrum_offset(float t)
{
    vec3  ret;
    float lo = step(t, 0.5);
    float hi = 1.0 - lo;
    float w  = linterp(remap(t, 1.0 / 6.0, 5.0 / 6.0));
    ret      = vec3(lo, 1.0, hi) * vec3(1.0 - w, w, 1.0 - w);

    return pow(ret, vec3(1.0 / 2.2));
}
vec4
barrelBlurChroma(float iterations, float barrelPower, sampler2D channel0, vec2 texcoord)
{
    float reci_num_iter_f = 1.0 / (3.0 + iterations);
    vec3  sumcol          = vec3(0.0);
    vec3  sumw            = vec3(0.0);
    for (float i = 0.0; i < (3.0 + iterations); i += 1.0) {
        float t = float(i) * reci_num_iter_f;
        vec3  w = spectrum_offset(t);
        sumw += w;
        sumcol += w * texture(channel0, barrelDistortion(texcoord, barrelPower * t)).rgb;
    }
    return vec4(sumcol.rgb / sumw, 1.0);
}

void
main()
{
    FragColorChannel0 = barrelBlurChroma(1, 1, channel0, vec2(1,1));
}