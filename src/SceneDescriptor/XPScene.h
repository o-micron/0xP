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
#include <SceneDescriptor/XPFilter.h>
#include <Utilities/XPBitFlag.h>
#include <Utilities/XPMemoryPool.h>

#include <list>
#include <map>
#include <set>
#include <stdint.h>
#include <string>
#include <string_view>

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wall"
#endif
#include <nlohmann/json.hpp>
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

class XPRegistry;
class XPLayer;
class XPNode;
class XPSceneDescriptorStore;
class XPSceneStore;

class XPScene
{
    XP_MPL_MEMORY_POOL(XPScene)

  public:
    [[nodiscard]] XPRegistry* getRegistry() const;

    // returns the name of the scene
    [[nodiscard]] std::string getName() const;

    // returns the id of the scene
    [[nodiscard]] uint32_t getId() const;

    // returns the main store
    [[nodiscard]] XPSceneDescriptorStore* getSceneDescriptorStore() const;

    // returns the scene store
    [[nodiscard]] XPSceneStore* getSceneStore() const;

    // allocate memory and initialize a layer as a child and return it if it's not a duplicate
    std::optional<XPLayer*> createLayer(std::string name);

    // attempt to get or if failed, allocate memory and initialize a layer as a child
    std::optional<XPLayer*> getOrCreateLayer(std::string name);

    // remove all layers
    void destroyAllLayers();

    // remove layer from children and reclaim memory
    void destroyLayer(XPLayer* layer);

    // search for layer by name, if found, remove it from children and reclaim memory
    void destroyLayer(std::string name);

    // return a layer iterator from the layers list if found
    [[nodiscard]] std::list<XPLayer*>::const_iterator findLayer(XPLayer* layer) const;

    // return a layer iterator from the layers list if found
    [[nodiscard]] std::list<XPLayer*>::const_iterator findLayer(std::string name) const;

    // optionally returns a layer if found a matching layer with same name
    [[nodiscard]] std::optional<XPLayer*> getLayer(std::string name) const;

    // returns all the children layers
    [[nodiscard]] const std::list<XPLayer*>& getLayers() const;

    // filter the scene nodes using the passed attachment descriptor
    [[nodiscard]] const std::unordered_set<XPNode*>& getNodes(XPAttachmentDescriptor attachmentDescriptor);

    // clears all selected nodes and sets only this node to be selected
    void setSelectedNode(XPNode* node);

    // adds a node to the selected nodes list
    void addSelectedNode(XPNode* node);

    // removes a previously selected node from the selected nodes list
    void removeSelectedNode(XPNode* node);

    // remove all selected nodes from list
    void clearSelectedNodes();

    // returns the selected nodes list
    [[nodiscard]] const std::list<XPNode*>& getSelectedNodes() const;

    // adds attachment changes flag
    void addAttachmentChanges(uint32_t changesFlags, bool propagateDownwards = false);

    // removes changes flag
    void removeAttachmentChanges(uint32_t changesFlags, bool propagateDownwards = false);

    // returns true if all the given changes exist
    [[nodiscard]] bool hasAllAttachmentChanges(uint32_t changesFlags) const;

    // returns true if any of the given changes exist
    [[nodiscard]] bool hasAnyAttachmentChanges(uint32_t changesFlags) const;

    // returns true if only the given changes exist
    [[nodiscard]] bool hasOnlyAttachmentChanges(uint32_t changesFlags) const;

    // [serialize] fills a json object representing the scene
    static void to_json(nlohmann::json& j, const XPScene& scene);

    // [deserialize] returns an optional scene from a json object that represents the scene
    static std::optional<XPScene*> from_json(const nlohmann::json& j);

    // notify that a layer has been created after it has been created
    void onLayerCreated(XPLayer* layer);

    // notify that a layer has been destroyed before it actually get destroyed
    void onLayerDestroyed(XPLayer* layer);

    // notify that a node has been created after it has been created
    void onNodeCreated(XPNode* node);

    // notify that a node is going to be destroyed before it actually get destroyed
    void onNodeDestroyed(XPNode* node);

    // notify that a node has added attachments to the attachment descriptor
    void onNodeAttachmentDescriptorAdded(XPNode* node, XPAttachmentDescriptor descriptor);

    // notify that a node has removed attachments from the attachment descriptor
    void onNodeAttachmentDescriptorRemoved(XPNode* node, XPAttachmentDescriptor descriptor);

    // creates a new scene nodes filter pass using the passed attachment descriptor
    XPFilter& createNodeFilterPass(XPAttachmentDescriptor attachmentDescriptor);

    // destroys a scene nodes filter pass that has the corresponding attachment descriptor
    void destroyNodeFilterPass(XPAttachmentDescriptor attachmentDescriptor);

  private:
    // only accessible through memory pool
    XPScene(XPRegistry* const registry, std::string name, uint32_t id, XPSceneDescriptorStore* sceneDescriptorStore);

    // only accessible through memory pool
    ~XPScene();

    // the engine registry
    XPRegistry* const _registry;

    // name of the scene
    std::string _name;

    // children layers
    std::list<XPLayer*> _layers;

    // holds the selected nodes
    std::list<XPNode*> _selectedNodes;

    // a list that holds all the available filters deployed to the scene
    std::map<XPAttachmentDescriptor, XPFilter> _filters;

    // represents the id of the scene compared to the attachment system it is attached to
    uint32_t _id;

    // embeds interactive information about the layer
    XPBitFlag<uint32_t> _interaction;

    // main store
    XPSceneDescriptorStore* _sceneDescriptorStore;

    // store to hold the scene data
    XPSceneStore* _sceneStore;
};
