#version 410 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexcoord;

layout (std140) uniform Common
{
                            // base alignment   // aligned offset
    mat4 View;              // 16               // 0     (column 0)
                            // 16               // 16    (column 1)
                            // 16               // 32    (column 2)
                            // 16               // 48    (column 3)
    mat4 Projection;        // 16               // 64    (column 0)
                            // 16               // 80    (column 1)
                            // 16               // 96    (column 2)
                            // 16               // 112   (column 3)
    float Near;             // 4                // 128
    float Far;              // 4                // 132
    vec2 MouseXY;           // 8                // 136
    vec4 Resolution;        // 16               // 144
    vec4 LightColor;        // 16               // 160
};

layout(location = 0) out vec2   _Texcoord;
layout(location = 1) out vec2   _Resolution;
layout(location = 2) out float  _Time;

void
main()
{
    _Texcoord       = inTexcoord;
    _Resolution     = Resolution.xy;
    _Time           = Resolution.w;
    gl_Position     = vec4(inPosition.xy, 0.0, 1.0);

}