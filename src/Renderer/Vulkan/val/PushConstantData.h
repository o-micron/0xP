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

#include <Utilities/XPMaths.h>
#include <Utilities/XPPlatforms.h>

namespace val {

struct PushConstantData
{
    XPMat4<float> modelMatrix;
    uint32_t      objectId;
    float         unused00;
    float         unused01;
    float         unused10;
    XPVec4<float> unused1;
    XPVec4<float> unused2;
    XPVec4<float> unused3;
};
static_assert(sizeof(PushConstantData) == 128);

} // namespace val
