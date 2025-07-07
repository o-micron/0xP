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
#include <Renderer/SW/XPSWTexture.h>
#include <Renderer/SW/XPSWThirdParty.h>

#if defined(_WIN32) || defined(_WIN64)
    #pragma warning(disable : 4996)
    #define _CRT_SECURE_NO_DEPRECATE
#endif

#include <filesystem>
#include <iostream>
#include <limits>
#include <map>

static const size_t WIDTH  = 1920;
static const size_t HEIGHT = 1080;

template<typename T>
struct XPSWRay
{
    XPVec3<T> start;
    XPVec3<T> end;
};

template<typename T>
struct XPSWPlane
{
    union
    {
        struct
        {
            T a, b, c;
            T distance;
        };
        struct
        {
            XPVec4<T> normal;
        };
    };

    void normalize()
    {
        double length = std::sqrt(static_cast<double>(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z));
        normal.x /= length;
        normal.y /= length;
        normal.z /= length;
        normal.w /= length;
    }

    T distanceFromPoint(XPVec3<T> point) const
    {
        return normal.x * point.x + normal.y * point.y + normal.z * point.z + distance;
    }

    [[nodiscard]] static std::array<XPSWPlane, 6> extractFrustumPlanes(const XPMat4<T>& viewProjectionMatrix)
    {
        std::array<XPSWPlane, 6> frustumPlanes = {};
        // Left plane
        frustumPlanes[0].normal.x = viewProjectionMatrix[3] + viewProjectionMatrix[0];
        frustumPlanes[0].normal.y = viewProjectionMatrix[7] + viewProjectionMatrix[4];
        frustumPlanes[0].normal.z = viewProjectionMatrix[11] + viewProjectionMatrix[8];
        frustumPlanes[0].normal.w = viewProjectionMatrix[15] + viewProjectionMatrix[12];
        frustumPlanes[0].normalize();

        // Right plane
        frustumPlanes[1].normal.x = viewProjectionMatrix[3] - viewProjectionMatrix[0];
        frustumPlanes[1].normal.y = viewProjectionMatrix[7] - viewProjectionMatrix[4];
        frustumPlanes[1].normal.z = viewProjectionMatrix[11] - viewProjectionMatrix[8];
        frustumPlanes[1].normal.w = viewProjectionMatrix[15] - viewProjectionMatrix[12];
        frustumPlanes[1].normalize();

        // Bottom plane
        frustumPlanes[2].normal.x = viewProjectionMatrix[3] + viewProjectionMatrix[1];
        frustumPlanes[2].normal.y = viewProjectionMatrix[7] + viewProjectionMatrix[5];
        frustumPlanes[2].normal.z = viewProjectionMatrix[11] + viewProjectionMatrix[9];
        frustumPlanes[2].normal.w = viewProjectionMatrix[15] + viewProjectionMatrix[13];
        frustumPlanes[2].normalize();

        // Top plane
        frustumPlanes[3].normal.x = viewProjectionMatrix[3] - viewProjectionMatrix[1];
        frustumPlanes[3].normal.y = viewProjectionMatrix[7] - viewProjectionMatrix[5];
        frustumPlanes[3].normal.z = viewProjectionMatrix[11] - viewProjectionMatrix[9];
        frustumPlanes[3].normal.w = viewProjectionMatrix[15] - viewProjectionMatrix[13];
        frustumPlanes[3].normalize();

        // Near plane
        frustumPlanes[4].normal.x = viewProjectionMatrix[3] + viewProjectionMatrix[2];
        frustumPlanes[4].normal.y = viewProjectionMatrix[7] + viewProjectionMatrix[6];
        frustumPlanes[4].normal.z = viewProjectionMatrix[11] + viewProjectionMatrix[10];
        frustumPlanes[4].normal.w = viewProjectionMatrix[15] + viewProjectionMatrix[14];
        frustumPlanes[4].normalize();

        // Far plane
        frustumPlanes[5].normal.x = viewProjectionMatrix[3] - viewProjectionMatrix[2];
        frustumPlanes[5].normal.y = viewProjectionMatrix[7] - viewProjectionMatrix[6];
        frustumPlanes[5].normal.z = viewProjectionMatrix[11] - viewProjectionMatrix[10];
        frustumPlanes[5].normal.w = viewProjectionMatrix[15] - viewProjectionMatrix[14];
        frustumPlanes[5].normalize();

        return frustumPlanes;
    }
};

template<typename T>
struct XPSWBoundingSquare
{
    XPSWBoundingSquare()
      : min(XPVec2<T>{ std::numeric_limits<T>::min(), std::numeric_limits<T>::min() })
      , max(XPVec2<T>{ std::numeric_limits<T>::max(), std::numeric_limits<T>::max() })
    {
    }
    XPSWBoundingSquare(XPVec2<T> min, XPVec2<T> max)
      : min(min)
      , max(max)
    {
    }

    XPVec2<T> min;
    XPVec2<T> max;
};

enum XPSWEBoundingBoxFrustumTest
{
    XPSWEBoundingBoxFrustumTest_FullyInside  = 0,
    XPSWEBoundingBoxFrustumTest_FullyOutside = 1,
    XPSWEBoundingBoxFrustumTest_Intersecting = 2
};

template<typename T>
struct XPSWBoundingBox
{
    XPSWBoundingBox()
      : min(XPVec3<T>{ std::numeric_limits<T>::min(), std::numeric_limits<T>::min(), std::numeric_limits<T>::min() })
      , max(XPVec3<T>{ std::numeric_limits<T>::max(), std::numeric_limits<T>::max(), std::numeric_limits<T>::max() })
    {
    }
    XPSWBoundingBox(XPVec3<T> min, XPVec3<T> max)
      : min(min)
      , max(max)
    {
    }
    // Function to check if an AABB is inside, intersecting, or outside the frustum
    XPSWEBoundingBoxFrustumTest testFrustum(const std::array<XPSWPlane<T>, 6>& frustum)
    {
        bool isFullyInside = true;

        for (int i = 0; i < 6; i++) {
            const XPSWPlane<T>& plane = frustum[i];

            // Find the AABB corner that is farthest in the direction of the plane normal
            XPVec3<T> positiveCorner;
            positiveCorner.x = (plane.normal.x > 0) ? max.x : min.x;
            positiveCorner.y = (plane.normal.y > 0) ? max.y : min.y;
            positiveCorner.z = (plane.normal.z > 0) ? max.z : min.z;

            // Find the AABB corner that is closest in the direction of the plane normal
            XPVec3<T> negativeCorner;
            negativeCorner.x = (plane.normal.x > 0) ? min.x : max.x;
            negativeCorner.y = (plane.normal.y > 0) ? min.y : max.y;
            negativeCorner.z = (plane.normal.z > 0) ? min.z : max.z;

            // If the farthest corner is outside the plane, the AABB is outside the frustum
            if (plane.distanceFromPoint(positiveCorner) < 0) { return XPSWEBoundingBoxFrustumTest_FullyOutside; }

            // If the closest corner is outside the plane, the AABB is intersecting the frustum
            if (plane.distanceFromPoint(negativeCorner) < 0) {
                isFullyInside = false; // AABB is intersecting the frustum
            }
        }

        // If the AABB is not fully inside and not fully outside, it is intersecting
        if (!isFullyInside) { return XPSWEBoundingBoxFrustumTest_Intersecting; }

        return XPSWEBoundingBoxFrustumTest_FullyInside;
    }

    XPVec3<T> min;
    XPVec3<T> max;
};

template<typename T>
struct XPSWCamera
{
    ~XPSWCamera() { destroyFrameBuffers(); }

    void updateMatrices()
    {
        viewMatrix       = createViewMatrix(location, target, up);
        projectionMatrix = createPerspectiveProjectionMatrix(
          static_cast<T>(fov), static_cast<T>(resolution.x), static_cast<T>(resolution.y), zNearPlane, zFarPlane);
        // projectionMatrix = createOrthographicProjectionMatrix(
        //   static_cast<T>(resolution.x), static_cast<T>(resolution.y), zNearPlane, zFarPlane);
        inverseViewMatrix.glm       = glm::inverse(viewMatrix.glm);
        inverseProjectionMatrix.glm = glm::inverse(projectionMatrix.glm);
    }

    bool isBackFace(const XPVec4<T>& projectedVertex0,
                    const XPVec4<T>& projectedVertex1,
                    const XPVec4<T>& projectedVertex2) const
    {
        // Perform back-face culling
        XPVec3<T> cameraDirection = { -viewMatrix.glm[0][2], -viewMatrix.glm[1][2], -viewMatrix.glm[2][2] };
        cameraDirection.normalize();

        glm::mat<4, 4, T, glm::defaultp> inverseViewMatrix       = glm::inverse(viewMatrix.glm);
        glm::mat<4, 4, T, glm::defaultp> inverseProjectionMatrix = glm::inverse(projectionMatrix.glm);

        glm::vec<4, T, glm::defaultp> wv0     = inverseViewMatrix * inverseProjectionMatrix * projectedVertex0.glm;
        XPVec3<T>                     worldV0 = { wv0.x, wv0.y, wv0.z };

        glm::vec<4, T, glm::defaultp> wv1     = inverseViewMatrix * inverseProjectionMatrix * projectedVertex1.glm;
        XPVec3<T>                     worldV1 = { wv1.x, wv1.y, wv1.z };

        glm::vec<4, T, glm::defaultp> wv2     = inverseViewMatrix * inverseProjectionMatrix * projectedVertex2.glm;
        XPVec3<T>                     worldV2 = { wv2.x, wv2.y, wv2.z };

        XPVec3<T> edge1  = worldV1 - worldV0;
        XPVec3<T> edge2  = worldV2 - worldV0;
        XPVec3<T> normal = edge1.cross(edge2);
        normal.normalize();

        return normal.dot(cameraDirection) < 0;
    }

    bool isBackFace(const XPVec3<T>& worldVertex0, const XPVec3<T>& worldVertex1, const XPVec3<T>& worldVertex2) const
    {
        // Perform back-face culling
        XPVec3<T> cameraDirection = { -viewMatrix.glm[0][2], -viewMatrix.glm[1][2], -viewMatrix.glm[2][2] };
        cameraDirection.normalize();

        XPVec3<T> edge1  = worldVertex1 - worldVertex0;
        XPVec3<T> edge2  = worldVertex2 - worldVertex0;
        XPVec3<T> normal = edge1.cross(edge2);
        normal.normalize();

        return normal.dot(cameraDirection) < 0;
    }

    XPSWRay<T> getProjectedRay(const XPVec2<int>& pixel)
    {
        // NDC coordinates (-1 to 1)
        const float x_ndc = (2.0f * (static_cast<float>(pixel.x) + 0.5f) / resolution.x) - 1.0f;
        const float y_ndc = 1.0f - (2.0f * (static_cast<float>(pixel.y) + 0.5f) / resolution.y);

        // Clip-space (z=-1 for OpenGL, z=0 for DirectX)
        const XPVec4<T> clip_coords = { x_ndc, y_ndc, 0.0f, 1.0f };

        // Transform to world-space
        auto world_pos = inverseViewMatrix * inverseProjectionMatrix * clip_coords;
        world_pos.x    = world_pos.x / world_pos.w;
        world_pos.y    = world_pos.y / world_pos.w;
        world_pos.z    = world_pos.z / world_pos.w;

        // Ray origin (camera position)
        auto ray_origin = inverseViewMatrix.row3.xyz;

        // Ray direction
        auto ray_dir = world_pos.xyz - ray_origin;
        ray_dir.normalize();

        XPSWRay<T> ray = {};
        ray.start      = XPVec3<T>{ ray_origin.x + ray_dir.x * zNearPlane,
                                    ray_origin.y + ray_dir.y * zNearPlane,
                                    ray_origin.z + ray_dir.z * zNearPlane };
        ray.end        = XPVec3<T>{ ray_origin.x + ray_dir.x * zFarPlane,
                                    ray_origin.y + ray_dir.y * zFarPlane,
                                    ray_origin.z + ray_dir.z * zFarPlane };
        return ray;
    }

    [[nodiscard]] size_t getDepthBufferNumBytes() const { return resolution.x * resolution.y * sizeof(float); }
    [[nodiscard]] size_t getColorBufferNumBytes() const { return resolution.x * resolution.y * 4 * sizeof(float); }

    void createFrameBuffers()
    {
        size_t numBytes = getDepthBufferNumBytes() + getColorBufferNumBytes();

        frameBuffersPtr = static_cast<uint8_t*>(malloc(numBytes));
        if (frameBuffersPtr == nullptr) {
            LOGV_CRITICAL("Could not allocate camera framebuffers memory of {} bytes", numBytes);
        }
        depthBuffer = reinterpret_cast<float*>(frameBuffersPtr);
        colorBuffer = reinterpret_cast<float*>(frameBuffersPtr + getDepthBufferNumBytes());
    }

    void destroyFrameBuffers()
    {
        if (frameBuffersPtr) {
            free(frameBuffersPtr);
            frameBuffersPtr = nullptr;
        }
    }

    void clearColorBuffer() const
    {
        for (size_t i = 0; i < getColorBufferNumBytes() / sizeof(float); ++i) { colorBuffer[i] = 0.0f; }
    }

    void clearDepthBuffer() const
    {
        for (size_t i = 0; i < getDepthBufferNumBytes() / sizeof(float); ++i) { depthBuffer[i] = FLT_MAX; }
    }

    [[nodiscard]] const XPVec4<float>& getColorBufferPixel(const int x, const int y) const
    {
        return static_cast<XPVec4<T>&>(&colorBuffer[4 * (y * resolution.x + x)]);
    }
    void writeColorBuffer(const XPVec4<float>& val, const int x, const int y) const
    {
        const size_t idx         = y * resolution.x + x;
        colorBuffer[4 * idx + 0] = val.x;
        colorBuffer[4 * idx + 1] = val.y;
        colorBuffer[4 * idx + 2] = val.z;
        colorBuffer[4 * idx + 3] = val.w;
    }

    [[nodiscard]] const float& getDepthBufferPixel(const int x, const int y) const
    {
        return depthBuffer[y * resolution.x + x];
    }
    void writeDepthBuffer(const float& val, const int x, const int y) const
    {
        const size_t idx = y * resolution.x + x;
        {
            if (val < depthBuffer[idx]) { depthBuffer[idx] = val; }
        }
    }

    bool saveColorToEXR(const std::string& filename) const
    {
#ifndef __EMSCRIPTEN__
        // Prepare EXR image
        EXRHeader header;
        InitEXRHeader(&header);

        EXRImage image;
        InitEXRImage(&image);

        image.num_channels = 3;
        image.width        = resolution.x;
        image.height       = resolution.y;

        // Split the color buffer into separate channels
        std::vector<float> rChannel(resolution.x * resolution.y);
        std::vector<float> gChannel(resolution.x * resolution.y);
        std::vector<float> bChannel(resolution.x * resolution.y);

        for (int i = 0; i < resolution.x * resolution.y; ++i) {
            rChannel[i] = colorBuffer[4 * i + 0]; // R
            gChannel[i] = colorBuffer[4 * i + 1]; // G
            bChannel[i] = colorBuffer[4 * i + 2]; // B
        }

        // Pointers to each channel
        std::vector<float*> image_ptr(image.num_channels);
        image_ptr[0] = rChannel.data(); // R
        image_ptr[1] = gChannel.data(); // G
        image_ptr[2] = bChannel.data(); // B

        image.images = reinterpret_cast<unsigned char**>(image_ptr.data());

        // Set up EXR header
        header.num_channels = image.num_channels;
        header.channels     = new EXRChannelInfo[header.num_channels];
        strncpy(header.channels[0].name, "R", 255);
        strncpy(header.channels[1].name, "G", 255);
        strncpy(header.channels[2].name, "B", 255);

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
            return false;
        }

        // Cleanup
        delete[] header.channels;
        delete[] header.pixel_types;
        delete[] header.requested_pixel_types;
#endif

        return true;
    }

    [[nodiscard]] bool saveDepthToEXR(const std::string& filename) const
    {
#ifndef __EMSCRIPTEN__
        // Convert uint32_t depth buffer to float
        std::vector<float> depthBufferFloat(getDepthBufferNumBytes() / sizeof(float));
        float              maxVal = FLT_MIN, minVal = FLT_MAX;
        for (size_t i = 0; i < depthBufferFloat.size(); ++i) {
            float val = ExponentialInvertedToLinearZ(depthBuffer[i]);
            if (val > maxVal) { maxVal = val; }
            if (val < minVal) { minVal = val; }
        }
        float rangeVal = fabs(maxVal - minVal);
        for (size_t i = 0; i < depthBufferFloat.size(); ++i) {
            depthBufferFloat[i] = (ExponentialInvertedToLinearZ(depthBuffer[i]) / rangeVal) * 255.0f;
        }

        // Prepare EXR image
        EXRHeader header;
        InitEXRHeader(&header);

        EXRImage image;
        InitEXRImage(&image);

        image.num_channels = 1; // 1 channel for depth (Z)
        image.width        = resolution.x;
        image.height       = resolution.y;

        // Pointers to the depth channel
        std::vector<float*> image_ptr(1);
        image_ptr[0] = depthBufferFloat.data(); // Z

        image.images = reinterpret_cast<unsigned char**>(image_ptr.data());

        // Set up EXR header
        header.num_channels = 1;
        header.channels     = new EXRChannelInfo[header.num_channels];
        strncpy(header.channels[0].name, "Z", 255);
        header.compression_type = TINYEXR_COMPRESSIONTYPE_NONE;

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
            fprintf(stderr, "Error saving depth EXR file: %s\n", err);
            FreeEXRErrorMessage(err);
            return false;
        }

        // Cleanup
        delete[] header.channels;
        delete[] header.pixel_types;
        delete[] header.requested_pixel_types;
#endif
        return true;
    }

    // void exportDepth(const std::string& filename) const
    // {
    //     std::vector<uint8_t> data;
    //     data.resize(resolution.x * resolution.y);
    //
    //     std::filesystem::path filepath(filename);
    //     std::filesystem::create_directories("./output/");
    //     auto outName = std::string("./output/").append(filepath.filename().string()).append(".png");
    //
    //     for (size_t i = 0; i < resolution.x * resolution.y; ++i) {
    //         if (depthBuffer[i] == DEPTH_MAX_VAL) {
    //             data[i] = 0;
    //             continue;
    //         }
    //         T linearDepth = linearizeDepth(static_cast<double>(depthBuffer[i]) / DEPTH_MAX_VAL);
    //         data[i]       = static_cast<uint8_t>(linearDepth * 255.0);
    //     }
    //
    //     stbi_write_png(outName.c_str(), resolution.x, resolution.y, 1, data.data(), resolution.x);
    // }

    // void exportColor(const std::string& filename) const
    // {
    //     std::vector<uint8_t> data;
    //     data.resize(resolution.x * resolution.y * 3);
    //
    //     std::filesystem::path filepath(filename);
    //     std::filesystem::create_directories("./output/");
    //     auto outName = std::string("./output/").append(filepath.filename().string()).append(".png");
    //
    //     for (size_t i = 0; i < resolution.x * resolution.y * 3; ++i) {
    //         data[i] = static_cast<uint8_t>((static_cast<double>(colorBuffer[i]) / COLOR_MAX_VAL) * 255.0);
    //     }
    //
    //     stbi_write_png(outName.c_str(), resolution.x, resolution.y, 3, data.data(), resolution.x * 3);
    // }

    std::string      name;
    XPVec2<uint32_t> resolution;
    T                fov;
    T                zNearPlane;
    T                zFarPlane;
    XPVec3<T>        location;
    XPVec3<T>        target;
    XPVec3<T>        up;
    XPMat4<T>        viewMatrix;
    XPMat4<T>        projectionMatrix;
    XPMat4<T>        inverseViewMatrix;
    XPMat4<T>        inverseProjectionMatrix;
    uint8_t*         frameBuffersPtr;
    float*           colorBuffer;
    float*           depthBuffer;
};

enum XPSWELightType
{
    XPSWELightType_Directional = 0,
    XPSWELightType_Point,
    XPSWELightType_Spot,
};

template<typename T>
struct XPSWLight
{
    std::string    name;
    XPVec3<T>      location;
    XPVec3<T>      direction;
    XPVec3<T>      ambient;
    XPVec3<T>      diffuse;
    XPVec3<T>      specular;
    T              intensity;
    T              attenuationConstant;
    T              attenuationLinear;
    T              attenuationQuadratic;
    XPSWELightType type;
    float          angleInnerCone;
    float          angleOuterCone;
};

template<typename T>
struct XPSWVertex
{
    XPVec4<T> location;
    XPVec3<T> normal;
    XPVec2<T> coord;
    XPVec3<T> tangent;
    XPVec3<T> biTangent;
    XPVec4<T> color;
};

template<typename T>
struct XPSWVertexFragmentVaryings
{
    XPVec4<T> fragPos;
    XPVec3<T> fragNormal;
    XPVec3<T> fragTangent;
    XPVec3<T> fragBiTangent;
    XPVec2<T> fragTextureCoord;
    XPVec3<T> fragColor;
};

template<typename T>
struct XPSWVertexFragmentFlatVaryings
{
    std::vector<XPSWELightType> lightType;
    std::vector<float>          lightAttenuationConstant;
    std::vector<float>          lightAttenuationLinear;
    std::vector<float>          lightAttenuationQuadratic;
    std::vector<float>          lightAngleOuterCone;
    std::vector<XPVec3<T>>      lightPos;
    std::vector<XPVec3<T>>      lightDirection;
    std::vector<XPVec3<T>>      lightColor;
    std::vector<float>          lightIntensity;
    XPVec3<T>                   viewPos;
};

template<typename T>
struct XPSWTriangle
{
    XPSWVertex<T> v0, v1, v2;

    [[nodiscard]] static T edgeFunction(const XPVec2<T>& v0, const XPVec2<T>& v1, const XPVec2<T>& v2)
    {
        return (v2.x - v0.x) * (v1.y - v0.y) - (v2.y - v0.y) * (v1.x - v0.x);
    }
    [[nodiscard]] static T area(const XPVec2<T>& v0, const XPVec2<T>& v1, const XPVec2<T>& v2)
    {
        return XPSWTriangle::edgeFunction(v0, v1, v2);
    }
    [[nodiscard]] XPVec3<T> normal() const
    {
        // Calculate normal (cross product of two edges)
        const XPVec3<T> edge1 = { v1.location.x - v0.location.x,
                                  v1.location.y - v0.location.y,
                                  v1.location.z - v0.location.z };
        const XPVec3<T> edge2 = { v2.location.x - v0.location.x,
                                  v2.location.y - v0.location.y,
                                  v2.location.z - v0.location.z };
        return XPVec3<T>{ edge1.y * edge2.z - edge1.z * edge2.y,
                          edge1.z * edge2.x - edge1.x * edge2.z,
                          edge1.x * edge2.y - edge1.y * edge2.x };
    }
    static void generateTangentsAndBiTangents(const XPVec3<T>& v0Pos,
                                              const XPVec3<T>& v1Pos,
                                              const XPVec3<T>& v2Pos,
                                              const XPVec2<T>& v0UV,
                                              const XPVec2<T>& v1UV,
                                              const XPVec2<T>& v2UV,
                                              XPVec3<T>&       tangent,
                                              XPVec3<T>&       biTangent)
    {
        // Position edges
        XPVec3<T> deltaPos1 = v1Pos - v0Pos;
        XPVec3<T> deltaPos2 = v2Pos - v0Pos;

        // UV edges
        XPVec2<T> deltaUV1 = v1UV - v0UV;
        XPVec2<T> deltaUV2 = v2UV - v0UV;

        // Calculate denominator
        float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

        // Calculate tangent and bitangent
        tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
        tangent.normalize();
        biTangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;
        biTangent.normalize();
    }
};

// template<typename T>
// struct MeshBuffer;

// template<typename T>
// struct MeshObject
// {
//     std::string    name;
//     MeshBuffer<T>* meshBuffer;
//     uint32_t       vertexOffset;
//     uint32_t       indexOffset;
//     uint32_t       numIndices;
//     BoundingBox<T> boundingBox;
//     XPMat4<T>        modelMatrix;
// };

// template<typename T>
// struct MeshBuffer
// {
//     XPVec3<T>*       positions;
//     XPVec3<T>*       normals;
//     XPVec2<T>*       texcoords;
//     uint32_t*      indices;
//     MeshObject<T>* objects;
//     unsigned char* backingMemory;
//     size_t         reservedPositionsCount;
//     size_t         reservedNormalsCount;
//     size_t         reservedTexcoordsCount;
//     size_t         reservedIndicesCount;
//     size_t         reservedObjectsCount;

//     void setPositionsSize(const size_t size) { reservedPositionsCount = size; }
//     void setNormalsSize(const size_t size) { reservedNormalsCount = size; }
//     void setTexcoordsSize(const size_t size) { reservedTexcoordsCount = size; }
//     void setIndicesSize(const size_t size) { reservedIndicesCount = size; }
//     void setObjectsSize(const size_t size) { reservedObjectsCount = size; }
//     void allocateForResources()
//     {
//         const size_t numBytes = reservedPositionsCount * sizeof(XPVec3<T>) + reservedNormalsCount * sizeof(XPVec3<T>)
//         +
//                                 reservedTexcoordsCount * sizeof(XPVec2<T>) + reservedIndicesCount * sizeof(uint32_t)
//                                 + reservedObjectsCount * sizeof(MeshObject<T>);
//         backingMemory = static_cast<unsigned char*>(malloc(numBytes));
//         memset(backingMemory, 0, numBytes);

//         unsigned char* refPtr = backingMemory;
//         positions             = reinterpret_cast<XPVec3<T>*>(refPtr);
//         refPtr += reservedPositionsCount * sizeof(XPVec3<T>);

//         normals = reinterpret_cast<XPVec3<T>*>(refPtr);
//         refPtr += reservedNormalsCount * sizeof(XPVec3<T>);

//         texcoords = reinterpret_cast<XPVec2<T>*>(refPtr);
//         refPtr += reservedTexcoordsCount * sizeof(XPVec2<T>);

//         indices = reinterpret_cast<uint32_t*>(refPtr);
//         refPtr += reservedIndicesCount * sizeof(uint32_t);

//         objects = reinterpret_cast<MeshObject<T>*>(refPtr);
//         refPtr += reservedObjectsCount * sizeof(MeshObject<T>);
//     }
//     void deallocateResources()
//     {
//         if (backingMemory) {
//             free(backingMemory);
//             objects                = nullptr;
//             indices                = nullptr;
//             texcoords              = nullptr;
//             normals                = nullptr;
//             positions              = nullptr;
//             backingMemory          = nullptr;
//             reservedPositionsCount = 0;
//             reservedNormalsCount   = 0;
//             reservedTexcoordsCount = 0;
//             reservedIndicesCount   = 0;
//             reservedObjectsCount   = 0;
//         }
//     }
// };

// Struct to store mesh data
template<typename T>
struct XPSWMesh
{
    XPSWMesh() noexcept                 = default;
    XPSWMesh(XPSWMesh&& other) noexcept = default;
    XPSWMesh(const XPSWMesh& other)     = delete;

    std::string            name;
    XPMat4<T>              transform;
    std::vector<XPVec4<T>> vertices;
    std::vector<XPVec3<T>> normals;
    std::vector<XPVec2<T>> texCoords;
    std::vector<XPVec3<T>> colors;
    std::vector<XPVec3<T>> tangents;
    std::vector<XPVec3<T>> biTangents;
    std::vector<uint32_t>  indices;
    XPSWBoundingBox<T>     boundingBox;
    unsigned int           materialIndex;
};

template<typename T>
struct XPSWMaterial
{
    XPSWMaterial() noexcept                     = default;
    XPSWMaterial(XPSWMaterial&& other) noexcept = default;
    XPSWMaterial(const XPSWMaterial& other)     = delete;

    std::string name;

    XPSWTexture2D baseColorTexture;
    XPSWTexture2D normalMapTexture;
    XPSWTexture2D emissionColorTexture;
    XPSWTexture2D metallicTexture;
    XPSWTexture2D roughnessTexture;
    XPSWTexture2D aoTexture;

    XPVec3<float> baseColorValue;
    XPVec3<float> emissionColorValue;
    float         metallicValue;
    float         roughnessValue;
    float         aoValue;

    bool hasBaseColorTexture;
    bool hasNormalMapTexture;
    bool hasEmissionColorTexture;
    bool hasMetallicTexture;
    bool hasRoughnessTexture;
    bool hasAOTexture;
};

template<typename T>
struct XPSWScene
{
    std::string                                              filepath;
    std::vector<XPSWMesh<T>>                                 meshes;
    std::vector<XPSWCamera<T>>                               cameras;
    std::vector<XPSWLight<T>>                                lights;
    std::map<uint32_t, XPSWMaterial<T>>                      materials;
    std::map<std::string, std::tuple<bool*, XPSWTexture2D*>> pendingDownloadTextures;
};
