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

#include "Logger.h"
#include "Maths.h"
#include "SceneDescription.h"

#define MAX_CLIPPED_TRIANGLE_VERTICES (3 * 3)
#define WIDTH                         120
#define HEIGHT                        120

class Renderer;

template<typename T>
struct Scene;

void
writeColorBuffer(FramebufferRef& fbref, const Vec4<float>& val, const int x, const int y);

template<typename T>
[[nodiscard]] Vec4<float>
fragmentShader(FramebufferRef&                      fbref,
               FrameMemoryPool&                     fmp,
               const Scene<T>&                      scene,
               const Camera<T>&                     camera,
               const Material<T>&                   material,
               const VertexFragmentVaryings<T>&     vertexFragmentVaryings,
               const VertexFragmentFlatVaryings<T>& vertexFragmentFlatVaryings);

template<typename T>
void
drawTriangle(FramebufferRef&                                 fbref,
             FrameMemoryPool&                                fmp,
             const Scene<T>&                                 scene,
             const Camera<T>&                                camera,
             const std::array<Vec4<T>, 3>&                   projectedVertices,
             unsigned int                                    materialIndex,
             const std::array<VertexFragmentVaryings<T>, 3>& vertexFragmentVaryings,
             const VertexFragmentFlatVaryings<T>&            vertexFragmentFlatVaryings);

template<typename T>
[[nodiscard]] int
clipAgainstPlane3D(FramebufferRef&  fbref,
                   FrameMemoryPool& fmp,
                   const Vertex<T>* input,
                   int              inputCount,
                   Vertex<T>*       output,
                   float            edge,
                   int              axis,
                   int              isMax,
                   int              isZForwardPositive);

template<typename T>
void
clipTriangle3D(FramebufferRef&         fbref,
               FrameMemoryPool&        fmp,
               const Triangle<T>&      triangle,
               std::vector<Vertex<T>>& clippedPolygon,
               int                     isZForwardPositive);

template<typename T>
void
clipTriangleToTriangles3D(FramebufferRef&           fbref,
                          FrameMemoryPool&          fmp,
                          const Triangle<T>&        triangle,
                          std::vector<Triangle<T>>& clippedTriangles,
                          int                       isZForwardPositive);

template<typename T>
void
clipStage(FramebufferRef&                         fbref,
          FrameMemoryPool&                        fmp,
          const Scene<T>&                         scene,
          const Camera<T>&                        camera,
          const Triangle<T>&                      projectedVertices,
          const Mat4<T>&                          modelMatrix,
          const glm::mat<3, 3, T, glm::defaultp>& normalMatrix,
          unsigned int                            materialIndex);

template<typename T>
void
vertexShader(FramebufferRef&                         fbref,
             FrameMemoryPool&                        fmp,
             const Scene<T>&                         scene,
             const Camera<T>&                        camera,
             const Triangle<T>&                      t,
             const Mat4<T>&                          modelMatrix,
             const glm::mat<3, 3, T, glm::defaultp>& normalMatrix,
             unsigned int                            materialIndex);

template<typename T>
void
renderFrame(FramebufferRef& fbref, FrameMemoryPool& fmp, const Scene<T>& scene, const Camera<T>& camera);

[[nodiscard]] const float
getDepthBufferPixel(const FramebufferRef& fbref, const int x, const int y);

void
writeDepthBuffer(FramebufferRef& fbref, const float& val, const int x, const int y);

template<typename T>
void
zDrawTriangle(FramebufferRef&               fbref,
              FrameMemoryPool&              fmp,
              const Scene<T>&               scene,
              const Camera<T>&              camera,
              const std::array<Vec4<T>, 3>& projectedVertices);

template<typename T>
[[nodiscard]] int
zClipAgainstPlane3D(FramebufferRef&  fbref,
                    FrameMemoryPool& fmp,
                    const Vertex<T>* input,
                    int              inputCount,
                    Vertex<T>*       output,
                    float            edge,
                    int              axis,
                    int              isMax,
                    int              isZForwardPositive);

template<typename T>
void
zClipTriangle3D(FramebufferRef&         fbref,
                FrameMemoryPool&        fmp,
                const Triangle<T>&      triangle,
                std::vector<Vertex<T>>& clippedPolygon,
                int                     isZForwardPositive);

template<typename T>
void
zClipTriangleToTriangles3D(FramebufferRef&           fbref,
                           FrameMemoryPool&          fmp,
                           const Triangle<T>&        triangle,
                           std::vector<Triangle<T>>& clippedTriangles,
                           int                       isZForwardPositive);

template<typename T>
void
zClipStage(FramebufferRef&    fbref,
           FrameMemoryPool&   fmp,
           const Scene<T>&    scene,
           const Camera<T>&   camera,
           const Triangle<T>& projectedVertices);

template<typename T>
void
zVertexShader(FramebufferRef&    fbref,
              FrameMemoryPool&   fmp,
              const Scene<T>&    scene,
              const Camera<T>&   camera,
              const Triangle<T>& t,
              const Mat4<T>&     modelMatrix);

template<typename T>
void
renderZPrePass(FramebufferRef& fbref, FrameMemoryPool& fmp, const Scene<T>& scene, const Camera<T>& camera);

template<typename T>
class Rasterizer
{
  public:
    Rasterizer(Renderer* renderer);
    ~Rasterizer();
    void setScene(Scene<T>* scene);
    void render(FramebufferRef& fbref, FrameMemoryPool& fmp);

  private:
    Renderer* renderer;
    Scene<T>* scene;
};

template<typename T>
Rasterizer<T>::Rasterizer(Renderer* renderer)
  : renderer(renderer)
  , scene(nullptr)
{
}

template<typename T>
Rasterizer<T>::~Rasterizer()
{
}

template<typename T>
void
Rasterizer<T>::setScene(Scene<T>* scene)
{
    this->scene = scene;
}

template<typename T>
void
Rasterizer<T>::render(FramebufferRef& fbref, FrameMemoryPool& fmp)
{
    if (scene == nullptr) { return; }

    for (size_t ci = 0; ci < scene->cameras.size(); ++ci) {
        // uncomment to clear all and remove the checkerboard background
        // for (size_t i = 0; i < (fbref.width * fbref.height * 3); ++i) { fbref.colorAttachmentPtr[i] = 0.0f; }
        for (size_t i = 0; i < (fbref.width * fbref.height); ++i) { fbref.depthAttachmentPtr[i] = FLT_MAX; }

        fmp.popAllMemory();
        fmp.memsetZeros();

        Camera<T>& camera = scene->cameras[ci];

        renderZPrePass(fbref, fmp, *scene, camera);
        fmp.popAllMemory();

        renderFrame(fbref, fmp, *scene, camera);
        fmp.popAllMemory();
    }
}

// Main Pass ----------------------------------------------------------------------------------------------------------
void
writeColorBuffer(FramebufferRef& fbref, const Vec4<float>& val, const int x, const int y)
{
    const size_t idx                      = y * fbref.width + x;
    fbref.colorAttachmentPtr[3 * idx + 0] = val.x;
    fbref.colorAttachmentPtr[3 * idx + 1] = val.y;
    fbref.colorAttachmentPtr[3 * idx + 2] = val.z;
}

template<typename T>
[[nodiscard]] Vec4<float>
fragmentShader(FramebufferRef&                      fbref,
               FrameMemoryPool&                     fmp,
               const Scene<T>&                      scene,
               const Camera<T>&                     camera,
               const Material<T>&                   material,
               const VertexFragmentVaryings<T>&     vertexFragmentVaryings,
               const VertexFragmentFlatVaryings<T>& vertexFragmentFlatVaryings)
{
    // -------------------------------------------------------------------------------------------------------------
    // VERTEX/FRAGMENT SHADER INTERPOLATED VALUES
    // -------------------------------------------------------------------------------------------------------------
    const std::vector<ELightType>& LightType                 = vertexFragmentFlatVaryings.lightType;
    const std::vector<float>&      LightAttenuationConstant  = vertexFragmentFlatVaryings.lightAttenuationConstant;
    const std::vector<float>&      LightAttenuationLinear    = vertexFragmentFlatVaryings.lightAttenuationLinear;
    const std::vector<float>&      LightAttenuationQuadratic = vertexFragmentFlatVaryings.lightAttenuationQuadratic;
    const std::vector<float>&      LightAngleOuterCone       = vertexFragmentFlatVaryings.lightAngleOuterCone;
    const std::vector<Vec3<T>>&    LightPos                  = vertexFragmentFlatVaryings.lightPos;
    const std::vector<Vec3<T>>&    LightDirection            = vertexFragmentFlatVaryings.lightDirection;
    const std::vector<Vec3<T>>&    LightColor                = vertexFragmentFlatVaryings.lightColor;
    const std::vector<float>&      LightIntensity            = vertexFragmentFlatVaryings.lightIntensity;
    const Vec3<T>&                 ViewPos                   = vertexFragmentFlatVaryings.viewPos;
    const Vec4<T>&                 FragPos                   = vertexFragmentVaryings.fragPos;
    const Vec3<T>&                 FragNormal                = vertexFragmentVaryings.fragNormal;
    const Vec2<T>&                 FragTexCoord              = vertexFragmentVaryings.fragTextureCoord;
    Vec3<T>                        FragColor                 = Vec3<T>{ FragPos.x, FragPos.y, FragPos.z };
    // -------------------------------------------------------------------------------------------------------------

    Vec3<T> BaseColor;
    Vec3<T> N;
    Vec3<T> EmissionColor;
    float   Metallic;
    float   Roughness;
    float   AO;
    getPBRMaterial(
      material, FragTexCoord, FragColor, FragNormal, BaseColor, N, EmissionColor, Metallic, Roughness, AO, true);
    // -------------------------------------------------------------------------------------------------------------

    Vec3<T> V = ViewPos - FragPos.xyz;
    V.normalize();

    Vec3<T> Lo = { 0.0f, 0.0f, 0.0f };
    for (size_t li = 0; li < LightColor.size(); ++li) {
        if (scene.lights[li].type == ELightType_Spot) {
            Lo = Lo + computeSpotLightContribution(
                        V, FragPos.xyz, N, scene.lights[li], BaseColor, EmissionColor, Metallic, Roughness, AO);
        } else if (scene.lights[li].type == ELightType_Point) {
            Lo = Lo + computePointLightContribution(
                        V, FragPos.xyz, N, scene.lights[li], BaseColor, EmissionColor, Metallic, Roughness, AO);
        } else if (scene.lights[li].type == ELightType_Directional) {
            Lo = Lo + computeDirectionalLightContribution(
                        V, FragPos.xyz, N, scene.lights[li], BaseColor, EmissionColor, Metallic, Roughness, AO);
        } else {
            ASSERT_ERROR(false, "Unreachable");
        }
    }

    Vec3<T> color      = Lo;
    color              = gammaCorrect(reinhardToneMapping(color), 2.2);
    Vec4<T> finalcolor = Vec4<T>{ color.x, color.y, color.z, 1.0f };
    return finalcolor;
}

template<typename T>
void
drawTriangle(FramebufferRef&                                 fbref,
             FrameMemoryPool&                                fmp,
             const Scene<T>&                                 scene,
             const Camera<T>&                                camera,
             const std::array<Vec4<T>, 3>&                   projectedVertices,
             unsigned int                                    materialIndex,
             const std::array<VertexFragmentVaryings<T>, 3>& vertexFragmentVaryings,
             const VertexFragmentFlatVaryings<T>&            vertexFragmentFlatVaryings)
{

    T area = Triangle<T>::area(projectedVertices[0].xy, projectedVertices[1].xy, projectedVertices[2].xy);
    if (area == 0) { return; }

    BoundingSquare<int64_t> bs = calculateTriangleBoundingSquare(
      Vec4<T>{ 0, 0, static_cast<T>(camera.resolution.x), static_cast<T>(camera.resolution.y) },
      projectedVertices[0].xy,
      projectedVertices[1].xy,
      projectedVertices[2].xy);

    for (int64_t y = bs.min.y; y <= bs.max.y; ++y) {
        for (int64_t x = bs.min.x; x <= bs.max.x; ++x) {
            Vec3<T> barycentricCoordinates = computeBarycentricCoordinates(
              projectedVertices[0].xy, projectedVertices[1].xy, projectedVertices[2].xy, x, y);

            if (barycentricCoordinates.x >= 0 && barycentricCoordinates.y >= 0 && barycentricCoordinates.z >= 0) {
                T d = calculateDepthZeroToOne(barycentricCoordinates,
                                              projectedVertices[0],
                                              projectedVertices[1],
                                              projectedVertices[2],
                                              camera.zNearPlane,
                                              camera.zFarPlane);
                ASSERT_ERROR(d >= 0.0 && d <= 1.0, "Depth should be between 0.0 and 1.0");
                if (d <= getDepthBufferPixel(fbref, x, y)) {
                    VertexFragmentVaryings<T>* fragmentVaryings =
                      (VertexFragmentVaryings<T>*)fmp.pushMemory(sizeof(VertexFragmentVaryings<T>));
                    interpolateVertex(
                      barycentricCoordinates, projectedVertices, vertexFragmentVaryings, *fragmentVaryings);

                    Vec4<float> fragColor = fragmentShader(fbref,
                                                           fmp,
                                                           scene,
                                                           camera,
                                                           scene.materials.at(materialIndex),
                                                           *fragmentVaryings,
                                                           vertexFragmentFlatVaryings);

                    writeColorBuffer(fbref, fragColor, x, y);

                    fmp.popMemory(sizeof(VertexFragmentVaryings<T>));
                }
            }
        }
    }
}

template<typename T>
[[nodiscard]] int
clipAgainstPlane3D(FramebufferRef&  fbref,
                   FrameMemoryPool& fmp,
                   const Vertex<T>* input,
                   int              inputCount,
                   Vertex<T>*       output,
                   float            edge,
                   int              axis,
                   int              isMax,
                   int              isZForwardPositive)
{
    int   outputCount = 0;
    auto& prevVertex  = *(Vertex<T>*)fmp.pushMemory(sizeof(Vertex<T>));
    prevVertex        = input[inputCount - 1];
    float prevVal = (axis == 0) ? prevVertex.location.x : (axis == 1) ? prevVertex.location.y : prevVertex.location.z;
    int   prevInside;

    if (axis == 2 && !isZForwardPositive) {
        prevInside = isMax ? (prevVal <= edge * prevVertex.location.w) : (prevVal >= edge * prevVertex.location.w);
    } else {
        prevInside = isMax ? (prevVal <= edge * prevVertex.location.w) : (prevVal >= edge * prevVertex.location.w);
    }

    for (int i = 0; i < inputCount; i++) {
        auto& currentVertex = *(Vertex<T>*)fmp.pushMemory(sizeof(Vertex<T>));
        currentVertex       = input[i];
        float currVal       = (axis == 0)   ? currentVertex.location.x
                              : (axis == 1) ? currentVertex.location.y
                                            : currentVertex.location.z;
        int   currInside;

        if (axis == 2 && !isZForwardPositive) {
            currInside =
              isMax ? (currVal <= edge * currentVertex.location.w) : (currVal >= edge * currentVertex.location.w);
        } else {
            currInside =
              isMax ? (currVal <= edge * currentVertex.location.w) : (currVal >= edge * currentVertex.location.w);
        }

        if (currInside != prevInside) {
            float t = (edge * prevVertex.location.w - prevVal) /
                      ((edge * prevVertex.location.w - prevVal) - (edge * currentVertex.location.w - currVal));
            Vertex<T>& intersect = output[outputCount];
            intersect.location.x = prevVertex.location.x + t * (currentVertex.location.x - prevVertex.location.x);
            intersect.location.y = prevVertex.location.y + t * (currentVertex.location.y - prevVertex.location.y);
            intersect.location.z = prevVertex.location.z + t * (currentVertex.location.z - prevVertex.location.z);
            intersect.location.w = prevVertex.location.w + t * (currentVertex.location.w - prevVertex.location.w);
            intersect.normal.x   = prevVertex.normal.x + t * (currentVertex.normal.x - prevVertex.normal.x);
            intersect.normal.y   = prevVertex.normal.y + t * (currentVertex.normal.y - prevVertex.normal.y);
            intersect.normal.z   = prevVertex.normal.z + t * (currentVertex.normal.z - prevVertex.normal.z);
            intersect.coord.x    = prevVertex.coord.x + t * (currentVertex.coord.x - prevVertex.coord.x);
            intersect.coord.y    = prevVertex.coord.y + t * (currentVertex.coord.y - prevVertex.coord.y);
            outputCount++;
        }

        if (currInside) {
            output[outputCount] = currentVertex;
            outputCount++;
        }

        prevVertex = currentVertex;
        prevVal    = currVal;
        prevInside = currInside;

        fmp.popMemory(sizeof(Vertex<T>));
    }
    fmp.popMemory(sizeof(Vertex<T>));
    return outputCount;
}

template<typename T>
void
clipTriangle3D(FramebufferRef&         fbref,
               FrameMemoryPool&        fmp,
               const Triangle<T>&      triangle,
               std::vector<Vertex<T>>& clippedPolygon,
               int                     isZForwardPositive)
{
    auto* input  = (Vertex<T>*)fmp.pushMemory(sizeof(Vertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
    auto* output = (Vertex<T>*)fmp.pushMemory(sizeof(Vertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);

    int inputCount = 3;

    input[0] = triangle.v0;
    input[1] = triangle.v1;
    input[2] = triangle.v2;

    const float xPlanes[] = { -1.0f, 1.0f };
    const float yPlanes[] = { -1.0f, 1.0f };
    const float zPlanes[] = { isZForwardPositive ? 0.0f : -1.0f, 1.0f };

    const float planes[] = { xPlanes[0], xPlanes[1], yPlanes[0], yPlanes[1], zPlanes[0], zPlanes[1] };
    const int   axes[]   = { 0, 0, 1, 1, 2, 2 }; // x, x, y, y, z, z

    for (int i = 0; i < 6; i++) {
        int outputCount =
          clipAgainstPlane3D(fbref, fmp, input, inputCount, output, planes[i], axes[i], i % 2, isZForwardPositive);
        if (outputCount == 0) {
            clippedPolygon.clear();
            fmp.popMemory(sizeof(Vertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
            fmp.popMemory(sizeof(Vertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
            return;
        }
        memcpy((void*)input, (void*)output, outputCount * sizeof(Vertex<T>));
        inputCount = outputCount;
    }

    if (inputCount > clippedPolygon.size()) { clippedPolygon.resize(inputCount - clippedPolygon.size()); }
    for (int i = 0; i < inputCount; i++) { clippedPolygon[i] = input[i]; }

    fmp.popMemory(sizeof(Vertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
    fmp.popMemory(sizeof(Vertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
}

template<typename T>
void
clipTriangleToTriangles3D(FramebufferRef&           fbref,
                          FrameMemoryPool&          fmp,
                          const Triangle<T>&        triangle,
                          std::vector<Triangle<T>>& clippedTriangles,
                          int                       isZForwardPositive)
{
    std::vector<Vertex<T>> clippedPolygon;
    clipTriangle3D(fbref, fmp, triangle, clippedPolygon, isZForwardPositive);

    if (clippedPolygon.size() < 3) {
        clippedTriangles.clear();
        return;
    }
    if (clippedPolygon.size() > 3) {
        clippedTriangles.resize(clippedPolygon.size() - 2);
        int numTriangles = 0;
        for (int i = 1; i < clippedPolygon.size() - 1; ++i) {
            clippedTriangles[numTriangles].v0 = clippedPolygon[0];
            clippedTriangles[numTriangles].v1 = clippedPolygon[i];
            clippedTriangles[numTriangles].v2 = clippedPolygon[i + 1];
            ++numTriangles;
        }
        return;
    }

    clippedTriangles.resize(1);
    clippedTriangles[0].v0 = clippedPolygon[0];
    clippedTriangles[0].v1 = clippedPolygon[1];
    clippedTriangles[0].v2 = clippedPolygon[2];
}

template<typename T>
void
clipStage(FramebufferRef&                         fbref,
          FrameMemoryPool&                        fmp,
          const Scene<T>&                         scene,
          const Camera<T>&                        camera,
          const Triangle<T>&                      projectedVertices,
          const Mat4<T>&                          modelMatrix,
          const glm::mat<3, 3, T, glm::defaultp>& normalMatrix,
          unsigned int                            materialIndex)
{
    std::vector<Triangle<T>> clippedTriangles;
    clipTriangleToTriangles3D(fbref, fmp, projectedVertices, clippedTriangles, 1);
    if (clippedTriangles.empty()) { return; }

    auto& worldTriangle = *(Triangle<T>*)fmp.pushMemory(sizeof(Triangle<T>));

    for (size_t i = 0; i < clippedTriangles.size(); ++i) {
        VertexFragmentFlatVaryings<T>            vertexFragmentFlatVaryings = {};
        std::array<VertexFragmentVaryings<T>, 3> vertexFragmentVaryings     = {};

        Triangle<T>& clippedTriangle = clippedTriangles[i];

        worldTriangle = clippedTriangle;
        worldTriangle.v0.location.glm =
          camera.inverseViewMatrix.glm * camera.inverseProjectionMatrix.glm * worldTriangle.v0.location.glm;
        worldTriangle.v1.location.glm =
          camera.inverseViewMatrix.glm * camera.inverseProjectionMatrix.glm * worldTriangle.v1.location.glm;
        worldTriangle.v2.location.glm =
          camera.inverseViewMatrix.glm * camera.inverseProjectionMatrix.glm * worldTriangle.v2.location.glm;

        clippedTriangle.v0.location.x /= clippedTriangle.v0.location.w;
        clippedTriangle.v0.location.y /= clippedTriangle.v0.location.w;
        clippedTriangle.v0.location.z /= clippedTriangle.v0.location.w;
        clippedTriangle.v1.location.x /= clippedTriangle.v1.location.w;
        clippedTriangle.v1.location.y /= clippedTriangle.v1.location.w;
        clippedTriangle.v1.location.z /= clippedTriangle.v1.location.w;
        clippedTriangle.v2.location.x /= clippedTriangle.v2.location.w;
        clippedTriangle.v2.location.y /= clippedTriangle.v2.location.w;
        clippedTriangle.v2.location.z /= clippedTriangle.v2.location.w;

        clippedTriangle.v0.location.x = (clippedTriangle.v0.location.x + 1.0f) * 0.5f * WIDTH;
        clippedTriangle.v0.location.y = (1.0f - clippedTriangle.v0.location.y) * 0.5f * HEIGHT;
        clippedTriangle.v0.location.z =
          (clippedTriangle.v0.location.z + 1.0f) * 0.5f * (camera.zFarPlane - camera.zNearPlane) + camera.zNearPlane;
        clippedTriangle.v1.location.x = (clippedTriangle.v1.location.x + 1.0f) * 0.5f * WIDTH;
        clippedTriangle.v1.location.y = (1.0f - clippedTriangle.v1.location.y) * 0.5f * HEIGHT;
        clippedTriangle.v1.location.z =
          (clippedTriangle.v1.location.z + 1.0f) * 0.5f * (camera.zFarPlane - camera.zNearPlane) + camera.zNearPlane;
        clippedTriangle.v2.location.x = (clippedTriangle.v2.location.x + 1.0f) * 0.5f * WIDTH;
        clippedTriangle.v2.location.y = (1.0f - clippedTriangle.v2.location.y) * 0.5f * HEIGHT;
        clippedTriangle.v2.location.z =
          (clippedTriangle.v2.location.z + 1.0f) * 0.5f * (camera.zFarPlane - camera.zNearPlane) + camera.zNearPlane;

        // if (!camera.isBackFace(
        //       clippedTriangle.v0.location, clippedTriangle.v1.location, clippedTriangle.v2.location)) {

        // interpolate varyings ------------------------------------------------------------------------
        vertexFragmentFlatVaryings.lightType.resize(scene.lights.size());
        vertexFragmentFlatVaryings.lightAttenuationConstant.resize(scene.lights.size());
        vertexFragmentFlatVaryings.lightAttenuationLinear.resize(scene.lights.size());
        vertexFragmentFlatVaryings.lightAttenuationQuadratic.resize(scene.lights.size());
        vertexFragmentFlatVaryings.lightAngleOuterCone.resize(scene.lights.size());
        vertexFragmentFlatVaryings.lightPos.resize(scene.lights.size());
        vertexFragmentFlatVaryings.lightDirection.resize(scene.lights.size());
        vertexFragmentFlatVaryings.lightColor.resize(scene.lights.size());
        vertexFragmentFlatVaryings.lightIntensity.resize(scene.lights.size());
        vertexFragmentFlatVaryings.viewPos =
          Vec3<T>{ camera.inverseViewMatrix.row0.w, camera.inverseViewMatrix.row1.w, camera.inverseViewMatrix.row2.w };

        vertexFragmentVaryings[0].fragPos          = worldTriangle.v0.location;
        vertexFragmentVaryings[0].fragNormal       = worldTriangle.v0.normal;
        vertexFragmentVaryings[0].fragTextureCoord = worldTriangle.v0.coord;
        vertexFragmentVaryings[1].fragPos          = worldTriangle.v1.location;
        vertexFragmentVaryings[1].fragNormal       = worldTriangle.v1.normal;
        vertexFragmentVaryings[1].fragTextureCoord = worldTriangle.v1.coord;
        vertexFragmentVaryings[2].fragPos          = worldTriangle.v2.location;
        vertexFragmentVaryings[2].fragNormal       = worldTriangle.v2.normal;
        vertexFragmentVaryings[2].fragTextureCoord = worldTriangle.v2.coord;
        for (size_t li = 0; li < scene.lights.size(); ++li) {
            vertexFragmentFlatVaryings.lightType[li]                 = scene.lights[li].type;
            vertexFragmentFlatVaryings.lightAttenuationConstant[li]  = scene.lights[li].attenuationConstant;
            vertexFragmentFlatVaryings.lightAttenuationLinear[li]    = scene.lights[li].attenuationLinear;
            vertexFragmentFlatVaryings.lightAttenuationQuadratic[li] = scene.lights[li].attenuationQuadratic;
            vertexFragmentFlatVaryings.lightAngleOuterCone[li]       = scene.lights[li].angleOuterCone;
            vertexFragmentFlatVaryings.lightPos[li]                  = scene.lights[li].location;
            vertexFragmentFlatVaryings.lightDirection[li]            = scene.lights[li].direction;
            vertexFragmentFlatVaryings.lightColor[li]                = scene.lights[li].diffuse;
            vertexFragmentFlatVaryings.lightIntensity[li]            = scene.lights[li].intensity;
        }
        // ---------------------------------------------------------------------------------------------
        std::array<Vec4<T>, 3> projectedVerticesToRasterize = { clippedTriangle.v0.location,
                                                                clippedTriangle.v1.location,
                                                                clippedTriangle.v2.location };
        drawTriangle(fbref,
                     fmp,
                     scene,
                     camera,
                     projectedVerticesToRasterize,
                     materialIndex,
                     vertexFragmentVaryings,
                     vertexFragmentFlatVaryings);
        // }
    }
    // worldTriangle
    fmp.popMemory(sizeof(Triangle<T>));
}

template<typename T>
void
vertexShader(FramebufferRef&                         fbref,
             FrameMemoryPool&                        fmp,
             const Scene<T>&                         scene,
             const Camera<T>&                        camera,
             const Triangle<T>&                      t,
             const Mat4<T>&                          modelMatrix,
             const glm::mat<3, 3, T, glm::defaultp>& normalMatrix,
             unsigned int                            materialIndex)
{
    auto& projectedVertices = *(Triangle<T>*)fmp.pushMemory(sizeof(Triangle<T>));
    projectedVertices       = t;

    projectedVertices.v0.normal = normalMatrix * t.v0.normal.glm;
    projectedVertices.v1.normal = normalMatrix * t.v1.normal.glm;
    projectedVertices.v2.normal = normalMatrix * t.v2.normal.glm;

    projectedVertices.v0.location = camera.viewProjectionMatrix * modelMatrix * t.v0.location;
    projectedVertices.v1.location = camera.viewProjectionMatrix * modelMatrix * t.v1.location;
    projectedVertices.v2.location = camera.viewProjectionMatrix * modelMatrix * t.v2.location;

    clipStage(fbref, fmp, scene, camera, projectedVertices, modelMatrix, normalMatrix, materialIndex);

    fmp.popMemory(sizeof(Triangle<T>));
}

template<typename T>
void
renderFrame(FramebufferRef& fbref, FrameMemoryPool& fmp, const Scene<T>& scene, const Camera<T>& camera)
{
    std::array<Plane<T>, 6>          frustumPlanes = Plane<T>::extractFrustumPlanes(camera.viewProjectionMatrix);
    glm::mat<3, 3, T, glm::defaultp> normalMatrix;
    for (int64_t mi = 0; mi < scene.meshes.size(); ++mi) {
        const Mesh<T>& mesh = scene.meshes[mi];
        if (mesh.boundingBox.testFrustum(frustumPlanes) == EBoundingBoxFrustumTest_FullyOutside) {
            LOGV_DEBUG("[FRUSTUM CULLING ELIMINATED] {}", mesh.name);
            continue;
        }
        normalMatrix = glm::transpose(glm::inverse(glm::mat3(mesh.transform.glm)));
        for (size_t ii = 0; ii < mesh.indices.size() - 2; ii += 3) {
            uint32_t i0 = mesh.indices[ii];
            uint32_t i1 = mesh.indices[ii + 1];
            uint32_t i2 = mesh.indices[ii + 2];

            Triangle<T> tr = {};
            tr.v0.location = mesh.vertices[i0];
            tr.v0.normal   = mesh.normals[i0];
            tr.v0.coord    = mesh.texCoords[i0];

            tr.v1.location = mesh.vertices[i1];
            tr.v1.normal   = mesh.normals[i1];
            tr.v1.coord    = mesh.texCoords[i1];

            tr.v2.location = mesh.vertices[i2];
            tr.v2.normal   = mesh.normals[i2];
            tr.v2.coord    = mesh.texCoords[i2];
            vertexShader(fbref, fmp, scene, camera, tr, mesh.transform, normalMatrix, mesh.materialIndex);
        }
    }
}

[[nodiscard]] const float
getDepthBufferPixel(const FramebufferRef& fbref, const int x, const int y)
{
    return fbref.depthAttachmentPtr[y * fbref.width + x];
}

void
writeDepthBuffer(FramebufferRef& fbref, const float& val, const int x, const int y)
{
    const size_t idx = y * fbref.width + x;
    {
        if (val < fbref.depthAttachmentPtr[idx]) { fbref.depthAttachmentPtr[idx] = val; }
    }
}

template<typename T>
void
zDrawTriangle(FramebufferRef&               fbref,
              FrameMemoryPool&              fmp,
              const Scene<T>&               scene,
              const Camera<T>&              camera,
              const std::array<Vec4<T>, 3>& projectedVertices)
{
    T area = Triangle<T>::area(projectedVertices[0].xy, projectedVertices[1].xy, projectedVertices[2].xy);
    if (area == 0) { return; }

    BoundingSquare<int64_t> bs = calculateTriangleBoundingSquare(
      Vec4<T>{ 0, 0, static_cast<T>(camera.resolution.x), static_cast<T>(camera.resolution.y) },
      projectedVertices[0].xy,
      projectedVertices[1].xy,
      projectedVertices[2].xy);

    for (int64_t y = bs.min.y; y <= bs.max.y; ++y) {
        for (int64_t x = bs.min.x; x <= bs.max.x; ++x) {
            Vec3<T> barycentricCoordinates = computeBarycentricCoordinates(
              projectedVertices[0].xy, projectedVertices[1].xy, projectedVertices[2].xy, x, y);

            if (barycentricCoordinates.x >= 0 && barycentricCoordinates.y >= 0 && barycentricCoordinates.z >= 0) {
                T d = calculateDepthZeroToOne(barycentricCoordinates,
                                              projectedVertices[0],
                                              projectedVertices[1],
                                              projectedVertices[2],
                                              camera.zNearPlane,
                                              camera.zFarPlane);
                ASSERT_ERROR(d >= 0.0 && d <= 1.0, "Depth should be between 0.0 and 1.0");
                writeDepthBuffer(fbref, d, x, y);
            }
        }
    }
}

template<typename T>
[[nodiscard]] int
zClipAgainstPlane3D(FramebufferRef&  fbref,
                    FrameMemoryPool& fmp,
                    const Vertex<T>* input,
                    int              inputCount,
                    Vertex<T>*       output,
                    float            edge,
                    int              axis,
                    int              isMax,
                    int              isZForwardPositive)
{
    int   outputCount = 0;
    auto& prevVertex  = *(Vertex<T>*)fmp.pushMemory(sizeof(Vertex<T>));
    prevVertex        = input[inputCount - 1];
    float prevVal = (axis == 0) ? prevVertex.location.x : (axis == 1) ? prevVertex.location.y : prevVertex.location.z;
    int   prevInside;

    if (axis == 2 && !isZForwardPositive) {
        prevInside = isMax ? (prevVal <= edge * prevVertex.location.w) : (prevVal >= edge * prevVertex.location.w);
    } else {
        prevInside = isMax ? (prevVal <= edge * prevVertex.location.w) : (prevVal >= edge * prevVertex.location.w);
    }

    for (int i = 0; i < inputCount; i++) {
        auto& currentVertex = *(Vertex<T>*)fmp.pushMemory(sizeof(Vertex<T>));
        currentVertex       = input[i];
        float currVal       = (axis == 0)   ? currentVertex.location.x
                              : (axis == 1) ? currentVertex.location.y
                                            : currentVertex.location.z;
        int   currInside;

        if (axis == 2 && !isZForwardPositive) {
            currInside =
              isMax ? (currVal <= edge * currentVertex.location.w) : (currVal >= edge * currentVertex.location.w);
        } else {
            currInside =
              isMax ? (currVal <= edge * currentVertex.location.w) : (currVal >= edge * currentVertex.location.w);
        }

        if (currInside != prevInside) {
            float t = (edge * prevVertex.location.w - prevVal) /
                      ((edge * prevVertex.location.w - prevVal) - (edge * currentVertex.location.w - currVal));
            Vertex<T>& intersect = output[outputCount];
            intersect.location.x = prevVertex.location.x + t * (currentVertex.location.x - prevVertex.location.x);
            intersect.location.y = prevVertex.location.y + t * (currentVertex.location.y - prevVertex.location.y);
            intersect.location.z = prevVertex.location.z + t * (currentVertex.location.z - prevVertex.location.z);
            intersect.location.w = prevVertex.location.w + t * (currentVertex.location.w - prevVertex.location.w);
            outputCount++;
        }

        if (currInside) {
            output[outputCount] = currentVertex;
            outputCount++;
        }

        prevVertex = currentVertex;
        prevVal    = currVal;
        prevInside = currInside;

        fmp.popMemory(sizeof(Vertex<T>));
    }
    fmp.popMemory(sizeof(Vertex<T>));
    return outputCount;
}

template<typename T>
void
zClipTriangle3D(FramebufferRef&         fbref,
                FrameMemoryPool&        fmp,
                const Triangle<T>&      triangle,
                std::vector<Vertex<T>>& clippedPolygon,
                int                     isZForwardPositive)
{
    auto* input  = (Vertex<T>*)fmp.pushMemory(sizeof(Vertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
    auto* output = (Vertex<T>*)fmp.pushMemory(sizeof(Vertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);

    int inputCount = 3;

    input[0] = triangle.v0;
    input[1] = triangle.v1;
    input[2] = triangle.v2;

    const float xPlanes[] = { -1.0f, 1.0f };
    const float yPlanes[] = { -1.0f, 1.0f };
    const float zPlanes[] = { isZForwardPositive ? 0.0f : -1.0f, 1.0f };

    const float planes[] = { xPlanes[0], xPlanes[1], yPlanes[0], yPlanes[1], zPlanes[0], zPlanes[1] };
    const int   axes[]   = { 0, 0, 1, 1, 2, 2 }; // x, x, y, y, z, z

    for (int i = 0; i < 6; ++i) {
        int outputCount =
          zClipAgainstPlane3D(fbref, fmp, input, inputCount, output, planes[i], axes[i], i % 2, isZForwardPositive);
        if (outputCount == 0) {
            clippedPolygon.clear();
            fmp.popMemory(sizeof(Vertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
            fmp.popMemory(sizeof(Vertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
            return;
        }
        memcpy(input, output, outputCount * sizeof(Vertex<T>));
        inputCount = outputCount;
    }

    if (inputCount > clippedPolygon.size()) { clippedPolygon.resize(inputCount - clippedPolygon.size()); }
    for (int i = 0; i < inputCount; ++i) { clippedPolygon[i] = input[i]; }

    fmp.popMemory(sizeof(Vertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
    fmp.popMemory(sizeof(Vertex<T>) * MAX_CLIPPED_TRIANGLE_VERTICES);
}

template<typename T>
void
zClipTriangleToTriangles3D(FramebufferRef&           fbref,
                           FrameMemoryPool&          fmp,
                           const Triangle<T>&        triangle,
                           std::vector<Triangle<T>>& clippedTriangles,
                           int                       isZForwardPositive)
{
    std::vector<Vertex<T>> clippedPolygon;
    zClipTriangle3D(fbref, fmp, triangle, clippedPolygon, isZForwardPositive);

    if (clippedPolygon.size() < 3) {
        clippedTriangles.clear();
        return;
    }
    if (clippedPolygon.size() > 3) {
        clippedTriangles.resize(clippedPolygon.size() - 2);
        int numTriangles = 0;
        for (int i = 1; i < clippedPolygon.size() - 1; ++i) {
            clippedTriangles[numTriangles].v0 = clippedPolygon[0];
            clippedTriangles[numTriangles].v1 = clippedPolygon[i];
            clippedTriangles[numTriangles].v2 = clippedPolygon[i + 1];
            ++numTriangles;
        }
        return;
    }

    clippedTriangles.resize(1);
    clippedTriangles[0].v0 = clippedPolygon[0];
    clippedTriangles[0].v1 = clippedPolygon[1];
    clippedTriangles[0].v2 = clippedPolygon[2];
}

template<typename T>
void
zClipStage(FramebufferRef&    fbref,
           FrameMemoryPool&   fmp,
           const Scene<T>&    scene,
           const Camera<T>&   camera,
           const Triangle<T>& projectedVertices)
{
    std::vector<Triangle<T>> clippedTriangles;
    zClipTriangleToTriangles3D(fbref, fmp, projectedVertices, clippedTriangles, 1);
    if (clippedTriangles.empty()) { return; }

    auto& worldTriangle = *(Triangle<T>*)fmp.pushMemory(sizeof(Triangle<T>));

    for (size_t i = 0; i < clippedTriangles.size(); ++i) {
        Triangle<T>& clippedTriangle = clippedTriangles[i];

        worldTriangle = clippedTriangle;
        worldTriangle.v0.location.glm =
          camera.inverseViewMatrix.glm * camera.inverseProjectionMatrix.glm * worldTriangle.v0.location.glm;
        worldTriangle.v1.location.glm =
          camera.inverseViewMatrix.glm * camera.inverseProjectionMatrix.glm * worldTriangle.v1.location.glm;
        worldTriangle.v2.location.glm =
          camera.inverseViewMatrix.glm * camera.inverseProjectionMatrix.glm * worldTriangle.v2.location.glm;

        clippedTriangle.v0.location.x /= clippedTriangle.v0.location.w;
        clippedTriangle.v0.location.y /= clippedTriangle.v0.location.w;
        clippedTriangle.v0.location.z /= clippedTriangle.v0.location.w;
        clippedTriangle.v1.location.x /= clippedTriangle.v1.location.w;
        clippedTriangle.v1.location.y /= clippedTriangle.v1.location.w;
        clippedTriangle.v1.location.z /= clippedTriangle.v1.location.w;
        clippedTriangle.v2.location.x /= clippedTriangle.v2.location.w;
        clippedTriangle.v2.location.y /= clippedTriangle.v2.location.w;
        clippedTriangle.v2.location.z /= clippedTriangle.v2.location.w;

        clippedTriangle.v0.location.x = (clippedTriangle.v0.location.x + 1.0f) * 0.5f * WIDTH;
        clippedTriangle.v0.location.y = (1.0f - clippedTriangle.v0.location.y) * 0.5f * HEIGHT;
        clippedTriangle.v0.location.z =
          (clippedTriangle.v0.location.z + 1.0f) * 0.5f * (camera.zFarPlane - camera.zNearPlane) + camera.zNearPlane;
        clippedTriangle.v1.location.x = (clippedTriangle.v1.location.x + 1.0f) * 0.5f * WIDTH;
        clippedTriangle.v1.location.y = (1.0f - clippedTriangle.v1.location.y) * 0.5f * HEIGHT;
        clippedTriangle.v1.location.z =
          (clippedTriangle.v1.location.z + 1.0f) * 0.5f * (camera.zFarPlane - camera.zNearPlane) + camera.zNearPlane;
        clippedTriangle.v2.location.x = (clippedTriangle.v2.location.x + 1.0f) * 0.5f * WIDTH;
        clippedTriangle.v2.location.y = (1.0f - clippedTriangle.v2.location.y) * 0.5f * HEIGHT;
        clippedTriangle.v2.location.z =
          (clippedTriangle.v2.location.z + 1.0f) * 0.5f * (camera.zFarPlane - camera.zNearPlane) + camera.zNearPlane;

        // if (!camera.isBackFace(
        //       clippedTriangle.v0.location, clippedTriangle.v1.location, clippedTriangle.v2.location)) {
        std::array<Vec4<T>, 3> projectedVerticesToRasterize = { clippedTriangle.v0.location,
                                                                clippedTriangle.v1.location,
                                                                clippedTriangle.v2.location };
        zDrawTriangle(fbref, fmp, scene, camera, projectedVerticesToRasterize);
        // }
    }
    // worldTriangle
    fmp.popMemory(sizeof(Triangle<T>));
}

template<typename T>
void
zVertexShader(FramebufferRef&    fbref,
              FrameMemoryPool&   fmp,
              const Scene<T>&    scene,
              const Camera<T>&   camera,
              const Triangle<T>& t,
              const Mat4<T>&     modelMatrix)
{
    auto& projectedVertices = *(Triangle<T>*)fmp.pushMemory(sizeof(Triangle<T>));
    projectedVertices       = t;

    projectedVertices.v0.location = camera.viewProjectionMatrix * modelMatrix * t.v0.location;
    projectedVertices.v1.location = camera.viewProjectionMatrix * modelMatrix * t.v1.location;
    projectedVertices.v2.location = camera.viewProjectionMatrix * modelMatrix * t.v2.location;

    zClipStage(fbref, fmp, scene, camera, projectedVertices);

    fmp.popMemory(sizeof(Triangle<T>));
}

template<typename T>
void
renderZPrePass(FramebufferRef& fbref, FrameMemoryPool& fmp, const Scene<T>& scene, const Camera<T>& camera)
{
    std::array<Plane<T>, 6> frustumPlanes = Plane<T>::extractFrustumPlanes(camera.viewProjectionMatrix);
    for (int64_t mi = 0; mi < scene.meshes.size(); ++mi) {
        const Mesh<T>& mesh = scene.meshes.at(mi);
        if (mesh.boundingBox.testFrustum(frustumPlanes) == EBoundingBoxFrustumTest_FullyOutside) { continue; }
        for (size_t ii = 0; ii < mesh.indices.size() - 2; ii += 3) {
            uint32_t i0 = mesh.indices[ii];
            uint32_t i1 = mesh.indices[ii + 1];
            uint32_t i2 = mesh.indices[ii + 2];

            Triangle<T> tr = {};
            tr.v0.location = mesh.vertices[i0];
            tr.v1.location = mesh.vertices[i1];
            tr.v2.location = mesh.vertices[i2];
            zVertexShader(fbref, fmp, scene, camera, tr, mesh.transform);
        }
    }
}
// --------------------------------------------------------------------------------------------------------------------