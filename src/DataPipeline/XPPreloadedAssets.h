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

#include <map>
#include <string>
#include <vector>

struct XPPreloadedMesh
{
    std::vector<XPVec4<float>> positions;
    std::vector<XPVec4<float>> normals;
    std::vector<XPVec4<float>> texcoords;
    std::vector<uint32_t>      indices;
    XPBoundingBox              boundingBox;
};

class XPPreloadedAssets
{
  public:
    static const std::map<const std::string, XPPreloadedMesh>& meshes();

    XPPreloadedAssets()  = delete;
    ~XPPreloadedAssets() = delete;
};