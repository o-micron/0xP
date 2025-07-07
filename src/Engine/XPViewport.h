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

#include <Utilities/XPMaths.h>
#include <Utilities/XPPlatforms.h>

#include <optional>

class XPScene;
struct FreeCamera;

class XPViewport
{
  public:
    XPViewport();
    ~XPViewport();
    void                    setScene(XPScene* scene);
    void                    setExtent(XPVec4<uint32_t> extent);
    std::optional<XPScene*> getScene() const;
    XPVec4<uint32_t>        getExtent() const;
    FreeCamera&             getCamera();

  private:
    FreeCamera*      _localCamera;
    XPScene*         _scene;
    XPVec4<uint32_t> _extent;
};
