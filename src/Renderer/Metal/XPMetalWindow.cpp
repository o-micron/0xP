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
#include <DataPipeline/XPDataPipelineStore.h>
#include <DataPipeline/XPFile.h>
#include <DataPipeline/XPMeshAsset.h>
#include <Engine/XPConsole.h>
#include <Engine/XPEngine.h>
#include <Engine/XPRegistry.h>
#include <Renderer/Metal/XPMetal.h>
#include <Renderer/Metal/XPMetalRenderer.h>
#include <Renderer/Metal/XPMetalWindow.h>
#include <SceneDescriptor/XPNode.h>
#include <SceneDescriptor/XPScene.h>
#include <SceneDescriptor/XPSceneDescriptorStore.h>
#if defined(XP_EDITOR_MODE)
    #include <UI/Interface/XPIUI.h>
#endif
#include <Utilities/XPFreeCameraSystem.h>
#include <Utilities/XPLogger.h>

XPMetalWindow::XPMetalWindow(XPMetalRenderer* const renderer)
  : _renderer(renderer)
  , _input(XP_NEW XPInput(renderer->getRegistry()))
{
    _input->subscribeKeyForStreams(SDLK_w);
    _input->subscribeKeyForStreams(SDLK_s);
    _input->subscribeKeyForStreams(SDLK_d);
    _input->subscribeKeyForStreams(SDLK_a);
    _input->subscribeKeyForStreams(SDLK_i);
    _input->subscribeKeyForStreams(SDLK_k);
}

XPMetalWindow::~XPMetalWindow() { XP_DELETE _input; }

void
XPMetalWindow::initialize()
{
    _state = 0;

    auto console = _renderer->getRegistry()->getEngine()->getConsole();
    auto size    = console->getVariableValue<XPVec2<int>>("r.res");

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        XP_LOGV(XPLoggerSeverityFatal, "Failed to initialize window, %s", SDL_GetError());
        return;
    }

    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal");
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, XP_FORCE_VSYNC ? "1" : "0");

    _window = SDL_CreateWindow("XPENGINE [" XP_PLATFORM_NAME "][" XP_CONFIG_NAME "][METAL]",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               size.x,
                               size.y,
                               SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN);

    if (!_window) {
        XP_LOGV(XPLoggerSeverityFatal, "Failed to create a window, %s", SDL_GetError());
        return;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        XP_LOGV(XPLoggerSeverityFatal, "Failed to create renderer, %s", SDL_GetError());
        return;
    }

    auto layer = (CA::MetalLayer*)SDL_RenderGetMetalLayer(renderer);
    if (!layer) {
        XP_LOGV(XPLoggerSeverityFatal, "Failed to get a valid metal layer, %s", SDL_GetError());
        return;
    }
    // layer->setPixelFormat(MTL::PixelFormatBGRA8Unorm_sRGB);

    auto device = layer->device();
    if (!device) {
        XP_LOGV(XPLoggerSeverityFatal, "Failed to get a valid metal device, %s", SDL_GetError());
        return;
    }

    _renderer->setDevice(device);
    _renderer->setLayer(layer);
    _renderer->setDrawable(layer->nextDrawable());
}

void
XPMetalWindow::finalize()
{
    SDL_DestroyRenderer(SDL_GetRenderer(_window));
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

XPProfilable void
XPMetalWindow::pollEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            _state |= XPEWindowStateShouldQuit;
            _renderer->getRegistry()->getEngine()->quit();
        }
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
            // TODO: _state |= XPEWindowStateLostFocus;
            // TODO: shouldSleep = true;
            // SDL_Event e;
            // while (true) {
            //     if (SDL_WaitEvent(&e) == 1) {
            //         if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) { break; }
            //     }
            // }
        }
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
            // TODO: _state &= ~XPEWindowStateLostFocus;
            // TODO: shouldSleep = false;
        }
        if (event.type == SDL_WINDOWEVENT &&
            (event.window.event == SDL_WINDOWEVENT_RESIZED || event.window.event == SDL_WINDOWEVENT_MAXIMIZED ||
             event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
             event.window.event == SDL_WINDOWEVENT_DISPLAY_CHANGED)) {}

        if (event.type == SDL_MOUSEMOTION) {
            XPVec2<int> windowSize;
            SDL_GetWindowSize(_window, &windowSize.x, &windowSize.y);
            int x, y;
            SDL_GetMouseState(&x, &y);
            _mouseLocation.x           = (float)x;
            _mouseLocation.y           = (float)y;
            _normalizedMouseLocation.x = (float)x / (float)windowSize.x;
            _normalizedMouseLocation.y = (float)y / (float)windowSize.y;
        }
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT) { _mousePresses[0] = true; }
            if (event.button.button == SDL_BUTTON_MIDDLE) { _mousePresses[1] = true; }
            if (event.button.button == SDL_BUTTON_RIGHT) { _mousePresses[2] = true; }
        }
        if (event.type == SDL_MOUSEBUTTONUP) {
            if (event.button.button == SDL_BUTTON_LEFT) { _mousePresses[0] = false; }
            if (event.button.button == SDL_BUTTON_MIDDLE) { _mousePresses[1] = false; }
            if (event.button.button == SDL_BUTTON_RIGHT) { _mousePresses[2] = false; }
        }

        _input->onEvent(&event);

        XPRegistry* registry        = _renderer->getRegistry();
        XPScene*    scene           = registry->getScene();
        auto        freeCameraNodes = scene->getNodes(FreeCameraAttachmentDescriptor);
        auto        firstCameraNode = freeCameraNodes.begin();
        if (firstCameraNode != freeCameraNodes.end()) {
            FreeCamera* freeCamera = (*firstCameraNode)->getFreeCamera();
            if (event.type == SDL_MOUSEMOTION) {
                if (_input->isLeftMouseDown() || _input->isRightMouseDown()) {
                    if (freeCamera) { CameraSystemRotate(freeCamera, event.motion.xrel, event.motion.yrel); }
                }
            }
        }

        if (event.type == SDL_KEYUP) {
            if (event.key.keysym.sym == SDLK_F8) {
                XPRegistry*         registry = _renderer->getRegistry();
                XPScene*            scene    = registry->getScene();
                std::list<XPScene*> scenes   = registry->getScene()->getSceneDescriptorStore()->getScenes();
                for (auto it = scenes.begin(); it != scenes.end(); ++it) {
                    if ((*it)->getId() == scene->getId()) {
                        ++it;
                        if (it == scenes.end()) {
                            registry->setSceneBuffered(*scenes.begin());
                            break;
                        } else {
                            registry->setSceneBuffered(*it);
                            break;
                        }
                    }
                }
            } else if (event.key.keysym.sym == SDLK_F6) {
                XPRegistry*          registry   = _renderer->getRegistry();
                XPDataPipelineStore* store      = registry->getDataPipelineStore();
                auto&                meshAssets = store->getMeshAssets();
                for (auto& meshAsset : meshAssets) { XPFile::onFileModified(meshAsset.second->getFile()); }
                store->setFilesNeedReload();
            }
        }
    }
}

XPProfilable void
XPMetalWindow::refresh()
{
    _deltaTimeInMilliseconds = SDL_GetTicks64() - _timeInMilliseconds;
    _deltaTimeInMilliseconds = _deltaTimeInMilliseconds <= 0 ? 1 : _deltaTimeInMilliseconds;
    _timeInMilliseconds      = SDL_GetTicks64();
    int width, height;
    SDL_GetRendererOutputSize(SDL_GetRenderer(_window), &width, &height);
    _renderer->getLayer()->setDrawableSize(CGSizeMake(width, height));
    _renderer->setDrawable(_renderer->getLayer()->nextDrawable());
    if (!_renderer->getDrawable()) { XP_LOG(XPLoggerSeverityFatal, "Failed to get a valid metal drawable\n"); }
    _input->update(_deltaTimeInMilliseconds);
}

XPMetalRenderer*
XPMetalWindow::getRenderer() const
{
    return _renderer;
}

SDL_Window*
XPMetalWindow::getWindow() const
{
    return _window;
}

SDL_Renderer*
XPMetalWindow::getWindowRenderer() const
{
    return SDL_GetRenderer(_window);
}

XPInput&
XPMetalWindow::getInput() const
{
    return *_input;
}

uint32_t
XPMetalWindow::getState() const
{
    return _state;
}

XPVec2<int>
XPMetalWindow::getWindowSize() const
{
    XPVec2<int> s;
    SDL_Metal_GetDrawableSize(_window, &s.x, &s.y);
    return s;
}

XPVec2<float>
XPMetalWindow::getMouseLocation() const
{
    return _mouseLocation;
}

XPVec2<float>
XPMetalWindow::getNormalizedMouseLocation() const
{
    return _normalizedMouseLocation;
}

bool
XPMetalWindow::isLeftMouseButtonPressed() const
{
    return _mousePresses.x;
}

bool
XPMetalWindow::isMiddleMouseButtonPressed() const
{
    return _mousePresses.y;
}

bool
XPMetalWindow::isRightMouseButtonPressed() const
{
    return _mousePresses.z;
}

float
XPMetalWindow::getDeltaTimeSeconds() const
{
    return _deltaTimeInMilliseconds / 1000.0f;
}

uint64_t
XPMetalWindow::getTimeMilliseconds() const
{
    return _timeInMilliseconds;
}

void
XPMetalWindow::simulateCopy(const char* text)
{
    SDL_SetClipboardText(text);
}

std::string
XPMetalWindow::simulatePaste()
{
    return { SDL_GetClipboardText() };
}