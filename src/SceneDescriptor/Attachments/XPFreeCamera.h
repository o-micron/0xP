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

#ifndef XP_ATTACHMENT_FREE_CAMERA_H
#define XP_ATTACHMENT_FREE_CAMERA_H

#include <Utilities/XPAnnotations.h>
#include <Utilities/XPMaths.h>
#include <Utilities/XPPlatforms.h>

class XPNode;
class XPIUI;

struct XPAttachStructSecondary CameraProperties
{
    CameraProperties(float width, float height, float fov, float znear, float zfar)
      : viewProjectionMatrix()
      , inverseViewProjectionMatrix()
      , viewMatrix()
      , inverseViewMatrix()
      , projectionMatrix()
      , width(width)
      , height(height)
      , fov(fov)
      , znear(znear)
      , zfar(zfar)
      , location(0.0f, 0.0f, 0.0f)
      , euler(0.0f, 0.0f, 0.0f)
    {
    }

    XPMat4<float>       viewProjectionMatrix;
    XPMat4<float>       inverseViewProjectionMatrix;
    XPMat4<float>       viewMatrix;
    XPMat4<float>       inverseViewMatrix;
    XPMat4<float>       projectionMatrix;
    float               width;
    float               height;
    XPAttachField float fov;
    XPAttachField float znear;
    XPAttachField float zfar;
    XPAttachField XPVec3<float> location;
    XPAttachField XPVec3<float> euler;
};

struct XPAttachStruct FreeCamera
{
    explicit FreeCamera(XPNode* owner)
      : owner(owner)
      , activeProperties(1024, 720, 60, 0.01f, 1000.0f)
      , frozenProperties(1024, 720, 60, 0.01f, 1000.0f)
      , linearVelocity(0, 0, 0)
      , linearRotation(0, 0)
      , interpolationTime(0)
      , lookSensitivity(0.01f)
      , moveSensitivity(0.1f)
    {
    }
    void onChanged_activeProperties_fov() {}
    void onChanged_activeProperties_znear() {}
    void onChanged_activeProperties_zfar() {}
    void onChanged_activeProperties_location() {}
    void onChanged_activeProperties_euler() {}
    void onChanged_lookSensitivity() {}
    void onChanged_moveSensitivity() {}

    XPNode*                        owner;
    XPAttachField CameraProperties activeProperties;
    CameraProperties               frozenProperties;
    XPVec3<float>                  linearVelocity;
    XPVec2<float>                  linearRotation;
    float                          interpolationTime;
    XPAttachField float            lookSensitivity;
    XPAttachField float            moveSensitivity;
};

[[maybe_unused]] static void
onTraitAttached(FreeCamera* camera)
{
}

[[maybe_unused]] static void
onTraitDettached(FreeCamera* camera)
{
}

[[maybe_unused]] static void
onRenderUI(FreeCamera* camera, XPIUI* ui)
{
}

#endif