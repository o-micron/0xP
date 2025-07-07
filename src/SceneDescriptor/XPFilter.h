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

#include <list>
#include <stdint.h>
#include <unordered_set>
#include <vector>

class XPScene;
class XPLayer;
class XPNode;

class XPFilter
{
  public:
    XPFilter(XPAttachmentDescriptor attachmentDescriptor);
    XPFilter(XPAttachmentDescriptor attachmentDescriptor, std::list<XPLayer*> layers);
    ~XPFilter();

    void onAddNode(XPNode* node);
    void onRemoveNode(XPNode* node);
    void onAddNodeAttachments(XPNode* node, XPAttachmentDescriptor attachmentDescriptor);
    void onRemoveNodeAttachments(XPNode* node, XPAttachmentDescriptor attachmentDescriptor);

    const std::unordered_set<XPNode*>& getData() const;
    XPAttachmentDescriptor             getAttachmentDescriptor() const;

  private:
    std::unordered_set<XPNode*>       _data;
    XPBitFlag<XPAttachmentDescriptor> _attachmentDescriptor;
};
