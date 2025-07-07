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
#include <Renderer/SW/XPSWRenderer.h>

#include <sstream>

template<typename T>
void
testTriangle()
{
    LOG_DEBUG("=================================================================================");
    LOG_DEBUG("START TEST TRIANGLE");
    LOG_DEBUG("=================================================================================");

    XPSWScene<T> scene = {};
    scene.filepath     = "triangle";

    XPSWRenderer renderer;

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
    scene.meshes[0].texCoords[0] = Vec2<T>{ 0, 0 };
    scene.meshes[0].texCoords[1] = Vec2<T>{ 0.5, 1 };
    scene.meshes[0].texCoords[2] = Vec2<T>{ 1, 0 };
    scene.meshes[0].colors.resize(3);
    scene.meshes[0].colors[0]       = Vec3<T>{ 1, 0, 0 };
    scene.meshes[0].colors[1]       = Vec3<T>{ 0, 1, 0 };
    scene.meshes[0].colors[2]       = Vec3<T>{ 0, 0, 1 };
    scene.meshes[0].materialIndex   = 0;
    scene.meshes[0].transform       = Mat4<T>::identity();
    scene.meshes[0].boundingBox.min = Vec3<T>{ -s, -s, 0 };
    scene.meshes[0].boundingBox.max = Vec3<T>{ s, s, 0 };
    Vec3<T> tangent, biTangent;
    XPSWTriangle<T>::generateTangentsAndBiTangents(scene.meshes[0].vertices[0].xyz,
                                                   scene.meshes[0].vertices[1].xyz,
                                                   scene.meshes[0].vertices[2].xyz,
                                                   scene.meshes[0].texCoords[0],
                                                   scene.meshes[0].texCoords[1],
                                                   scene.meshes[0].texCoords[2],
                                                   tangent,
                                                   biTangent);
    scene.meshes[0].tangents.resize(3);
    scene.meshes[0].tangents[0] = tangent;
    scene.meshes[0].tangents[1] = tangent;
    scene.meshes[0].tangents[2] = tangent;
    scene.meshes[0].biTangents.resize(3);
    scene.meshes[0].biTangents[0] = biTangent;
    scene.meshes[0].biTangents[1] = biTangent;
    scene.meshes[0].biTangents[2] = biTangent;
    {
        scene.lights.push_back({});
        Light<T>& light            = scene.lights.back();
        light.location             = Vec3<T>{ static_cast<T>(0), static_cast<T>(5), static_cast<T>(-5) };
        light.direction            = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(-1) };
        light.ambient              = Vec3<T>{ static_cast<T>(1.0), static_cast<T>(1.0), static_cast<T>(1.0) };
        light.diffuse              = Vec3<T>{ static_cast<T>(100.0), static_cast<T>(100.0), static_cast<T>(100.0) };
        light.specular             = Vec3<T>{ static_cast<T>(100.0), static_cast<T>(100.0), static_cast<T>(100.0) };
        light.intensity            = static_cast<T>(100);
        light.attenuationConstant  = static_cast<T>(0.1);
        light.attenuationLinear    = static_cast<T>(0.01);
        light.attenuationQuadratic = static_cast<T>(0.001);
        light.type                 = ELightType_Point;
        light.angleInnerCone       = static_cast<T>(20);
        light.angleOuterCone       = static_cast<T>(45);

        scene.cameras.push_back({});
        Camera<T>& camera = scene.cameras.back();
        camera.resolution = u32vec2{ WIDTH, HEIGHT };
        camera.fov        = static_cast<T>(90);
        camera.zNearPlane = static_cast<T>(0.01);
        camera.zFarPlane  = static_cast<T>(10.0);
        camera.location   = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(1) };
        camera.target     = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(0) };
        camera.up         = Vec3<T>{ static_cast<T>(0), static_cast<T>(1), static_cast<T>(0) };
        camera.updateMatrices();

        XPSWMaterial<T>& material        = scene.materials[0];
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

    XPSWRasterizerEventListener rasterizerEventListener;
    rasterizerEventListener.onFrameRenderBoundingSquare = [&](size_t xmin, size_t xmax, size_t ymin, size_t ymax) {};
    rasterizerEventListener.onFrameSetColorBufferPtr    = [&](float* data, size_t w, size_t h) {};

    renderer.render(&scene, rasterizerEventListener);

    LOG_DEBUG("=================================================================================");
}

template<typename T>
void
testTriangles()
{
    LOG_DEBUG("=================================================================================");
    LOG_DEBUG("START TEST TRIANGLES");
    LOG_DEBUG("=================================================================================");

    XPSWScene<T> scene = {};
    scene.filepath     = "triangles";

    XPSWRenderer renderer;

    constexpr float s = static_cast<T>(0.25);
    Vec3<T>         tangent, biTangent;

    // CLOCKWISE WINDING ORDER
    scene.meshes.resize(4);
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
    scene.meshes[0].texCoords[0] = Vec2<T>{ 0, 0 };
    scene.meshes[0].texCoords[1] = Vec2<T>{ 0.5, 1 };
    scene.meshes[0].texCoords[2] = Vec2<T>{ 1, 0 };
    scene.meshes[0].colors.resize(3);
    scene.meshes[0].colors[0]     = Vec3<T>{ 1, 0, 0 };
    scene.meshes[0].colors[1]     = Vec3<T>{ 0, 1, 0 };
    scene.meshes[0].colors[2]     = Vec3<T>{ 0, 0, 1 };
    scene.meshes[0].materialIndex = 0;
    {
        Vec3<T> translation           = Vec3<T>{ static_cast<T>(s), static_cast<T>(0), static_cast<T>(0) };
        Vec3<T> rotation              = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(90) };
        Vec3<T> scale                 = Vec3<T>{ static_cast<T>(1), static_cast<T>(1), static_cast<T>(1) };
        scene.meshes[0].transform.glm = createModelMatrix(translation, rotation, scale).glm;
    }
    scene.meshes[0].boundingBox.min = Vec3<T>{ -s, -s, 0 };
    scene.meshes[0].boundingBox.max = Vec3<T>{ s, s, 0 };
    XPSWTriangle<T>::generateTangentsAndBiTangents(scene.meshes[0].vertices[0].xyz,
                                                   scene.meshes[0].vertices[1].xyz,
                                                   scene.meshes[0].vertices[2].xyz,
                                                   scene.meshes[0].texCoords[0],
                                                   scene.meshes[0].texCoords[1],
                                                   scene.meshes[0].texCoords[2],
                                                   tangent,
                                                   biTangent);
    scene.meshes[0].tangents.resize(3);
    scene.meshes[0].tangents[0] = tangent;
    scene.meshes[0].tangents[1] = tangent;
    scene.meshes[0].tangents[2] = tangent;
    scene.meshes[0].biTangents.resize(3);
    scene.meshes[0].biTangents[0] = biTangent;
    scene.meshes[0].biTangents[1] = biTangent;
    scene.meshes[0].biTangents[2] = biTangent;

    scene.meshes[1].indices = { 0, 1, 2 };
    scene.meshes[1].vertices.resize(3);
    scene.meshes[1].vertices[0] = Vec4<T>{ -s, -s, 0, 1 };
    scene.meshes[1].vertices[1] = Vec4<T>{ 0, s, 0, 1 };
    scene.meshes[1].vertices[2] = Vec4<T>{ s, -s, 0, 1 };
    scene.meshes[1].normals.resize(3);
    scene.meshes[1].normals[0] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[1].normals[1] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[1].normals[2] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[1].texCoords.resize(3);
    scene.meshes[1].texCoords[0] = Vec2<T>{ 0, 0 };
    scene.meshes[1].texCoords[1] = Vec2<T>{ 0.5, 1 };
    scene.meshes[1].texCoords[2] = Vec2<T>{ 1, 0 };
    scene.meshes[1].colors.resize(3);
    scene.meshes[1].colors[0]     = Vec3<T>{ 1, 0, 0 };
    scene.meshes[1].colors[1]     = Vec3<T>{ 0, 1, 0 };
    scene.meshes[1].colors[2]     = Vec3<T>{ 0, 0, 1 };
    scene.meshes[1].materialIndex = 0;
    {
        Vec3<T> translation       = Vec3<T>{ static_cast<T>(-s), static_cast<T>(0), static_cast<T>(0) };
        Vec3<T> rotation          = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(-90) };
        Vec3<T> scale             = Vec3<T>{ static_cast<T>(1), static_cast<T>(1), static_cast<T>(1) };
        scene.meshes[1].transform = createModelMatrix(translation, rotation, scale).glm;
    }
    scene.meshes[1].boundingBox.min = Vec3<T>{ -s, -s, 0 };
    scene.meshes[1].boundingBox.max = Vec3<T>{ s, s, 0 };
    XPSWTriangle<T>::generateTangentsAndBiTangents(scene.meshes[1].vertices[0].xyz,
                                                   scene.meshes[1].vertices[1].xyz,
                                                   scene.meshes[1].vertices[2].xyz,
                                                   scene.meshes[1].texCoords[0],
                                                   scene.meshes[1].texCoords[1],
                                                   scene.meshes[1].texCoords[2],
                                                   tangent,
                                                   biTangent);
    scene.meshes[1].tangents.resize(3);
    scene.meshes[1].tangents[0] = tangent;
    scene.meshes[1].tangents[1] = tangent;
    scene.meshes[1].tangents[2] = tangent;
    scene.meshes[1].biTangents.resize(3);
    scene.meshes[1].biTangents[0] = biTangent;
    scene.meshes[1].biTangents[1] = biTangent;
    scene.meshes[1].biTangents[2] = biTangent;

    scene.meshes[2].indices = { 0, 1, 2 };
    scene.meshes[2].vertices.resize(3);
    scene.meshes[2].vertices[0] = Vec4<T>{ -s, -s, 0, 1 };
    scene.meshes[2].vertices[1] = Vec4<T>{ 0, s, 0, 1 };
    scene.meshes[2].vertices[2] = Vec4<T>{ s, -s, 0, 1 };
    scene.meshes[2].normals.resize(3);
    scene.meshes[2].normals[0] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[2].normals[1] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[2].normals[2] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[2].texCoords.resize(3);
    scene.meshes[2].texCoords[0] = Vec2<T>{ 0, 0 };
    scene.meshes[2].texCoords[1] = Vec2<T>{ 0.5, 1 };
    scene.meshes[2].texCoords[2] = Vec2<T>{ 1, 0 };
    scene.meshes[2].colors.resize(3);
    scene.meshes[2].colors[0]     = Vec3<T>{ 1, 0, 0 };
    scene.meshes[2].colors[1]     = Vec3<T>{ 0, 1, 0 };
    scene.meshes[2].colors[2]     = Vec3<T>{ 0, 0, 1 };
    scene.meshes[2].materialIndex = 0;
    {
        Vec3<T> translation       = Vec3<T>{ static_cast<T>(0), static_cast<T>(s), static_cast<T>(0) };
        Vec3<T> rotation          = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(180.0) };
        Vec3<T> scale             = Vec3<T>{ static_cast<T>(1), static_cast<T>(1), static_cast<T>(1) };
        scene.meshes[2].transform = createModelMatrix(translation, rotation, scale).glm;
    }
    scene.meshes[2].boundingBox.min = Vec3<T>{ -s, -s, 0 };
    scene.meshes[2].boundingBox.max = Vec3<T>{ s, s, 0 };
    XPSWTriangle<T>::generateTangentsAndBiTangents(scene.meshes[2].vertices[0].xyz,
                                                   scene.meshes[2].vertices[1].xyz,
                                                   scene.meshes[2].vertices[2].xyz,
                                                   scene.meshes[2].texCoords[0],
                                                   scene.meshes[2].texCoords[1],
                                                   scene.meshes[2].texCoords[2],
                                                   tangent,
                                                   biTangent);
    scene.meshes[2].tangents.resize(3);
    scene.meshes[2].tangents[0] = tangent;
    scene.meshes[2].tangents[1] = tangent;
    scene.meshes[2].tangents[2] = tangent;
    scene.meshes[2].biTangents.resize(3);
    scene.meshes[2].biTangents[0] = biTangent;
    scene.meshes[2].biTangents[1] = biTangent;
    scene.meshes[2].biTangents[2] = biTangent;

    scene.meshes[3].indices = { 0, 1, 2 };
    scene.meshes[3].vertices.resize(3);
    scene.meshes[3].vertices[0] = Vec4<T>{ -s, -s, 0, 1 };
    scene.meshes[3].vertices[1] = Vec4<T>{ 0, s, 0, 1 };
    scene.meshes[3].vertices[2] = Vec4<T>{ s, -s, 0, 1 };
    scene.meshes[3].normals.resize(3);
    scene.meshes[3].normals[0] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[3].normals[1] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[3].normals[2] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[3].texCoords.resize(3);
    scene.meshes[3].texCoords[0] = Vec2<T>{ 0, 0 };
    scene.meshes[3].texCoords[1] = Vec2<T>{ 0.5, 1 };
    scene.meshes[3].texCoords[2] = Vec2<T>{ 1, 0 };
    scene.meshes[3].colors.resize(3);
    scene.meshes[3].colors[0]     = Vec3<T>{ 1, 0, 0 };
    scene.meshes[3].colors[1]     = Vec3<T>{ 0, 1, 0 };
    scene.meshes[3].colors[2]     = Vec3<T>{ 0, 0, 1 };
    scene.meshes[3].materialIndex = 0;
    {
        Vec3<T> translation       = { static_cast<T>(0), static_cast<T>(-s), static_cast<T>(0) };
        Vec3<T> rotation          = { static_cast<T>(0), static_cast<T>(0), static_cast<T>(0) };
        Vec3<T> scale             = { static_cast<T>(1), static_cast<T>(1), static_cast<T>(1) };
        scene.meshes[3].transform = createModelMatrix(translation, rotation, scale).glm;
    }
    scene.meshes[3].boundingBox.min = Vec3<T>{ -s, -s, 0 };
    scene.meshes[3].boundingBox.max = Vec3<T>{ s, s, 0 };
    XPSWTriangle<T>::generateTangentsAndBiTangents(scene.meshes[3].vertices[0].xyz,
                                                   scene.meshes[3].vertices[1].xyz,
                                                   scene.meshes[3].vertices[2].xyz,
                                                   scene.meshes[3].texCoords[0],
                                                   scene.meshes[3].texCoords[1],
                                                   scene.meshes[3].texCoords[2],
                                                   tangent,
                                                   biTangent);
    scene.meshes[3].tangents.resize(3);
    scene.meshes[3].tangents[0] = tangent;
    scene.meshes[3].tangents[1] = tangent;
    scene.meshes[3].tangents[2] = tangent;
    scene.meshes[3].biTangents.resize(3);
    scene.meshes[3].biTangents[0] = biTangent;
    scene.meshes[3].biTangents[1] = biTangent;
    scene.meshes[3].biTangents[2] = biTangent;

    {
        scene.lights.push_back({});
        Light<T>& light            = scene.lights.back();
        light.location             = Vec3<T>{ static_cast<T>(0), static_cast<T>(5), static_cast<T>(-5) };
        light.direction            = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(-1) };
        light.ambient              = Vec3<T>{ static_cast<T>(1.0), static_cast<T>(1.0), static_cast<T>(1.0) };
        light.diffuse              = Vec3<T>{ static_cast<T>(100.0), static_cast<T>(100.0), static_cast<T>(100.0) };
        light.specular             = Vec3<T>{ static_cast<T>(100.0), static_cast<T>(100.0), static_cast<T>(100.0) };
        light.intensity            = static_cast<T>(100);
        light.attenuationConstant  = static_cast<T>(0.1);
        light.attenuationLinear    = static_cast<T>(0.01);
        light.attenuationQuadratic = static_cast<T>(0.001);
        light.type                 = ELightType_Point;
        light.angleInnerCone       = static_cast<T>(20);
        light.angleOuterCone       = static_cast<T>(45);

        scene.cameras.push_back({});
        Camera<T>& camera = scene.cameras.back();
        camera.resolution = u32vec2{ WIDTH, HEIGHT };
        camera.fov        = static_cast<T>(90);
        camera.zNearPlane = static_cast<T>(0.1);
        camera.zFarPlane  = static_cast<T>(10.0);
        camera.location   = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(1) };
        camera.target     = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(0) };
        camera.up         = Vec3<T>{ static_cast<T>(0), static_cast<T>(1), static_cast<T>(0) };
        camera.updateMatrices();

        XPSWMaterial<T>& material        = scene.materials[0];
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

    XPSWRasterizerEventListener rasterizerEventListener;
    rasterizerEventListener.onFrameRenderBoundingSquare = [&](size_t xmin, size_t xmax, size_t ymin, size_t ymax) {};
    rasterizerEventListener.onFrameSetColorBufferPtr    = [&](float* data, size_t w, size_t h) {};

    renderer.render(&scene, rasterizerEventListener);

    LOG_DEBUG("=================================================================================");
}

template<typename T>
void
testPlane()
{
    LOG_DEBUG("=================================================================================");
    LOG_DEBUG("START TEST PLANE");
    LOG_DEBUG("=================================================================================");

    XPSWScene<T> scene = {};
    scene.filepath     = "plane";

    XPSWRenderer renderer;

    constexpr float s = static_cast<T>(0.5);

    // CLOCKWISE WINDING ORDER
    scene.meshes.resize(2);
    scene.meshes[0].indices = { 0, 1, 2 };
    scene.meshes[0].vertices.resize(3);
    scene.meshes[0].vertices[0] = Vec4<T>{ -s, -s, 0, 1 };
    scene.meshes[0].vertices[1] = Vec4<T>{ -s, s, 0, 1 };
    scene.meshes[0].vertices[2] = Vec4<T>{ s, s, 0, 1 };
    scene.meshes[0].normals.resize(3);
    scene.meshes[0].normals[0] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[0].normals[1] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[0].normals[2] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[0].texCoords.resize(3);
    scene.meshes[0].texCoords[0] = Vec2<T>{ 0, 1 };
    scene.meshes[0].texCoords[1] = Vec2<T>{ 0, 0 };
    scene.meshes[0].texCoords[2] = Vec2<T>{ 1, 0 };
    scene.meshes[0].colors.resize(3);
    scene.meshes[0].colors[0]       = Vec3<T>{ 1, 0, 0 };
    scene.meshes[0].colors[1]       = Vec3<T>{ 0, 1, 0 };
    scene.meshes[0].colors[2]       = Vec3<T>{ 0, 0, 1 };
    scene.meshes[0].materialIndex   = 0;
    scene.meshes[0].transform       = Mat4<T>::identity();
    scene.meshes[0].boundingBox.min = Vec3<T>{ -s, -s, 0 };
    scene.meshes[0].boundingBox.max = Vec3<T>{ s, s, 0 };
    {
        Vec3<T> tangent, biTangent;
        XPSWTriangle<T>::generateTangentsAndBiTangents(scene.meshes[0].vertices[0].xyz,
                                                       scene.meshes[0].vertices[1].xyz,
                                                       scene.meshes[0].vertices[2].xyz,
                                                       scene.meshes[0].texCoords[0],
                                                       scene.meshes[0].texCoords[1],
                                                       scene.meshes[0].texCoords[2],
                                                       tangent,
                                                       biTangent);
        scene.meshes[0].tangents.resize(3);
        scene.meshes[0].tangents[0] = tangent;
        scene.meshes[0].tangents[1] = tangent;
        scene.meshes[0].tangents[2] = tangent;
        scene.meshes[0].biTangents.resize(3);
        scene.meshes[0].biTangents[0] = biTangent;
        scene.meshes[0].biTangents[1] = biTangent;
        scene.meshes[0].biTangents[2] = biTangent;
    }

    scene.meshes[1].indices = { 0, 1, 2 };
    scene.meshes[1].vertices.resize(3);
    scene.meshes[1].vertices[0] = Vec4<T>{ s, s, 0, 1 };
    scene.meshes[1].vertices[1] = Vec4<T>{ s, -s, 0, 1 };
    scene.meshes[1].vertices[2] = Vec4<T>{ -s, -s, 0, 1 };
    scene.meshes[1].normals.resize(3);
    scene.meshes[1].normals[0] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[1].normals[1] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[1].normals[2] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[1].texCoords.resize(3);
    scene.meshes[1].texCoords[0] = Vec2<T>{ 1, 0 };
    scene.meshes[1].texCoords[1] = Vec2<T>{ 1, 1 };
    scene.meshes[1].texCoords[2] = Vec2<T>{ 0, 1 };
    scene.meshes[1].colors.resize(3);
    scene.meshes[1].colors[0]       = Vec3<T>{ 0, 0, 1 };
    scene.meshes[1].colors[1]       = Vec3<T>{ 0, 1, 0 };
    scene.meshes[1].colors[2]       = Vec3<T>{ 1, 0, 0 };
    scene.meshes[1].materialIndex   = 0;
    scene.meshes[1].transform       = Mat4<T>::identity();
    scene.meshes[1].boundingBox.min = Vec3<T>{ -s, -s, 0 };
    scene.meshes[1].boundingBox.max = Vec3<T>{ s, s, 0 };
    {
        Vec3<T> tangent, biTangent;
        XPSWTriangle<T>::generateTangentsAndBiTangents(scene.meshes[1].vertices[0].xyz,
                                                       scene.meshes[1].vertices[1].xyz,
                                                       scene.meshes[1].vertices[2].xyz,
                                                       scene.meshes[1].texCoords[0],
                                                       scene.meshes[1].texCoords[1],
                                                       scene.meshes[1].texCoords[2],
                                                       tangent,
                                                       biTangent);
        scene.meshes[1].tangents.resize(3);
        scene.meshes[1].tangents[0] = tangent;
        scene.meshes[1].tangents[1] = tangent;
        scene.meshes[1].tangents[2] = tangent;
        scene.meshes[1].biTangents.resize(3);
        scene.meshes[1].biTangents[0] = biTangent;
        scene.meshes[1].biTangents[1] = biTangent;
        scene.meshes[1].biTangents[2] = biTangent;
    }
    {
        scene.lights.push_back({});
        Light<T>& light            = scene.lights.back();
        light.location             = Vec3<T>{ static_cast<T>(0), static_cast<T>(5), static_cast<T>(-5) };
        light.direction            = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(-1) };
        light.ambient              = Vec3<T>{ static_cast<T>(1.0), static_cast<T>(1.0), static_cast<T>(1.0) };
        light.diffuse              = Vec3<T>{ static_cast<T>(100.0), static_cast<T>(100.0), static_cast<T>(100.0) };
        light.specular             = Vec3<T>{ static_cast<T>(100.0), static_cast<T>(100.0), static_cast<T>(100.0) };
        light.intensity            = static_cast<T>(100);
        light.attenuationConstant  = static_cast<T>(0.1);
        light.attenuationLinear    = static_cast<T>(0.01);
        light.attenuationQuadratic = static_cast<T>(0.001);
        light.type                 = ELightType_Point;
        light.angleInnerCone       = static_cast<T>(20);
        light.angleOuterCone       = static_cast<T>(45);

        scene.cameras.push_back({});
        Camera<T>& camera = scene.cameras.back();
        camera.resolution = u32vec2{ WIDTH, HEIGHT };
        camera.fov        = static_cast<T>(90);
        camera.zNearPlane = static_cast<T>(0.01);
        camera.zFarPlane  = static_cast<T>(10.0);
        camera.location   = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(1) };
        camera.target     = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(0) };
        camera.up         = Vec3<T>{ static_cast<T>(0), static_cast<T>(1), static_cast<T>(0) };
        camera.updateMatrices();

        XPSWMaterial<T>& material          = scene.materials[0];
        material.hasBaseColorTexture       = false;
        material.hasNormalMapTexture       = false;
        material.hasEmissionColorTexture   = false;
        material.hasMetallicTexture        = false;
        material.hasRoughnessTexture       = false;
        material.hasAOTexture              = false;
        material.baseColorTexture.width    = 2;
        material.baseColorTexture.height   = 2;
        material.baseColorTexture.channels = 3;
        material.baseColorTexture.isHDR    = false;
        material.baseColorValue            = Vec3<T>{ static_cast<T>(1), static_cast<T>(1), static_cast<T>(1) };
        material.emissionColorValue        = Vec3<T>{ static_cast<T>(0.1), static_cast<T>(0.1), static_cast<T>(0.1) };
        material.metallicValue             = 0.0f;
        material.roughnessValue            = 0.5f;
        material.aoValue                   = 1.0f;
    }

    XPSWRasterizerEventListener rasterizerEventListener;
    rasterizerEventListener.onFrameRenderBoundingSquare = [&](size_t xmin, size_t xmax, size_t ymin, size_t ymax) {};
    rasterizerEventListener.onFrameSetColorBufferPtr    = [&](float* data, size_t w, size_t h) {};

    renderer.render(&scene, rasterizerEventListener);

    LOG_DEBUG("=================================================================================");
}

template<typename T>
void
testPlaneTextured()
{
    LOG_DEBUG("=================================================================================");
    LOG_DEBUG("START TEST PLANE TEXTURED");
    LOG_DEBUG("=================================================================================");

    XPSWScene<T> scene = {};
    scene.filepath     = "plane textured";

    XPSWRenderer renderer;

    constexpr float s = static_cast<T>(0.5);

    // CLOCKWISE WINDING ORDER
    scene.meshes.resize(2);
    scene.meshes[0].indices = { 0, 1, 2 };
    scene.meshes[0].vertices.resize(3);
    scene.meshes[0].vertices[0] = Vec4<T>{ -s, -s, 0, 1 };
    scene.meshes[0].vertices[1] = Vec4<T>{ -s, s, 0, 1 };
    scene.meshes[0].vertices[2] = Vec4<T>{ s, s, 0, 1 };
    scene.meshes[0].normals.resize(3);
    scene.meshes[0].normals[0] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[0].normals[1] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[0].normals[2] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[0].texCoords.resize(3);
    scene.meshes[0].texCoords[0] = Vec2<T>{ 0, 1 };
    scene.meshes[0].texCoords[1] = Vec2<T>{ 0, 0 };
    scene.meshes[0].texCoords[2] = Vec2<T>{ 1, 0 };
    scene.meshes[0].colors.resize(3);
    scene.meshes[0].colors[0]       = Vec3<T>{ 1, 0, 0 };
    scene.meshes[0].colors[1]       = Vec3<T>{ 0, 1, 0 };
    scene.meshes[0].colors[2]       = Vec3<T>{ 0, 0, 1 };
    scene.meshes[0].materialIndex   = 0;
    scene.meshes[0].transform       = Mat4<T>::identity();
    scene.meshes[0].boundingBox.min = Vec3<T>{ -s, -s, 0 };
    scene.meshes[0].boundingBox.max = Vec3<T>{ s, s, 0 };
    {
        Vec3<T> tangent, biTangent;
        XPSWTriangle<T>::generateTangentsAndBiTangents(scene.meshes[0].vertices[0].xyz,
                                                       scene.meshes[0].vertices[1].xyz,
                                                       scene.meshes[0].vertices[2].xyz,
                                                       scene.meshes[0].texCoords[0],
                                                       scene.meshes[0].texCoords[1],
                                                       scene.meshes[0].texCoords[2],
                                                       tangent,
                                                       biTangent);
        scene.meshes[0].tangents.resize(3);
        scene.meshes[0].tangents[0] = tangent;
        scene.meshes[0].tangents[1] = tangent;
        scene.meshes[0].tangents[2] = tangent;
        scene.meshes[0].biTangents.resize(3);
        scene.meshes[0].biTangents[0] = biTangent;
        scene.meshes[0].biTangents[1] = biTangent;
        scene.meshes[0].biTangents[2] = biTangent;
    }

    scene.meshes[1].indices = { 0, 1, 2 };
    scene.meshes[1].vertices.resize(3);
    scene.meshes[1].vertices[0] = Vec4<T>{ s, s, 0, 1 };
    scene.meshes[1].vertices[1] = Vec4<T>{ s, -s, 0, 1 };
    scene.meshes[1].vertices[2] = Vec4<T>{ -s, -s, 0, 1 };
    scene.meshes[1].normals.resize(3);
    scene.meshes[1].normals[0] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[1].normals[1] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[1].normals[2] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[1].texCoords.resize(3);
    scene.meshes[1].texCoords[0] = Vec2<T>{ 1, 0 };
    scene.meshes[1].texCoords[1] = Vec2<T>{ 1, 1 };
    scene.meshes[1].texCoords[2] = Vec2<T>{ 0, 1 };
    scene.meshes[1].colors.resize(3);
    scene.meshes[1].colors[0]       = Vec3<T>{ 0, 0, 1 };
    scene.meshes[1].colors[1]       = Vec3<T>{ 0, 1, 0 };
    scene.meshes[1].colors[2]       = Vec3<T>{ 1, 0, 0 };
    scene.meshes[1].materialIndex   = 0;
    scene.meshes[1].transform       = Mat4<T>::identity();
    scene.meshes[1].boundingBox.min = Vec3<T>{ -s, -s, 0 };
    scene.meshes[1].boundingBox.max = Vec3<T>{ s, s, 0 };
    {
        Vec3<T> tangent, biTangent;
        XPSWTriangle<T>::generateTangentsAndBiTangents(scene.meshes[1].vertices[0].xyz,
                                                       scene.meshes[1].vertices[1].xyz,
                                                       scene.meshes[1].vertices[2].xyz,
                                                       scene.meshes[1].texCoords[0],
                                                       scene.meshes[1].texCoords[1],
                                                       scene.meshes[1].texCoords[2],
                                                       tangent,
                                                       biTangent);
        scene.meshes[1].tangents.resize(3);
        scene.meshes[1].tangents[0] = tangent;
        scene.meshes[1].tangents[1] = tangent;
        scene.meshes[1].tangents[2] = tangent;
        scene.meshes[1].biTangents.resize(3);
        scene.meshes[1].biTangents[0] = biTangent;
        scene.meshes[1].biTangents[1] = biTangent;
        scene.meshes[1].biTangents[2] = biTangent;
    }
    {
        scene.lights.push_back({});
        Light<T>& light            = scene.lights.back();
        light.location             = Vec3<T>{ static_cast<T>(0), static_cast<T>(5), static_cast<T>(-5) };
        light.direction            = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(-1) };
        light.ambient              = Vec3<T>{ static_cast<T>(1.0), static_cast<T>(1.0), static_cast<T>(1.0) };
        light.diffuse              = Vec3<T>{ static_cast<T>(100.0), static_cast<T>(100.0), static_cast<T>(100.0) };
        light.specular             = Vec3<T>{ static_cast<T>(100.0), static_cast<T>(100.0), static_cast<T>(100.0) };
        light.intensity            = static_cast<T>(100);
        light.attenuationConstant  = static_cast<T>(0.1);
        light.attenuationLinear    = static_cast<T>(0.01);
        light.attenuationQuadratic = static_cast<T>(0.001);
        light.type                 = ELightType_Point;
        light.angleInnerCone       = static_cast<T>(20);
        light.angleOuterCone       = static_cast<T>(45);

        scene.cameras.push_back({});
        Camera<T>& camera = scene.cameras.back();
        camera.resolution = u32vec2{ WIDTH, HEIGHT };
        camera.fov        = static_cast<T>(90);
        camera.zNearPlane = static_cast<T>(0.01);
        camera.zFarPlane  = static_cast<T>(10.0);
        camera.location   = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(1) };
        camera.target     = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(0) };
        camera.up         = Vec3<T>{ static_cast<T>(0), static_cast<T>(1), static_cast<T>(0) };
        camera.updateMatrices();

        XPSWMaterial<T>& material          = scene.materials[0];
        material.hasBaseColorTexture       = true;
        material.hasNormalMapTexture       = false;
        material.hasEmissionColorTexture   = false;
        material.hasMetallicTexture        = false;
        material.hasRoughnessTexture       = false;
        material.hasAOTexture              = false;
        material.baseColorTexture.width    = 2;
        material.baseColorTexture.height   = 2;
        material.baseColorTexture.channels = 4;
        material.baseColorTexture.isHDR    = false;
        material.baseColorTexture.u8data =
          (uint8_t*)malloc(sizeof(uint8_t) * material.baseColorTexture.width * material.baseColorTexture.height *
                           material.baseColorTexture.channels);
        // -- black and white checker texture
        material.baseColorTexture.u8data[0]  = 255;
        material.baseColorTexture.u8data[1]  = 0;
        material.baseColorTexture.u8data[2]  = 0;
        material.baseColorTexture.u8data[3]  = 255;
        material.baseColorTexture.u8data[4]  = 255;
        material.baseColorTexture.u8data[5]  = 0;
        material.baseColorTexture.u8data[6]  = 0;
        material.baseColorTexture.u8data[7]  = 255;
        material.baseColorTexture.u8data[8]  = 255;
        material.baseColorTexture.u8data[9]  = 0;
        material.baseColorTexture.u8data[10] = 0;
        material.baseColorTexture.u8data[11] = 255;
        material.baseColorTexture.u8data[12] = 255;
        material.baseColorTexture.u8data[13] = 0;
        material.baseColorTexture.u8data[14] = 0;
        material.baseColorTexture.u8data[15] = 255;
        // --
        material.baseColorValue     = Vec3<T>{ static_cast<T>(1), static_cast<T>(1), static_cast<T>(1) };
        material.emissionColorValue = Vec3<T>{ static_cast<T>(0.1), static_cast<T>(0.1), static_cast<T>(0.1) };
        material.metallicValue      = 0.0f;
        material.roughnessValue     = 0.5f;
        material.aoValue            = 1.0f;
    }

    XPSWRasterizerEventListener rasterizerEventListener;
    rasterizerEventListener.onFrameRenderBoundingSquare = [&](size_t xmin, size_t xmax, size_t ymin, size_t ymax) {};
    rasterizerEventListener.onFrameSetColorBufferPtr    = [&](float* data, size_t w, size_t h) {};

    renderer.render(&scene, rasterizerEventListener);

    LOG_DEBUG("=================================================================================");
}

template<typename T>
void
testPlaneTexturedInterpolated()
{
    LOG_DEBUG("=================================================================================");
    LOG_DEBUG("START TEST PLANE TEXTURED INTERPOLATED");
    LOG_DEBUG("=================================================================================");

    XPSWScene<T> scene = {};
    scene.filepath     = "plane textured interpolated";

    XPSWRenderer renderer;

    constexpr float s = static_cast<T>(0.5);

    // CLOCKWISE WINDING ORDER
    scene.meshes.resize(2);
    scene.meshes[0].indices = { 0, 1, 2 };
    scene.meshes[0].vertices.resize(3);
    scene.meshes[0].vertices[0] = Vec4<T>{ -s, -s, 0, 1 };
    scene.meshes[0].vertices[1] = Vec4<T>{ -s, s, 0, 1 };
    scene.meshes[0].vertices[2] = Vec4<T>{ s, s, 0, 1 };
    scene.meshes[0].normals.resize(3);
    scene.meshes[0].normals[0] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[0].normals[1] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[0].normals[2] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[0].texCoords.resize(3);
    scene.meshes[0].texCoords[0] = Vec2<T>{ 0, 1 };
    scene.meshes[0].texCoords[1] = Vec2<T>{ 0, 0 };
    scene.meshes[0].texCoords[2] = Vec2<T>{ 1, 0 };
    scene.meshes[0].colors.resize(3);
    scene.meshes[0].colors[0]       = Vec3<T>{ 1, 0, 0 };
    scene.meshes[0].colors[1]       = Vec3<T>{ 0, 1, 0 };
    scene.meshes[0].colors[2]       = Vec3<T>{ 0, 0, 1 };
    scene.meshes[0].materialIndex   = 0;
    scene.meshes[0].transform       = Mat4<T>::identity();
    scene.meshes[0].boundingBox.min = Vec3<T>{ -s, -s, 0 };
    scene.meshes[0].boundingBox.max = Vec3<T>{ s, s, 0 };
    {
        Vec3<T> tangent, biTangent;
        XPSWTriangle<T>::generateTangentsAndBiTangents(scene.meshes[0].vertices[0].xyz,
                                                       scene.meshes[0].vertices[1].xyz,
                                                       scene.meshes[0].vertices[2].xyz,
                                                       scene.meshes[0].texCoords[0],
                                                       scene.meshes[0].texCoords[1],
                                                       scene.meshes[0].texCoords[2],
                                                       tangent,
                                                       biTangent);
        scene.meshes[0].tangents.resize(3);
        scene.meshes[0].tangents[0] = tangent;
        scene.meshes[0].tangents[1] = tangent;
        scene.meshes[0].tangents[2] = tangent;
        scene.meshes[0].biTangents.resize(3);
        scene.meshes[0].biTangents[0] = biTangent;
        scene.meshes[0].biTangents[1] = biTangent;
        scene.meshes[0].biTangents[2] = biTangent;
    }

    scene.meshes[1].indices = { 0, 1, 2 };
    scene.meshes[1].vertices.resize(3);
    scene.meshes[1].vertices[0] = Vec4<T>{ s, s, 0, 1 };
    scene.meshes[1].vertices[1] = Vec4<T>{ s, -s, 0, 1 };
    scene.meshes[1].vertices[2] = Vec4<T>{ -s, -s, 0, 1 };
    scene.meshes[1].normals.resize(3);
    scene.meshes[1].normals[0] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[1].normals[1] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[1].normals[2] = Vec3<T>{ 0, 0, -1 };
    scene.meshes[1].texCoords.resize(3);
    scene.meshes[1].texCoords[0] = Vec2<T>{ 1, 0 };
    scene.meshes[1].texCoords[1] = Vec2<T>{ 1, 1 };
    scene.meshes[1].texCoords[2] = Vec2<T>{ 0, 1 };
    scene.meshes[1].colors.resize(3);
    scene.meshes[1].colors[0]       = Vec3<T>{ 0, 0, 1 };
    scene.meshes[1].colors[1]       = Vec3<T>{ 0, 1, 0 };
    scene.meshes[1].colors[2]       = Vec3<T>{ 1, 0, 0 };
    scene.meshes[1].materialIndex   = 0;
    scene.meshes[1].transform       = Mat4<T>::identity();
    scene.meshes[1].boundingBox.min = Vec3<T>{ -s, -s, 0 };
    scene.meshes[1].boundingBox.max = Vec3<T>{ s, s, 0 };
    {
        Vec3<T> tangent, biTangent;
        XPSWTriangle<T>::generateTangentsAndBiTangents(scene.meshes[1].vertices[0].xyz,
                                                       scene.meshes[1].vertices[1].xyz,
                                                       scene.meshes[1].vertices[2].xyz,
                                                       scene.meshes[1].texCoords[0],
                                                       scene.meshes[1].texCoords[1],
                                                       scene.meshes[1].texCoords[2],
                                                       tangent,
                                                       biTangent);
        scene.meshes[1].tangents.resize(3);
        scene.meshes[1].tangents[0] = tangent;
        scene.meshes[1].tangents[1] = tangent;
        scene.meshes[1].tangents[2] = tangent;
        scene.meshes[1].biTangents.resize(3);
        scene.meshes[1].biTangents[0] = biTangent;
        scene.meshes[1].biTangents[1] = biTangent;
        scene.meshes[1].biTangents[2] = biTangent;
    }
    {
        scene.lights.push_back({});
        Light<T>& light            = scene.lights.back();
        light.location             = Vec3<T>{ static_cast<T>(0), static_cast<T>(5), static_cast<T>(-5) };
        light.direction            = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(-1) };
        light.ambient              = Vec3<T>{ static_cast<T>(1.0), static_cast<T>(1.0), static_cast<T>(1.0) };
        light.diffuse              = Vec3<T>{ static_cast<T>(100.0), static_cast<T>(100.0), static_cast<T>(100.0) };
        light.specular             = Vec3<T>{ static_cast<T>(100.0), static_cast<T>(100.0), static_cast<T>(100.0) };
        light.intensity            = static_cast<T>(100);
        light.attenuationConstant  = static_cast<T>(0.1);
        light.attenuationLinear    = static_cast<T>(0.01);
        light.attenuationQuadratic = static_cast<T>(0.001);
        light.type                 = ELightType_Point;
        light.angleInnerCone       = static_cast<T>(20);
        light.angleOuterCone       = static_cast<T>(45);

        scene.cameras.push_back({});
        Camera<T>& camera = scene.cameras.back();
        camera.resolution = u32vec2{ WIDTH, HEIGHT };
        camera.fov        = static_cast<T>(90);
        camera.zNearPlane = static_cast<T>(0.01);
        camera.zFarPlane  = static_cast<T>(10.0);
        camera.location   = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(1) };
        camera.target     = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(0) };
        camera.up         = Vec3<T>{ static_cast<T>(0), static_cast<T>(1), static_cast<T>(0) };
        camera.updateMatrices();

        XPSWMaterial<T>& material          = scene.materials[0];
        material.hasBaseColorTexture       = true;
        material.hasNormalMapTexture       = false;
        material.hasEmissionColorTexture   = false;
        material.hasMetallicTexture        = false;
        material.hasRoughnessTexture       = false;
        material.hasAOTexture              = false;
        material.baseColorTexture.width    = 2;
        material.baseColorTexture.height   = 2;
        material.baseColorTexture.channels = 4;
        material.baseColorTexture.isHDR    = false;
        material.baseColorTexture.u8data =
          (uint8_t*)malloc(sizeof(uint8_t) * material.baseColorTexture.width * material.baseColorTexture.height *
                           material.baseColorTexture.channels);
        // -- black and white checker texture
        material.baseColorTexture.u8data[0] = 255;
        material.baseColorTexture.u8data[1] = 0;
        material.baseColorTexture.u8data[2] = 0;
        material.baseColorTexture.u8data[3] = 255;

        material.baseColorTexture.u8data[4] = 0;
        material.baseColorTexture.u8data[5] = 0;
        material.baseColorTexture.u8data[6] = 255;
        material.baseColorTexture.u8data[7] = 255;

        material.baseColorTexture.u8data[8]  = 0;
        material.baseColorTexture.u8data[9]  = 255;
        material.baseColorTexture.u8data[10] = 0;
        material.baseColorTexture.u8data[11] = 255;

        material.baseColorTexture.u8data[12] = 0;
        material.baseColorTexture.u8data[13] = 0;
        material.baseColorTexture.u8data[14] = 0;
        material.baseColorTexture.u8data[15] = 255;
        // --
        stbi_write_jpg("./output/testPlaneTexturedInterpolated.jpg",
                       material.baseColorTexture.width,
                       material.baseColorTexture.height,
                       material.baseColorTexture.channels,
                       material.baseColorTexture.u8data,
                       100);
        material.baseColorValue     = Vec3<T>{ static_cast<T>(1), static_cast<T>(1), static_cast<T>(1) };
        material.emissionColorValue = Vec3<T>{ static_cast<T>(0.1), static_cast<T>(0.1), static_cast<T>(0.1) };
        material.metallicValue      = 0.0f;
        material.roughnessValue     = 0.5f;
        material.aoValue            = 1.0f;
    }

    XPSWRasterizerEventListener rasterizerEventListener;
    rasterizerEventListener.onFrameRenderBoundingSquare = [&](size_t xmin, size_t xmax, size_t ymin, size_t ymax) {};
    rasterizerEventListener.onFrameSetColorBufferPtr    = [&](float* data, size_t w, size_t h) {};

    renderer.render(&scene, rasterizerEventListener);

    LOG_DEBUG("=================================================================================");
}

template<typename T>
void
saveEXR()
{
    auto SaveColorToEXR =
      [](const std::vector<float>& colorBuffer, int width, int height, const char* filename) -> bool {
        // Prepare EXR image
        EXRHeader header;
        InitEXRHeader(&header);

        EXRImage image;
        InitEXRImage(&image);

        image.num_channels = 3; // 3 channels for color (R, G, B)
        image.width        = width;
        image.height       = height;

        // Split the color buffer into separate channels
        std::vector<float> rChannel(width * height);
        std::vector<float> gChannel(width * height);
        std::vector<float> bChannel(width * height);

        for (int i = 0; i < width * height; ++i) {
            rChannel[i] = colorBuffer[3 * i + 0]; // R
            gChannel[i] = colorBuffer[3 * i + 1]; // G
            bChannel[i] = colorBuffer[3 * i + 2]; // B
        }

        // Pointers to each channel
        std::vector<float*> image_ptr(3);
        image_ptr[0] = rChannel.data(); // R
        image_ptr[1] = gChannel.data(); // G
        image_ptr[2] = bChannel.data(); // B

        image.images = reinterpret_cast<unsigned char**>(image_ptr.data());

        // Set up EXR header
        header.num_channels = 3;
        header.channels     = new EXRChannelInfo[header.num_channels];
        strncpy(header.channels[0].name, "R", 255); // Red channel
        strncpy(header.channels[1].name, "G", 255); // Green channel
        strncpy(header.channels[2].name, "B", 255); // Blue channel

        header.pixel_types           = new int[header.num_channels];
        header.requested_pixel_types = new int[header.num_channels];
        for (int i = 0; i < header.num_channels; i++) {
            header.pixel_types[i]           = TINYEXR_PIXELTYPE_FLOAT; // Input pixel type (float for HDR)
            header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT; // Output pixel type (float for HDR)
        }

        // Save EXR file
        const char* err = nullptr;
        int         ret = SaveEXRImageToFile(&image, &header, filename, &err);
        if (ret != TINYEXR_SUCCESS) {
            fprintf(stderr, "Error saving color EXR file: %s\n", err);
            FreeEXRErrorMessage(err);
            return false;
        }

        // Cleanup
        delete[] header.channels;
        delete[] header.pixel_types;
        delete[] header.requested_pixel_types;

        return true;
    };

    auto SaveDepthToEXR =
      [](const std::vector<uint32_t>& depthBuffer, int width, int height, const char* filename) -> bool {
        // Convert uint32_t depth buffer to float
        std::vector<float> depthBufferFloat(depthBuffer.size());
        for (size_t i = 0; i < depthBuffer.size(); ++i) {
            depthBufferFloat[i] = static_cast<float>(depthBuffer[i]) / static_cast<float>(UINT32_MAX);
        }

        // Prepare EXR image
        EXRHeader header;
        InitEXRHeader(&header);

        EXRImage image;
        InitEXRImage(&image);

        image.num_channels = 1; // 1 channel for depth (Z)
        image.width        = width;
        image.height       = height;

        // Pointers to the depth channel
        std::vector<float*> image_ptr(1);
        image_ptr[0] = depthBufferFloat.data(); // Z

        image.images = reinterpret_cast<unsigned char**>(image_ptr.data());

        // Set up EXR header
        header.num_channels = 1;
        header.channels     = new EXRChannelInfo[header.num_channels];
        strncpy(header.channels[0].name, "Z", 255); // Depth channel

        header.pixel_types           = new int[header.num_channels];
        header.requested_pixel_types = new int[header.num_channels];
        for (int i = 0; i < header.num_channels; i++) {
            header.pixel_types[i]           = TINYEXR_PIXELTYPE_FLOAT; // Input pixel type (float for HDR)
            header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT; // Output pixel type (float for HDR)
        }

        // Save EXR file
        const char* err = nullptr;
        int         ret = SaveEXRImageToFile(&image, &header, filename, &err);
        if (ret != TINYEXR_SUCCESS) {
            fprintf(stderr, "Error saving depth EXR file: %s\n", err);
            FreeEXRErrorMessage(err);
            return false;
        }

        // Cleanup
        delete[] header.channels;
        delete[] header.pixel_types;
        delete[] header.requested_pixel_types;

        return true;
    };

    // Example color buffer (RGB32F) with HDR values
    const int          width  = 256;
    const int          height = 256;
    std::vector<float> colorBuffer(width * height * 3); // 3 channels (R, G, B)

    // Example depth buffer (uint32_t)
    std::vector<uint32_t> depthBuffer(width * height);

    // Fill the color buffer with some example HDR data
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index                  = y * width + x;
            colorBuffer[3 * index + 0] = static_cast<float>(x) / width * 10.0f;  // R (HDR range)
            colorBuffer[3 * index + 1] = static_cast<float>(y) / height * 10.0f; // G (HDR range)
            colorBuffer[3 * index + 2] = 5.0f;                                   // B (HDR range)
        }
    }

    // Fill the depth buffer with some example data
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) { depthBuffer[y * width + x] = static_cast<uint32_t>((x + y) * 1000); }
    }

    // Save the color buffer to an EXR file
    if (SaveColorToEXR(colorBuffer, width, height, "color_hdr.exr")) {
        printf("Color buffer saved to color_hdr.exr\n");
    } else {
        printf("Failed to save color buffer\n");
    }

    // Save the depth buffer to an EXR file
    if (SaveDepthToEXR(depthBuffer, width, height, "depth_hdr.exr")) {
        printf("Depth buffer saved to depth_hdr.exr\n");
    } else {
        printf("Failed to save depth buffer\n");
    }
}