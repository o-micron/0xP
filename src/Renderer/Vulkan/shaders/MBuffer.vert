#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec2 outTexcoord;
layout(location = 1) out vec2 outDumm2;
layout(location = 2) out vec3 outDummy3;

void
main()
{
    const vec2 positions[6] = vec2[](
        vec2(-1, -1),
        vec2(+1, -1),
        vec2(-1, +1),
        vec2(+1, -1),
        vec2(+1, +1),
        vec2(-1, +1)
    );
    const vec2 coords[6] = vec2[](
        vec2(0, 0),
        vec2(1, 0),
        vec2(0, 1),
        vec2(1, 0),
        vec2(1, 1),
        vec2(0, 1)
    );
    outTexcoord = coords[gl_VertexIndex];
    outDumm2 = vec2(0, 0);
    outDummy3 = vec3(0, 0, 0);
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}
