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

#include <Utilities/XPMacros.h>
#include <Utilities/XPPlatforms.h>

#include <SceneDescriptor/XPAttachments.h>
#include <Utilities/XPBitFlag.h>
#include <Utilities/XPMemoryPool.h>

#include <list>
#include <optional>
#include <set>
#include <stdint.h>
#include <string>
#include <string_view>

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wall"
    #pragma clang diagnostic ignored "-Weverything"
#endif
#include <nlohmann/json.hpp>
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

class XPScene;
class XPNode;

class XPLayer
{
    XP_MPL_MEMORY_POOL(XPLayer)

  public:
    // returns the name of the layer
    [[nodiscard]] std::string getName() const;

    // returns the id of the layer
    [[nodiscard]] uint32_t getId() const;

    // returns the parent scene
    [[nodiscard]] XPScene* getParent() const;

    // allocate memory and initialize a node as a child and return it if it's not a duplicate
    std::optional<XPNode*> createNode();

    // allocate memory and initialize a node as a child and return it if it's not a duplicate
    std::optional<XPNode*> createNode(std::string name);

    // attempt to get or if failed, allocate memory and initialize a node as a child
    std::optional<XPNode*> getOrCreateNode(std::string name);

    // remove all nodes
    void destroyAllNodes();

    // remove node from children and reclaim memory
    void destroyNode(XPNode* node);

    // search for node by name, if found, remove it from children and reclaim memory
    void destroyNode(std::string name);

    // return a node iterator from the nodes list if found
    [[nodiscard]] std::list<XPNode*>::const_iterator findNode(XPNode* node) const;

    // return a node iterator from the nodes list if found
    [[nodiscard]] std::list<XPNode*>::const_iterator findNode(std::string name) const;

    // optionally returns a child node if matching the name
    [[nodiscard]] std::optional<XPNode*> getNode(std::string name) const;

    // returns all children nodes
    [[nodiscard]] const std::list<XPNode*>& getNodes() const;

    // adds attachment changes flag
    void addAttachmentChanges(uint32_t changesFlags, bool propagateUpwards = true, bool propagateDownwards = false);

    // removes changes flag
    void removeAttachmentChanges(uint32_t changesFlags, bool propagateUpwards = true, bool propagateDownwards = false);

    // returns true if all the given changes exist
    [[nodiscard]] bool hasAllAttachmentChanges(uint32_t changesFlags) const;

    // returns true if any of the given changes exist
    [[nodiscard]] bool hasAnyAttachmentChanges(uint32_t changesFlags) const;

    // returns true if only the given changes exist
    [[nodiscard]] bool hasOnlyAttachmentChanges(uint32_t changesFlags) const;

    // [serialize] fills a json object representing the layer
    static void to_json(nlohmann::json& j, const XPLayer& layer);

    // [deserialize] returns an optional layer from a json object that represents the layer
    static std::optional<XPLayer*> from_json(const nlohmann::json& j);

  private:
    // only accessible through memory pool
    XPLayer(std::string name, uint32_t id, XPScene* parent);

    // only accessible through memory pool
    ~XPLayer();

    // holds the parent scene of the layer
    XPScene* _parent;

    // children nodes
    std::list<XPNode*> _nodes;

    // name of the layer
    std::string _name;

    // represents the id of the layer compared to the attachment system it is attached to
    uint32_t _id;

    // embeds interactive information about the layer
    XPBitFlag<uint32_t> _interaction;
};
