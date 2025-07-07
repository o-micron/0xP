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

#include <SceneDescriptor/XPScene.h>

#include <SceneDescriptor/XPLayer.h>
#include <SceneDescriptor/XPNode.h>
#include <SceneDescriptor/XPSceneStore.h>

#include <utility>

XPScene::XPScene(XPRegistry* const       registry,
                 std::string             name,
                 uint32_t                id,
                 XPSceneDescriptorStore* sceneDescriptorStore)
  : _registry(registry)
  , _name(std::move(name))
  , _id(id)
  , _interaction(XPEInteractionHasRenderingChanges)
  , _sceneDescriptorStore(sceneDescriptorStore)
  , _sceneStore(XP_NEW XPSceneStore(this))
{
}

XPScene::~XPScene()
{
    _filters.clear();
    while (!_layers.empty()) {
        if (XPLayer* layer = _layers.front()) {
            _sceneStore->destroyLayer(layer);
            _layers.pop_front();
        }
    }
    _layers.clear();
    delete _sceneStore;
    _id = 0;
}

XPRegistry*
XPScene::getRegistry() const
{
    return _registry;
}

std::string
XPScene::getName() const
{
    return _name;
}

uint32_t
XPScene::getId() const
{
    return _id;
}

XPSceneDescriptorStore*
XPScene::getSceneDescriptorStore() const
{
    return _sceneDescriptorStore;
}

XPSceneStore*
XPScene::getSceneStore() const
{
    return _sceneStore;
}

std::optional<XPLayer*>
XPScene::createLayer(std::string name)
{
    auto it = findLayer(name);
    if (it == _layers.end()) {
        XPLayer* layer = _sceneStore->createLayer(this, name);
        _layers.push_front(layer);
        return { layer };
    }
    return std::nullopt;
}

std::optional<XPLayer*>
XPScene::getOrCreateLayer(std::string name)
{
    auto it = findLayer(name);
    if (it == _layers.end()) {
        XPLayer* layer = _sceneStore->createLayer(this, name);
        _layers.push_front(layer);
        return { layer };
    }
    return { *it };
}

void
XPScene::destroyAllLayers()
{
    for (auto it = _layers.begin(); it != _layers.end();) {
        XPLayer* layer = *it;
        it             = _layers.erase(it);
        _sceneStore->destroyLayer(layer);
    }
}

void
XPScene::destroyLayer(XPLayer* layer)
{
    auto it = findLayer(layer);
    if (it != _layers.end()) {
        _layers.erase(it);
        _sceneStore->destroyLayer(layer);
    }
}

void
XPScene::destroyLayer(std::string name)
{
    auto it = findLayer(std::move(name));
    if (it != _layers.end()) {
        XPLayer* layer = *it;
        _layers.erase(it);
        _sceneStore->destroyLayer(layer);
    }
}

std::list<XPLayer*>::const_iterator
XPScene::findLayer(std::string name) const
{
    for (auto it = _layers.begin(); it != _layers.end(); ++it) {
        if ((*it)->getName() == name) { return it; }
    }
    return _layers.end();
}

std::list<XPLayer*>::const_iterator
XPScene::findLayer(XPLayer* layer) const
{
    return std::find(_layers.begin(), _layers.end(), layer);
}

std::optional<XPLayer*>
XPScene::getLayer(std::string name) const
{
    auto it = findLayer(std::move(name));
    if (it != _layers.end()) { return { *it }; }
    return std::nullopt;
}

const std::list<XPLayer*>&
XPScene::getLayers() const
{
    return _layers;
}

XPFilter&
XPScene::createNodeFilterPass(XPAttachmentDescriptor attachmentDescriptor)
{
    if (_filters.find(attachmentDescriptor) == _filters.end()) {
        _filters.insert({ attachmentDescriptor, XPFilter(attachmentDescriptor, _layers) });
    }
    return _filters.at(attachmentDescriptor);
}

void
XPScene::destroyNodeFilterPass(XPAttachmentDescriptor attachmentDescriptor)
{
    if (_filters.find(attachmentDescriptor) != _filters.end()) { _filters.erase(attachmentDescriptor); }
}

const std::unordered_set<XPNode*>&
XPScene::getNodes(XPAttachmentDescriptor attachmentDescriptor)
{
    auto it = _filters.find(attachmentDescriptor);
    if (it != _filters.end()) { return it->second.getData(); }
    return createNodeFilterPass(attachmentDescriptor).getData();
}

void
XPScene::setSelectedNode(XPNode* node)
{
    clearSelectedNodes();
    addSelectedNode(node);
}

void
XPScene::addSelectedNode(XPNode* node)
{
    auto it = std::find(_selectedNodes.begin(), _selectedNodes.end(), node);
    if (it == _selectedNodes.end()) {
        node->setSelected(true);
        _selectedNodes.push_front(node);
    }
}

void
XPScene::removeSelectedNode(XPNode* node)
{
    auto it = std::find(_selectedNodes.begin(), _selectedNodes.end(), node);
    if (it != _selectedNodes.end()) {
        node->setSelected(false);
        _selectedNodes.erase(it);
    }
}

void
XPScene::clearSelectedNodes()
{
    for (XPNode* node : _selectedNodes) { node->setSelected(false); }
    _selectedNodes.clear();
}

const std::list<XPNode*>&
XPScene::getSelectedNodes() const
{
    return _selectedNodes;
}

void
XPScene::addAttachmentChanges(uint32_t changesFlags, bool propagateDownwards)
{
    _interaction.add(changesFlags);
    if (propagateDownwards) {
        for (auto it = _layers.begin(); it != _layers.end(); ++it) { (*it)->addAttachmentChanges(changesFlags, true); }
    }
}

void
XPScene::removeAttachmentChanges(uint32_t changesFlags, bool propagateDownwards)
{
    _interaction.remove(changesFlags);
    if (propagateDownwards) {
        for (auto it = _layers.begin(); it != _layers.end(); ++it) {
            (*it)->removeAttachmentChanges(changesFlags, true);
        }
    }
}

bool
XPScene::hasAllAttachmentChanges(uint32_t changesFlags) const
{
    return _interaction.has(changesFlags);
}

bool
XPScene::hasAnyAttachmentChanges(uint32_t changesFlags) const
{
    return _interaction.hasAny(changesFlags);
}

bool
XPScene::hasOnlyAttachmentChanges(uint32_t changesFlags) const
{
    return _interaction.is(changesFlags);
}

void
XPScene::to_json(nlohmann::json& j, const XPScene& scene)
{
    XP_UNUSED(j)
    XP_UNUSED(scene)
    XP_UNIMPLEMENTED("[UNIMPLEMENTED]");
}

std::optional<XPScene*>
XPScene::from_json(const nlohmann::json& j)
{
    XP_UNUSED(j)
    XP_UNIMPLEMENTED("[UNIMPLEMENTED]");
    return std::nullopt;
}

void
XPScene::onLayerCreated(XPLayer* layer)
{
    XP_UNUSED(layer)

    // nothing to do here for now ..
}

void
XPScene::onLayerDestroyed(XPLayer* layer)
{
    // nothing to do here for now ..
    layer->destroyAllNodes();
}

void
XPScene::onNodeCreated(XPNode* node)
{
    for (auto& filter : _filters) { filter.second.onAddNode(node); }
}

void
XPScene::onNodeDestroyed(XPNode* node)
{
    node->destroyAllNodes();
    for (auto& filter : _filters) { filter.second.onRemoveNode(node); }
}

void
XPScene::onNodeAttachmentDescriptorAdded(XPNode* node, XPAttachmentDescriptor descriptor)
{
    for (auto& filter : _filters) { filter.second.onAddNodeAttachments(node, descriptor); }
}

void
XPScene::onNodeAttachmentDescriptorRemoved(XPNode* node, XPAttachmentDescriptor descriptor)
{
    for (auto& filter : _filters) { filter.second.onRemoveNodeAttachments(node, descriptor); }
}
