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

#include <Utilities/XPMaths.h>

#include <string>
#include <unordered_map>
#include <vector>

class XPIContext;
class XPILink;
class XPIBlock;
class XPIPin;

class XP_PURE_ABSTRACT XPIGraph
{
  public:
    XPIGraph(XPIContext* const context) { XP_UNUSED(context) }
    virtual ~XPIGraph() {}
    virtual void        addPrototype(std::unique_ptr<XPIBlock> prototype)                        = 0;
    virtual XPIBlock*   addBlock(std::string name, XPVec2<float> coordinates)                    = 0;
    virtual void        removeBlock(uint32_t blockId)                                            = 0;
    virtual bool        testLink(XPIPin* from, XPIPin* to)                                       = 0;
    virtual XPILink*    addLink(XPIPin* from, XPIPin* to)                                        = 0;
    virtual void        removeLink(uint32_t linkId)                                              = 0;
    virtual XPIContext* getContext() const                                                       = 0;
    virtual const std::unordered_map<uint32_t, std::unique_ptr<XPIBlock>>& getBlocks() const     = 0;
    virtual const std::unordered_map<uint32_t, std::unique_ptr<XPIBlock>>& getPrototypes() const = 0;
    virtual const std::unordered_map<uint32_t, std::unique_ptr<XPILink>>&  getLinks() const      = 0;
};
