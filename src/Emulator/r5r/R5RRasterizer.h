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

#include "R5RLogger.h"
#include "R5RMaths.h"
#include "R5RRaytracer.h"
#include "R5RRenderer.h"
#include "R5RRendererCommon.h"
#include "R5RSceneDescriptor.h"
#include "R5RThreadPool.h"

#include <array>
#include <functional>
#include <iostream>
#include <sstream>

struct R5RRenderer;

#define MAX_CLIPPED_TRIANGLE_VERTICES (3 * 3)

template<typename T>
struct R5RRasterizer
{
    explicit R5RRasterizer(R5RRenderer* renderer)
      : renderer(renderer)
      , scene(nullptr)
    {
        // generate BRDF Texture
        // generateBRDFTexture();
        // saveToEXR(brdfTexture, "brdf");
        // destroyBRDFTexture();

        // allocate memory to be used during frame rasterization
        // frameMemoryEnd   = 8 * 1024 * 1024 * sizeof(uint8_t);
        // frameMemory      = static_cast<uint8_t*>(malloc(frameMemoryEnd));
        // frameMemoryStart = 0;

#if defined(R5R_USE_THREADS)
        threadPool = new R5RThreadPool(std::thread::hardware_concurrency());
#endif
    }
    ~R5RRasterizer()
    {
        // ASSERT_ERROR(frameMemoryStart == 0, "Memory pool wasn't properly cleared out");
        // free(frameMemory);
        // frameMemory    = nullptr;
        // frameMemoryEnd = 0;

#if defined(R5R_USE_THREADS)
        threadPool->waitForWork();
        delete threadPool;
#endif
    }
    void generateBRDFTexture()
    {
        brdfTexture.isHDR    = true;
        brdfTexture.width    = 512;
        brdfTexture.height   = brdfTexture.width;
        brdfTexture.channels = 3;
        brdfTexture.f32data =
          (float*)malloc(sizeof(float) * brdfTexture.width * brdfTexture.height * brdfTexture.channels);

        // http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
        // efficient VanDerCorpus calculation.
        auto RadicalInverse_VdC = [](unsigned int bits) -> float {
            bits = (bits << 16u) | (bits >> 16u);
            bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
            bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
            bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
            bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
            return float(bits) * 2.3283064365386963e-10; // / 0x100000000
        };

        auto Hammersley = [&RadicalInverse_VdC](unsigned int i, unsigned int N) -> Vec2<T> {
            return Vec2<T>(float(i) / float(N), RadicalInverse_VdC(i));
        };

        auto ImportanceSampleGGX = [](Vec2<T> Xi, Vec3<T> N, float roughness) -> Vec3<T> {
            float a        = roughness * roughness;
            float phi      = 2.0f * static_cast<float>(M_PI) * Xi.x;
            float cosTheta = std::sqrt((1.0f - Xi.y) / (1.0f + (a * a - 1.0f) * Xi.y));
            float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);

            Vec3<T> H;
            H.x = cos(phi) * sinTheta;
            H.y = sin(phi) * sinTheta;
            H.z = cosTheta;

            Vec3<T> up      = abs(N.z) < 0.999 ? Vec3<T>(0.0, 0.0, 1.0) : Vec3<T>(1.0, 0.0, 0.0);
            Vec3<T> tangent = up.cross(N);
            tangent.normalize();
            Vec3<T> bitangent = N.cross(tangent);
            Vec3<T> sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
            sampleVec.normalize();
            return sampleVec;
        };
        auto GeometrySchlickGGX = [](float NdotV, float roughness) -> float {
            // note that we use a different k for IBL
            float a = roughness;
            float k = (a * a) / 2.0;

            float nom   = NdotV;
            float denom = NdotV * (1.0 - k) + k;

            return nom / denom;
        };
        // ----------------------------------------------------------------------------
        auto GeometrySmith = [&GeometrySchlickGGX](Vec3<T> N, Vec3<T> V, Vec3<T> L, float roughness) -> float {
            float NdotV = std::max(N.dot(V), 0.0f);
            float NdotL = std::max(N.dot(L), 0.0f);
            float ggx2  = GeometrySchlickGGX(NdotV, roughness);
            float ggx1  = GeometrySchlickGGX(NdotL, roughness);

            return ggx1 * ggx2;
        };
        // ----------------------------------------------------------------------------
        auto IntegrateBRDF = [&Hammersley, &ImportanceSampleGGX, &GeometrySmith](
                               float NdotV, float roughness, unsigned int sampleCount) -> Vec2<T> {
            Vec3<T> V;
            V.x = sqrt(1.0 - NdotV * NdotV);
            V.y = 0.0;
            V.z = NdotV;

            float A = 0.0;
            float B = 0.0;

            Vec3<T> N = Vec3<T>(0.0, 0.0, 1.0);

            const unsigned int SAMPLE_COUNT = sampleCount;
            for (unsigned int i = 0u; i < SAMPLE_COUNT; ++i) {
                // generates a sample vector that's biased towards the
                // preferred alignment direction (importance sampling).
                Vec2<T> Xi = Hammersley(i, SAMPLE_COUNT);
                Vec3<T> H  = ImportanceSampleGGX(Xi, N, roughness);
                Vec3<T> L  = H * V.dot(H) * 2.0f - V;
                L.normalize();

                float NdotL = std::max(L.z, 0.0f);
                float NdotH = std::max(H.z, 0.0f);
                float VdotH = std::max(V.dot(H), 0.0f);

                if (NdotL > 0.0) {
                    float G     = GeometrySmith(N, V, L, roughness);
                    float G_Vis = (G * VdotH) / (NdotH * NdotV);
                    float Fc    = std::pow(1.0f - VdotH, 5.0f);

                    A += (1.0 - Fc) * G_Vis;
                    B += Fc * G_Vis;
                }
            }
            A /= float(SAMPLE_COUNT);
            B /= float(SAMPLE_COUNT);
            return Vec2<T>(A, B);
        };

        for (size_t hi = 0; hi < brdfTexture.height; ++hi) {
            for (size_t wi = 0; wi < brdfTexture.width; ++wi) {
                Vec2<T> coord          = { static_cast<float>((double)wi / (double)brdfTexture.width),
                                           static_cast<float>((double)hi / (double)brdfTexture.height) };
                Vec2<T> integratedBRDF = IntegrateBRDF(coord.x, coord.y, brdfTexture.width);
                brdfTexture.f32rgbpixels[hi * brdfTexture.width + wi] =
                  fvec3{ integratedBRDF.x, integratedBRDF.y, 0.0f };
            }
        }
    }
    void destroyBRDFTexture()
    {
        free(brdfTexture.u8data);
        brdfTexture.u8data   = nullptr;
        brdfTexture.width    = 0;
        brdfTexture.height   = 0;
        brdfTexture.channels = 0;
    }
    // [[nodiscard]] uint8_t* pushFrameMemory(size_t numBytes)
    // {
    //     if (frameMemoryStart + numBytes <= frameMemoryEnd) {
    //         uint8_t* ptr = &frameMemory[frameMemoryStart];
    //         memset(ptr, 0, numBytes);
    //         frameMemoryStart += numBytes;
    //         return ptr;
    //     }
    //     ASSERT_ERROR(false, "Memory full, you need to expand the maximum size of a frame memory");
    //     return nullptr;
    // }
    // void popFrameMemory(size_t numBytes)
    // {
    //     ASSERT_ERROR((frameMemoryStart - static_cast<int64_t>(numBytes)) >= 0,
    //                  "You need to push memory first then return it back via calling this function");
    //     frameMemoryStart -= numBytes;
    // }
    void                  setScene(Scene<T>* scene) { this->scene = scene; }
    [[nodiscard]] Vec4<T> interpolateVec4(const Vec3<T>& barycentricCoordinates,
                                          const Vec4<T>& v0,
                                          const Vec4<T>& v1,
                                          const Vec4<T>& v2)
    {
        return Vec4<T>{
            barycentricCoordinates.x * v0.x + barycentricCoordinates.y * v1.x + barycentricCoordinates.z * v2.x,
            barycentricCoordinates.x * v0.y + barycentricCoordinates.y * v1.y + barycentricCoordinates.z * v2.y,
            barycentricCoordinates.x * v0.z + barycentricCoordinates.y * v1.z + barycentricCoordinates.z * v2.z,
            barycentricCoordinates.x * v0.w + barycentricCoordinates.y * v1.w + barycentricCoordinates.z * v2.w
        };
    }
    [[nodiscard]] Vec3<T> interpolateVec3(const Vec3<T>& barycentricCoordinates,
                                          const Vec3<T>& v0,
                                          const Vec3<T>& v1,
                                          const Vec3<T>& v2)
    {
        return Vec3<T>{
            barycentricCoordinates.x * v0.x + barycentricCoordinates.y * v1.x + barycentricCoordinates.z * v2.x,
            barycentricCoordinates.x * v0.y + barycentricCoordinates.y * v1.y + barycentricCoordinates.z * v2.y,
            barycentricCoordinates.x * v0.z + barycentricCoordinates.y * v1.z + barycentricCoordinates.z * v2.z
        };
    }
    [[nodiscard]] Vec2<T> interpolateVec2(const Vec3<T>& barycentricCoordinates,
                                          const Vec2<T>& v0,
                                          const Vec2<T>& v1,
                                          const Vec2<T>& v2)
    {
        return Vec2<T>{
            barycentricCoordinates.x * v0.x + barycentricCoordinates.y * v1.x + barycentricCoordinates.z * v2.x,
            barycentricCoordinates.x * v0.y + barycentricCoordinates.y * v1.y + barycentricCoordinates.z * v2.y
        };
    }
    [[nodiscard]] static BoundingSquare<int64_t> calculateTriangleBoundingSquare(const Vec4<T>& viewport,
                                                                                 const Vec2<T>& v0,
                                                                                 const Vec2<T>& v1,
                                                                                 const Vec2<T>& v2)
    {
        int64_t minX = std::max(T(viewport.x), std::min({ v0.x, v1.x, v2.x }));
        int64_t maxX = std::min(static_cast<T>(viewport.z) - T(1), std::max({ v0.x, v1.x, v2.x }));
        int64_t minY = std::max(T(viewport.y), std::min({ v0.y, v1.y, v2.y }));
        int64_t maxY = std::min(static_cast<T>(viewport.w) - T(1), std::max({ v0.y, v1.y, v2.y }));
        return { Vec2<int64_t>(minX, minY), Vec2<int64_t>(maxX, maxY) };
    }
    // Helper: Compute barycentric weights for a pixel
    [[nodiscard]] static Vec3<T> computeBarycentricCoordinates(const Vec2<T>& A,
                                                               const Vec2<T>& B,
                                                               const Vec2<T>& C,
                                                               float          px,
                                                               float          py)
    {
        Vec3<T>     barycentricCoordinates;
        const float denominator  = (B.y - C.y) * (A.x - C.x) + (C.x - B.x) * (A.y - C.y);
        barycentricCoordinates.x = ((B.y - C.y) * (px - C.x) + (C.x - B.x) * (py - C.y)) / denominator;
        barycentricCoordinates.y = ((C.y - A.y) * (px - C.x) + (A.x - C.x) * (py - C.y)) / denominator;
        barycentricCoordinates.z = 1.0f - barycentricCoordinates.x - barycentricCoordinates.y;
        return barycentricCoordinates;
    }
    // Interpolate vertex attributes using barycentric weights (u, v, w)
    static void interpolateVertex(const Vec3<T>&                                  barycentricCoordinates,
                                  const std::array<Vec4<T>, 3>&                   projectedVertices,
                                  const std::array<VertexFragmentVaryings<T>, 3>& inVFV,
                                  VertexFragmentVaryings<T>&                      outVFV)
    {
        float u = barycentricCoordinates.x;
        float v = barycentricCoordinates.y;
        float w = barycentricCoordinates.z;

        Vec4<T> out;
        // clang-format off

        // Perspective-correct interpolation: divide by w
        float denominator = 1.0f / (u / projectedVertices[0].w + v / projectedVertices[1].w + w / projectedVertices[2].w);

        // Position (needs perspective divide later)
        outVFV.fragPos.x = u * inVFV[0].fragPos.x + v * inVFV[1].fragPos.x + w * inVFV[2].fragPos.x;
        outVFV.fragPos.y = u * inVFV[0].fragPos.y + v * inVFV[1].fragPos.y + w * inVFV[2].fragPos.y;
        outVFV.fragPos.z = u * inVFV[0].fragPos.z + v * inVFV[1].fragPos.z + w * inVFV[2].fragPos.z;
        // outVFV.fragPos.w = (u * inVFV[0].fragPos.w + v * inVFV[1].fragPos.w + w * inVFV[2].fragPos.w) * denominator; // Actually 1.0, but kept for clarity

        // Normals (linear interpolation, no perspective)
        outVFV.fragNormal.x = u * inVFV[0].fragNormal.x + v * inVFV[1].fragNormal.x + w * inVFV[2].fragNormal.x;
        outVFV.fragNormal.y = u * inVFV[0].fragNormal.y + v * inVFV[1].fragNormal.y + w * inVFV[2].fragNormal.y;
        outVFV.fragNormal.z = u * inVFV[0].fragNormal.z + v * inVFV[1].fragNormal.z + w * inVFV[2].fragNormal.z;
        outVFV.fragNormal.normalize();

        // Tangents (linear interpolation, no perspective)
        outVFV.fragTangent.x = u * inVFV[0].fragTangent.x + v * inVFV[1].fragTangent.x + w * inVFV[2].fragTangent.x;
        outVFV.fragTangent.y = u * inVFV[0].fragTangent.y + v * inVFV[1].fragTangent.y + w * inVFV[2].fragTangent.y;
        outVFV.fragTangent.z = u * inVFV[0].fragTangent.z + v * inVFV[1].fragTangent.z + w * inVFV[2].fragTangent.z;
        outVFV.fragTangent.normalize();

        // BiTangents (linear interpolation, no perspective)
        outVFV.fragBiTangent.x = u * inVFV[0].fragBiTangent.x + v * inVFV[1].fragBiTangent.x + w * inVFV[2].fragBiTangent.x;
        outVFV.fragBiTangent.y = u * inVFV[0].fragBiTangent.y + v * inVFV[1].fragBiTangent.y + w * inVFV[2].fragBiTangent.y;
        outVFV.fragBiTangent.z = u * inVFV[0].fragBiTangent.z + v * inVFV[1].fragBiTangent.z + w * inVFV[2].fragBiTangent.z;
        outVFV.fragBiTangent.normalize();

        // UVs (perspective-correct)
        outVFV.fragTextureCoord.x = u * inVFV[0].fragTextureCoord.x + v * inVFV[1].fragTextureCoord.x + w * inVFV[2].fragTextureCoord.x;
        outVFV.fragTextureCoord.y = u * inVFV[0].fragTextureCoord.y + v * inVFV[1].fragTextureCoord.y + w * inVFV[2].fragTextureCoord.y;

        // Colors (linear interpolation, no perspective)
        outVFV.fragColor.x = u * inVFV[0].fragColor.x + v * inVFV[1].fragColor.x + w * inVFV[2].fragColor.x;
        outVFV.fragColor.y = u * inVFV[0].fragColor.y + v * inVFV[1].fragColor.y + w * inVFV[2].fragColor.y;
        outVFV.fragColor.z = u * inVFV[0].fragColor.z + v * inVFV[1].fragColor.z + w * inVFV[2].fragColor.z;

        // clang-format on
    }
    [[nodiscard]] static T calculateDepthZeroToOne(const Vec3<T>& barycentricCoordinates,
                                                   const Vec4<T>& projectedVert0,
                                                   const Vec4<T>& projectedVert1,
                                                   const Vec4<T>& projectedVert2,
                                                   T              zNear,
                                                   T              zFar)
    {
        const T zNDC0 = projectedVert0.w;
        const T zNDC1 = projectedVert1.w;
        const T zNDC2 = projectedVert2.w;

        T depth =
          barycentricCoordinates.x * zNDC0 + barycentricCoordinates.y * zNDC1 + barycentricCoordinates.z * zNDC2;

        return LinearToExponentialInvertedZ(depth, zNear, zFar);
    }
    [[nodiscard]] Vec3<T> calculateRadiance(const Vec3<T>& vWorldPos,
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
        // Calculate per-light radiance
        Vec3<T> L           = glm::normalize(lightPos.glm - vWorldPos.glm);
        Vec3<T> H           = glm::normalize(V.glm + L.glm);
        float   distance    = glm::length(lightPos.glm - vWorldPos.glm);
        float   attenuation = 1.0f / (distance * distance);
        Vec3<T> radiance    = lightColor * lightIntensity * attenuation;

        // Cook-Torrance BRDF
        float   NDF = DistributionGGX(N, H, roughness);
        float   G   = GeometrySmith(N, V, L, roughness);
        Vec3<T> F0  = Vec3<T>(0.04f, 0.04f, 0.04f);
        Vec3<T> F   = FresnelSchlick(std::max(H.dot(V), 0.0f), F0);

        Vec3<T> numerator   = NDF * G * F;
        float   denominator = 4.0f * std::max(N.dot(V), 0.0f) * std::max(N.dot(L), 0.0f);
        Vec3<T> specular    = numerator / std::max(denominator, 0.001f);

        // kS is equal to Fresnel
        Vec3<T> kS = F;
        // For energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        Vec3<T> kD = 1.0f - kS;
        // Multiply kD by the inverse metalness such that only non-metals
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD = kD * (1.0f - metallic);

        // Scale light by NdotL
        float NdotL = std::max(N.dot(L), 0.0f);

        // Add to outgoing radiance Lo
        return (kD * albedo + specular) * radiance * NdotL * ao;
    }
    [[nodiscard]] Vec3<T> computeSpotLightContribution(const R5RRaytracer<T>& raytracer,
                                                       const Vec3<T>&         V,
                                                       const Vec3<T>&         point,
                                                       const Vec3<T>&         N,
                                                       const Light<T>&        light,
                                                       const fvec3&           BaseColor,
                                                       const fvec3&           EmissionColor,
                                                       const float            Metallic,
                                                       const float            Roughness,
                                                       const float            AO)
    {
        auto lightDir = light.location - point;
        lightDir.normalize();

        auto spotlightDir = light.direction;
        spotlightDir.normalize();

        // Check if point is inside the spotlight cone
        float cosAngle = -lightDir.dot(spotlightDir);
        float coneCos  = std::cos(light.angleOuterCone);

        if (cosAngle < coneCos) {
            // Outside spotlight cone → only ambient light
            Vec3<T> radiance = light.ambient * light.intensity + BaseColor;

#if defined(R5R_RASTERIZER_RAYTRACED_SHADOWS)
            Ray<T> lightRay  = {};
            auto   direction = point - light.location;
            direction.normalize();
            lightRay.start = light.location;
            lightRay.end   = point - direction * REFLECTED_RAY_MIN_OFFSET;
            R5RRaytracerHitPoint<T> hit;
            raytracer.raycast(lightRay, hit);
            if (hit.mesh != nullptr) { radiance = radiance * 0.1f; }
#endif

            return radiance;
        }

        // Spotlight falloff (smooth transition at edges)
        float fallOff = std::pow((cosAngle - coneCos) / (1.0f - coneCos), light.attenuationConstant);

        // Diffuse component (Lambertian reflectance)
        Vec3<T> diffuseIntensity = std::max(0.0f, N.dot(lightDir)) * light.diffuse;

        // Specular component (Phong reflection)
        Vec3<T> reflectDir        = 2.0f * N.dot(lightDir) * N - lightDir;
        Vec3<T> specularIntensity = std::pow(std::max(0.0f, V.dot(reflectDir)), 1.0f) * light.specular;

        // FINAL
        // ambient + fallOff * (diffuseIntensity + specularIntensity)
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
#if defined(R5R_RASTERIZER_RAYTRACED_SHADOWS)
        Ray<T> lightRay  = {};
        auto   direction = point - light.location;
        direction.normalize();
        lightRay.start = light.location;
        lightRay.end   = point - direction * REFLECTED_RAY_MIN_OFFSET;
        R5RRaytracerHitPoint<T> hit;
        raytracer.raycast(lightRay, hit);
        if (hit.mesh != nullptr) { radiance = radiance * 0.1f; }
#endif

        return radiance;
    }

    [[nodiscard]] Vec3<T> computePointLightContribution(const R5RRaytracer<T>& raytracer,
                                                        const Vec3<T>&         V,
                                                        const Vec3<T>&         point,
                                                        const Vec3<T>&         N,
                                                        const Light<T>&        light,
                                                        const fvec3&           BaseColor,
                                                        const fvec3&           EmissionColor,
                                                        const float            Metallic,
                                                        const float            Roughness,
                                                        const float            AO)
    {
        // Ambient lighting
        Vec3<T> ambient = light.ambient * light.intensity;

        // Diffuse lighting
        Vec3<T> lightDir = (light.location - point);
        lightDir.normalize();
        float   diff    = std::max(N.dot(lightDir), 0.0f);
        Vec3<T> diffuse = light.diffuse * diff * light.intensity;

        // Specular lighting (Blinn-Phong)
        Vec3<T> halfwayDir = (lightDir + V);
        halfwayDir.normalize();
        float   spec     = std::pow(std::max(N.dot(halfwayDir), 0.0f), 1.0f);
        Vec3<T> specular = light.specular * spec * light.intensity;

        // Attenuation (light falloff with distance)
        float distance    = glm::length((light.location - point).glm);
        float attenuation = 1.0f / (light.attenuationConstant + light.attenuationLinear * distance +
                                    light.attenuationQuadratic * (distance * distance));

        // FINAL
        // ambient + (diffuse + specular) * attenuation;
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
#if defined(R5R_RASTERIZER_RAYTRACED_SHADOWS)
        Ray<T> lightRay  = {};
        auto   direction = point - light.location;
        direction.normalize();
        lightRay.start = light.location;
        lightRay.end   = point - direction * REFLECTED_RAY_MIN_OFFSET;
        R5RRaytracerHitPoint<T> hit;
        raytracer.raycast(lightRay, hit);
        if (hit.mesh != nullptr) { radiance = radiance * 0.1f; }
#endif

        return radiance;
    }

    [[nodiscard]] Vec3<T> computeDirectionalLightContribution(const R5RRaytracer<T>& raytracer,
                                                              const Vec3<T>&         V,
                                                              const Vec3<T>&         point,
                                                              const Vec3<T>&         N,
                                                              const Light<T>&        light,
                                                              const fvec3&           BaseColor,
                                                              const fvec3&           EmissionColor,
                                                              const float            Metallic,
                                                              const float            Roughness,
                                                              const float            AO)
    {
        // Ambient lighting (constant)
        Vec3<T> ambient = light.ambient * light.intensity;

        // Diffuse lighting (Lambert's cosine law)
        float   diff    = std::max(N.dot(-1.0f * light.direction), 0.0f);
        Vec3<T> diffuse = light.diffuse * diff * light.intensity;

        // Specular lighting (Blinn-Phong)
        Vec3<T> halfwayDir = (-1.0f * light.direction + V);
        halfwayDir.normalize();
        float   spec     = std::pow(std::max(N.dot(halfwayDir), 0.0f), 1.0f);
        Vec3<T> specular = light.specular * spec * light.intensity;

        // FINAL
        // ambient + diffuse + specular
        Vec3<T> radiance = calculateRadiance(point,
                                             V,
                                             light.location,
                                             ambient + diffuse + specular,
                                             light.intensity,
                                             BaseColor,
                                             N,
                                             Roughness,
                                             Metallic,
                                             AO);
#if defined(R5R_RASTERIZER_RAYTRACED_SHADOWS)
        Ray<T> lightRay  = {};
        auto   direction = point - light.location;
        direction.normalize();
        lightRay.start = light.location;
        lightRay.end   = point - direction * REFLECTED_RAY_MIN_OFFSET;
        R5RRaytracerHitPoint<T> hit;
        raytracer.raycast(lightRay, hit);
        if (hit.mesh != nullptr) { radiance = radiance * 0.1f; }
#endif

        return radiance;
    }
    [[nodiscard]] Vec4<float> fragmentShader(R5RThreadPoolMemory&                 tpm,
                                             const Camera<T>&                     camera,
                                             const Material<T>&                   material,
                                             const VertexFragmentVaryings<T>&     vertexFragmentVaryings,
                                             const VertexFragmentFlatVaryings<T>& vertexFragmentFlatVaryings)
    {
        // -------------------------------------------------------------------------------------------------------------
        // VERTEX/FRAGMENT SHADER INTERPOLATED VALUES
        // -------------------------------------------------------------------------------------------------------------
        const std::vector<ELightType>& LightType                 = vertexFragmentFlatVaryings.lightType;
        const std::vector<float>&      LightAttenuationConstant  = vertexFragmentFlatVaryings.lightAttenuationConstant;
        const std::vector<float>&      LightAttenuationLinear    = vertexFragmentFlatVaryings.lightAttenuationLinear;
        const std::vector<float>&      LightAttenuationQuadratic = vertexFragmentFlatVaryings.lightAttenuationQuadratic;
        const std::vector<float>&      LightAngleOuterCone       = vertexFragmentFlatVaryings.lightAngleOuterCone;
        const std::vector<Vec3<T>>&    LightPos                  = vertexFragmentFlatVaryings.lightPos;
        const std::vector<Vec3<T>>&    LightDirection            = vertexFragmentFlatVaryings.lightDirection;
        const std::vector<Vec3<T>>&    LightColor                = vertexFragmentFlatVaryings.lightColor;
        const std::vector<float>&      LightIntensity            = vertexFragmentFlatVaryings.lightIntensity;
        const Vec3<T>&                 ViewPos                   = vertexFragmentFlatVaryings.viewPos;
        const Vec4<T>&                 FragPos                   = vertexFragmentVaryings.fragPos;
        const Vec3<T>&                 FragNormal                = vertexFragmentVaryings.fragNormal;
        const Vec3<T>&                 FragTangent               = vertexFragmentVaryings.fragTangent;
        const Vec3<T>&                 FragBiTangent             = vertexFragmentVaryings.fragBiTangent;
        const Vec2<T>&                 FragTexCoord              = vertexFragmentVaryings.fragTextureCoord;
        const Vec3<T>&                 FragColor                 = vertexFragmentVaryings.fragColor;
        // -------------------------------------------------------------------------------------------------------------

        fvec3 BaseColor;
        fvec3 N;
        fvec3 EmissionColor;
        float Metallic;
        float Roughness;
        float AO;
        getPBRMaterial(
          material, FragTexCoord, FragColor, FragNormal, BaseColor, N, EmissionColor, Metallic, Roughness, AO, true);
        // -------------------------------------------------------------------------------------------------------------

        Vec3<T> V = ViewPos - FragPos.xyz;
        V.normalize();

        Vec3<T> Lo = { 0.0f, 0.0f, 0.0f };
        for (size_t li = 0; li < LightColor.size(); ++li) {
            if (scene->lights[li].type == ELightType_Spot) {
                R5RRaytracer<T>* raytracer = nullptr;
#if defined(R5R_RASTERIZER_RAYTRACED_SHADOWS)
                raytracer = renderer->raytracer;
#endif
                Lo = Lo + computeSpotLightContribution(*raytracer,
                                                       V,
                                                       FragPos.xyz,
                                                       N,
                                                       scene->lights[li],
                                                       BaseColor,
                                                       EmissionColor,
                                                       Metallic,
                                                       Roughness,
                                                       AO);
            } else if (scene->lights[li].type == ELightType_Point) {
                R5RRaytracer<T>* raytracer = nullptr;
#if defined(R5R_RASTERIZER_RAYTRACED_SHADOWS)
                raytracer = renderer->raytracer;
#endif
                Lo = Lo + computePointLightContribution(*raytracer,
                                                        V,
                                                        FragPos.xyz,
                                                        N,
                                                        scene->lights[li],
                                                        BaseColor,
                                                        EmissionColor,
                                                        Metallic,
                                                        Roughness,
                                                        AO);
            } else if (scene->lights[li].type == ELightType_Directional) {
                R5RRaytracer<T>* raytracer = nullptr;
#if defined(R5R_RASTERIZER_RAYTRACED_SHADOWS)
                raytracer = renderer->raytracer;
#endif
                Lo = Lo + computeDirectionalLightContribution(*raytracer,
                                                              V,
                                                              FragPos.xyz,
                                                              N,
                                                              scene->lights[li],
                                                              BaseColor,
                                                              EmissionColor,
                                                              Metallic,
                                                              Roughness,
                                                              AO);
            } else {
                ASSERT_ERROR(false, "Unreachable");
            }
        }

        // Final color
        Vec3<T> color      = Lo;
        color              = gammaCorrect(reinhardToneMapping(color), 2.2);
        Vec4<T> finalcolor = Vec4<T>{ color.x, color.y, color.z, 1.0f };
        return finalcolor;
    }
    void drawTriangle(R5RThreadPoolMemory&                            tpm,
                      RasterizerEventListener&                        listener,
                      const std::array<Vec4<T>, 3>&                   projectedVertices,
                      const Camera<T>&                                camera,
                      unsigned int                                    materialIndex,
                      const std::array<VertexFragmentVaryings<T>, 3>& vertexFragmentVaryings,
                      const VertexFragmentFlatVaryings<T>&            vertexFragmentFlatVaryings)
    {
        // Avoid degenerate triangles
        // -------------------------------------------------------------------------------------
        T area = Triangle<T>::area(projectedVertices[0].xy, projectedVertices[1].xy, projectedVertices[2].xy);
        if (area == 0) { return; }
        // ----------------------------------------------------------------------------------------------------------------

        // bounding square around triangle
        // --------------------------------------------------------------------------------
        BoundingSquare<int64_t> bs = calculateTriangleBoundingSquare(
          Vec4<T>{ 0, 0, static_cast<T>(camera.resolution.x), static_cast<T>(camera.resolution.y) },
          projectedVertices[0].xy,
          projectedVertices[1].xy,
          projectedVertices[2].xy);
        // ----------------------------------------------------------------------------------------------------------------

        // loop over bounding square
        for (int64_t y = bs.min.y; y <= bs.max.y; ++y) {
            for (int64_t x = bs.min.x; x <= bs.max.x; ++x) {
                // Compute barycentric coordinates for (x, y)
                Vec3<T> barycentricCoordinates = computeBarycentricCoordinates(
                  projectedVertices[0].xy, projectedVertices[1].xy, projectedVertices[2].xy, x, y);

                if (barycentricCoordinates.x >= 0 && barycentricCoordinates.y >= 0 && barycentricCoordinates.z >= 0) {
                    // point is inside the triangle
                    T d = calculateDepthZeroToOne(barycentricCoordinates,
                                                  projectedVertices[0],
                                                  projectedVertices[1],
                                                  projectedVertices[2],
                                                  camera.zNearPlane,
                                                  camera.zFarPlane);
                    ASSERT_ERROR(d >= 0.0 && d <= 1.0, "Depth should be between 0.0 and 1.0");
                    if (d <= camera.getDepthBufferPixel(x, y)) {
                        // Interpolate attributes ----------------------------------------------------------------------
                        VertexFragmentVaryings<T>* fragmentVaryings =
                          (VertexFragmentVaryings<T>*)tpm.pushFrameMemory(sizeof(VertexFragmentVaryings<T>));
                        interpolateVertex(
                          barycentricCoordinates, projectedVertices, vertexFragmentVaryings, *fragmentVaryings);
                        // ---------------------------------------------------------------------------------------------

                        Vec4<float> fragColor = fragmentShader(tpm,
                                                               camera,
                                                               scene->materials.at(materialIndex),
                                                               *fragmentVaryings,
                                                               vertexFragmentFlatVaryings);

                        camera.writeColorBuffer(fragColor, x, y);

                        // fragmentVaryings
                        tpm.popFrameMemory(sizeof(VertexFragmentVaryings<T>));
                    }
                }
            }
        }
    }
    // Clip against a single 3D plane (e.g., near/far)
    [[nodiscard]] int clipAgainstPlane3D(R5RThreadPoolMemory& tpm,
                                         const Vertex<T>*     input,
                                         int                  inputCount,
                                         Vertex<T>*           output,
                                         float                edge,
                                         int                  axis,
                                         int                  isMax,
                                         int                  isZForwardPositive)
    {
        int   outputCount = 0;
        auto& prevVertex  = *(Vertex<T>*)tpm.pushFrameMemory(sizeof(Vertex<T>));
        prevVertex        = input[inputCount - 1];
        float prevVal     = (axis == 0)   ? prevVertex.location.x
                            : (axis == 1) ? prevVertex.location.y
                                          : prevVertex.location.z;
        int   prevInside;

        // Handle near/far planes differently for -z vs. +z forward
        if (axis == 2 && !isZForwardPositive) {
            // OpenGL-style (-z forward): Near plane is z = -w, Far plane is z = w
            prevInside = isMax ? (prevVal <= edge * prevVertex.location.w) : (prevVal >= edge * prevVertex.location.w);
        } else {
            // Direct3D/Vulkan-style (+z forward): Near plane is z = 0, Far plane is z = w
            prevInside = isMax ? (prevVal <= edge * prevVertex.location.w) : (prevVal >= edge * prevVertex.location.w);
        }

        for (int i = 0; i < inputCount; i++) {
            auto& currentVertex = *(Vertex<T>*)tpm.pushFrameMemory(sizeof(Vertex<T>));
            currentVertex       = input[i];
            float currVal       = (axis == 0)   ? currentVertex.location.x
                                  : (axis == 1) ? currentVertex.location.y
                                                : currentVertex.location.z;
            int   currInside;

            if (axis == 2 && !isZForwardPositive) {
                currInside =
                  isMax ? (currVal <= edge * currentVertex.location.w) : (currVal >= edge * currentVertex.location.w);
            } else {
                currInside =
                  isMax ? (currVal <= edge * currentVertex.location.w) : (currVal >= edge * currentVertex.location.w);
            }

            if (currInside != prevInside) {
                // Compute intersection (lerp with perspective correction)
                float t = (edge * prevVertex.location.w - prevVal) /
                          ((edge * prevVertex.location.w - prevVal) - (edge * currentVertex.location.w - currVal));
                Vertex<T>& intersect = output[outputCount];
                intersect.location.x = prevVertex.location.x + t * (currentVertex.location.x - prevVertex.location.x);
                intersect.location.y = prevVertex.location.y + t * (currentVertex.location.y - prevVertex.location.y);
                intersect.location.z = prevVertex.location.z + t * (currentVertex.location.z - prevVertex.location.z);
                intersect.location.w = prevVertex.location.w + t * (currentVertex.location.w - prevVertex.location.w);
                intersect.normal.x   = prevVertex.normal.x + t * (currentVertex.normal.x - prevVertex.normal.x);
                intersect.normal.y   = prevVertex.normal.y + t * (currentVertex.normal.y - prevVertex.normal.y);
                intersect.normal.z   = prevVertex.normal.z + t * (currentVertex.normal.z - prevVertex.normal.z);
                intersect.coord.x    = prevVertex.coord.x + t * (currentVertex.coord.x - prevVertex.coord.x);
                intersect.coord.y    = prevVertex.coord.y + t * (currentVertex.coord.y - prevVertex.coord.y);
                intersect.tangent.x  = prevVertex.tangent.x + t * (currentVertex.tangent.x - prevVertex.tangent.x);
                intersect.tangent.y  = prevVertex.tangent.y + t * (currentVertex.tangent.y - prevVertex.tangent.y);
                intersect.tangent.z  = prevVertex.tangent.z + t * (currentVertex.tangent.z - prevVertex.tangent.z);
                intersect.biTangent.x =
                  prevVertex.biTangent.x + t * (currentVertex.biTangent.x - prevVertex.biTangent.x);
                intersect.biTangent.y =
                  prevVertex.biTangent.y + t * (currentVertex.biTangent.y - prevVertex.biTangent.y);
                intersect.biTangent.z =
                  prevVertex.biTangent.z + t * (currentVertex.biTangent.z - prevVertex.biTangent.z);
                intersect.color.x = prevVertex.color.x + t * (currentVertex.color.x - prevVertex.color.x);
                intersect.color.y = prevVertex.color.y + t * (currentVertex.color.y - prevVertex.color.y);
                intersect.color.z = prevVertex.color.z + t * (currentVertex.color.z - prevVertex.color.z);
                intersect.color.w = prevVertex.color.w + t * (currentVertex.color.w - prevVertex.color.w);
                outputCount++;
            }

            if (currInside) {
                output[outputCount] = currentVertex;
                outputCount++;
            }

            prevVertex = currentVertex;
            prevVal    = currVal;
            prevInside = currInside;

            tpm.popFrameMemory(sizeof(Vertex<T>));
        }
        tpm.popFrameMemory(sizeof(Vertex<T>));
        return outputCount;
    }
    void clipTriangle3D(R5RThreadPoolMemory&    tpm,
                        const Triangle<T>&      triangle,
                        std::vector<Vertex<T>>& clippedPolygon,
                        int                     isZForwardPositive)
    {
        auto* input  = (Vertex<T>*)tpm.pushFrameMemory(sizeof(Vertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
        auto* output = (Vertex<T>*)tpm.pushFrameMemory(sizeof(Vertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);

        int inputCount = 3;

        // Copy the triangle vertices to the input array
        input[0] = triangle.v0;
        input[1] = triangle.v1;
        input[2] = triangle.v2;

        // Clip against each of the 6 frustum planes
        // Define clip planes based on forward direction
        const float xPlanes[] = { -1.0f, 1.0f };                             // xmin, xmax
        const float yPlanes[] = { -1.0f, 1.0f };                             // ymin, ymax
        const float zPlanes[] = { isZForwardPositive ? 0.0f : -1.0f, 1.0f }; // zmin, zmax

        // Clip against each of the 6 frustum planes
        const float planes[] = { xPlanes[0], xPlanes[1], yPlanes[0], yPlanes[1], zPlanes[0], zPlanes[1] };
        const int   axes[]   = { 0, 0, 1, 1, 2, 2 }; // x, x, y, y, z, z

        for (int i = 0; i < 6; i++) {
            int outputCount =
              clipAgainstPlane3D(tpm, input, inputCount, output, planes[i], axes[i], i % 2, isZForwardPositive);
            if (outputCount == 0) {
                // Fully clipped
                clippedPolygon.clear();
                tpm.popFrameMemory(sizeof(Vertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
                tpm.popFrameMemory(sizeof(Vertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
                return;
            }
            memcpy((void*)input, (void*)output, outputCount * sizeof(Vertex<T>));
            inputCount = outputCount;
        }

        // Copy the final clipped vertices to the output polygon
        if (inputCount > clippedPolygon.size()) { clippedPolygon.resize(inputCount - clippedPolygon.size()); }
        for (int i = 0; i < inputCount; i++) { clippedPolygon[i] = input[i]; }

        tpm.popFrameMemory(sizeof(Vertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
        tpm.popFrameMemory(sizeof(Vertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
    }
    void clipTriangleToTriangles3D(R5RThreadPoolMemory&      tpm,
                                   const Triangle<T>&        triangle,
                                   std::vector<Triangle<T>>& clippedTriangles,
                                   int                       isZForwardPositive)
    {
        std::vector<Vertex<T>> clippedPolygon;
        clipTriangle3D(tpm, triangle, clippedPolygon, isZForwardPositive);

        if (clippedPolygon.size() < 3) {
            // No output (fully clipped)
            clippedTriangles.clear();
            return;
        }
        if (clippedPolygon.size() > 3) {
            // Triangulate the clipped polygon using a triangle fan
            clippedTriangles.resize(clippedPolygon.size() - 2);
            int numTriangles = 0;
            for (int i = 1; i < clippedPolygon.size() - 1; ++i) {
                clippedTriangles[numTriangles].v0 = clippedPolygon[0];
                clippedTriangles[numTriangles].v1 = clippedPolygon[i];
                clippedTriangles[numTriangles].v2 = clippedPolygon[i + 1];
                ++numTriangles;
            }
            return;
        }

        // same triangle, just use the originally passed triangle
        clippedTriangles.resize(1);
        clippedTriangles[0].v0 = clippedPolygon[0];
        clippedTriangles[0].v1 = clippedPolygon[1];
        clippedTriangles[0].v2 = clippedPolygon[2];
        return;
    }
    void clipStage(R5RThreadPoolMemory&                    tpm,
                   RasterizerEventListener&                listener,
                   const Triangle<T>&                      projectedVertices,
                   const Mat4<T>&                          modelMatrix,
                   const glm::mat<3, 3, T, glm::defaultp>& normalMatrix,
                   const Mat4<T>&                          viewProjectionMatrix,
                   const Camera<T>&                        camera,
                   unsigned int                            materialIndex)
    {
        // clipping
        std::vector<Triangle<T>> clippedTriangles;
        clipTriangleToTriangles3D(tpm, projectedVertices, clippedTriangles, 1);
        if (clippedTriangles.empty()) {
            // all triangle vertices are actually clipped
            return;
        }

        auto& inverseViewMatrix =
          *(glm::mat<4, 4, T, glm::defaultp>*)tpm.pushFrameMemory(sizeof(glm::mat<4, 4, T, glm::defaultp>));
        auto& inverseProjectionMatrix =
          *(glm::mat<4, 4, T, glm::defaultp>*)tpm.pushFrameMemory(sizeof(glm::mat<4, 4, T, glm::defaultp>));
        auto&                         worldTriangle = *(Triangle<T>*)tpm.pushFrameMemory(sizeof(Triangle<T>));
        VertexFragmentFlatVaryings<T> vertexFragmentFlatVaryings        = {};
        std::array<VertexFragmentVaryings<T>, 3> vertexFragmentVaryings = {};

        inverseViewMatrix       = glm::inverse(camera.viewMatrix.glm);
        inverseProjectionMatrix = glm::inverse(camera.projectionMatrix.glm);

        for (size_t i = 0; i < clippedTriangles.size(); ++i) {
            Triangle<T>& clippedTriangle = clippedTriangles[i];

            // get back world triangle
            worldTriangle                 = clippedTriangle;
            worldTriangle.v0.location.glm = inverseViewMatrix * inverseProjectionMatrix * worldTriangle.v0.location.glm;
            worldTriangle.v1.location.glm = inverseViewMatrix * inverseProjectionMatrix * worldTriangle.v1.location.glm;
            worldTriangle.v2.location.glm = inverseViewMatrix * inverseProjectionMatrix * worldTriangle.v2.location.glm;

            // perspective division
            clippedTriangle.v0.location.x /= clippedTriangle.v0.location.w;
            clippedTriangle.v0.location.y /= clippedTriangle.v0.location.w;
            clippedTriangle.v0.location.z /= clippedTriangle.v0.location.w;
            clippedTriangle.v1.location.x /= clippedTriangle.v1.location.w;
            clippedTriangle.v1.location.y /= clippedTriangle.v1.location.w;
            clippedTriangle.v1.location.z /= clippedTriangle.v1.location.w;
            clippedTriangle.v2.location.x /= clippedTriangle.v2.location.w;
            clippedTriangle.v2.location.y /= clippedTriangle.v2.location.w;
            clippedTriangle.v2.location.z /= clippedTriangle.v2.location.w;

            // viewport transformation
            clippedTriangle.v0.location.x = (clippedTriangle.v0.location.x + 1.0f) * 0.5f * WIDTH;
            clippedTriangle.v0.location.y = (1.0f - clippedTriangle.v0.location.y) * 0.5f * HEIGHT;
            clippedTriangle.v0.location.z =
              (clippedTriangle.v0.location.z + 1.0f) * 0.5f * (camera.zFarPlane - camera.zNearPlane) +
              camera.zNearPlane;
            clippedTriangle.v1.location.x = (clippedTriangle.v1.location.x + 1.0f) * 0.5f * WIDTH;
            clippedTriangle.v1.location.y = (1.0f - clippedTriangle.v1.location.y) * 0.5f * HEIGHT;
            clippedTriangle.v1.location.z =
              (clippedTriangle.v1.location.z + 1.0f) * 0.5f * (camera.zFarPlane - camera.zNearPlane) +
              camera.zNearPlane;
            clippedTriangle.v2.location.x = (clippedTriangle.v2.location.x + 1.0f) * 0.5f * WIDTH;
            clippedTriangle.v2.location.y = (1.0f - clippedTriangle.v2.location.y) * 0.5f * HEIGHT;
            clippedTriangle.v2.location.z =
              (clippedTriangle.v2.location.z + 1.0f) * 0.5f * (camera.zFarPlane - camera.zNearPlane) +
              camera.zNearPlane;

            // if (!camera.isBackFace(
            //       clippedTriangle.v0.location, clippedTriangle.v1.location, clippedTriangle.v2.location)) {

            // interpolate varyings ------------------------------------------------------------------------
            vertexFragmentFlatVaryings.lightType.resize(scene->lights.size());
            vertexFragmentFlatVaryings.lightAttenuationConstant.resize(scene->lights.size());
            vertexFragmentFlatVaryings.lightAttenuationLinear.resize(scene->lights.size());
            vertexFragmentFlatVaryings.lightAttenuationQuadratic.resize(scene->lights.size());
            vertexFragmentFlatVaryings.lightAngleOuterCone.resize(scene->lights.size());
            vertexFragmentFlatVaryings.lightPos.resize(scene->lights.size());
            vertexFragmentFlatVaryings.lightDirection.resize(scene->lights.size());
            vertexFragmentFlatVaryings.lightColor.resize(scene->lights.size());
            vertexFragmentFlatVaryings.lightIntensity.resize(scene->lights.size());
            vertexFragmentFlatVaryings.viewPos = camera.location;

            vertexFragmentVaryings[0].fragPos          = worldTriangle.v0.location;
            vertexFragmentVaryings[0].fragNormal       = worldTriangle.v0.normal;
            vertexFragmentVaryings[0].fragTangent      = worldTriangle.v0.tangent;
            vertexFragmentVaryings[0].fragBiTangent    = worldTriangle.v0.biTangent;
            vertexFragmentVaryings[0].fragTextureCoord = worldTriangle.v0.coord;
            vertexFragmentVaryings[0].fragColor        = worldTriangle.v0.color.xyz;
            vertexFragmentVaryings[1].fragPos          = worldTriangle.v1.location;
            vertexFragmentVaryings[1].fragNormal       = worldTriangle.v1.normal;
            vertexFragmentVaryings[1].fragTangent      = worldTriangle.v1.tangent;
            vertexFragmentVaryings[1].fragBiTangent    = worldTriangle.v1.biTangent;
            vertexFragmentVaryings[1].fragTextureCoord = worldTriangle.v1.coord;
            vertexFragmentVaryings[1].fragColor        = worldTriangle.v1.color.xyz;
            vertexFragmentVaryings[2].fragPos          = worldTriangle.v2.location;
            vertexFragmentVaryings[2].fragNormal       = worldTriangle.v2.normal;
            vertexFragmentVaryings[2].fragTangent      = worldTriangle.v2.tangent;
            vertexFragmentVaryings[2].fragBiTangent    = worldTriangle.v2.biTangent;
            vertexFragmentVaryings[2].fragTextureCoord = worldTriangle.v2.coord;
            vertexFragmentVaryings[2].fragColor        = worldTriangle.v2.color.xyz;
            for (size_t li = 0; li < scene->lights.size(); ++li) {
                vertexFragmentFlatVaryings.lightType[li]                 = scene->lights[li].type;
                vertexFragmentFlatVaryings.lightAttenuationConstant[li]  = scene->lights[li].attenuationConstant;
                vertexFragmentFlatVaryings.lightAttenuationLinear[li]    = scene->lights[li].attenuationLinear;
                vertexFragmentFlatVaryings.lightAttenuationQuadratic[li] = scene->lights[li].attenuationQuadratic;
                vertexFragmentFlatVaryings.lightAngleOuterCone[li]       = scene->lights[li].angleOuterCone;
                vertexFragmentFlatVaryings.lightPos[li]                  = scene->lights[li].location;
                vertexFragmentFlatVaryings.lightDirection[li]            = scene->lights[li].direction;
                vertexFragmentFlatVaryings.lightColor[li]                = scene->lights[li].diffuse;
                vertexFragmentFlatVaryings.lightIntensity[li]            = scene->lights[li].intensity;
            }
            // ---------------------------------------------------------------------------------------------
            std::array<Vec4<T>, 3> projectedVerticesToRasterize = { clippedTriangle.v0.location,
                                                                    clippedTriangle.v1.location,
                                                                    clippedTriangle.v2.location };
            drawTriangle(tpm,
                         listener,
                         projectedVerticesToRasterize,
                         camera,
                         materialIndex,
                         vertexFragmentVaryings,
                         vertexFragmentFlatVaryings);
            // }
        }
        // inverseViewMatrix
        tpm.popFrameMemory(sizeof(glm::mat<4, 4, T, glm::defaultp>));
        // inverseProjectionMatrix
        tpm.popFrameMemory(sizeof(glm::mat<4, 4, T, glm::defaultp>));
        // worldTriangle
        tpm.popFrameMemory(sizeof(Triangle<T>));
        // vertexFragmentFlatVaryings
        // tpm.popFrameMemory(sizeof(VertexFragmentFlatVaryings<T>));
        // vertexFragmentVaryings
        // tpm.popFrameMemory(sizeof(std::array<VertexFragmentVaryings<T>, 3>));
    }
    void vertexShader(R5RThreadPoolMemory&                    tpm,
                      RasterizerEventListener&                listener,
                      const Triangle<T>&                      t,
                      const Mat4<T>&                          modelMatrix,
                      const glm::mat<3, 3, T, glm::defaultp>& normalMatrix,
                      const Mat4<T>&                          viewProjectionMatrix,
                      const Camera<T>&                        camera,
                      unsigned int                            materialIndex)
    {
        auto& projectedVertices = *(Triangle<T>*)tpm.pushFrameMemory(sizeof(Triangle<T>));
        projectedVertices       = t;

        projectedVertices.v0.normal = normalMatrix * t.v0.normal.glm;
        projectedVertices.v1.normal = normalMatrix * t.v1.normal.glm;
        projectedVertices.v2.normal = normalMatrix * t.v2.normal.glm;

        projectedVertices.v0.location = viewProjectionMatrix * modelMatrix * t.v0.location;
        projectedVertices.v1.location = viewProjectionMatrix * modelMatrix * t.v1.location;
        projectedVertices.v2.location = viewProjectionMatrix * modelMatrix * t.v2.location;
        // Here, officially traditional vertex shader ends -------------------------------

        clipStage(
          tpm, listener, projectedVertices, modelMatrix, normalMatrix, viewProjectionMatrix, camera, materialIndex);

        // projectedVertices
        tpm.popFrameMemory(sizeof(Triangle<T>));
    }
    void renderFrame(RasterizerEventListener& listener, Camera<T>& camera)
    {
#ifndef R5R_USE_THREADS
        R5RThreadPoolMemory tpm(32 * 1024 * sizeof(uint8_t));
#endif
        camera.clearColorBuffer();
#ifdef __EMSCRIPTEN__
        listener.onFrameSetColorBufferPtr(camera.colorBuffer, camera.resolution.x, camera.resolution.y);
#endif
        const Mat4<T>&                   viewProjectionMatrix = camera.projectionMatrix * camera.viewMatrix;
        std::array<Plane<T>, 6>          frustumPlanes        = Plane<T>::extractFrustumPlanes(viewProjectionMatrix);
        glm::mat<3, 3, T, glm::defaultp> normalMatrix;
        for (int64_t mi = 0; mi < scene->meshes.size(); ++mi) {
            Mesh<T>& mesh = scene->meshes[mi];
            // frustum culling
            if (mesh.boundingBox.testFrustum(frustumPlanes) == EBoundingBoxFrustumTest_FullyOutside) {
                LOGV_DEBUG("[FRUSTUM CULLING ELIMINATED] {}", mesh.name);
                continue;
            }
            normalMatrix = glm::transpose(glm::inverse(glm::mat3(mesh.transform.glm)));
            for (size_t ii = 0; ii < mesh.indices.size() - 2; ii += 3) {
                uint32_t i0 = mesh.indices[ii];
                uint32_t i1 = mesh.indices[ii + 1];
                uint32_t i2 = mesh.indices[ii + 2];

                Triangle<T> tr  = {};
                tr.v0.location  = mesh.vertices[i0];
                tr.v0.normal    = mesh.normals[i0];
                tr.v0.coord     = mesh.texCoords[i0];
                tr.v0.color     = Vec4<T>{ mesh.colors[i0].x, mesh.colors[i0].y, mesh.colors[i0].z, 1.0f };
                tr.v0.tangent   = mesh.tangents[i0];
                tr.v0.biTangent = mesh.biTangents[i0];

                tr.v1.location  = mesh.vertices[i1];
                tr.v1.normal    = mesh.normals[i1];
                tr.v1.coord     = mesh.texCoords[i1];
                tr.v1.color     = Vec4<T>{ mesh.colors[i1].x, mesh.colors[i1].y, mesh.colors[i1].z, 1.0f };
                tr.v1.tangent   = mesh.tangents[i1];
                tr.v1.biTangent = mesh.biTangents[i1];

                tr.v2.location  = mesh.vertices[i2];
                tr.v2.normal    = mesh.normals[i2];
                tr.v2.coord     = mesh.texCoords[i2];
                tr.v2.color     = Vec4<T>{ mesh.colors[i2].x, mesh.colors[i2].y, mesh.colors[i2].z, 1.0f };
                tr.v2.tangent   = mesh.tangents[i2];
                tr.v2.biTangent = mesh.biTangents[i2];
#if defined(R5R_USE_THREADS)
                threadPool->submit(
                  [&listener, tr, normalMatrix, viewProjectionMatrix, &camera, &mesh, this](R5RThreadPoolMemory& tpm) {
#endif
                      vertexShader(tpm,
                                   listener,
                                   tr,
                                   mesh.transform,
                                   normalMatrix,
                                   viewProjectionMatrix,
                                   camera,
                                   mesh.materialIndex);
#if defined(R5R_USE_THREADS)
                  });
#else
                tpm.checkClear();
                tpm.popAllFrameMemory();
#endif
            }
        }
#if defined(R5R_USE_THREADS)
        threadPool->waitForWork();
#endif

#ifndef __EMSCRIPTEN__
        std::stringstream ss;
        ss.str("");

        // display
        ss << std::filesystem::path(scene->filepath).stem().string() << "_" << camera.name << "_depth";
        ASSERT_ERROR(camera.saveDepthToEXR(ss.str()), "Failed to save depth to EXR file");
        ss.str("");
        ss << std::filesystem::path(scene->filepath).stem().string() << "_" << camera.name << "_color";
        ASSERT_ERROR(camera.saveColorToEXR(ss.str()), "Failed to save color to EXR file");
#else
        LOG_ALERT("WRITING BACK TO GL TEXTURES");
        listener.onFrameRenderBoundingSquare(0, camera.resolution.x, 0, camera.resolution.y);
#endif
    }
    void zDrawTriangle(const std::array<Vec4<T>, 3>& projectedVertices, const Camera<T>& camera)
    {
        // Avoid degenerate triangles
        // -------------------------------------------------------------------------------------
        T area = Triangle<T>::area(projectedVertices[0].xy, projectedVertices[1].xy, projectedVertices[2].xy);
        if (area == 0) { return; }
        // ----------------------------------------------------------------------------------------------------------------

        // bounding square around triangle
        // --------------------------------------------------------------------------------
        BoundingSquare<int64_t> bs = calculateTriangleBoundingSquare(
          Vec4<T>{ 0, 0, static_cast<T>(camera.resolution.x), static_cast<T>(camera.resolution.y) },
          projectedVertices[0].xy,
          projectedVertices[1].xy,
          projectedVertices[2].xy);
        // ----------------------------------------------------------------------------------------------------------------

        // loop over bounding square
        for (int64_t y = bs.min.y; y <= bs.max.y; ++y) {
            for (int64_t x = bs.min.x; x <= bs.max.x; ++x) {
                // Compute barycentric coordinates for (x, y)
                Vec3<T> barycentricCoordinates = computeBarycentricCoordinates(
                  projectedVertices[0].xy, projectedVertices[1].xy, projectedVertices[2].xy, x, y);

                if (barycentricCoordinates.x >= 0 && barycentricCoordinates.y >= 0 && barycentricCoordinates.z >= 0) {
                    // point is inside the triangle
                    T d = calculateDepthZeroToOne(barycentricCoordinates,
                                                  projectedVertices[0],
                                                  projectedVertices[1],
                                                  projectedVertices[2],
                                                  camera.zNearPlane,
                                                  camera.zFarPlane);
                    ASSERT_ERROR(d >= 0.0 && d <= 1.0, "Depth should be between 0.0 and 1.0");
                    camera.writeDepthBuffer(d, x, y);
                }
            }
        }
    }
    // Clip against a single 3D plane (e.g., near/far)
    [[nodiscard]] int zClipAgainstPlane3D(R5RThreadPoolMemory& tpm,
                                          const Vertex<T>*     input,
                                          int                  inputCount,
                                          Vertex<T>*           output,
                                          float                edge,
                                          int                  axis,
                                          int                  isMax,
                                          int                  isZForwardPositive)
    {
        int   outputCount = 0;
        auto& prevVertex  = *(Vertex<T>*)tpm.pushFrameMemory(sizeof(Vertex<T>));
        prevVertex        = input[inputCount - 1];
        float prevVal     = (axis == 0)   ? prevVertex.location.x
                            : (axis == 1) ? prevVertex.location.y
                                          : prevVertex.location.z;
        int   prevInside;

        // Handle near/far planes differently for -z vs. +z forward
        if (axis == 2 && !isZForwardPositive) {
            // OpenGL-style (-z forward): Near plane is z = -w, Far plane is z = w
            prevInside = isMax ? (prevVal <= edge * prevVertex.location.w) : (prevVal >= edge * prevVertex.location.w);
        } else {
            // Direct3D/Vulkan-style (+z forward): Near plane is z = 0, Far plane is z = w
            prevInside = isMax ? (prevVal <= edge * prevVertex.location.w) : (prevVal >= edge * prevVertex.location.w);
        }

        for (int i = 0; i < inputCount; i++) {
            auto& currentVertex = *(Vertex<T>*)tpm.pushFrameMemory(sizeof(Vertex<T>));
            currentVertex       = input[i];
            float currVal       = (axis == 0)   ? currentVertex.location.x
                                  : (axis == 1) ? currentVertex.location.y
                                                : currentVertex.location.z;
            int   currInside;

            if (axis == 2 && !isZForwardPositive) {
                currInside =
                  isMax ? (currVal <= edge * currentVertex.location.w) : (currVal >= edge * currentVertex.location.w);
            } else {
                currInside =
                  isMax ? (currVal <= edge * currentVertex.location.w) : (currVal >= edge * currentVertex.location.w);
            }

            if (currInside != prevInside) {
                // Compute intersection (lerp with perspective correction)
                float t = (edge * prevVertex.location.w - prevVal) /
                          ((edge * prevVertex.location.w - prevVal) - (edge * currentVertex.location.w - currVal));
                Vertex<T>& intersect = output[outputCount];
                intersect.location.x = prevVertex.location.x + t * (currentVertex.location.x - prevVertex.location.x);
                intersect.location.y = prevVertex.location.y + t * (currentVertex.location.y - prevVertex.location.y);
                intersect.location.z = prevVertex.location.z + t * (currentVertex.location.z - prevVertex.location.z);
                intersect.location.w = prevVertex.location.w + t * (currentVertex.location.w - prevVertex.location.w);
                outputCount++;
            }

            if (currInside) {
                output[outputCount] = currentVertex;
                outputCount++;
            }

            prevVertex = currentVertex;
            prevVal    = currVal;
            prevInside = currInside;

            tpm.popFrameMemory(sizeof(Vertex<T>));
        }
        tpm.popFrameMemory(sizeof(Vertex<T>));
        return outputCount;
    }
    void zClipTriangle3D(R5RThreadPoolMemory&    tpm,
                         const Triangle<T>&      triangle,
                         std::vector<Vertex<T>>& clippedPolygon,
                         int                     isZForwardPositive)
    {
        auto* input  = (Vertex<T>*)tpm.pushFrameMemory(sizeof(Vertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
        auto* output = (Vertex<T>*)tpm.pushFrameMemory(sizeof(Vertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);

        int inputCount = 3;

        // Copy the triangle vertices to the input array
        input[0] = triangle.v0;
        input[1] = triangle.v1;
        input[2] = triangle.v2;

        // Clip against each of the 6 frustum planes
        // Define clip planes based on forward direction
        const float xPlanes[] = { -1.0f, 1.0f };                             // xmin, xmax
        const float yPlanes[] = { -1.0f, 1.0f };                             // ymin, ymax
        const float zPlanes[] = { isZForwardPositive ? 0.0f : -1.0f, 1.0f }; // zmin, zmax

        // Clip against each of the 6 frustum planes
        const float planes[] = { xPlanes[0], xPlanes[1], yPlanes[0], yPlanes[1], zPlanes[0], zPlanes[1] };
        const int   axes[]   = { 0, 0, 1, 1, 2, 2 }; // x, x, y, y, z, z

        for (int i = 0; i < 6; ++i) {
            int outputCount =
              clipAgainstPlane3D(tpm, input, inputCount, output, planes[i], axes[i], i % 2, isZForwardPositive);
            if (outputCount == 0) {
                // Fully clipped
                clippedPolygon.clear();
                tpm.popFrameMemory(sizeof(Vertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
                tpm.popFrameMemory(sizeof(Vertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
                return;
            }
            memcpy(input, output, outputCount * sizeof(Vertex<T>));
            inputCount = outputCount;
        }

        // Copy the final clipped vertices to the output polygon
        if (inputCount > clippedPolygon.size()) { clippedPolygon.resize(inputCount - clippedPolygon.size()); }
        for (int i = 0; i < inputCount; ++i) { clippedPolygon[i] = input[i]; }

        tpm.popFrameMemory(sizeof(Vertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
        tpm.popFrameMemory(sizeof(Vertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
    }
    void zClipTriangleToTriangles3D(R5RThreadPoolMemory&      tpm,
                                    const Triangle<T>&        triangle,
                                    std::vector<Triangle<T>>& clippedTriangles,
                                    int                       isZForwardPositive)
    {
        std::vector<Vertex<T>> clippedPolygon;
        clipTriangle3D(tpm, triangle, clippedPolygon, isZForwardPositive);

        if (clippedPolygon.size() < 3) {
            // No output (fully clipped)
            clippedTriangles.clear();
            return;
        }
        if (clippedPolygon.size() > 3) {
            // Triangulate the clipped polygon using a triangle fan
            clippedTriangles.resize(clippedPolygon.size() - 2);
            int numTriangles = 0;
            for (int i = 1; i < clippedPolygon.size() - 1; ++i) {
                clippedTriangles[numTriangles].v0 = clippedPolygon[0];
                clippedTriangles[numTriangles].v1 = clippedPolygon[i];
                clippedTriangles[numTriangles].v2 = clippedPolygon[i + 1];
                ++numTriangles;
            }
            return;
        }

        // same triangle, just use the originally passed triangle
        clippedTriangles.resize(1);
        clippedTriangles[0].v0 = clippedPolygon[0];
        clippedTriangles[0].v1 = clippedPolygon[1];
        clippedTriangles[0].v2 = clippedPolygon[2];
    }
    void zClipStage(R5RThreadPoolMemory& tpm, Triangle<T>& projectedVertices, const Camera<T>& camera)
    {
        // clipping
        std::vector<Triangle<T>> clippedTriangles;
        zClipTriangleToTriangles3D(tpm, projectedVertices, clippedTriangles, 1);
        if (clippedTriangles.empty()) {
            // all triangle vertices are actually clipped
            return;
        }

        auto& inverseViewMatrix =
          *(glm::mat<4, 4, T, glm::defaultp>*)tpm.pushFrameMemory(sizeof(glm::mat<4, 4, T, glm::defaultp>));
        auto& inverseProjectionMatrix =
          *(glm::mat<4, 4, T, glm::defaultp>*)tpm.pushFrameMemory(sizeof(glm::mat<4, 4, T, glm::defaultp>));
        auto& worldTriangle = *(Triangle<T>*)tpm.pushFrameMemory(sizeof(Triangle<T>));

        inverseViewMatrix       = glm::inverse(camera.viewMatrix.glm);
        inverseProjectionMatrix = glm::inverse(camera.projectionMatrix.glm);

        for (size_t i = 0; i < clippedTriangles.size(); ++i) {
            Triangle<T>& clippedTriangle = clippedTriangles[i];

            // get back world triangle
            worldTriangle                 = clippedTriangle;
            worldTriangle.v0.location.glm = inverseViewMatrix * inverseProjectionMatrix * worldTriangle.v0.location.glm;
            worldTriangle.v1.location.glm = inverseViewMatrix * inverseProjectionMatrix * worldTriangle.v1.location.glm;
            worldTriangle.v2.location.glm = inverseViewMatrix * inverseProjectionMatrix * worldTriangle.v2.location.glm;

            // perspective division
            clippedTriangle.v0.location.x /= clippedTriangle.v0.location.w;
            clippedTriangle.v0.location.y /= clippedTriangle.v0.location.w;
            clippedTriangle.v0.location.z /= clippedTriangle.v0.location.w;
            clippedTriangle.v1.location.x /= clippedTriangle.v1.location.w;
            clippedTriangle.v1.location.y /= clippedTriangle.v1.location.w;
            clippedTriangle.v1.location.z /= clippedTriangle.v1.location.w;
            clippedTriangle.v2.location.x /= clippedTriangle.v2.location.w;
            clippedTriangle.v2.location.y /= clippedTriangle.v2.location.w;
            clippedTriangle.v2.location.z /= clippedTriangle.v2.location.w;

            // viewport transformation
            clippedTriangle.v0.location.x = (clippedTriangle.v0.location.x + 1.0f) * 0.5f * WIDTH;
            clippedTriangle.v0.location.y = (1.0f - clippedTriangle.v0.location.y) * 0.5f * HEIGHT;
            clippedTriangle.v0.location.z =
              (clippedTriangle.v0.location.z + 1.0f) * 0.5f * (camera.zFarPlane - camera.zNearPlane) +
              camera.zNearPlane;
            clippedTriangle.v1.location.x = (clippedTriangle.v1.location.x + 1.0f) * 0.5f * WIDTH;
            clippedTriangle.v1.location.y = (1.0f - clippedTriangle.v1.location.y) * 0.5f * HEIGHT;
            clippedTriangle.v1.location.z =
              (clippedTriangle.v1.location.z + 1.0f) * 0.5f * (camera.zFarPlane - camera.zNearPlane) +
              camera.zNearPlane;
            clippedTriangle.v2.location.x = (clippedTriangle.v2.location.x + 1.0f) * 0.5f * WIDTH;
            clippedTriangle.v2.location.y = (1.0f - clippedTriangle.v2.location.y) * 0.5f * HEIGHT;
            clippedTriangle.v2.location.z =
              (clippedTriangle.v2.location.z + 1.0f) * 0.5f * (camera.zFarPlane - camera.zNearPlane) +
              camera.zNearPlane;

            // if (!camera.isBackFace(
            //       clippedTriangle.v0.location, clippedTriangle.v1.location, clippedTriangle.v2.location)) {
            std::array<Vec4<T>, 3> projectedVerticesToRasterize = { clippedTriangle.v0.location,
                                                                    clippedTriangle.v1.location,
                                                                    clippedTriangle.v2.location };
            zDrawTriangle(projectedVerticesToRasterize, camera);
            // }
        }
        // inverseViewMatrix
        tpm.popFrameMemory(sizeof(glm::mat<4, 4, T, glm::defaultp>));
        // inverseProjectionMatrix
        tpm.popFrameMemory(sizeof(glm::mat<4, 4, T, glm::defaultp>));
        // worldTriangle
        tpm.popFrameMemory(sizeof(Triangle<T>));
    }
    void zVertexShader(R5RThreadPoolMemory& tpm,
                       const Triangle<T>&   t,
                       const Mat4<T>&       modelMatrix,
                       const Mat4<T>&       viewProjectionMatrix,
                       const Camera<T>&     camera)
    {
        auto& projectedVertices = *(Triangle<T>*)tpm.pushFrameMemory(sizeof(Triangle<T>));
        projectedVertices       = t;

        projectedVertices.v0.location = viewProjectionMatrix * modelMatrix * t.v0.location;
        projectedVertices.v1.location = viewProjectionMatrix * modelMatrix * t.v1.location;
        projectedVertices.v2.location = viewProjectionMatrix * modelMatrix * t.v2.location;
        // Here, officially traditional vertex shader ends -------------------------------

        zClipStage(tpm, projectedVertices, camera);

        tpm.popFrameMemory(sizeof(Triangle<T>));
    }
    void renderZPrePass(Camera<T>& camera)
    {
        R5RThreadPoolMemory tpm(32 * 1024 * sizeof(uint8_t));
        camera.clearDepthBuffer();

        const Mat4<T>&          viewProjectionMatrix = camera.projectionMatrix * camera.viewMatrix;
        std::array<Plane<T>, 6> frustumPlanes        = Plane<T>::extractFrustumPlanes(viewProjectionMatrix);
        for (int64_t mi = 0; mi < scene->meshes.size(); ++mi) {
            Mesh<T>& mesh = scene->meshes[mi];
            // frustum culling
            if (mesh.boundingBox.testFrustum(frustumPlanes) == EBoundingBoxFrustumTest_FullyOutside) {
                LOGV_DEBUG("[FRUSTUM CULLING ELIMINATED] {}", mesh.name);
                continue;
            }
            for (size_t ii = 0; ii < mesh.indices.size() - 2; ii += 3) {
                uint32_t i0 = mesh.indices[ii];
                uint32_t i1 = mesh.indices[ii + 1];
                uint32_t i2 = mesh.indices[ii + 2];

                Triangle<T> tr = {};
                tr.v0.location = mesh.vertices[i0];
                tr.v1.location = mesh.vertices[i1];
                tr.v2.location = mesh.vertices[i2];
                zVertexShader(tpm, tr, mesh.transform, viewProjectionMatrix, camera);
                tpm.checkClear();
                tpm.popAllFrameMemory();
            }
        }
    }
    void render(RasterizerEventListener& listener)
    {
        if (scene == nullptr) { return; }
        for (size_t ci = 0; ci < scene->cameras.size(); ++ci) {
            Camera<T>& camera = scene->cameras[ci];
            LOGV_DEBUG("[CAMERA] {}", camera.name);
            camera.createFrameBuffers();

            LOG_ALERT("RENDERING Z PRE_PASS");
            renderZPrePass(camera);
            LOG_ALERT("RENDERING FRAME");
            renderFrame(listener, camera);
            LOGV_ALERT("DONE FRAME {}", ci);

#ifndef __EMSCRIPTEN__
            // for now on the web app we need the camera framebuffers still afterwards
            camera.destroyFrameBuffers();
#endif
        }
    }
#if defined(R5R_USE_THREADS)
    R5RThreadPool* threadPool;
#endif
    R5RRenderer* renderer;
    Scene<T>*    scene;
    // uint8_t*       frameMemory;
    // int64_t        frameMemoryStart;
    // int64_t        frameMemoryEnd;
    Texture2D brdfTexture;
};
