/// --------------------------------------------------------------------------------------
/// Copyright 2025 Omar Sherif Fathy
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
/// --------------------------------------------------------------------------------------

#pragma once

#include <Renderer/SW/XPSWMaths.h>
#include <Renderer/SW/XPSWThirdParty.h>

#if defined(_WIN32) || defined(_WIN64)
    #pragma warning(disable : 4996)
    #define _CRT_SECURE_NO_DEPRECATE
#endif

#include <filesystem>
#include <vector>

struct XPSWTexture2D
{
    XPSWTexture2D()
      : u8data(nullptr)
      , width(0)
      , height(0)
      , channels(0)
      , isHDR(false)
    {
    }
    ~XPSWTexture2D()
    {
        if (u8data) {
            stbi_image_free(u8data);
            u8data = nullptr;
        }
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
            XPVec2<uint8_t>* u8rgpixels;
        };
        struct
        {
            XPVec2<uint8_t>* u8rgbpixels;
        };
        struct
        {
            XPVec4<uint8_t>* u8rgbapixels;
        };
        struct
        {
            float* f32data;
        };
        struct
        {
            XPVec2<float>* f32rgpixels;
        };
        struct
        {
            XPVec3<float>* f32rgbpixels;
        };
        struct
        {
            XPVec4<float>* f32rgbapixels;
        };
    };
    std::string path;
    int         width;
    int         height;
    int         channels;
    bool        isHDR;
};

template<typename T>
inline XPVec4<float>
sampleTextureLinear(const XPSWTexture2D& texture, float u, float v, bool repeat, bool applyGammeCorrect)
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
    const XPVec4<uint8_t>* row0 = texture.u8rgbapixels + y0 * texture.width;
    const XPVec4<uint8_t>* row1 = texture.u8rgbapixels + y1 * texture.width;

    auto c00 = XPVec4<float>{ static_cast<float>(row0[x0].x) / 255.0f,
                              static_cast<float>(row0[x0].y) / 255.0f,
                              static_cast<float>(row0[x0].z) / 255.0f,
                              static_cast<float>(row0[x0].w) / 255.0f };
    auto c10 = XPVec4<float>{ static_cast<float>(row0[x1].x) / 255.0f,
                              static_cast<float>(row0[x1].y) / 255.0f,
                              static_cast<float>(row0[x1].z) / 255.0f,
                              static_cast<float>(row0[x1].w) / 255.0f };
    auto c01 = XPVec4<float>{ static_cast<float>(row1[x0].x) / 255.0f,
                              static_cast<float>(row1[x0].y) / 255.0f,
                              static_cast<float>(row1[x0].z) / 255.0f,
                              static_cast<float>(row1[x0].w) / 255.0f };
    auto c11 = XPVec4<float>{ static_cast<float>(row1[x1].x) / 255.0f,
                              static_cast<float>(row1[x1].y) / 255.0f,
                              static_cast<float>(row1[x1].z) / 255.0f,
                              static_cast<float>(row1[x1].w) / 255.0f };

    // Bilinear interpolation using macros
#define LERP(a, b, t) ((a) + ((b) - (a)) * (t))
    XPVec4<float> result;
    result.x = LERP(LERP(c00.x, c10.x, x_frac), LERP(c01.x, c11.x, x_frac), y_frac);
    result.y = LERP(LERP(c00.y, c10.y, x_frac), LERP(c01.y, c11.y, x_frac), y_frac);
    result.z = LERP(LERP(c00.z, c10.z, x_frac), LERP(c01.z, c11.z, x_frac), y_frac);
    result.w = LERP(LERP(c00.w, c10.w, x_frac), LERP(c01.w, c11.w, x_frac), y_frac);
#undef LERP

    if (applyGammeCorrect) {
        result.x = std::pow(result.x, 1.0f / 2.2f);
        result.y = std::pow(result.y, 1.0f / 2.2f);
        result.z = std::pow(result.z, 1.0f / 2.2f);
    }

    return result;
}

inline void
saveToEXR(const XPSWTexture2D& texture, const std::string& filename)
{
#ifndef __EMSCRIPTEN__
    // Prepare EXR image
    EXRHeader header;
    InitEXRHeader(&header);

    EXRImage image;
    InitEXRImage(&image);

    image.num_channels = texture.channels;
    image.width        = texture.width;
    image.height       = texture.height;

    // Split the color buffer into separate channels
    std::vector<float> rChannel(texture.width * texture.height);
    std::vector<float> gChannel(texture.width * texture.height);
    std::vector<float> bChannel(texture.width * texture.height);
    std::vector<float> aChannel(texture.width * texture.height);

    for (int i = 0; i < texture.width * texture.height; ++i) {
        if (texture.channels >= 1) {
            if (texture.isHDR) {
                rChannel[i] = texture.f32data[texture.channels * i + 0]; // R
            } else {
                rChannel[i] = texture.u8data[texture.channels * i + 0]; // R
            }
        }
        if (texture.channels >= 2) {
            if (texture.isHDR) {
                gChannel[i] = texture.f32data[texture.channels * i + 1]; // G
            } else {
                gChannel[i] = texture.u8data[texture.channels * i + 1]; // G
            }
        }
        if (texture.channels >= 3) {
            if (texture.isHDR) {
                bChannel[i] = texture.f32data[texture.channels * i + 2]; // B
            } else {
                bChannel[i] = texture.u8data[texture.channels * i + 2]; // B
            }
        }
        if (texture.channels >= 4) {
            if (texture.isHDR) {
                aChannel[i] = texture.f32data[texture.channels * i + 3]; // A
            } else {
                aChannel[i] = texture.u8data[texture.channels * i + 3]; // A
            }
        }
    }

    // Pointers to each channel
    std::vector<float*> image_ptr(texture.channels);
    if (texture.channels >= 1) {
        image_ptr[0] = rChannel.data(); // R
    }
    if (texture.channels >= 2) {
        image_ptr[1] = gChannel.data(); // G
    }
    if (texture.channels >= 3) {
        image_ptr[2] = bChannel.data(); // B
    }
    if (texture.channels >= 4) {
        image_ptr[3] = aChannel.data(); // A
    }

    image.images = reinterpret_cast<unsigned char**>(image_ptr.data());

    // Set up EXR header
    header.num_channels = texture.channels;
    header.channels     = new EXRChannelInfo[header.num_channels];
    if (texture.channels >= 1) {
        strncpy(header.channels[0].name, "R", 255); // Red channel
    }
    if (texture.channels >= 2) {
        strncpy(header.channels[1].name, "G", 255); // Green channel
    }
    if (texture.channels >= 3) {
        strncpy(header.channels[2].name, "B", 255); // Blue channel
    }
    if (texture.channels >= 4) {
        strncpy(header.channels[4].name, "A", 255); // Blue channel
    }

    header.pixel_types           = new int[header.num_channels];
    header.requested_pixel_types = new int[header.num_channels];
    for (int i = 0; i < header.num_channels; i++) {
        header.pixel_types[i]           = TINYEXR_PIXELTYPE_FLOAT; // Input pixel type (float for HDR)
        header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT; // Output pixel type (float for HDR)
    }

    // Save EXR file
    std::filesystem::path filepath(filename);
    std::filesystem::create_directories("./output/");
    auto outName = std::string("./output/").append(filepath.filename().string()).append(".exr");

    const char* err = nullptr;
    int         ret = SaveEXRImageToFile(&image, &header, outName.c_str(), &err);
    if (ret != TINYEXR_SUCCESS) {
        fprintf(stderr, "Error saving color EXR file: %s\n", err);
        FreeEXRErrorMessage(err);
        return;
    }

    // Cleanup
    delete[] header.channels;
    delete[] header.pixel_types;
    delete[] header.requested_pixel_types;
#endif
}