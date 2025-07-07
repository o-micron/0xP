#pragma once

#include "Maths.h"

struct Texture2D
{
    Texture2D()
      : u8data(nullptr)
      , width(0)
      , height(0)
      , channels(0)
      , isHDR(false)
    {
    }
    ~Texture2D()
    {
        if (u8data) { u8data = nullptr; }
        width    = 0;
        height   = 0;
        channels = 0;
        isHDR    = false;
    }
    union
    {
        struct
        {
            uint8_t* u8data;
        };
        struct
        {
            u8vec2* u8rgpixels;
        };
        struct
        {
            u8vec3* u8rgbpixels;
        };
        struct
        {
            u8vec4* u8rgbapixels;
        };
        struct
        {
            float* f32data;
        };
        struct
        {
            fvec2* f32rgpixels;
        };
        struct
        {
            fvec3* f32rgbpixels;
        };
        struct
        {
            fvec4* f32rgbapixels;
        };
    };

    int  width;
    int  height;
    int  channels;
    bool isHDR;
};
template<typename T>
inline Vec4<float>
sampleTextureLinear(const Texture2D& texture, float u, float v, bool repeat, bool applyGammeCorrect)
{
    // Handle UV wrapping (can be changed to CLAMP or other modes)
    if (repeat) {
        u = u - floor(u);
        v = v - floor(v);
    }

    u = glm::fclamp(u, 0.0f, 1.0f);
    v = glm::fclamp(v, 0.0f, 1.0f);

    // Convert UV to texture coordinates
    float x = u * static_cast<float>(texture.width) - 0.5f; // Adjust for pixel centers
    float y = v * static_cast<float>(texture.height) - 0.5f;

    x = glm::fclamp(x, 0.0f, static_cast<float>(texture.width));
    y = glm::fclamp(y, 0.0f, static_cast<float>(texture.height));

    // Get integer coordinates of surrounding pixels
    int x0 = (int)floorf(x);
    int y0 = (int)floorf(y);
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    // Clamp coordinates to texture boundaries
    x0 = glm::clamp(x0, 0, texture.width - 1);
    x1 = glm::clamp(x1, 0, texture.width - 1);
    y0 = glm::clamp(y0, 0, texture.height - 1);
    y1 = glm::clamp(y1, 0, texture.height - 1);

    // Calculate fractional parts
    float x_frac = x - static_cast<float>(x0);
    float y_frac = y - static_cast<float>(y0);

    // Sample four texels
    const Vec4<uint8_t>* row0 = texture.u8rgbapixels + y0 * texture.width;
    const Vec4<uint8_t>* row1 = texture.u8rgbapixels + y1 * texture.width;

    auto c00 = Vec4<float>{ static_cast<float>(row0[x0].x) / 255.0f,
                            static_cast<float>(row0[x0].y) / 255.0f,
                            static_cast<float>(row0[x0].z) / 255.0f,
                            static_cast<float>(row0[x0].w) / 255.0f };
    auto c10 = Vec4<float>{ static_cast<float>(row0[x1].x) / 255.0f,
                            static_cast<float>(row0[x1].y) / 255.0f,
                            static_cast<float>(row0[x1].z) / 255.0f,
                            static_cast<float>(row0[x1].w) / 255.0f };
    auto c01 = Vec4<float>{ static_cast<float>(row1[x0].x) / 255.0f,
                            static_cast<float>(row1[x0].y) / 255.0f,
                            static_cast<float>(row1[x0].z) / 255.0f,
                            static_cast<float>(row1[x0].w) / 255.0f };
    auto c11 = Vec4<float>{ static_cast<float>(row1[x1].x) / 255.0f,
                            static_cast<float>(row1[x1].y) / 255.0f,
                            static_cast<float>(row1[x1].z) / 255.0f,
                            static_cast<float>(row1[x1].w) / 255.0f };

    // Bilinear interpolation using macros
#define LERP(a, b, t) ((a) + ((b) - (a)) * (t))
    Vec4<float> result;
    result.x = LERP(LERP(c00.x, c10.x, x_frac), LERP(c01.x, c11.x, x_frac), y_frac);
    result.y = LERP(LERP(c00.y, c10.y, x_frac), LERP(c01.y, c11.y, x_frac), y_frac);
    result.z = LERP(LERP(c00.z, c10.z, x_frac), LERP(c01.z, c11.z, x_frac), y_frac);
    result.w = LERP(LERP(c00.w, c10.w, x_frac), LERP(c01.w, c11.w, x_frac), y_frac);
#undef LERP

    if (applyGammeCorrect) {
        result.x = std::pow(result.x, 2.2f);
        result.y = std::pow(result.y, 2.2f);
        result.z = std::pow(result.z, 2.2f);
    }

    return result;
}
