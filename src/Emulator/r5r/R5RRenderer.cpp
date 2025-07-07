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

#include "R5RRenderer.h"
#include "R5RRasterizer.h"
#ifndef __EMSCRIPTEN__
    #include "R5RRaytracer.h"
#endif

R5RRenderer::R5RRenderer()
{
#if defined(R5R_RASTERIZER_ENABLE)
    rasterizer = new R5RRasterizer<float>(this);
#endif
#if defined(R5R_RAYTRACER_ENABLE) || defined(R5R_RASTERIZER_RAYTRACED_SHADOWS)
    raytracer = new R5RRaytracer<float>(this);
#endif
}

R5RRenderer::~R5RRenderer()
{
#if defined(R5R_RASTERIZER_ENABLE)
    delete rasterizer;
#endif
#if defined(R5R_RAYTRACER_ENABLE) || defined(R5R_RASTERIZER_RAYTRACED_SHADOWS)
    delete raytracer;
#endif
}

void
R5RRenderer::render(Scene<float>* scene, RasterizerEventListener& listener)
{
#if defined(R5R_RASTERIZER_ENABLE)
    rasterizer->setScene(scene);
#endif
#if defined(R5R_RAYTRACER_ENABLE) || defined(R5R_RASTERIZER_RAYTRACED_SHADOWS)
    raytracer->setScene(scene);
#endif
#if defined(R5R_RAYTRACER_ENABLE) || defined(R5R_RASTERIZER_RAYTRACED_SHADOWS)
    raytracer->initialize();
#endif
#if defined(R5R_RASTERIZER_ENABLE)
    rasterizer->render(listener);
#endif
#if defined(R5R_RAYTRACER_ENABLE)
    raytracer->render(listener);
#endif
#if defined(R5R_RAYTRACER_ENABLE) || defined(R5R_RASTERIZER_RAYTRACED_SHADOWS)
    raytracer->finalize();
#endif
}