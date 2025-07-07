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

struct FreeCamera;
class XPInput;
class XPRegistry;

extern void
CameraSystemUpdateAll(FreeCamera* camera, XPRegistry* registry, const XPInput& input, float deltaTime);
extern void
CameraSystemSetRotation(FreeCamera* camera, const glm::vec2& rotation);
extern void
CameraSystemSetTranslation(FreeCamera* camera, const glm::vec3& translation);
extern void
CameraSystemRotate(FreeCamera* camera, float x, float y);
extern void
CameraSystemTranslate(FreeCamera* camera, float x, float y, float z);
extern void
CameraSystemSetResolution(FreeCamera* camera, float width, float height);
extern void
CameraSystemSetFov(FreeCamera* camera, float fov);
extern void
CameraSystemSetNear(FreeCamera* camera, float near);
extern void
CameraSystemSetFar(FreeCamera* camera, float far);
extern void
CameraSystemSetViewMatrix(FreeCamera* camera, const glm::mat4& matrix);
extern void
CameraSystemSetProjectionMatrix(FreeCamera* camera, const glm::mat4& matrix);
