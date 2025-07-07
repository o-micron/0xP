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

#include <Engine/XPViewport.h>

#include <SceneDescriptor/Attachments/XPFreeCamera.h>
#include <SceneDescriptor/XPNode.h>
#include <SceneDescriptor/XPScene.h>

XPViewport::XPViewport()
  : _localCamera(XP_NEW FreeCamera(nullptr))
  , _scene(nullptr)
  , _extent(XPVec4<uint32_t>(0, 0, XP_INITIAL_WINDOW_WIDTH, XP_INITIAL_WINDOW_HEIGHT))
{
}

XPViewport::~XPViewport() { delete _localCamera; }

void
XPViewport::setScene(XPScene* scene)
{
    _scene = scene;
}

void
XPViewport::setExtent(XPVec4<uint32_t> extent)
{
    _extent = extent;
}

std::optional<XPScene*>
XPViewport::getScene() const
{
    if (_scene) { return { _scene }; }
    return std::nullopt;
}

XPVec4<uint32_t>
XPViewport::getExtent() const
{
    return _extent;
}

FreeCamera&
XPViewport::getCamera()
{
    if (_scene) {
        auto        freeCameraNodes = _scene->getNodes(FreeCameraAttachmentDescriptor);
        FreeCamera* freeCamera      = (*freeCameraNodes.begin())->getFreeCamera();
        return *freeCamera;
    }
    return *_localCamera;
}
