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

#include <Engine/XPRegistry.h>
#include <Renderer/Interface/XPIRenderer.h>
#include <SceneDescriptor/XPScene.h>
#if defined(XP_EDITOR_MODE)
    #include <UI/Interface/XPIUI.h>
#endif
#include <DataPipeline/XPDataPipelineStore.h>
#include <DataPipeline/XPFile.h>
#include <DataPipeline/XPMeshAsset.h>
#include <DataPipeline/XPMeshBuffer.h>
#include <Engine/XPEngine.h>
#include <SceneDescriptor/XPLayer.h>
#include <SceneDescriptor/XPNode.h>
#include <SceneDescriptor/XPScene.h>
#include <SceneDescriptor/XPSceneDescriptorStore.h>
#include <Shortcuts/XPShortcuts.h>
#include <Utilities/XPFreeCameraSystem.h>
#include <Utilities/XPLogger.h>

XPInput::XPInput(XPRegistry* const registry)
  : _registry(registry)
  , _flags(XPInputFlags_None)
{
}

XPInput::~XPInput() {}

void
XPInput::update(float deltaTime)
{
    for (auto& pair : _keyValues) {
        if (!pair.second.first) {
            if (pair.second.second > 0.0f) {
                pair.second.second -= pair.second.second * 10.0f * deltaTime / 1000.0f;
            } else {
                pair.second.second = 0.0f;
            }
        } else {
            if (pair.second.second < 1.0f) {
                pair.second.second += 0.01f + (1.0f - pair.second.second) * 10.0f * deltaTime / 1000.0f;
            } else {
                pair.second.second = 1.0f;
            }
        }
    }
}

void
XPInput::releaseAllKeyboardKeys()
{
    for (auto& pair : _keyValues) { pair.second.first = false; }
}

void
XPInput::onEvent(SDL_Event* event)
{
    if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.mod == KMOD_LCTRL | event->key.keysym.mod == KMOD_RCTRL) {
            _flags = static_cast<XPInputFlags>(_flags | XPInputFlags_CtrlDown);
        }
        if (event->key.keysym.mod == KMOD_LSHIFT | event->key.keysym.mod == KMOD_RSHIFT) {
            _flags = static_cast<XPInputFlags>(_flags | XPInputFlags_ShiftDown);
        }
        if (event->key.keysym.mod == KMOD_LALT | event->key.keysym.mod == KMOD_RALT) {
            _flags = static_cast<XPInputFlags>(_flags | XPInputFlags_AltDown);
        }
        if (event->key.keysym.mod == KMOD_LGUI | event->key.keysym.mod == KMOD_RGUI) {
            _flags = static_cast<XPInputFlags>(_flags | XPInputFlags_CmdDown);
        }
        if (_keyValues.find(event->key.keysym.sym) != _keyValues.end()) {
            _keyValues[event->key.keysym.sym].first = true;
        }
    }
    if (event->type == SDL_KEYUP) {
        if (event->key.keysym.mod == KMOD_LCTRL | event->key.keysym.mod == KMOD_RCTRL) {
            _flags = static_cast<XPInputFlags>(_flags & ~XPInputFlags_CtrlDown);
        }
        if (event->key.keysym.mod == KMOD_LSHIFT | event->key.keysym.mod == KMOD_RSHIFT) {
            _flags = static_cast<XPInputFlags>(_flags & ~XPInputFlags_ShiftDown);
        }
        if (event->key.keysym.mod == KMOD_LALT | event->key.keysym.mod == KMOD_RALT) {
            _flags = static_cast<XPInputFlags>(_flags & ~XPInputFlags_AltDown);
        }
        if (event->key.keysym.mod == KMOD_LGUI | event->key.keysym.mod == KMOD_RGUI) {
            _flags = static_cast<XPInputFlags>(_flags & ~XPInputFlags_CmdDown);
        }
        if (_keyValues.find(event->key.keysym.sym) != _keyValues.end()) {
            _keyValues[event->key.keysym.sym].first = false;
        }
    }
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        if (event->button.button == SDL_BUTTON_LEFT) {
            _flags = static_cast<XPInputFlags>(_flags | XPInputFlags_LeftMouseDown);
        }
        if (event->button.button == SDL_BUTTON_MIDDLE) {
            _flags = static_cast<XPInputFlags>(_flags | XPInputFlags_MiddleMouseDown);
        }
        if (event->button.button == SDL_BUTTON_RIGHT) {
            _flags = static_cast<XPInputFlags>(_flags | XPInputFlags_RightMouseDown);
        }
    }
    if (event->type == SDL_MOUSEBUTTONUP) {
        if (event->button.button == SDL_BUTTON_LEFT) {
            _flags = static_cast<XPInputFlags>(_flags & ~XPInputFlags_LeftMouseDown);
        }
        if (event->button.button == SDL_BUTTON_MIDDLE) {
            _flags = static_cast<XPInputFlags>(_flags & ~XPInputFlags_MiddleMouseDown);
        }
        if (event->button.button == SDL_BUTTON_RIGHT) {
            _flags = static_cast<XPInputFlags>(_flags & ~XPInputFlags_RightMouseDown);
        }
    }

    bool blocked = ((_flags & XPInputFlags_ShiftDown) == XPInputFlags_ShiftDown);
    for (auto& pair : _keyValues) {
        if (event->key.keysym.sym == pair.first) {
            if (blocked || event->type == SDL_KEYUP) {
                pair.second.first = false;
            } else if (event->type == SDL_KEYDOWN) {
                pair.second.first = true;
            }
        }
    }

    XPScene* scene           = _registry->getScene();
    auto     freeCameraNodes = scene->getNodes(FreeCameraAttachmentDescriptor);
    auto     firstCameraNode = freeCameraNodes.begin();
    if (firstCameraNode != freeCameraNodes.end()) {
        FreeCamera* freeCamera = (*firstCameraNode)->getFreeCamera();
        if (event->type == SDL_MOUSEMOTION) {
            if (isLeftMouseDown() || isRightMouseDown()) {
                if (freeCamera) { CameraSystemRotate(freeCamera, event->motion.xrel, event->motion.yrel); }
            }
        }
    }

    if (event->type == SDL_KEYUP) {
        if (event->key.keysym.sym == SDLK_F8) {
            std::list<XPScene*> scenes = _registry->getScene()->getSceneDescriptorStore()->getScenes();
            for (auto it = scenes.begin(); it != scenes.end(); ++it) {
                if ((*it)->getId() == scene->getId()) {
                    ++it;
                    if (it == scenes.end()) {
                        _registry->setSceneBuffered(*scenes.begin());
                        break;
                    } else {
                        _registry->setSceneBuffered(*it);
                        break;
                    }
                }
            }
        }
    }

    if (event->type == SDL_KEYUP) {
        if (event->key.keysym.sym == SDLK_SPACE) {
            auto&       freeCameras = _registry->getScene()->getNodes(FreeCameraAttachmentDescriptor);
            FreeCamera* camera      = (*freeCameras.begin())->getFreeCamera();
            spawnCube(_registry->getScene(), camera);
        }
    }

    if (event->type == SDL_MOUSEBUTTONUP) {
        if (event->button.button == SDL_BUTTON_LEFT) {
            if (firstCameraNode != freeCameraNodes.end()) {
                FreeCamera*                               freeCamera = (*firstCameraNode)->getFreeCamera();
                static std::function<void(XPNode * node)> cbfn       = [this](XPNode* node) {
                    XPScene* scene = _registry->getScene();
                    scene->clearSelectedNodes();
                    if (node) { scene->setSelectedNode(node); }
                };
                XPIRenderer* renderer = _registry->getRenderer();
                renderer->getSelectedNodeFromViewport(renderer->getNormalizedMouseLocation(), cbfn);
            }
        }
    }
}

void
XPInput::onKeyboard(int key, int scancode, XPInputAction action, int mods)
{
    XP_UNUSED(key)
    XP_UNUSED(scancode)
    XP_UNUSED(action)
    XP_UNUSED(mods)
}

void
XPInput::onMouse(int button, int action, int mods)
{
    XP_UNUSED(button)
    XP_UNUSED(action)
    XP_UNUSED(mods)
}

void
XPInput::subscribeKeyForStreams(int key)
{
    _keyValues.insert({ key, { false, 0.0f } });
}

void
XPInput::unsubscribeKeyForStreams(int key)
{
    _keyValues.erase(key);
}

float
XPInput::fetchKeyNormalizedValue(int key) const
{
    auto it = _keyValues.find(key);
    if (it != _keyValues.end()) { return it->second.second; }
    return 0.0f;
}

bool
XPInput::fetchKeyPressed(int key) const
{
    auto it = _keyValues.find(key);
    if (it != _keyValues.end()) { return static_cast<float>(it->second.first); }
    return false;
}

bool
XPInput::isShiftDown() const
{
    return (_flags & XPInputFlags_ShiftDown) == XPInputFlags_ShiftDown;
}

bool
XPInput::isCtrlDown() const
{
    return (_flags & XPInputFlags_CtrlDown) == XPInputFlags_CtrlDown;
}

bool
XPInput::isAltDown() const
{
    return (_flags & XPInputFlags_AltDown) == XPInputFlags_AltDown;
}

bool
XPInput::isCmdDown() const
{
    return (_flags & XPInputFlags_CmdDown) == XPInputFlags_CmdDown;
}

bool
XPInput::isFnDown() const
{
    return (_flags & XPInputFlags_FnDown) == XPInputFlags_FnDown;
}

bool
XPInput::isLeftMouseDown() const
{
    return (_flags & XPInputFlags_LeftMouseDown) == XPInputFlags_LeftMouseDown;
}

bool
XPInput::isMiddleMouseDown() const
{
    return (_flags & XPInputFlags_MiddleMouseDown) == XPInputFlags_MiddleMouseDown;
}

bool
XPInput::isRightMouseDown() const
{
    return (_flags & XPInputFlags_RightMouseDown) == XPInputFlags_RightMouseDown;
}

XPRegistry*
XPInput::getRegistry() const
{
    return _registry;
}
