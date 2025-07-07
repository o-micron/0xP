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

#include <Lang/Interface/XPIGraph.h>

#include <memory>
#include <stdint.h>
#include <unordered_map>

class XPIContext;

class XPWGSLGraph final : public XPIGraph
{
  public:
    XPWGSLGraph(XPIContext* const context);
    ~XPWGSLGraph() final;
    void        addPrototype(std::unique_ptr<XPIBlock> prototype) final;
    XPIBlock*   addBlock(std::string name, XPVec2<float> coordinates) final;
    void        removeBlock(uint32_t blockId) final;
    bool        testLink(XPIPin* from, XPIPin* to) final;
    XPILink*    addLink(XPIPin* from, XPIPin* to) final;
    void        removeLink(uint32_t linkId) final;
    XPIContext* getContext() const final;
    const std::unordered_map<uint32_t, std::unique_ptr<XPIBlock>>& getBlocks() const final;
    const std::unordered_map<uint32_t, std::unique_ptr<XPIBlock>>& getPrototypes() const final;
    const std::unordered_map<uint32_t, std::unique_ptr<XPILink>>&  getLinks() const final;

  private:
    XPIContext* const                                       _context;
    std::unordered_map<uint32_t, std::unique_ptr<XPIBlock>> _blocks;
    std::unordered_map<uint32_t, std::unique_ptr<XPIBlock>> _prototypes;
    std::unordered_map<uint32_t, std::unique_ptr<XPILink>>  _links;
};
