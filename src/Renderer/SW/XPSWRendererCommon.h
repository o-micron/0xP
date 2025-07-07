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
#include <Renderer/SW/XPSWSceneDescriptor.h>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

template<typename T>
T
DistributionGGX(XPVec3<T> N, XPVec3<T> H, T roughness)
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
GeometrySmith(const XPVec3<T>& N, const XPVec3<T>& V, const XPVec3<T>& L, T roughness)
{
    T NdotV = std::max(N.dot(V), T(0));
    T NdotL = std::max(N.dot(L), T(0));
    T ggx1  = GeometrySchlickGGX(NdotV, roughness);
    T ggx2  = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

template<typename T>
XPVec3<T>
FresnelSchlick(float cosTheta, const XPVec3<T>& F0)
{
    return F0 +
           XPVec3<T>{ 1.0f - F0.x, 1.0f - F0.y, 1.0f - F0.z } * std::pow(glm::clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
}

template<typename T>
XPVec3<T>
reinhardToneMapping(const XPVec3<T>& color)
{
    return color / (color + XPVec3<T>(1.0f, 1.0f, 1.0f));
}

template<typename T>
XPVec3<T>
gammaCorrect(const XPVec3<T>& color, float gamma)
{
    return { std::pow(color.x, 1.0f / gamma), std::pow(color.y, 1.0f / gamma), std::pow(color.z, 1.0f / gamma) };
}

template<typename T>
float
calculateReflectivity(XPVec3<T> albedo, float metallic, float roughness)
{
    // Base reflectivity for dielectrics (non-metals)
    const XPVec3<T> F0_dielectric = XPVec3<T>(0.04f, 0.04f, 0.04f);

    // For metals, F0 comes from the albedo (base color)
    XPVec3<T> F0 = glm::mix(F0_dielectric.glm, albedo.glm, metallic);

    // Roughness affects the sharpness of reflections but not the base reflectivity
    // (though in some models you might want to modify F0 based on roughness)
    return F0.x;
}

template<typename T>
XPVec3<T>
transformNormalToWorldSpace(const XPVec3<T>& sampledNormal,
                            const XPVec3<T>& tangent,
                            const XPVec3<T>& biTangent,
                            const XPVec3<T>& normal)
{
    XPVec3<T> TT = tangent;
    TT.normalize();
    XPVec3<T> B = biTangent;
    B.normalize();
    XPVec3<T> N = normal;
    N.normalize();

    TT = TT - N * (N.x * TT.x + N.y * TT.y + N.z * TT.z);
    TT.normalize();

    XPVec3<T> worldNormal;
    worldNormal.x = sampledNormal.x * TT.x + sampledNormal.y * B.x + sampledNormal.z * N.x;
    worldNormal.y = sampledNormal.x * TT.y + sampledNormal.y * B.y + sampledNormal.z * N.y;
    worldNormal.z = sampledNormal.x * TT.z + sampledNormal.y * B.z + sampledNormal.z * N.z;
    worldNormal.normalize();

    return worldNormal;
}

template<typename T>
void
getPBRMaterial(const XPSWMaterial<T>& material,
               const XPVec2<T>&       texCoord,
               const XPVec3<T>&       vertexBaseColor,
               const XPVec3<T>&       vertexNormal,
               XPVec3<T>&             baseColor,
               XPVec3<T>&             normal,
               XPVec3<T>&             emission,
               float&                 metallic,
               float&                 roughness,
               float&                 ao,
               bool                   repeatUVCoords)
{
    XPVec4<T> baseColorTexture =
      material.hasBaseColorTexture
        ? sampleTextureLinear<T>(material.baseColorTexture, texCoord.x, texCoord.y, repeatUVCoords, true)
        : XPVec4<T>{ vertexBaseColor.x, vertexBaseColor.y, vertexBaseColor.z, 1.0f };
    XPVec4<T> normalTexture =
      material.hasNormalMapTexture
        ? 2.0f * sampleTextureLinear<T>(material.normalMapTexture, texCoord.x, texCoord.y, repeatUVCoords, false) - 1.0f
        : XPVec4<T>{ 1.0f, 1.0f, 1.0f, 1.0f };
    XPVec4<T> emissionTexture =
      material.hasEmissionColorTexture
        ? sampleTextureLinear<T>(material.emissionColorTexture, texCoord.x, texCoord.y, repeatUVCoords, false)
        : XPVec4<T>{ 1.0f, 1.0f, 1.0f, 1.0f };
    XPVec4<T> metallicTexture =
      material.hasMetallicTexture
        ? sampleTextureLinear<T>(material.metallicTexture, texCoord.x, texCoord.y, repeatUVCoords, true)
        : XPVec4<T>{ 1.0f, 1.0f, 1.0f, 1.0f };
    XPVec4<T> roughnessTexture =
      material.hasRoughnessTexture
        ? sampleTextureLinear<T>(material.roughnessTexture, texCoord.x, texCoord.y, repeatUVCoords, true)
        : XPVec4<T>{ 1.0f, 1.0f, 1.0f, 1.0f };
    XPVec4<T> aoTexture = material.hasAOTexture
                            ? sampleTextureLinear<T>(material.aoTexture, texCoord.x, texCoord.y, repeatUVCoords, false)
                            : XPVec4<T>{ 1.0f, 1.0f, 1.0f, 1.0f };

    baseColor = material.hasBaseColorTexture ? (baseColorTexture).xyz : vertexBaseColor;
    normal    = material.hasNormalMapTexture ? (normalTexture).xyz : vertexNormal;
    normal.normalize();
    emission  = material.hasEmissionColorTexture ? (emissionTexture).xyz : material.emissionColorValue;
    metallic  = material.hasMetallicTexture ? (metallicTexture).x : material.metallicValue;
    roughness = material.hasRoughnessTexture ? (roughnessTexture).x : material.roughnessValue;
    ao        = material.hasAOTexture ? (aoTexture).x : material.aoValue;
}
