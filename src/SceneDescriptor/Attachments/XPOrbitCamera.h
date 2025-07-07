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

#ifndef XP_ATTACHMENT_ORBIT_CAMERA_H
#define XP_ATTACHMENT_ORBIT_CAMERA_H

#include <Utilities/XPAnnotations.h>
#include <Utilities/XPMaths.h>
#include <Utilities/XPPlatforms.h>

class XPNode;
class XPIUI;

struct XPAttachStruct OrbitCamera
{
    explicit OrbitCamera(XPNode* owner)
      : owner(owner)
      , location(0.0f, 0.0f, 0.0f)
      , euler(0.0f, 0.0f, 0.0f)
      , fieldOfView(60.0f)
      , znear(0.001f)
      , zfar(1000.0f)
      , viewMatrix()
      , projectionMatrix()
    {
    }

    void onChanged_location() {}
    void onChanged_euler() {}
    void onChanged_fieldOfView() {}
    void onChanged_znear() {}
    void onChanged_zfar() {}

    XPNode*       owner;
    XPAttachField XPVec3<float> location;
    XPAttachField XPVec3<float> euler;
    XPAttachField float         fieldOfView;
    XPAttachField float         znear;
    XPAttachField float         zfar;
    XPMat4<float>               viewMatrix;
    XPMat4<float>               projectionMatrix;
};

[[maybe_unused]] static void
onTraitAttached(OrbitCamera* camera)
{
}

[[maybe_unused]] static void
onTraitDettached(OrbitCamera* camera)
{
}

[[maybe_unused]] static void
onRenderUI(OrbitCamera* camera, XPIUI* ui)
{
}

#endif