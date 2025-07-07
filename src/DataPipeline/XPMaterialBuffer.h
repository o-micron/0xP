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

#include <Utilities/XPPlatforms.h>

#include <Utilities/XPMaths.h>
#include <Utilities/XPMemoryPool.h>

#include <optional>
#include <string>
#include <variant>

class XPMaterialAsset;

struct XPPhongSystem
{
    XPVec3<float> diffuse;
    XPVec3<float> specular;
    XPVec3<float> ambient;
    XPVec3<float> emission;
    std::string   diffuseTexture;
    std::string   specularTexture;
    std::string   ambientTexture;
    std::string   normalTexture;
    std::string   emissionTexture;
};

struct XPPBRSystem
{
    XPVec3<float> albedo;
    XPVec3<float> ambient;
    XPVec3<float> emission;
    float         metallic;
    float         roughness;
    std::string   albedoTexture;
    std::string   ambientTexture;
    std::string   normalTexture;
    std::string   metallicTexture;
    std::string   roughnessTexture;
    std::string   emissionTexture;
};

class XPMaterialBuffer
{
    XP_MPL_MEMORY_POOL(XPMaterialBuffer)

  public:
    explicit XPMaterialBuffer(XPMaterialAsset* const materialAsset, const uint32_t id);
    ~XPMaterialBuffer();
    [[nodiscard]] XPMaterialAsset*             getMaterialAsset() const;
    [[nodiscard]] uint32_t                     getId() const;
    [[nodiscard]] std::optional<XPPhongSystem> getPhongSystem() const;
    [[nodiscard]] std::optional<XPPBRSystem>   getPBRSystem() const;
    void                                       setSystem(XPPhongSystem phongSystem);
    void                                       setSystem(XPPBRSystem pbrSystem);

  private:
    XPMaterialAsset* const                   _materialAsset;
    const uint32_t                           _id;
    std::variant<XPPhongSystem, XPPBRSystem> _system;
};
