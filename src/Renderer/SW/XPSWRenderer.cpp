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

#include <Renderer/SW/XPSWImporter.h>
#include <Renderer/SW/XPSWRasterizer.h>
#include <Renderer/SW/XPSWRenderer.h>
#include <Renderer/SW/XPSWSceneDescriptor.h>

#include <thread>

XPSWRenderer::XPSWRenderer(XPRegistry* const registry)
  : _registry(registry)
  , _scene(nullptr)
{
    rasterizer = new XPSWRasterizer<float>(this);
}

XPSWRenderer::~XPSWRenderer() { delete rasterizer; }

void
XPSWRenderer::initialize()
{
}

void
XPSWRenderer::loadScene(std::string filepath)
{
    std::thread t([this, filepath] {
        std::lock_guard<std::mutex> l(_mut);
        if (_scene) { delete _scene; }
        _scene           = new XPSWScene<float>();
        _scene->filepath = filepath;
        _status          = importAsset(*_scene);
    });
    t.detach();
}

void
XPSWRenderer::render(XPSWRasterizerEventListener& listener)
{
    std::thread t([this, &listener] {
        std::lock_guard<std::mutex> l(_mut);
        if (_scene) {
            if (_status) {
                rasterizer->setScene(_scene);
                rasterizer->render(listener);
            }
        }
    });
    t.detach();
}

void
XPSWRenderer::finalize()
{
    {
        std::lock_guard<std::mutex> l(_mut);
        // in case we are still loading or rendering, wait for all work here
    }
}