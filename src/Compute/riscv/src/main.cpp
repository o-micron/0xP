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

#include "../../shaders/shared/config.h"

#include "Renderer.h"
#include "SceneDescription.h"

#include <stdint.h>
#include <stdlib.h>

extern "C" char _bottom_stack;
extern "C" char _top_stack;

extern "C" char _start_heap;
extern "C" char _end_heap;

template<typename T>
void
CameraCreateMatrices(Camera<T>& camera, const Vec3<T>& location, const Vec3<T>& target, const Vec3<T>& up)
{
    camera.viewMatrix                  = createViewMatrix(location, target, up);
    camera.projectionMatrix            = createPerspectiveProjectionMatrix(static_cast<T>(camera.fov),
                                                                static_cast<T>(camera.resolution.x),
                                                                static_cast<T>(camera.resolution.y),
                                                                camera.zNearPlane,
                                                                camera.zFarPlane);
    camera.viewProjectionMatrix        = camera.projectionMatrix * camera.viewMatrix;
    camera.inverseViewMatrix.glm       = glm::inverse(camera.viewMatrix.glm);
    camera.inverseProjectionMatrix.glm = glm::inverse(camera.projectionMatrix.glm);
}

template<typename T>
void
renderTest(FramebufferRef& fbr, FrameMemoryPool& fmp)
{
    Scene<T> scene = {};

    Renderer renderer;

    constexpr float s = static_cast<T>(0.5);

    // CLOCKWISE WINDING ORDER
    scene.meshes.resize(1);
    scene.meshes[0].indices = { 0, 1, 2 };
    scene.meshes[0].vertices.resize(3);
    scene.meshes[0].vertices[0] = Vec4<T>{ -s, -s, 0, 1 };
    scene.meshes[0].vertices[1] = Vec4<T>{ 0, s, 0, 1 };
    scene.meshes[0].vertices[2] = Vec4<T>{ s, -s, 0, 1 };
    scene.meshes[0].normals.resize(3);
    scene.meshes[0].normals[0] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[0].normals[1] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[0].normals[2] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[0].texCoords.resize(3);
    scene.meshes[0].texCoords[0]    = Vec2<T>{ 0, 0 };
    scene.meshes[0].texCoords[1]    = Vec2<T>{ 0.5, 1 };
    scene.meshes[0].texCoords[2]    = Vec2<T>{ 1, 0 };
    scene.meshes[0].materialIndex   = 0;
    scene.meshes[0].transform       = Mat4<T>::identity();
    scene.meshes[0].boundingBox.min = Vec3<T>{ -s, -s, 0 };
    scene.meshes[0].boundingBox.max = Vec3<T>{ s, s, 0 };
    {
        scene.lights.push_back({});
        Light<T>& light            = scene.lights.back();
        light.location             = Vec3<T>{ static_cast<T>(0), static_cast<T>(5), static_cast<T>(-5) };
        light.direction            = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(-1) };
        light.ambient              = Vec3<T>{ static_cast<T>(1.0), static_cast<T>(1.0), static_cast<T>(1.0) };
        light.diffuse              = Vec3<T>{ static_cast<T>(100.0), static_cast<T>(100.0), static_cast<T>(100.0) };
        light.specular             = Vec3<T>{ static_cast<T>(100.0), static_cast<T>(100.0), static_cast<T>(100.0) };
        light.intensity            = static_cast<T>(0.01);
        light.attenuationConstant  = static_cast<T>(0.1);
        light.attenuationLinear    = static_cast<T>(0.01);
        light.attenuationQuadratic = static_cast<T>(0.001);
        light.type                 = ELightType_Point;
        light.angleInnerCone       = static_cast<T>(20);
        light.angleOuterCone       = static_cast<T>(45);

        scene.cameras.push_back({});
        Camera<T>& camera = scene.cameras.back();
        camera.resolution = u32vec2{ 120, 120 };
        camera.fov        = static_cast<T>(90);
        camera.zNearPlane = static_cast<T>(0.01);
        camera.zFarPlane  = static_cast<T>(10.0);
        Vec3<T> location  = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(1) };
        Vec3<T> target    = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(0) };
        Vec3<T> up        = Vec3<T>{ static_cast<T>(0), static_cast<T>(1), static_cast<T>(0) };
        CameraCreateMatrices(camera, location, target, up);

        Material<T>& material            = scene.materials[0];
        material.hasBaseColorTexture     = false;
        material.hasNormalMapTexture     = false;
        material.hasEmissionColorTexture = false;
        material.hasMetallicTexture      = false;
        material.hasRoughnessTexture     = false;
        material.hasAOTexture            = false;
        material.baseColorValue          = Vec3<T>{ static_cast<T>(1), static_cast<T>(1), static_cast<T>(1) };
        material.emissionColorValue      = Vec3<T>{ static_cast<T>(0.1), static_cast<T>(0.1), static_cast<T>(0.1) };
        material.metallicValue           = 0.0f;
        material.roughnessValue          = 0.5f;
        material.aoValue                 = 1.0f;
    }

    renderer.render(fbr, fmp, &scene);
}

int
main()
{
    *((volatile uint32_t*)(XP_XPU_CONFIG_HMM_BOTTOM_STACK_PTR)) = (uint32_t)((void*)&_bottom_stack);
    *((volatile uint32_t*)(XP_XPU_CONFIG_HMM_TOP_STACK_PTR))    = (uint32_t)((void*)&_top_stack);
    *((volatile uint32_t*)(XP_XPU_CONFIG_HMM_START_HEAP_PTR))   = (uint32_t)((void*)&_start_heap) + 8;
    *((volatile uint32_t*)(XP_XPU_CONFIG_HMM_END_HEAP_PTR))     = (uint32_t)((void*)&_end_heap);
    *((volatile uint32_t*)(XP_XPU_CONFIG_HMM_BASE))             = 0x1;

    uint8_t* framebuffer           = (uint8_t*)malloc(sizeof(float) * 120 * 120 * 4);
    float*   colorAttachmentBuffer = (float*)framebuffer;
    float*   depthAttachmentBuffer = colorAttachmentBuffer + 120 * 120 * 3;

    int square_size = 5;

    for (int y = 0; y < 120; y++) {
        for (int x = 0; x < 120; x++) {
            int pixel_index = (y * 120 + x) * 3;

            int square_x = x / square_size;
            int square_y = y / square_size;

            if ((square_x + square_y) % 2 == 0) {
                colorAttachmentBuffer[pixel_index + 0] = 1.0f;
                colorAttachmentBuffer[pixel_index + 1] = 1.0f;
                colorAttachmentBuffer[pixel_index + 2] = 1.0f;
            } else {
                colorAttachmentBuffer[pixel_index + 0] = 0.0f;
                colorAttachmentBuffer[pixel_index + 1] = 0.0f;
                colorAttachmentBuffer[pixel_index + 2] = 0.0f;
            }
        }
    }

    uint32_t frameMemoryPoolNumBytes = sizeof(uint8_t) * 250 * 1024;
    uint8_t* frameMemoryPool         = (uint8_t*)malloc(frameMemoryPoolNumBytes);

    FramebufferRef  fbr(colorAttachmentBuffer, depthAttachmentBuffer, 120, 120);
    FrameMemoryPool fmp(frameMemoryPool, frameMemoryPoolNumBytes);
    renderTest<float>(fbr, fmp);

    return 0;
}
