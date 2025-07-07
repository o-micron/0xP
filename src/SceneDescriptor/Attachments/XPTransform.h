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

#ifndef XP_ATTACHMENT_TRANSFORM_H
#define XP_ATTACHMENT_TRANSFORM_H

#include <Utilities/XPAnnotations.h>
#include <Utilities/XPMaths.h>
#include <Utilities/XPPlatforms.h>

class XPNode;
class XPIUI;

struct XPAttachStruct Transform
{
    explicit Transform(XPNode* owner)
      : owner(owner)
      , location(0.0f, 0.0f, 0.0f)
      , euler(0.0f, 0.0f, 0.0f)
      , scale(1.0f, 1.0f, 1.0f)
      , modelMatrix()
    {
    }

    void onChanged_location();
    void onChanged_euler();
    void onChanged_scale();
    void onChanged_modelMatrix();

    XPNode*       owner;
    XPAttachField XPVec3<float> location;
    XPAttachField XPVec3<float> euler;
    XPAttachField XPVec3<float> scale;
    XPAttachField XPMat4<float> modelMatrix;
};

[[maybe_unused]] static void
onTraitAttached(Transform* transform)
{
}

[[maybe_unused]] static void
onTraitDettached(Transform* transform)
{
}

[[maybe_unused]] static void
onRenderUI(Transform* transform, XPIUI* ui)
{
}

#endif