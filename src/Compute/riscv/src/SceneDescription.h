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

#include "Maths.h"
#include "Texture.h"

#include <algorithm>
#include <array>
#include <map>
#include <tuple>
#include <vector>

template<typename T>
struct Vertex
{
    Vec4<T> location;
    Vec3<T> normal;
    Vec2<T> coord;
};

template<typename T>
struct Triangle
{
    Vertex<T> v0, v1, v2;

    [[nodiscard]] static T edgeFunction(const Vec2<T>& v0, const Vec2<T>& v1, const Vec2<T>& v2);
    [[nodiscard]] static T area(const Vec2<T>& v0, const Vec2<T>& v1, const Vec2<T>& v2);
    [[nodiscard]] Vec3<T>  normal() const;
};

template<typename T>
struct Plane
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
            Vec4<T> normal;
        };
    };

    void                                      normalize();
    T                                         distanceFromPoint(Vec3<T> point) const;
    [[nodiscard]] static std::array<Plane, 6> extractFrustumPlanes(const Mat4<T>& viewProjectionMatrix);
};

enum ELightType
{
    ELightType_Directional = 0,
    ELightType_Point,
    ELightType_Spot,
};

template<typename T>
struct VertexFragmentVaryings
{
    VertexFragmentVaryings(VertexFragmentVaryings&&)            = delete;
    VertexFragmentVaryings(const VertexFragmentVaryings& other) = delete;

    Vec4<T> fragPos;
    Vec3<T> fragNormal;
    Vec2<T> fragTextureCoord;
};

template<typename T>
struct VertexFragmentFlatVaryings
{
    VertexFragmentFlatVaryings(VertexFragmentFlatVaryings&&)            = delete;
    VertexFragmentFlatVaryings(const VertexFragmentFlatVaryings& other) = delete;

    std::vector<ELightType> lightType;
    std::vector<float>      lightAttenuationConstant;
    std::vector<float>      lightAttenuationLinear;
    std::vector<float>      lightAttenuationQuadratic;
    std::vector<float>      lightAngleOuterCone;
    std::vector<Vec3<T>>    lightPos;
    std::vector<Vec3<T>>    lightDirection;
    std::vector<Vec3<T>>    lightColor;
    std::vector<float>      lightIntensity;
    Vec3<T>                 viewPos;
};

template<typename T>
struct BoundingSquare
{
    BoundingSquare()
      : min(Vec2<T>{ std::numeric_limits<T>::min(), std::numeric_limits<T>::min() })
      , max(Vec2<T>{ std::numeric_limits<T>::max(), std::numeric_limits<T>::max() })
    {
    }
    BoundingSquare(Vec2<T> min, Vec2<T> max)
      : min(min)
      , max(max)
    {
    }

    Vec2<T> min;
    Vec2<T> max;
};

enum EBoundingBoxFrustumTest
{
    EBoundingBoxFrustumTest_FullyInside  = 0,
    EBoundingBoxFrustumTest_FullyOutside = 1,
    EBoundingBoxFrustumTest_Intersecting = 2
};

template<typename T>
struct BoundingBox
{
    BoundingBox()
      : min(Vec3<T>{ std::numeric_limits<T>::min(), std::numeric_limits<T>::min(), std::numeric_limits<T>::min() })
      , max(Vec3<T>{ std::numeric_limits<T>::max(), std::numeric_limits<T>::max(), std::numeric_limits<T>::max() })
    {
    }
    BoundingBox(Vec3<T> min, Vec3<T> max)
      : min(min)
      , max(max)
    {
    }
    EBoundingBoxFrustumTest testFrustum(const std::array<Plane<T>, 6>& frustum) const;

    Vec3<T> min;
    Vec3<T> max;
};

template<typename T>
struct Ray
{
    Vec3<T> start;
    Vec3<T> end;
};

template<typename T>
struct Camera
{
    bool   isBackFace(const Vec4<T>& projectedVertex0,
                      const Vec4<T>& projectedVertex1,
                      const Vec4<T>& projectedVertex2) const;
    bool   isBackFace(const Vec3<T>& worldVertex0, const Vec3<T>& worldVertex1, const Vec3<T>& worldVertex2) const;
    Ray<T> getProjectedRay(const Vec2<int>& pixel);
    float  extractNear() const;
    float  extractFar() const;

    Mat4<T> viewMatrix;
    Mat4<T> projectionMatrix;
    Mat4<T> viewProjectionMatrix;
    Mat4<T> inverseViewMatrix;
    Mat4<T> inverseProjectionMatrix;
    u32vec2 resolution;
    T       fov;
    T       zNearPlane;
    T       zFarPlane;
};

template<typename T>
struct Mesh
{
    Mesh() noexcept             = default;
    Mesh(Mesh&& other) noexcept = default;
    Mesh(const Mesh& other)     = delete;

    Mat4<T>               transform;
    std::vector<Vec4<T>>  vertices;
    std::vector<Vec3<T>>  normals;
    std::vector<Vec2<T>>  texCoords;
    std::vector<uint32_t> indices;
    BoundingBox<T>        boundingBox;
    unsigned int          materialIndex;
};

template<typename T>
struct Material
{
    Material() noexcept                 = default;
    Material(Material&& other) noexcept = default;
    Material(const Material& other)     = delete;

    Texture2D baseColorTexture;
    Texture2D normalMapTexture;
    Texture2D emissionColorTexture;
    Texture2D metallicTexture;
    Texture2D roughnessTexture;
    Texture2D aoTexture;

    fvec3 baseColorValue;
    fvec3 emissionColorValue;
    float metallicValue;
    float roughnessValue;
    float aoValue;

    bool hasBaseColorTexture;
    bool hasNormalMapTexture;
    bool hasEmissionColorTexture;
    bool hasMetallicTexture;
    bool hasRoughnessTexture;
    bool hasAOTexture;
};

template<typename T>
struct Light
{
    Vec3<T>    location;
    Vec3<T>    direction;
    Vec3<T>    ambient;
    Vec3<T>    diffuse;
    Vec3<T>    specular;
    T          intensity;
    T          attenuationConstant;
    T          attenuationLinear;
    T          attenuationQuadratic;
    ELightType type;
    float      angleInnerCone;
    float      angleOuterCone;
};

template<typename T>
struct Scene
{
    Scene(Scene&& other) noexcept = delete;
    Scene(const Scene& other)     = delete;

    std::vector<Mesh<T>>                                 meshes;
    std::vector<Camera<T>>                               cameras;
    std::vector<Light<T>>                                lights;
    std::map<unsigned int, Material<T>>                  materials;
    std::map<std::string, std::tuple<bool*, Texture2D*>> pendingDownloadTextures;
};

struct FramebufferRef
{
    FramebufferRef(float* preAllocatedColorAttachmentPtr, float* preAllocatedDepthAttachmentPtr, int width, int height);

    float* colorAttachmentPtr;
    float* depthAttachmentPtr;
    int    width;
    int    height;
};

struct FrameMemoryPool
{
    FrameMemoryPool(uint8_t* preAllocatedMemPtr, size_t preAllocatedNumBytes);
    uint8_t* pushMemory(size_t numBytes);
    void     popMemory(size_t numBytes);
    void     popAllMemory();
    void     memsetZeros();

    uint8_t* memPtr;
    int64_t  memStartIndex;
    int64_t  memEndIndex;
};

template<typename T>
[[nodiscard]] inline Vec4<T>
interpolateVec4(const Vec3<T>& barycentricCoordinates, const Vec4<T>& v0, const Vec4<T>& v1, const Vec4<T>& v2)
{
    return Vec4<T>{ barycentricCoordinates.x * v0.x + barycentricCoordinates.y * v1.x + barycentricCoordinates.z * v2.x,
                    barycentricCoordinates.x * v0.y + barycentricCoordinates.y * v1.y + barycentricCoordinates.z * v2.y,
                    barycentricCoordinates.x * v0.z + barycentricCoordinates.y * v1.z + barycentricCoordinates.z * v2.z,
                    barycentricCoordinates.x * v0.w + barycentricCoordinates.y * v1.w +
                      barycentricCoordinates.z * v2.w };
}

template<typename T>
[[nodiscard]] inline Vec3<T>
interpolateVec3(const Vec3<T>& barycentricCoordinates, const Vec3<T>& v0, const Vec3<T>& v1, const Vec3<T>& v2)
{
    return Vec3<T>{ barycentricCoordinates.x * v0.x + barycentricCoordinates.y * v1.x + barycentricCoordinates.z * v2.x,
                    barycentricCoordinates.x * v0.y + barycentricCoordinates.y * v1.y + barycentricCoordinates.z * v2.y,
                    barycentricCoordinates.x * v0.z + barycentricCoordinates.y * v1.z +
                      barycentricCoordinates.z * v2.z };
}

template<typename T>
[[nodiscard]] inline Vec2<T>
interpolateVec2(const Vec3<T>& barycentricCoordinates, const Vec2<T>& v0, const Vec2<T>& v1, const Vec2<T>& v2)
{
    return Vec2<T>{ barycentricCoordinates.x * v0.x + barycentricCoordinates.y * v1.x + barycentricCoordinates.z * v2.x,
                    barycentricCoordinates.x * v0.y + barycentricCoordinates.y * v1.y +
                      barycentricCoordinates.z * v2.y };
}

template<typename T>
inline void
interpolateVertex(const Vec3<T>&                                  barycentricCoordinates,
                  const std::array<Vec4<T>, 3>&                   projectedVertices,
                  const std::array<VertexFragmentVaryings<T>, 3>& inVFV,
                  VertexFragmentVaryings<T>&                      outVFV)
{
    float u = barycentricCoordinates.x;
    float v = barycentricCoordinates.y;
    float w = barycentricCoordinates.z;

    Vec4<T> out;
    // clang-format off

    float denominator = 1.0f / (u / projectedVertices[0].w + v / projectedVertices[1].w + w / projectedVertices[2].w);

    outVFV.fragPos.x = u * inVFV[0].fragPos.x + v * inVFV[1].fragPos.x + w * inVFV[2].fragPos.x;
    outVFV.fragPos.y = u * inVFV[0].fragPos.y + v * inVFV[1].fragPos.y + w * inVFV[2].fragPos.y;
    outVFV.fragPos.z = u * inVFV[0].fragPos.z + v * inVFV[1].fragPos.z + w * inVFV[2].fragPos.z;

    outVFV.fragNormal.x = u * inVFV[0].fragNormal.x + v * inVFV[1].fragNormal.x + w * inVFV[2].fragNormal.x;
    outVFV.fragNormal.y = u * inVFV[0].fragNormal.y + v * inVFV[1].fragNormal.y + w * inVFV[2].fragNormal.y;
    outVFV.fragNormal.z = u * inVFV[0].fragNormal.z + v * inVFV[1].fragNormal.z + w * inVFV[2].fragNormal.z;
    outVFV.fragNormal.normalize();

    outVFV.fragTextureCoord.x = u * inVFV[0].fragTextureCoord.x + v * inVFV[1].fragTextureCoord.x + w * inVFV[2].fragTextureCoord.x;
    outVFV.fragTextureCoord.y = u * inVFV[0].fragTextureCoord.y + v * inVFV[1].fragTextureCoord.y + w * inVFV[2].fragTextureCoord.y;

    // clang-format on
}

template<typename T>
[[nodiscard]] inline BoundingSquare<int64_t>
calculateTriangleBoundingSquare(const Vec4<T>& viewport, const Vec2<T>& v0, const Vec2<T>& v1, const Vec2<T>& v2)
{
    int64_t minX = std::max(T(viewport.x), std::min({ v0.x, v1.x, v2.x }));
    int64_t maxX = std::min(static_cast<T>(viewport.z) - T(1), std::max({ v0.x, v1.x, v2.x }));
    int64_t minY = std::max(T(viewport.y), std::min({ v0.y, v1.y, v2.y }));
    int64_t maxY = std::min(static_cast<T>(viewport.w) - T(1), std::max({ v0.y, v1.y, v2.y }));
    return { Vec2<int64_t>(minX, minY), Vec2<int64_t>(maxX, maxY) };
}

template<typename T>
[[nodiscard]] inline Vec3<T>
computeBarycentricCoordinates(const Vec2<T>& A, const Vec2<T>& B, const Vec2<T>& C, float px, float py)
{
    Vec3<T>     barycentricCoordinates;
    const float denominator  = (B.y - C.y) * (A.x - C.x) + (C.x - B.x) * (A.y - C.y);
    barycentricCoordinates.x = ((B.y - C.y) * (px - C.x) + (C.x - B.x) * (py - C.y)) / denominator;
    barycentricCoordinates.y = ((C.y - A.y) * (px - C.x) + (A.x - C.x) * (py - C.y)) / denominator;
    barycentricCoordinates.z = 1.0f - barycentricCoordinates.x - barycentricCoordinates.y;
    return barycentricCoordinates;
}

template<typename T>
inline Mat4<T>
createModelMatrix(const Vec3<T>& location, const Vec3<T>& rotation, const Vec3<T>& scale)
{
    Mat4<T> result;
    result.glm = glm::mat<4, 4, T, glm::defaultp>(static_cast<T>(1));
    result.glm = glm::translate<T, glm::defaultp>(result.glm, location.glm);
    result.glm = result.glm * glm::eulerAngleZ<T>(glm::radians<T>(rotation.z)) *
                 glm::eulerAngleY<T>(glm::radians<T>(rotation.y)) * glm::eulerAngleX<T>(glm::radians<T>(rotation.x));
    result.glm = glm::scale<T, glm::defaultp>(result.glm, scale.glm);
    return result;
}

template<typename T>
inline Mat4<T>
createViewMatrix(const Vec3<T>& location, const Vec3<T>& target, const Vec3<T>& up)
{
    Vec3<T> UP = up;
    UP.normalize();

    Vec3<T> FWD = target - location;
    FWD.normalize();

    Vec3<T> RIGHT = FWD.cross(UP);
    RIGHT.normalize();

    UP = RIGHT.cross(FWD);

    return Mat4<T>{
        // clang-format off
        RIGHT.x,              UP.x,            -FWD.x,             T(0),
        RIGHT.y,              UP.y,            -FWD.y,             T(0),
        RIGHT.z,              UP.z,            -FWD.z,             T(0),
       -RIGHT.dot(location), -UP.dot(location), FWD.dot(location), T(1)
        // clang-format on
    };
}

template<typename T>
inline Mat4<T>
createPerspectiveProjectionMatrix(T fov, T width, T height, T near, T far)
{
    fov      = glm::radians(fov);
    T aspect = width / height;
    return Mat4<T>{
        // clang-format off
        T(1) / (tanf(fov * 0.5f) * aspect), T(0),                           T(0),                       T(0),
        T(0),                               T(1) / tanf(fov * 0.5f),        T(0),                       T(0),
        T(0),                               T(0),                    (far + near) / (near - far),       T(-1),
        T(0),                               T(0),                    T(2) * far * near / (near - far),  T(0)
        // clang-format on
    };
}

template<typename T>
inline Mat4<T>
createOrthographicProjectionMatrix(T width, T height, T near, T far)
{
    Mat4<T> result;
    result.glm =
      glm::orthoRH<T>(static_cast<T>(-1), static_cast<T>(1), static_cast<T>(-1), static_cast<T>(1), near, far);
    return result;
}

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

template<typename T>
T
DistributionGGX(Vec3<T> N, Vec3<T> H, T roughness)
{
    T a      = roughness * roughness;
    T a2     = a * a;
    T NdotH  = std::max(N.dot(H), T(0));
    T NdotH2 = NdotH * NdotH;
    T denom  = (NdotH2 * (a2 - T(1)) + T(1));
    denom    = static_cast<T>(M_PI) * denom * denom;

    return a2 / denom;
}

template<typename T>
T
GeometrySchlickGGX(T NdotV, T roughness)
{
    T r     = (roughness + T(1));
    T k     = (r * r) / T(8);
    T denom = NdotV * (T(1) - k) + k;
    return NdotV / denom;
}

template<typename T>
T
GeometrySmith(const Vec3<T>& N, const Vec3<T>& V, const Vec3<T>& L, T roughness)
{
    T NdotV = std::max(N.dot(V), T(0));
    T NdotL = std::max(N.dot(L), T(0));
    T ggx1  = GeometrySchlickGGX(NdotV, roughness);
    T ggx2  = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

template<typename T>
Vec3<T>
FresnelSchlick(float cosTheta, const Vec3<T>& F0)
{
    return F0 +
           Vec3<T>{ 1.0f - F0.x, 1.0f - F0.y, 1.0f - F0.z } * std::pow(glm::clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
}

template<typename T>
Vec3<T>
reinhardToneMapping(const Vec3<T>& color)
{
    return color / (color + Vec3<T>(1.0f, 1.0f, 1.0f));
}

template<typename T>
Vec3<T>
gammaCorrect(const Vec3<T>& color, float gamma)
{
    return { std::pow(color.x, 1.0f / gamma), std::pow(color.y, 1.0f / gamma), std::pow(color.z, 1.0f / gamma) };
}

template<typename T>
inline T
LinearizeDepth(T depth, T zNearPlane, T zFarPlane)
{
    return (2.0 * zNearPlane) / (zFarPlane + zNearPlane - depth * (zFarPlane - zNearPlane));
}

template<typename T>
inline T
LinearToExponentialInvertedZ(T linearZ, T zNearPlane, T zFarPlane)
{
    const T factor      = 1.0;
    T       normalizedZ = (linearZ - zNearPlane) / (zFarPlane - zNearPlane);
    return 1.0 - exp(-normalizedZ * factor);
}

template<typename T>
inline T
ExponentialInvertedToLinearZ(T invExpZ)
{
    const T factor = 1.0;
    invExpZ        = std::min(invExpZ, T(0.999999));
    return -log(1.0 - invExpZ) / factor;
}

template<typename T>
inline T
ReverseExponentialZ(T linearZ, T zNearPlane, T zFarPlane)
{
    T z = (linearZ - zNearPlane) / (zFarPlane - zNearPlane);
    return 1.0 - pow(2.0, -10.0 * z);
}

template<typename T>
[[nodiscard]] T
calculateDepthZeroToOne(const Vec3<T>& barycentricCoordinates,
                        const Vec4<T>& projectedVert0,
                        const Vec4<T>& projectedVert1,
                        const Vec4<T>& projectedVert2,
                        T              zNear,
                        T              zFar)
{
    const T zNDC0 = projectedVert0.w;
    const T zNDC1 = projectedVert1.w;
    const T zNDC2 = projectedVert2.w;

    T depth = barycentricCoordinates.x * zNDC0 + barycentricCoordinates.y * zNDC1 + barycentricCoordinates.z * zNDC2;

    return LinearToExponentialInvertedZ(depth, zNear, zFar);
}

template<typename T>
[[nodiscard]] Vec3<T>
calculateRadiance(const Vec3<T>& vWorldPos,
                  const Vec3<T>& V,
                  const Vec3<T>& lightPos,
                  const Vec3<T>& lightColor,
                  float          lightIntensity,
                  const Vec3<T>& albedo,
                  const Vec3<T>& N,
                  const float    roughness,
                  const float    metallic,
                  const float    ao)
{
    Vec3<T> L           = glm::normalize(lightPos.glm - vWorldPos.glm);
    Vec3<T> H           = glm::normalize(V.glm + L.glm);
    float   distance    = glm::length(lightPos.glm - vWorldPos.glm);
    float   attenuation = 1.0f / (distance * distance);
    Vec3<T> radiance    = lightColor * lightIntensity * attenuation;

    float   NDF = DistributionGGX(N, H, roughness);
    float   G   = GeometrySmith(N, V, L, roughness);
    Vec3<T> F0  = Vec3<T>(0.04f, 0.04f, 0.04f);
    Vec3<T> F   = FresnelSchlick(std::max(H.dot(V), 0.0f), F0);

    Vec3<T> numerator   = NDF * G * F;
    float   denominator = 4.0f * std::max(N.dot(V), 0.0f) * std::max(N.dot(L), 0.0f);
    Vec3<T> specular    = numerator / std::max(denominator, 0.001f);

    Vec3<T> kS = F;
    Vec3<T> kD = 1.0f - kS;
    kD         = kD * (1.0f - metallic);

    float NdotL = std::max(N.dot(L), 0.0f);

    return (kD * albedo + specular) * radiance * NdotL * ao;
}

template<typename T>
[[nodiscard]] Vec3<T>
computeSpotLightContribution(const Vec3<T>&  V,
                             const Vec3<T>&  point,
                             const Vec3<T>&  N,
                             const Light<T>& light,
                             const Vec3<T>&  BaseColor,
                             const Vec3<T>&  EmissionColor,
                             const float     Metallic,
                             const float     Roughness,
                             const float     AO)
{
    auto lightDir = light.location - point;
    lightDir.normalize();

    auto spotlightDir = light.direction;
    spotlightDir.normalize();

    float cosAngle = -lightDir.dot(spotlightDir);
    float coneCos  = std::cos(light.angleOuterCone);

    if (cosAngle < coneCos) {
        Vec3<T> radiance = light.ambient * light.intensity + BaseColor;

        return radiance;
    }

    float fallOff = std::pow((cosAngle - coneCos) / (1.0f - coneCos), light.attenuationConstant);

    Vec3<T> diffuseIntensity = std::max(0.0f, N.dot(lightDir)) * light.diffuse;

    Vec3<T> reflectDir        = 2.0f * N.dot(lightDir) * N - lightDir;
    Vec3<T> specularIntensity = std::pow(std::max(0.0f, V.dot(reflectDir)), 1.0f) * light.specular;

    Vec3<T> radiance = calculateRadiance(point,
                                         V,
                                         light.location,
                                         fallOff * (diffuseIntensity + specularIntensity),
                                         light.intensity,
                                         BaseColor,
                                         N,
                                         Roughness,
                                         Metallic,
                                         AO);

    return radiance;
}

template<typename T>
[[nodiscard]] Vec3<T>
computePointLightContribution(const Vec3<T>&  V,
                              const Vec3<T>&  point,
                              const Vec3<T>&  N,
                              const Light<T>& light,
                              const Vec3<T>&  BaseColor,
                              const Vec3<T>&  EmissionColor,
                              const float     Metallic,
                              const float     Roughness,
                              const float     AO)
{
    Vec3<T> ambient = light.ambient * light.intensity;

    Vec3<T> lightDir = (light.location - point);
    lightDir.normalize();
    float   diff    = std::max(N.dot(lightDir), 0.0f);
    Vec3<T> diffuse = light.diffuse * diff * light.intensity;

    Vec3<T> halfwayDir = (lightDir + V);
    halfwayDir.normalize();
    float   spec     = std::pow(std::max(N.dot(halfwayDir), 0.0f), 1.0f);
    Vec3<T> specular = light.specular * spec * light.intensity;

    float distance    = glm::length((light.location - point).glm);
    float attenuation = 1.0f / (light.attenuationConstant + light.attenuationLinear * distance +
                                light.attenuationQuadratic * (distance * distance));

    Vec3<T> radiance = calculateRadiance(point,
                                         V,
                                         light.location,
                                         ambient + (diffuse + specular) * attenuation,
                                         light.intensity,
                                         BaseColor,
                                         N,
                                         Roughness,
                                         Metallic,
                                         AO);

    return radiance;
}

template<typename T>
[[nodiscard]] Vec3<T>
computeDirectionalLightContribution(const Vec3<T>&  V,
                                    const Vec3<T>&  point,
                                    const Vec3<T>&  N,
                                    const Light<T>& light,
                                    const Vec3<T>&  BaseColor,
                                    const Vec3<T>&  EmissionColor,
                                    const float     Metallic,
                                    const float     Roughness,
                                    const float     AO)
{
    Vec3<T> ambient = light.ambient * light.intensity;

    float   diff    = std::max(N.dot(-1.0f * light.direction), 0.0f);
    Vec3<T> diffuse = light.diffuse * diff * light.intensity;

    Vec3<T> halfwayDir = (-1.0f * light.direction + V);
    halfwayDir.normalize();
    float   spec     = std::pow(std::max(N.dot(halfwayDir), 0.0f), 1.0f);
    Vec3<T> specular = light.specular * spec * light.intensity;

    Vec3<T> radiance = calculateRadiance(
      point, V, light.location, ambient + diffuse + specular, light.intensity, BaseColor, N, Roughness, Metallic, AO);

    return radiance;
}

template<typename T>
void
getPBRMaterial(const Material<T>& material,
               const Vec2<T>&     texCoord,
               const Vec3<T>&     vertexBaseColor,
               const Vec3<T>&     vertexNormal,
               Vec3<T>&           baseColor,
               Vec3<T>&           normal,
               Vec3<T>&           emission,
               float&             metallic,
               float&             roughness,
               float&             ao,
               bool               repeatUVCoords)
{
    Vec4<T> baseColorTexture =
      material.hasBaseColorTexture
        ? sampleTextureLinear<T>(material.baseColorTexture, texCoord.x, texCoord.y, repeatUVCoords, true)
        : Vec4<T>{ vertexBaseColor.x, vertexBaseColor.y, vertexBaseColor.z, 1.0f };
    Vec4<T> normalTexture =
      material.hasNormalMapTexture
        ? 2.0f * sampleTextureLinear<T>(material.normalMapTexture, texCoord.x, texCoord.y, repeatUVCoords, false) - 1.0f
        : Vec4<T>{ 1.0f, 1.0f, 1.0f, 1.0f };
    Vec4<T> emissionTexture =
      material.hasEmissionColorTexture
        ? sampleTextureLinear<T>(material.emissionColorTexture, texCoord.x, texCoord.y, repeatUVCoords, false)
        : Vec4<T>{ 1.0f, 1.0f, 1.0f, 1.0f };
    Vec4<T> metallicTexture =
      material.hasMetallicTexture
        ? sampleTextureLinear<T>(material.metallicTexture, texCoord.x, texCoord.y, repeatUVCoords, true)
        : Vec4<T>{ 1.0f, 1.0f, 1.0f, 1.0f };
    Vec4<T> roughnessTexture =
      material.hasRoughnessTexture
        ? sampleTextureLinear<T>(material.roughnessTexture, texCoord.x, texCoord.y, repeatUVCoords, true)
        : Vec4<T>{ 1.0f, 1.0f, 1.0f, 1.0f };
    Vec4<T> aoTexture = material.hasAOTexture
                          ? sampleTextureLinear<T>(material.aoTexture, texCoord.x, texCoord.y, repeatUVCoords, false)
                          : Vec4<T>{ 1.0f, 1.0f, 1.0f, 1.0f };

    baseColor = material.hasBaseColorTexture ? (baseColorTexture).xyz : vertexBaseColor;
    normal    = material.hasNormalMapTexture ? (normalTexture).xyz : vertexNormal;
    normal.normalize();
    emission  = material.hasEmissionColorTexture ? (emissionTexture).xyz : material.emissionColorValue;
    metallic  = material.hasMetallicTexture ? (metallicTexture).x : material.metallicValue;
    roughness = material.hasRoughnessTexture ? (roughnessTexture).x : material.roughnessValue;
    ao        = material.hasAOTexture ? (aoTexture).x : material.aoValue;
}
