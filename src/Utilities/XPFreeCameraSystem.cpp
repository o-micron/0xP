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

#include <Controllers/XPInput.h>
#include <Engine/XPConsole.h>
#include <Engine/XPEngine.h>
#include <Engine/XPRegistry.h>
#include <SceneDescriptor/Attachments/XPFreeCamera.h>
#include <Utilities/XPFreeCameraSystem.h>

extern void
CameraSystemUpdateAll(FreeCamera* camera, XPRegistry* registry, const XPInput& input, float deltaTime)
{
    float forward  = input.fetchKeyNormalizedValue(SDLK_w);
    float backward = input.fetchKeyNormalizedValue(SDLK_s);
    float right    = input.fetchKeyNormalizedValue(SDLK_d);
    float left     = input.fetchKeyNormalizedValue(SDLK_a);
    float up       = input.fetchKeyNormalizedValue(SDLK_i);
    float down     = input.fetchKeyNormalizedValue(SDLK_k);
    {
        float xdir = left - right;
        float ydir = down - up;
        float zdir = forward - backward;
        CameraSystemTranslate(camera, xdir, ydir, zdir);

        if (camera->interpolationTime < 1.0f) {
            camera->interpolationTime += 0.01f + (1.0f - camera->interpolationTime) * 10.0f * deltaTime / 1000.0f;
            camera->linearRotation.glm =
              glm::lerp(camera->linearRotation.glm, glm::vec2(0.0f, 0.0f), camera->interpolationTime);

            camera->activeProperties.euler.x -= camera->linearRotation.y * camera->lookSensitivity;
            camera->activeProperties.euler.y -= camera->linearRotation.x * camera->lookSensitivity;
            camera->activeProperties.euler.x = glm::clamp(camera->activeProperties.euler.x, -80.0f, 80.0f);
            camera->activeProperties.euler.y = fmod(camera->activeProperties.euler.y, 360.0f);
        }

        XPMat4<float>::buildViewProjectionMatrices(camera->activeProperties.viewProjectionMatrix,
                                                   camera->activeProperties.inverseViewProjectionMatrix,
                                                   camera->activeProperties.viewMatrix,
                                                   camera->activeProperties.inverseViewMatrix,
                                                   camera->activeProperties.projectionMatrix,
                                                   camera->activeProperties.location,
                                                   camera->activeProperties.euler,
                                                   camera->activeProperties.fov,
                                                   camera->activeProperties.width,
                                                   camera->activeProperties.height,
                                                   camera->activeProperties.znear,
                                                   camera->activeProperties.zfar);
    }
    XPConsole* console = registry->getEngine()->getConsole();
    if (console->getVariableValue<bool>("r.freeze") == false) {
        memcpy((void*)&camera->frozenProperties, &camera->activeProperties, sizeof(CameraProperties));
    }
}

extern void
CameraSystemSetRotation(FreeCamera* camera, const glm::vec2& rotation)
{
    camera->activeProperties.euler.x = glm::clamp(rotation.x, -80.0f, 80.0f);
    camera->activeProperties.euler.y = fmod(rotation.y, 360.0f);
    camera->interpolationTime        = 1.0f;
}

extern void
CameraSystemSetTranslation(FreeCamera* camera, const glm::vec3& translation)
{
    camera->activeProperties.location.x = translation.x;
    camera->activeProperties.location.y = translation.y;
    camera->activeProperties.location.z = translation.z;
}

extern void
CameraSystemRotate(FreeCamera* camera, float x, float y)
{
    camera->interpolationTime = 0.0f;
    camera->linearRotation.x += x;
    camera->linearRotation.y += y;
}

extern void
CameraSystemTranslate(FreeCamera* camera, float x, float y, float z)
{
    float     height = camera->activeProperties.location.y;
    glm::quat q =
      glm::eulerAngleYX(glm::radians(camera->activeProperties.euler.y), glm::radians(camera->activeProperties.euler.x));
    glm::vec3 step =
      q * glm::vec3(x * camera->moveSensitivity, y * camera->moveSensitivity, z * camera->moveSensitivity);
    camera->activeProperties.location.x -= step.x;
    camera->activeProperties.location.z -= step.z;
    camera->activeProperties.location.y = height - y * camera->moveSensitivity;
}

extern void
CameraSystemSetResolution(FreeCamera* camera, float width, float height)
{
    camera->activeProperties.width  = width;
    camera->activeProperties.height = height;
}

extern void
CameraSystemSetFov(FreeCamera* camera, float fov)
{
    camera->activeProperties.fov = fov;
}

extern void
CameraSystemSetNear(FreeCamera* camera, float near)
{
    camera->activeProperties.znear = near;
}

extern void
CameraSystemSetFar(FreeCamera* camera, float far)
{
    camera->activeProperties.zfar = far;
}

extern void
CameraSystemSetViewMatrix(FreeCamera* camera, const glm::mat4& matrix)
{
    memcpy(&camera->activeProperties.viewMatrix._00, &matrix[0], sizeof(glm::mat4));
}

extern void
CameraSystemSetProjectionMatrix(FreeCamera* camera, const glm::mat4& matrix)
{
    memcpy(&camera->activeProperties.projectionMatrix._00, &matrix[0], sizeof(glm::mat4));
}
