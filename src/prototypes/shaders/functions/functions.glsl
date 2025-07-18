#version 440 core

layout(location = 0) out vec4 FragColorChannel0;

#define MOD3 vec3(.1031, .11369, .13787)

float
rand(vec2 co)
{
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}
vec3
hash33(vec3 p3)
{
    p3 = fract(p3 * MOD3);
    p3 += dot(p3, p3.yxz + 19.19);
    return -1.0 + 2.0 * fract(vec3((p3.x + p3.y) * p3.z, (p3.x + p3.z) * p3.y, (p3.y + p3.z) * p3.x));
}
float
simplex_noise(vec3 p)
{
    const float K1 = 0.333333333;
    const float K2 = 0.166666667;
    vec3        i  = floor(p + (p.x + p.y + p.z) * K1);
    vec3        d0 = p - (i - (i.x + i.y + i.z) * K2);
    vec3        e  = step(vec3(0.0), d0 - d0.yzx);
    vec3        i1 = e * (1.0 - e.zxy);
    vec3        i2 = 1.0 - e.zxy * (1.0 - e);
    vec3        d1 = d0 - (i1 - 1.0 * K2);
    vec3        d2 = d0 - (i2 - 2.0 * K2);
    vec3        d3 = d0 - (1.0 - 3.0 * K2);
    vec4        h  = max(0.6 - vec4(dot(d0, d0), dot(d1, d1), dot(d2, d2), dot(d3, d3)), 0.0);
    vec4 n = h * h * h * h * vec4(dot(d0, hash33(i)), dot(d1, hash33(i + i1)), dot(d2, hash33(i + i2)), dot(d3, hash33(i + 1.0)));
    return dot(vec4(31.316), n);
}
vec4
mainImageElectrified(vec2 fragCoord, float time)
{
    vec4  fragColor;
    vec2  ouv = fragCoord;
    vec2  uv  = fragCoord - .5;
    float m   = 0.;
    for (int i = 0; i < 3; i++) {
        float f = floor(time * 20.) + float(i) * .5;
        float b = simplex_noise(vec3(f, uv.y * 1., 1.)) * .15 + simplex_noise(vec3(f, uv.y * 5., 5.)) * .1 +
                  simplex_noise(vec3(f, uv.y * 15., 10.)) * .02;

        float l = .000025 + (uv.y + .5) * .00001;
        m += .0005 / smoothstep(0., l * 25e3, abs(b - uv.x));
    }
    m         = min(m, 10.);
    fragColor = vec4(m);
    return fragColor;
}

void
main()
{
    FragColorChannel0 = mainImageElectrified(vec2(1,1), 1);
}