#version 410 core

uniform sampler2D channel0;

layout(location = 0) out vec4 FragColorChannel0;

float
convolve(mat3 kernel, mat3 image)
{
    float result = 0.0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result += kernel[i][j] * image[i][j];
        }
    }
    return result;
}
float
convolveComponent(mat3 kernelX, mat3 kernelY, mat3 image)
{
    vec2 result;
    result.x = convolve(kernelX, image);
    result.y = convolve(kernelY, image);
    return clamp(length(result), 0.0, 255.0);
}
vec4
colorEdge(float stepx, float stepy, vec2 center, mat3 kernelX, mat3 kernelY, sampler2D channel0)
{
    vec4 colors[9];
    colors[0] = texture(channel0, center + vec2(-stepx, stepy));
    colors[1] = texture(channel0, center + vec2(0, stepy));
    colors[2] = texture(channel0, center + vec2(stepx, stepy));
    colors[3] = texture(channel0, center + vec2(-stepx, 0));
    colors[4] = texture(channel0, center);
    colors[5] = texture(channel0, center + vec2(stepx, 0));
    colors[6] = texture(channel0, center + vec2(-stepx, -stepy));
    colors[7] = texture(channel0, center + vec2(0, -stepy));
    colors[8] = texture(channel0, center + vec2(stepx, -stepy));

    mat3 imageR, imageG, imageB, imageA;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            imageR[i][j] = colors[i * 3 + j].r;
            imageG[i][j] = colors[i * 3 + j].g;
            imageB[i][j] = colors[i * 3 + j].b;
            imageA[i][j] = colors[i * 3 + j].a;
        }
    }

    vec4 color;
    color.r = convolveComponent(kernelX, kernelY, imageR);
    color.g = convolveComponent(kernelX, kernelY, imageG);
    color.b = convolveComponent(kernelX, kernelY, imageB);
    color.a = convolveComponent(kernelX, kernelY, imageA);

    return color;
}
vec4
edge(float stepx, float stepy, vec2 center, mat3 kernelX, mat3 kernelY, sampler2D channel0)
{
    // get samples around pixel
    mat3 image = mat3(length(texture(channel0, center + vec2(-stepx, stepy)).rgb),
                      length(texture(channel0, center + vec2(0, stepy)).rgb),
                      length(texture(channel0, center + vec2(stepx, stepy)).rgb),
                      length(texture(channel0, center + vec2(-stepx, 0)).rgb),
                      length(texture(channel0, center).rgb),
                      length(texture(channel0, center + vec2(stepx, 0)).rgb),
                      length(texture(channel0, center + vec2(-stepx, -stepy)).rgb),
                      length(texture(channel0, center + vec2(0, -stepy)).rgb),
                      length(texture(channel0, center + vec2(stepx, -stepy)).rgb));
    vec2 result;
    result.x = convolve(kernelX, image);
    result.y = convolve(kernelY, image);

    float color = clamp(length(result), 0.0, 255.0);
    return vec4(color);
}
vec4
trueColorEdge(float stepx, float stepy, vec2 center, mat3 kernelX, mat3 kernelY, sampler2D channel0)
{
    vec4 edgeVal = edge(stepx, stepy, center, kernelX, kernelY, channel0);
    return edgeVal * texture(channel0, center);
}
vec4
edgeFilter(float stepx, float stepy, sampler2D channel0, vec2 texcoord)
{
    const mat3 sobelKernelX = mat3(1.0, 0.0, -1.0, 2.0, 0.0, -2.0, 1.0, 0.0, -1.0);
    const mat3 sobelKernelY = mat3(-1.0, -2.0, -1.0, 0.0, 0.0, 0.0, 1.0, 2.0, 1.0);
    return edge(stepx, stepy, texcoord, sobelKernelX, sobelKernelY, channel0);
}

void
main()
{
    FragColorChannel0 = edgeFilter(1, 1, channel0, vec2(1,1));
}