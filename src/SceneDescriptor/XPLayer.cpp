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

#include <SceneDescriptor/XPLayer.h>
#include <SceneDescriptor/XPNode.h>
#include <SceneDescriptor/XPScene.h>
#include <SceneDescriptor/XPSceneStore.h>

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wall"
    #pragma clang diagnostic ignored "-Weverything"
#endif
#define FMT_HEADER_ONLY
#include <fmt/format.h>
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

#include <utility>

XPLayer::XPLayer(std::string name, uint32_t id, XPScene* parent)
  : _parent(parent)
  , _name(std::move(name))
  , _id(id)
  , _interaction(0)
{
}

XPLayer::~XPLayer()
{
    while (!_nodes.empty()) {
        if (XPNode* node = _nodes.front()) {
            _parent->getSceneStore()->destroyNode(node);
            _nodes.pop_front();
        }
    }
    _nodes.clear();
    _id = 0;
}

std::string
XPLayer::getName() const
{
    return _name;
}

uint32_t
XPLayer::getId() const
{
    return _id;
}

XPScene*
XPLayer::getParent() const
{
    return _parent;
}

std::optional<XPNode*>
XPLayer::createNode()
{
    uint32_t    maxAllowedAttempts = 10;
    uint32_t    availableId        = _parent->getSceneStore()->getNextNodeId();
    std::string name               = fmt::format("node {}", availableId);
    auto        it                 = findNode(name);
    while (maxAllowedAttempts >= 1 && it != _nodes.end()) {
        ++availableId;
        name = fmt::format("node {}", availableId);
        it   = findNode(name);
        --maxAllowedAttempts;
    }

    if (it == _nodes.end()) {
        XPNode* node = _parent->getSceneStore()->createNode(this, name);
        _nodes.push_front(node);
        return { node };
    }

    return std::nullopt;
}

std::optional<XPNode*>
XPLayer::createNode(std::string name)
{
    auto it = findNode(name);
    if (it == _nodes.end()) {
        XPNode* node = _parent->getSceneStore()->createNode(this, name);
        _nodes.push_front(node);
        return { node };
    }
    return std::nullopt;
}

std::optional<XPNode*>
XPLayer::getOrCreateNode(std::string name)
{
    auto it = findNode(name);
    if (it == _nodes.end()) {
        XPNode* node = _parent->getSceneStore()->createNode(this, name);
        _nodes.push_front(node);
        return { node };
    }
    return { *it };
}

void
XPLayer::destroyAllNodes()
{
    for (auto it = _nodes.begin(); it != _nodes.end();) {
        XPNode* node = *it;
        it           = _nodes.erase(it);
        _parent->getSceneStore()->destroyNode(node);
    }
}

void
XPLayer::destroyNode(XPNode* node)
{
    auto it = findNode(node);
    if (it != _nodes.end()) {
        _nodes.erase(it);
        _parent->getSceneStore()->destroyNode(node);
    }
}

void
XPLayer::destroyNode(std::string name)
{
    auto it = findNode(std::move(name));
    if (it != _nodes.end()) {
        XPNode* node = *it;
        _nodes.erase(it);
        _parent->getSceneStore()->destroyNode(node);
    }
}

std::list<XPNode*>::const_iterator
XPLayer::findNode(XPNode* node) const
{
    return std::find(_nodes.begin(), _nodes.end(), node);
}

std::list<XPNode*>::const_iterator
XPLayer::findNode(std::string name) const
{
    for (auto it = _nodes.begin(); it != _nodes.end(); ++it) {
        if ((*it)->getName() == name) { return it; }
    }
    return _nodes.end();
}

std::optional<XPNode*>
XPLayer::getNode(std::string name) const
{
    auto it = findNode(std::move(name));
    if (it != _nodes.end()) { return { *it }; }
    return std::nullopt;
}

const std::list<XPNode*>&
XPLayer::getNodes() const
{
    return _nodes;
}

void
XPLayer::addAttachmentChanges(uint32_t changesFlags, bool propagateUpwards, bool propagateDownwards)
{
    _interaction.add(changesFlags);
    if (propagateUpwards) { _parent->addAttachmentChanges(changesFlags, false); }
    if (propagateDownwards) {
        for (auto it = _nodes.begin(); it != _nodes.end(); ++it) { (*it)->addAttachmentChanges(changesFlags, true); }
    }
}

void
XPLayer::removeAttachmentChanges(uint32_t changesFlags, bool propagateUpwards, bool propagateDownwards)
{
    _interaction.remove(changesFlags);
    if (propagateUpwards) { _parent->removeAttachmentChanges(changesFlags, false); }
    if (propagateDownwards) {
        for (auto it = _nodes.begin(); it != _nodes.end(); ++it) { (*it)->removeAttachmentChanges(changesFlags, true); }
    }
}

bool
XPLayer::hasAllAttachmentChanges(uint32_t changesFlags) const
{
    return _interaction.has(changesFlags);
}

bool
XPLayer::hasAnyAttachmentChanges(uint32_t changesFlags) const
{
    return _interaction.hasAny(changesFlags);
}

bool
XPLayer::hasOnlyAttachmentChanges(uint32_t changesFlags) const
{
    return _interaction.is(changesFlags);
}

void
XPLayer::to_json(nlohmann::json& j, const XPLayer& layer)
{
    XP_UNUSED(j)
    XP_UNUSED(layer)

    XP_UNIMPLEMENTED("[UNIMPLEMENTED]");
}

std::optional<XPLayer*>
XPLayer::from_json(const nlohmann::json& j)
{
    XP_UNUSED(j)

    XP_UNIMPLEMENTED("[UNIMPLEMENTED]");
    return {};
}
