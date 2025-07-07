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

#include <Renderer/SW/XPSWLogger.h>
#include <Renderer/SW/XPSWMaths.h>
#include <Renderer/SW/XPSWMemoryPool.h>
#include <Renderer/SW/XPSWRenderer.h>
#include <Renderer/SW/XPSWRendererCommon.h>
#include <Renderer/SW/XPSWSceneDescriptor.h>

#include <array>
#include <functional>
#include <iostream>
#include <sstream>

struct XPSWRenderer;

#define MAX_CLIPPED_TRIANGLE_VERTICES (3 * 3)

template<typename T>
struct XPSWRasterizer
{
    explicit XPSWRasterizer(XPSWRenderer* renderer)
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
    }
    ~XPSWRasterizer()
    {
        // ASSERT_ERROR(frameMemoryStart == 0, "Memory pool wasn't properly cleared out");
        // free(frameMemory);
        // frameMemory    = nullptr;
        // frameMemoryEnd = 0;
    }
    void                    setScene(XPSWScene<T>* scene) { this->scene = scene; }
    [[nodiscard]] XPVec4<T> interpolateVec4(const XPVec3<T>& barycentricCoordinates,
                                            const XPVec4<T>& v0,
                                            const XPVec4<T>& v1,
                                            const XPVec4<T>& v2)
    {
        return XPVec4<T>{
            barycentricCoordinates.x * v0.x + barycentricCoordinates.y * v1.x + barycentricCoordinates.z * v2.x,
            barycentricCoordinates.x * v0.y + barycentricCoordinates.y * v1.y + barycentricCoordinates.z * v2.y,
            barycentricCoordinates.x * v0.z + barycentricCoordinates.y * v1.z + barycentricCoordinates.z * v2.z,
            barycentricCoordinates.x * v0.w + barycentricCoordinates.y * v1.w + barycentricCoordinates.z * v2.w
        };
    }
    [[nodiscard]] XPVec3<T> interpolateVec3(const XPVec3<T>& barycentricCoordinates,
                                            const XPVec3<T>& v0,
                                            const XPVec3<T>& v1,
                                            const XPVec3<T>& v2)
    {
        return XPVec3<T>{
            barycentricCoordinates.x * v0.x + barycentricCoordinates.y * v1.x + barycentricCoordinates.z * v2.x,
            barycentricCoordinates.x * v0.y + barycentricCoordinates.y * v1.y + barycentricCoordinates.z * v2.y,
            barycentricCoordinates.x * v0.z + barycentricCoordinates.y * v1.z + barycentricCoordinates.z * v2.z
        };
    }
    [[nodiscard]] XPVec2<T> interpolateVec2(const XPVec3<T>& barycentricCoordinates,
                                            const XPVec2<T>& v0,
                                            const XPVec2<T>& v1,
                                            const XPVec2<T>& v2)
    {
        return XPVec2<T>{
            barycentricCoordinates.x * v0.x + barycentricCoordinates.y * v1.x + barycentricCoordinates.z * v2.x,
            barycentricCoordinates.x * v0.y + barycentricCoordinates.y * v1.y + barycentricCoordinates.z * v2.y
        };
    }
    [[nodiscard]] static XPSWBoundingSquare<int64_t> calculateTriangleBoundingSquare(const XPVec4<T>& viewport,
                                                                                     const XPVec2<T>& v0,
                                                                                     const XPVec2<T>& v1,
                                                                                     const XPVec2<T>& v2)
    {
        int64_t minX = std::max(T(viewport.x), std::min({ v0.x, v1.x, v2.x }));
        int64_t maxX = std::min(static_cast<T>(viewport.z) - T(1), std::max({ v0.x, v1.x, v2.x }));
        int64_t minY = std::max(T(viewport.y), std::min({ v0.y, v1.y, v2.y }));
        int64_t maxY = std::min(static_cast<T>(viewport.w) - T(1), std::max({ v0.y, v1.y, v2.y }));
        return { XPVec2<int64_t>(minX, minY), XPVec2<int64_t>(maxX, maxY) };
    }
    // Helper: Compute barycentric weights for a pixel
    [[nodiscard]] static XPVec3<T> computeBarycentricCoordinates(const XPVec2<T>& A,
                                                                 const XPVec2<T>& B,
                                                                 const XPVec2<T>& C,
                                                                 float            px,
                                                                 float            py)
    {
        XPVec3<T>   barycentricCoordinates;
        const float denominator  = (B.y - C.y) * (A.x - C.x) + (C.x - B.x) * (A.y - C.y);
        barycentricCoordinates.x = ((B.y - C.y) * (px - C.x) + (C.x - B.x) * (py - C.y)) / denominator;
        barycentricCoordinates.y = ((C.y - A.y) * (px - C.x) + (A.x - C.x) * (py - C.y)) / denominator;
        barycentricCoordinates.z = 1.0f - barycentricCoordinates.x - barycentricCoordinates.y;
        return barycentricCoordinates;
    }
    // Interpolate vertex attributes using barycentric weights (u, v, w)
    static void interpolateVertex(const XPVec3<T>&                                    barycentricCoordinates,
                                  const std::array<XPVec4<T>, 3>&                     projectedVertices,
                                  const std::array<XPSWVertexFragmentVaryings<T>, 3>& inVFV,
                                  XPSWVertexFragmentVaryings<T>&                      outVFV)
    {
        float u = barycentricCoordinates.x;
        float v = barycentricCoordinates.y;
        float w = barycentricCoordinates.z;

        XPVec4<T> out;
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
    [[nodiscard]] static T calculateDepthZeroToOne(const XPVec3<T>& barycentricCoordinates,
                                                   const XPVec4<T>& projectedVert0,
                                                   const XPVec4<T>& projectedVert1,
                                                   const XPVec4<T>& projectedVert2,
                                                   T                zNear,
                                                   T                zFar)
    {
        const T zNDC0 = projectedVert0.w;
        const T zNDC1 = projectedVert1.w;
        const T zNDC2 = projectedVert2.w;

        T depth =
          barycentricCoordinates.x * zNDC0 + barycentricCoordinates.y * zNDC1 + barycentricCoordinates.z * zNDC2;

        return LinearToExponentialInvertedZ(depth, zNear, zFar);
    }
    [[nodiscard]] XPVec3<T> calculateRadiance(const XPVec3<T>& vWorldPos,
                                              const XPVec3<T>& V,
                                              const XPVec3<T>& lightPos,
                                              const XPVec3<T>& lightColor,
                                              float            lightIntensity,
                                              const XPVec3<T>& albedo,
                                              const XPVec3<T>& N,
                                              const float      roughness,
                                              const float      metallic,
                                              const float      ao)
    {
        // Calculate per-light radiance
        XPVec3<T> L           = glm::normalize(lightPos.glm - vWorldPos.glm);
        XPVec3<T> H           = glm::normalize(V.glm + L.glm);
        float     distance    = glm::length(lightPos.glm - vWorldPos.glm);
        float     attenuation = 1.0f / (distance * distance);
        XPVec3<T> radiance    = lightColor * lightIntensity * attenuation;

        // Cook-Torrance BRDF
        float     NDF = DistributionGGX(N, H, roughness);
        float     G   = GeometrySmith(N, V, L, roughness);
        XPVec3<T> F0  = XPVec3<T>(0.04f, 0.04f, 0.04f);
        XPVec3<T> F   = FresnelSchlick(std::max(H.dot(V), 0.0f), F0);

        XPVec3<T> numerator   = NDF * G * F;
        float     denominator = 4.0f * std::max(N.dot(V), 0.0f) * std::max(N.dot(L), 0.0f);
        XPVec3<T> specular    = numerator / std::max(denominator, 0.001f);

        // kS is equal to Fresnel
        XPVec3<T> kS = F;
        // For energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        XPVec3<T> kD = 1.0f - kS;
        // Multiply kD by the inverse metalness such that only non-metals
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD = kD * (1.0f - metallic);

        // Scale light by NdotL
        float NdotL = std::max(N.dot(L), 0.0f);

        // Add to outgoing radiance Lo
        return (kD * albedo + specular) * radiance * NdotL * ao;
    }
    [[nodiscard]] XPVec3<T> computeSpotLightContribution(const XPVec3<T>&    V,
                                                         const XPVec3<T>&    point,
                                                         const XPVec3<T>&    N,
                                                         const XPSWLight<T>& light,
                                                         const XPVec3<T>&    BaseColor,
                                                         const XPVec3<T>&    EmissionColor,
                                                         const float         Metallic,
                                                         const float         Roughness,
                                                         const float         AO)
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
            XPVec3<T> radiance = light.ambient * light.intensity + BaseColor;

            return radiance;
        }

        // Spotlight falloff (smooth transition at edges)
        float fallOff = std::pow((cosAngle - coneCos) / (1.0f - coneCos), light.attenuationConstant);

        // Diffuse component (Lambertian reflectance)
        XPVec3<T> diffuseIntensity = std::max(0.0f, N.dot(lightDir)) * light.diffuse;

        // Specular component (Phong reflection)
        XPVec3<T> reflectDir        = 2.0f * N.dot(lightDir) * N - lightDir;
        XPVec3<T> specularIntensity = std::pow(std::max(0.0f, V.dot(reflectDir)), 1.0f) * light.specular;

        // FINAL
        // ambient + fallOff * (diffuseIntensity + specularIntensity)
        XPVec3<T> radiance = calculateRadiance(point,
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

    [[nodiscard]] XPVec3<T> computePointLightContribution(const XPVec3<T>&    V,
                                                          const XPVec3<T>&    point,
                                                          const XPVec3<T>&    N,
                                                          const XPSWLight<T>& light,
                                                          const XPVec3<T>&    BaseColor,
                                                          const XPVec3<T>&    EmissionColor,
                                                          const float         Metallic,
                                                          const float         Roughness,
                                                          const float         AO)
    {
        // Ambient lighting
        XPVec3<T> ambient = light.ambient * light.intensity;

        // Diffuse lighting
        XPVec3<T> lightDir = (light.location - point);
        lightDir.normalize();
        float     diff    = std::max(N.dot(lightDir), 0.0f);
        XPVec3<T> diffuse = light.diffuse * diff * light.intensity;

        // Specular lighting (Blinn-Phong)
        XPVec3<T> halfwayDir = (lightDir + V);
        halfwayDir.normalize();
        float     spec     = std::pow(std::max(N.dot(halfwayDir), 0.0f), 1.0f);
        XPVec3<T> specular = light.specular * spec * light.intensity;

        // Attenuation (light falloff with distance)
        float distance    = glm::length((light.location - point).glm);
        float attenuation = 1.0f / (light.attenuationConstant + light.attenuationLinear * distance +
                                    light.attenuationQuadratic * (distance * distance));

        // FINAL
        // ambient + (diffuse + specular) * attenuation;
        XPVec3<T> radiance = calculateRadiance(point,
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

    [[nodiscard]] XPVec3<T> computeDirectionalLightContribution(const XPVec3<T>&    V,
                                                                const XPVec3<T>&    point,
                                                                const XPVec3<T>&    N,
                                                                const XPSWLight<T>& light,
                                                                const XPVec3<T>&    BaseColor,
                                                                const XPVec3<T>&    EmissionColor,
                                                                const float         Metallic,
                                                                const float         Roughness,
                                                                const float         AO)
    {
        // Ambient lighting (constant)
        XPVec3<T> ambient = light.ambient * light.intensity;

        // Diffuse lighting (Lambert's cosine law)
        float     diff    = std::max(N.dot(-1.0f * light.direction), 0.0f);
        XPVec3<T> diffuse = light.diffuse * diff * light.intensity;

        // Specular lighting (Blinn-Phong)
        XPVec3<T> halfwayDir = (-1.0f * light.direction + V);
        halfwayDir.normalize();
        float     spec     = std::pow(std::max(N.dot(halfwayDir), 0.0f), 1.0f);
        XPVec3<T> specular = light.specular * spec * light.intensity;

        // FINAL
        // ambient + diffuse + specular
        XPVec3<T> radiance = calculateRadiance(point,
                                               V,
                                               light.location,
                                               ambient + diffuse + specular,
                                               light.intensity,
                                               BaseColor,
                                               N,
                                               Roughness,
                                               Metallic,
                                               AO);

        return radiance;
    }
    [[nodiscard]] XPVec4<float> fragmentShader(XPSWMemoryPool&                          tpm,
                                               const XPSWCamera<T>&                     camera,
                                               const XPSWMaterial<T>&                   material,
                                               const XPSWVertexFragmentVaryings<T>&     vertexFragmentVaryings,
                                               const XPSWVertexFragmentFlatVaryings<T>& vertexFragmentFlatVaryings)
    {
        // -------------------------------------------------------------------------------------------------------------
        // VERTEX/FRAGMENT SHADER INTERPOLATED VALUES
        // -------------------------------------------------------------------------------------------------------------
        const std::vector<XPSWELightType>& LightType            = vertexFragmentFlatVaryings.lightType;
        const std::vector<float>&     LightAttenuationConstant  = vertexFragmentFlatVaryings.lightAttenuationConstant;
        const std::vector<float>&     LightAttenuationLinear    = vertexFragmentFlatVaryings.lightAttenuationLinear;
        const std::vector<float>&     LightAttenuationQuadratic = vertexFragmentFlatVaryings.lightAttenuationQuadratic;
        const std::vector<float>&     LightAngleOuterCone       = vertexFragmentFlatVaryings.lightAngleOuterCone;
        const std::vector<XPVec3<T>>& LightPos                  = vertexFragmentFlatVaryings.lightPos;
        const std::vector<XPVec3<T>>& LightDirection            = vertexFragmentFlatVaryings.lightDirection;
        const std::vector<XPVec3<T>>& LightColor                = vertexFragmentFlatVaryings.lightColor;
        const std::vector<float>&     LightIntensity            = vertexFragmentFlatVaryings.lightIntensity;
        const XPVec3<T>&              ViewPos                   = vertexFragmentFlatVaryings.viewPos;
        const XPVec4<T>&              FragPos                   = vertexFragmentVaryings.fragPos;
        const XPVec3<T>&              FragNormal                = vertexFragmentVaryings.fragNormal;
        const XPVec3<T>&              FragTangent               = vertexFragmentVaryings.fragTangent;
        const XPVec3<T>&              FragBiTangent             = vertexFragmentVaryings.fragBiTangent;
        const XPVec2<T>&              FragTexCoord              = vertexFragmentVaryings.fragTextureCoord;
        const XPVec3<T>&              FragColor                 = vertexFragmentVaryings.fragColor;
        // -------------------------------------------------------------------------------------------------------------

        XPVec3<T> BaseColor;
        XPVec3<T> N;
        XPVec3<T> EmissionColor;
        float     Metallic;
        float     Roughness;
        float     AO;
        getPBRMaterial(
          material, FragTexCoord, FragColor, FragNormal, BaseColor, N, EmissionColor, Metallic, Roughness, AO, true);
        // -------------------------------------------------------------------------------------------------------------

        XPVec3<T> V = ViewPos - FragPos.xyz;
        V.normalize();

        XPVec3<T> Lo = { 0.0f, 0.0f, 0.0f };
        for (size_t li = 0; li < LightColor.size(); ++li) {
            if (scene->lights[li].type == XPSWELightType_Spot) {
                Lo = Lo + computeSpotLightContribution(
                            V, FragPos.xyz, N, scene->lights[li], BaseColor, EmissionColor, Metallic, Roughness, AO);
            } else if (scene->lights[li].type == XPSWELightType_Point) {
                Lo = Lo + computePointLightContribution(
                            V, FragPos.xyz, N, scene->lights[li], BaseColor, EmissionColor, Metallic, Roughness, AO);
            } else if (scene->lights[li].type == XPSWELightType_Directional) {
                Lo = Lo + computeDirectionalLightContribution(
                            V, FragPos.xyz, N, scene->lights[li], BaseColor, EmissionColor, Metallic, Roughness, AO);
            } else {
                XP_SW_ASSERT_ERROR(false, "Unreachable");
            }
        }

        // Final color
        XPVec3<T> color      = Lo;
        color                = gammaCorrect(reinhardToneMapping(color), 2.2);
        XPVec4<T> finalcolor = XPVec4<T>{ color.x, color.y, color.z, 1.0f };
        return finalcolor;
    }
    void drawTriangle(XPSWMemoryPool&                                     tpm,
                      XPSWRasterizerEventListener&                        listener,
                      const std::array<XPVec4<T>, 3>&                     projectedVertices,
                      const XPSWCamera<T>&                                camera,
                      unsigned int                                        materialIndex,
                      const std::array<XPSWVertexFragmentVaryings<T>, 3>& vertexFragmentVaryings,
                      const XPSWVertexFragmentFlatVaryings<T>&            vertexFragmentFlatVaryings)
    {
        // Avoid degenerate triangles
        // -------------------------------------------------------------------------------------
        T area = XPSWTriangle<T>::area(projectedVertices[0].xy, projectedVertices[1].xy, projectedVertices[2].xy);
        if (area == 0) { return; }
        // ----------------------------------------------------------------------------------------------------------------

        // bounding square around triangle
        // --------------------------------------------------------------------------------
        XPSWBoundingSquare<int64_t> bs = calculateTriangleBoundingSquare(
          XPVec4<T>{ 0, 0, static_cast<T>(camera.resolution.x), static_cast<T>(camera.resolution.y) },
          projectedVertices[0].xy,
          projectedVertices[1].xy,
          projectedVertices[2].xy);
        // ----------------------------------------------------------------------------------------------------------------

        // loop over bounding square
        for (int64_t y = bs.min.y; y <= bs.max.y; ++y) {
            for (int64_t x = bs.min.x; x <= bs.max.x; ++x) {
                // Compute barycentric coordinates for (x, y)
                XPVec3<T> barycentricCoordinates = computeBarycentricCoordinates(
                  projectedVertices[0].xy, projectedVertices[1].xy, projectedVertices[2].xy, x, y);

                if (barycentricCoordinates.x >= 0 && barycentricCoordinates.y >= 0 && barycentricCoordinates.z >= 0) {
                    // point is inside the triangle
                    T d = calculateDepthZeroToOne(barycentricCoordinates,
                                                  projectedVertices[0],
                                                  projectedVertices[1],
                                                  projectedVertices[2],
                                                  camera.zNearPlane,
                                                  camera.zFarPlane);
                    XP_SW_ASSERT_ERROR(d >= 0.0 && d <= 1.0, "Depth should be between 0.0 and 1.0");
                    if (d <= camera.getDepthBufferPixel(x, y)) {
                        // Interpolate attributes ----------------------------------------------------------------------
                        XPSWVertexFragmentVaryings<T>* fragmentVaryings =
                          (XPSWVertexFragmentVaryings<T>*)tpm.pushFrameMemory(sizeof(XPSWVertexFragmentVaryings<T>));
                        interpolateVertex(
                          barycentricCoordinates, projectedVertices, vertexFragmentVaryings, *fragmentVaryings);
                        // ---------------------------------------------------------------------------------------------

                        XPVec4<float> fragColor = fragmentShader(tpm,
                                                                 camera,
                                                                 scene->materials.at(materialIndex),
                                                                 *fragmentVaryings,
                                                                 vertexFragmentFlatVaryings);

                        camera.writeColorBuffer(fragColor, x, y);

                        // fragmentVaryings
                        tpm.popFrameMemory(sizeof(XPSWVertexFragmentVaryings<T>));
                    }
                }
            }
        }
    }
    // Clip against a single 3D plane (e.g., near/far)
    [[nodiscard]] int clipAgainstPlane3D(XPSWMemoryPool&      tpm,
                                         const XPSWVertex<T>* input,
                                         int                  inputCount,
                                         XPSWVertex<T>*       output,
                                         float                edge,
                                         int                  axis,
                                         int                  isMax,
                                         int                  isZForwardPositive)
    {
        int   outputCount = 0;
        auto& prevVertex  = *(XPSWVertex<T>*)tpm.pushFrameMemory(sizeof(XPSWVertex<T>));
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
            auto& currentVertex = *(XPSWVertex<T>*)tpm.pushFrameMemory(sizeof(XPSWVertex<T>));
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
                XPSWVertex<T>& intersect = output[outputCount];
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

            tpm.popFrameMemory(sizeof(XPSWVertex<T>));
        }
        tpm.popFrameMemory(sizeof(XPSWVertex<T>));
        return outputCount;
    }
    void clipTriangle3D(XPSWMemoryPool&             tpm,
                        const XPSWTriangle<T>&      triangle,
                        std::vector<XPSWVertex<T>>& clippedPolygon,
                        int                         isZForwardPositive)
    {
        auto* input  = (XPSWVertex<T>*)tpm.pushFrameMemory(sizeof(XPSWVertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
        auto* output = (XPSWVertex<T>*)tpm.pushFrameMemory(sizeof(XPSWVertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);

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
                tpm.popFrameMemory(sizeof(XPSWVertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
                tpm.popFrameMemory(sizeof(XPSWVertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
                return;
            }
            memcpy((void*)input, (void*)output, outputCount * sizeof(XPSWVertex<T>));
            inputCount = outputCount;
        }

        // Copy the final clipped vertices to the output polygon
        if (inputCount > clippedPolygon.size()) { clippedPolygon.resize(inputCount - clippedPolygon.size()); }
        for (int i = 0; i < inputCount; i++) { clippedPolygon[i] = input[i]; }

        tpm.popFrameMemory(sizeof(XPSWVertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
        tpm.popFrameMemory(sizeof(XPSWVertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
    }
    void clipTriangleToTriangles3D(XPSWMemoryPool&               tpm,
                                   const XPSWTriangle<T>&        triangle,
                                   std::vector<XPSWTriangle<T>>& clippedTriangles,
                                   int                           isZForwardPositive)
    {
        std::vector<XPSWVertex<T>> clippedPolygon;
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
    void clipStage(XPSWMemoryPool&                         tpm,
                   XPSWRasterizerEventListener&            listener,
                   const XPSWTriangle<T>&                  projectedVertices,
                   const XPMat4<T>&                        modelMatrix,
                   const glm::mat<3, 3, T, glm::defaultp>& normalMatrix,
                   const XPMat4<T>&                        viewProjectionMatrix,
                   const XPSWCamera<T>&                    camera,
                   unsigned int                            materialIndex)
    {
        // clipping
        std::vector<XPSWTriangle<T>> clippedTriangles;
        clipTriangleToTriangles3D(tpm, projectedVertices, clippedTriangles, 1);
        if (clippedTriangles.empty()) {
            // all triangle vertices are actually clipped
            return;
        }

        auto& inverseViewMatrix =
          *(glm::mat<4, 4, T, glm::defaultp>*)tpm.pushFrameMemory(sizeof(glm::mat<4, 4, T, glm::defaultp>));
        auto& inverseProjectionMatrix =
          *(glm::mat<4, 4, T, glm::defaultp>*)tpm.pushFrameMemory(sizeof(glm::mat<4, 4, T, glm::defaultp>));
        auto& worldTriangle = *(XPSWTriangle<T>*)tpm.pushFrameMemory(sizeof(XPSWTriangle<T>));

        inverseViewMatrix       = glm::inverse(camera.viewMatrix.glm);
        inverseProjectionMatrix = glm::inverse(camera.projectionMatrix.glm);

        for (size_t i = 0; i < clippedTriangles.size(); ++i) {
            XPSWVertexFragmentFlatVaryings<T>            vertexFragmentFlatVaryings = {};
            std::array<XPSWVertexFragmentVaryings<T>, 3> vertexFragmentVaryings     = {};

            XPSWTriangle<T>& clippedTriangle = clippedTriangles[i];

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
            std::array<XPVec4<T>, 3> projectedVerticesToRasterize = { clippedTriangle.v0.location,
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
        tpm.popFrameMemory(sizeof(XPSWTriangle<T>));
    }
    void vertexShader(XPSWMemoryPool&                         tpm,
                      XPSWRasterizerEventListener&            listener,
                      const XPSWTriangle<T>&                  t,
                      const XPMat4<T>&                        modelMatrix,
                      const glm::mat<3, 3, T, glm::defaultp>& normalMatrix,
                      const XPMat4<T>&                        viewProjectionMatrix,
                      const XPSWCamera<T>&                    camera,
                      unsigned int                            materialIndex)
    {
        auto& projectedVertices = *(XPSWTriangle<T>*)tpm.pushFrameMemory(sizeof(XPSWTriangle<T>));
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
        tpm.popFrameMemory(sizeof(XPSWTriangle<T>));
    }
    void renderFrame(XPSWRasterizerEventListener& listener, XPSWCamera<T>& camera)
    {
#ifndef XP_SW_USE_THREADS
        XPSWMemoryPool tpm(32 * 1024 * sizeof(uint8_t));
#endif
        camera.clearColorBuffer();
#ifdef __EMSCRIPTEN__
        listener.onFrameSetColorBufferPtr(camera.colorBuffer, camera.resolution.x, camera.resolution.y);
#endif
        const XPMat4<T>&                 viewProjectionMatrix = camera.projectionMatrix * camera.viewMatrix;
        std::array<XPSWPlane<T>, 6>      frustumPlanes = XPSWPlane<T>::extractFrustumPlanes(viewProjectionMatrix);
        glm::mat<3, 3, T, glm::defaultp> normalMatrix;
        for (int64_t mi = 0; mi < scene->meshes.size(); ++mi) {
            XPSWMesh<T>& mesh = scene->meshes[mi];
            // frustum culling
            if (mesh.boundingBox.testFrustum(frustumPlanes) == XPSWEBoundingBoxFrustumTest_FullyOutside) {
                LOGV_DEBUG("[FRUSTUM CULLING ELIMINATED] {}", mesh.name);
                continue;
            }
            normalMatrix = glm::transpose(glm::inverse(glm::mat3(mesh.transform.glm)));
            for (size_t ii = 0; ii < mesh.indices.size() - 2; ii += 3) {
                uint32_t i0 = mesh.indices[ii];
                uint32_t i1 = mesh.indices[ii + 1];
                uint32_t i2 = mesh.indices[ii + 2];

                XPSWTriangle<T> tr = {};
                tr.v0.location     = mesh.vertices[i0];
                tr.v0.normal       = mesh.normals[i0];
                tr.v0.coord        = mesh.texCoords[i0];
                tr.v0.color        = XPVec4<T>{ mesh.colors[i0].x, mesh.colors[i0].y, mesh.colors[i0].z, 1.0f };
                tr.v0.tangent      = mesh.tangents[i0];
                tr.v0.biTangent    = mesh.biTangents[i0];

                tr.v1.location  = mesh.vertices[i1];
                tr.v1.normal    = mesh.normals[i1];
                tr.v1.coord     = mesh.texCoords[i1];
                tr.v1.color     = XPVec4<T>{ mesh.colors[i1].x, mesh.colors[i1].y, mesh.colors[i1].z, 1.0f };
                tr.v1.tangent   = mesh.tangents[i1];
                tr.v1.biTangent = mesh.biTangents[i1];

                tr.v2.location  = mesh.vertices[i2];
                tr.v2.normal    = mesh.normals[i2];
                tr.v2.coord     = mesh.texCoords[i2];
                tr.v2.color     = XPVec4<T>{ mesh.colors[i2].x, mesh.colors[i2].y, mesh.colors[i2].z, 1.0f };
                tr.v2.tangent   = mesh.tangents[i2];
                tr.v2.biTangent = mesh.biTangents[i2];
#if defined(XP_SW_USE_THREADS)
                threadPool->submit(
                  [&listener, tr, normalMatrix, viewProjectionMatrix, &camera, &mesh, this](XPSWMemoryPool& tpm) {
#endif
                      vertexShader(tpm,
                                   listener,
                                   tr,
                                   mesh.transform,
                                   normalMatrix,
                                   viewProjectionMatrix,
                                   camera,
                                   mesh.materialIndex);
#if defined(XP_SW_USE_THREADS)
                  });
#else
                tpm.checkClear();
                tpm.popAllFrameMemory();
#endif
            }
        }
#if defined(XP_SW_USE_THREADS)
        threadPool->waitForWork();
#endif

#ifndef __EMSCRIPTEN__
        std::stringstream ss;
        ss.str("");

        // display
        ss << std::filesystem::path(scene->filepath).stem().string() << "_" << camera.name << "_depth";
        XP_SW_ASSERT_ERROR(camera.saveDepthToEXR(ss.str()), "Failed to save depth to EXR file");
        ss.str("");
        ss << std::filesystem::path(scene->filepath).stem().string() << "_" << camera.name << "_color";
        XP_SW_ASSERT_ERROR(camera.saveColorToEXR(ss.str()), "Failed to save color to EXR file");
#else
        LOG_ALERT("WRITING BACK TO GL TEXTURES");
        listener.onFrameRenderBoundingSquare(0, camera.resolution.x, 0, camera.resolution.y);
#endif
    }
    void zDrawTriangle(const std::array<XPVec4<T>, 3>& projectedVertices, const XPSWCamera<T>& camera)
    {
        // Avoid degenerate triangles
        // -------------------------------------------------------------------------------------
        T area = XPSWTriangle<T>::area(projectedVertices[0].xy, projectedVertices[1].xy, projectedVertices[2].xy);
        if (area == 0) { return; }
        // ----------------------------------------------------------------------------------------------------------------

        // bounding square around triangle
        // --------------------------------------------------------------------------------
        XPSWBoundingSquare<int64_t> bs = calculateTriangleBoundingSquare(
          XPVec4<T>{ 0, 0, static_cast<T>(camera.resolution.x), static_cast<T>(camera.resolution.y) },
          projectedVertices[0].xy,
          projectedVertices[1].xy,
          projectedVertices[2].xy);
        // ----------------------------------------------------------------------------------------------------------------

        // loop over bounding square
        for (int64_t y = bs.min.y; y <= bs.max.y; ++y) {
            for (int64_t x = bs.min.x; x <= bs.max.x; ++x) {
                // Compute barycentric coordinates for (x, y)
                XPVec3<T> barycentricCoordinates = computeBarycentricCoordinates(
                  projectedVertices[0].xy, projectedVertices[1].xy, projectedVertices[2].xy, x, y);

                if (barycentricCoordinates.x >= 0 && barycentricCoordinates.y >= 0 && barycentricCoordinates.z >= 0) {
                    // point is inside the triangle
                    T d = calculateDepthZeroToOne(barycentricCoordinates,
                                                  projectedVertices[0],
                                                  projectedVertices[1],
                                                  projectedVertices[2],
                                                  camera.zNearPlane,
                                                  camera.zFarPlane);
                    XP_SW_ASSERT_ERROR(d >= 0.0 && d <= 1.0, "Depth should be between 0.0 and 1.0");
                    camera.writeDepthBuffer(d, x, y);
                }
            }
        }
    }
    // Clip against a single 3D plane (e.g., near/far)
    [[nodiscard]] int zClipAgainstPlane3D(XPSWMemoryPool&      tpm,
                                          const XPSWVertex<T>* input,
                                          int                  inputCount,
                                          XPSWVertex<T>*       output,
                                          float                edge,
                                          int                  axis,
                                          int                  isMax,
                                          int                  isZForwardPositive)
    {
        int   outputCount = 0;
        auto& prevVertex  = *(XPSWVertex<T>*)tpm.pushFrameMemory(sizeof(XPSWVertex<T>));
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
            auto& currentVertex = *(XPSWVertex<T>*)tpm.pushFrameMemory(sizeof(XPSWVertex<T>));
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
                XPSWVertex<T>& intersect = output[outputCount];
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

            tpm.popFrameMemory(sizeof(XPSWVertex<T>));
        }
        tpm.popFrameMemory(sizeof(XPSWVertex<T>));
        return outputCount;
    }
    void zClipTriangle3D(XPSWMemoryPool&             tpm,
                         const XPSWTriangle<T>&      triangle,
                         std::vector<XPSWVertex<T>>& clippedPolygon,
                         int                         isZForwardPositive)
    {
        auto* input  = (XPSWVertex<T>*)tpm.pushFrameMemory(sizeof(XPSWVertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
        auto* output = (XPSWVertex<T>*)tpm.pushFrameMemory(sizeof(XPSWVertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);

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
                tpm.popFrameMemory(sizeof(XPSWVertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
                tpm.popFrameMemory(sizeof(XPSWVertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
                return;
            }
            memcpy(input, output, outputCount * sizeof(XPSWVertex<T>));
            inputCount = outputCount;
        }

        // Copy the final clipped vertices to the output polygon
        if (inputCount > clippedPolygon.size()) { clippedPolygon.resize(inputCount - clippedPolygon.size()); }
        for (int i = 0; i < inputCount; ++i) { clippedPolygon[i] = input[i]; }

        tpm.popFrameMemory(sizeof(XPSWVertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
        tpm.popFrameMemory(sizeof(XPSWVertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
    }
    void zClipTriangleToTriangles3D(XPSWMemoryPool&               tpm,
                                    const XPSWTriangle<T>&        triangle,
                                    std::vector<XPSWTriangle<T>>& clippedTriangles,
                                    int                           isZForwardPositive)
    {
        std::vector<XPSWVertex<T>> clippedPolygon;
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
    void zClipStage(XPSWMemoryPool& tpm, XPSWTriangle<T>& projectedVertices, const XPSWCamera<T>& camera)
    {
        // clipping
        std::vector<XPSWTriangle<T>> clippedTriangles;
        zClipTriangleToTriangles3D(tpm, projectedVertices, clippedTriangles, 1);
        if (clippedTriangles.empty()) {
            // all triangle vertices are actually clipped
            return;
        }

        auto& inverseViewMatrix =
          *(glm::mat<4, 4, T, glm::defaultp>*)tpm.pushFrameMemory(sizeof(glm::mat<4, 4, T, glm::defaultp>));
        auto& inverseProjectionMatrix =
          *(glm::mat<4, 4, T, glm::defaultp>*)tpm.pushFrameMemory(sizeof(glm::mat<4, 4, T, glm::defaultp>));
        auto& worldTriangle = *(XPSWTriangle<T>*)tpm.pushFrameMemory(sizeof(XPSWTriangle<T>));

        inverseViewMatrix       = glm::inverse(camera.viewMatrix.glm);
        inverseProjectionMatrix = glm::inverse(camera.projectionMatrix.glm);

        for (size_t i = 0; i < clippedTriangles.size(); ++i) {
            XPSWTriangle<T>& clippedTriangle = clippedTriangles[i];

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
            std::array<XPVec4<T>, 3> projectedVerticesToRasterize = { clippedTriangle.v0.location,
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
        tpm.popFrameMemory(sizeof(XPSWTriangle<T>));
    }
    void zVertexShader(XPSWMemoryPool&        tpm,
                       const XPSWTriangle<T>& t,
                       const XPMat4<T>&       modelMatrix,
                       const XPMat4<T>&       viewProjectionMatrix,
                       const XPSWCamera<T>&   camera)
    {
        auto& projectedVertices = *(XPSWTriangle<T>*)tpm.pushFrameMemory(sizeof(XPSWTriangle<T>));
        projectedVertices       = t;

        projectedVertices.v0.location = viewProjectionMatrix * modelMatrix * t.v0.location;
        projectedVertices.v1.location = viewProjectionMatrix * modelMatrix * t.v1.location;
        projectedVertices.v2.location = viewProjectionMatrix * modelMatrix * t.v2.location;
        // Here, officially traditional vertex shader ends -------------------------------

        zClipStage(tpm, projectedVertices, camera);

        tpm.popFrameMemory(sizeof(XPSWTriangle<T>));
    }
    void renderZPrePass(XPSWCamera<T>& camera)
    {
        XPSWMemoryPool tpm(32 * 1024 * sizeof(uint8_t));
        camera.clearDepthBuffer();

        const XPMat4<T>&            viewProjectionMatrix = camera.projectionMatrix * camera.viewMatrix;
        std::array<XPSWPlane<T>, 6> frustumPlanes        = XPSWPlane<T>::extractFrustumPlanes(viewProjectionMatrix);
        for (int64_t mi = 0; mi < scene->meshes.size(); ++mi) {
            XPSWMesh<T>& mesh = scene->meshes[mi];
            // frustum culling
            if (mesh.boundingBox.testFrustum(frustumPlanes) == XPSWEBoundingBoxFrustumTest_FullyOutside) {
                LOGV_DEBUG("[FRUSTUM CULLING ELIMINATED] {}", mesh.name);
                continue;
            }
            for (size_t ii = 0; ii < mesh.indices.size() - 2; ii += 3) {
                uint32_t i0 = mesh.indices[ii];
                uint32_t i1 = mesh.indices[ii + 1];
                uint32_t i2 = mesh.indices[ii + 2];

                XPSWTriangle<T> tr = {};
                tr.v0.location     = mesh.vertices[i0];
                tr.v1.location     = mesh.vertices[i1];
                tr.v2.location     = mesh.vertices[i2];
                zVertexShader(tpm, tr, mesh.transform, viewProjectionMatrix, camera);
                tpm.checkClear();
                tpm.popAllFrameMemory();
            }
        }
    }
    void render(XPSWRasterizerEventListener& listener)
    {
        if (scene == nullptr) { return; }
        for (size_t ci = 0; ci < scene->cameras.size(); ++ci) {
            XPSWCamera<T>& camera = scene->cameras[ci];
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
#if defined(XP_SW_USE_THREADS)
    XPSWThreadPool* threadPool;
#endif
    XPSWRenderer* renderer;
    XPSWScene<T>* scene;
    // uint8_t*       frameMemory;
    // int64_t        frameMemoryStart;
    // int64_t        frameMemoryEnd;
    XPSWTexture2D brdfTexture;
};
