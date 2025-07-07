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

#include <SceneDescriptor/XPFilter.h>
#include <SceneDescriptor/XPLayer.h>
#include <SceneDescriptor/XPNode.h>
#include <SceneDescriptor/XPScene.h>

#include <algorithm>

XPFilter::XPFilter(XPAttachmentDescriptor attachmentDescriptor)
{
    _attachmentDescriptor = XPBitFlag(attachmentDescriptor);
}

XPFilter::XPFilter(XPAttachmentDescriptor attachmentDescriptor, std::list<XPLayer*> layers)
{
    _attachmentDescriptor = XPBitFlag(attachmentDescriptor);
    for (XPLayer* layer : layers) {
        for (XPNode* layerNode : layer->getNodes()) {
            std::list<XPNode*> children;
            for (XPNode* childNode : layerNode->getNodes()) { children.push_back(childNode); }
            while (!children.empty()) {
                XPNode* frontNode = children.front();
                if (frontNode) {
                    for (XPNode* childNode : frontNode->getNodes()) { children.push_back(childNode); }
                    // work for each node (child of a node)
                    {
                        XPBitFlag bf = XPBitFlag(frontNode->getAttachmentDescriptor());
                        if (bf.has(_attachmentDescriptor.getBits())) { _data.insert(frontNode); }
                    }
                }
                children.pop_front();
            }
            // work for each node (child of a layer)
            {
                XPBitFlag bf = XPBitFlag(layerNode->getAttachmentDescriptor());
                if (bf.has(_attachmentDescriptor.getBits())) { _data.insert(layerNode); }
            }
        }
    }
}

XPFilter::~XPFilter() { _data.clear(); }

void
XPFilter::onAddNode(XPNode* node)
{
    if (_data.find(node) != _data.end()) return;
    XPBitFlag bf = XPBitFlag(node->getAttachmentDescriptor());
    if (bf.has(_attachmentDescriptor.getBits())) { _data.insert(node); }
}

void
XPFilter::onRemoveNode(XPNode* node)
{
    auto it = _data.find(node);
    if (it != _data.end()) { _data.erase(it); }
}

void
XPFilter::onAddNodeAttachments(XPNode* node, XPAttachmentDescriptor attachmentDescriptor)
{
    XP_UNUSED(attachmentDescriptor)

    if (_data.find(node) != _data.end()) { return; }
    XPBitFlag bf = XPBitFlag(node->getAttachmentDescriptor());
    if (bf.has(_attachmentDescriptor.getBits())) { _data.insert(node); }
}

void
XPFilter::onRemoveNodeAttachments(XPNode* node, XPAttachmentDescriptor attachmentDescriptor)
{
    auto it = _data.find(node);
    if (it == _data.end()) { return; }
    XPBitFlag bf = XPBitFlag(node->getAttachmentDescriptor());
    bf.remove(attachmentDescriptor);
    if (!bf.has(_attachmentDescriptor.getBits())) { _data.erase(it); }
}

const std::unordered_set<XPNode*>&
XPFilter::getData() const
{
    return _data;
}

XPAttachmentDescriptor
XPFilter::getAttachmentDescriptor() const
{
    return _attachmentDescriptor.getBits();
}
