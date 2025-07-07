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

#include <functional>
#include <mutex>

template<typename T>
struct XPSWRasterizer;

template<typename T>
struct XPSWScene;

class XPRegistry;

struct XPSWRasterizerEventListener
{
    XPSWRasterizerEventListener()
      : activated(false)
      , colorBufferPreviewPtr(nullptr)
      , colorBufferPreviewWidth(0)
      , colorBufferPreviewHeight(0)
      , onFrameSetColorBufferPtr({})
      , onFrameRenderBoundingSquare({})
    {
    }

    bool                                                                    activated;
    float*                                                                  colorBufferPreviewPtr;
    size_t                                                                  colorBufferPreviewWidth;
    size_t                                                                  colorBufferPreviewHeight;
    std::function<void(float* data, size_t width, size_t height)>           onFrameSetColorBufferPtr;
    std::function<void(size_t xmin, size_t xmax, size_t ymin, size_t ymax)> onFrameRenderBoundingSquare;
};

struct XPSWRenderer
{
    explicit XPSWRenderer(XPRegistry* const registry);
    ~XPSWRenderer();
    void initialize();
    void loadScene(std::string filepath);
    void render(XPSWRasterizerEventListener& listener);
    void finalize();

#if defined(XP_SW_RASTERIZER_ENABLE)
    XPSWRasterizer<float>* rasterizer;
#endif

  private:
    XPRegistry* const _registry;
    XPSWScene<float>* _scene;
    bool              _status;
    std::mutex        _mut;
};