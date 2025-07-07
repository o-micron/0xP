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

#include <Utilities/XPPlatforms.h>

#include <SceneDescriptor/XPLayer.h>
#include <SceneDescriptor/XPNode.h>
#include <SceneDescriptor/XPScene.h>
#include <Utilities/XPMemoryPool.h>

#include <optional>

class XPRegistry;

class XPSceneDescriptorStore
{
  public:
    XPSceneDescriptorStore(XPRegistry* const registry);
    ~XPSceneDescriptorStore();

    // allocates and creates a new scene if there's no scenes with same name
    std::optional<XPScene*> createScene(std::string name);

    // attempt to find a scene with same name, if not, allocates and creates a new scene
    std::optional<XPScene*> getOrCreateScene(std::string name);

    // returns all scenes
    [[nodiscard]] const std::list<XPScene*>& getScenes() const;

    // optionally returns a scene if found a matching one with same name
    [[nodiscard]] std::optional<XPScene*> getScene(std::string name);

    // returns an iterator to the scene list
    [[nodiscard]] std::list<XPScene*>::const_iterator findScene(XPScene* scene);

    // returns an iterator to the scene list
    [[nodiscard]] std::list<XPScene*>::const_iterator findScene(std::string name);

    // destroys and deallocates a scene
    void destroyScene(XPScene* scene);

    // search for scene by name, if found, remove it from children and reclaim memory
    void destroyScene(std::string name);

  private:
    XPRegistry* const      _registry;
    uint32_t               _nextSceneId;
    std::list<XPScene*>    _scenes;
    XPMemoryPool<XPScene>* _scenePool;
};