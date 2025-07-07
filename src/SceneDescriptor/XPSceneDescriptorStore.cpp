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

#include <SceneDescriptor/XPSceneDescriptorStore.h>

#include <SceneDescriptor/XPLayer.h>
#include <SceneDescriptor/XPNode.h>
#include <SceneDescriptor/XPScene.h>

#include <utility>

XPSceneDescriptorStore::XPSceneDescriptorStore(XPRegistry* const registry)
  : _registry(registry)
{
    _scenePool   = XP_NEW XPMemoryPool<XPScene>(1, 2);
    _nextSceneId = 1;
}

XPSceneDescriptorStore::~XPSceneDescriptorStore()
{
    delete _scenePool;
    _scenes.clear();
}

std::optional<XPScene*>
XPSceneDescriptorStore::createScene(std::string name)
{
    for (auto it = _scenes.begin(); it != _scenes.end(); ++it) {
        if ((*it)->getName() == name) { return std::nullopt; }
    }
    XPScene* scene = _scenePool->create(_registry, name, _nextSceneId++, this);
    _scenes.push_front(scene);
    return scene;
}

std::optional<XPScene*>
XPSceneDescriptorStore::getOrCreateScene(std::string name)
{
    for (auto it = _scenes.begin(); it != _scenes.end(); ++it) {
        if ((*it)->getName() == name) { return { *it }; }
    }
    XPScene* scene = _scenePool->create(_registry, name, _nextSceneId++, this);
    _scenes.push_front(scene);
    return scene;
}

const std::list<XPScene*>&
XPSceneDescriptorStore::getScenes() const
{
    return _scenes;
}

std::optional<XPScene*>
XPSceneDescriptorStore::getScene(std::string name)
{
    for (XPScene* scene : _scenes) {
        if (scene->getName() == name) { return { scene }; }
    }
    return std::nullopt;
}

std::list<XPScene*>::const_iterator
XPSceneDescriptorStore::findScene(XPScene* scene)
{
    return std::find(_scenes.begin(), _scenes.end(), scene);
}

std::list<XPScene*>::const_iterator
XPSceneDescriptorStore::findScene(std::string name)
{
    for (auto it = _scenes.begin(); it != _scenes.end(); ++it) {
        if ((*it)->getName() == name) { return it; }
    }
    return _scenes.end();
}

void
XPSceneDescriptorStore::destroyScene(XPScene* scene)
{
    auto it = findScene(scene);
    if (it != _scenes.end()) {
        XPScene* scene = *it;
        _scenes.erase(it);
        _scenePool->destroy(scene);
    }
}

void
XPSceneDescriptorStore::destroyScene(std::string name)
{
    auto it = findScene(name);
    if (it != _scenes.end()) {
        XPScene* scene = *it;
        _scenes.erase(it);
        _scenePool->destroy(scene);
    }
}