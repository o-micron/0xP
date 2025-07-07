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

#include <Lang/Interface/XPIBlock.h>
#include <Lang/Interface/XPILink.h>
#include <Lang/Interface/XPIPin.h>
#include <Renderer/WebGPU/WGSL/XPWGSLGraph.h>

XPWGSLGraph::XPWGSLGraph(XPIContext* const context)
  : XPIGraph(context)
  , _context(context)
{
}

XPWGSLGraph::~XPWGSLGraph() {}

void
XPWGSLGraph::addPrototype(std::unique_ptr<XPIBlock> prototype){ XP_UNUSED(prototype) }

XPIBlock* XPWGSLGraph::addBlock(std::string name, XPVec2<float> coordinates)
{
    XP_UNUSED(name)
    XP_UNUSED(coordinates)

    return nullptr;
}

void
XPWGSLGraph::removeBlock(uint32_t blockId)
{
    XP_UNUSED(blockId)
}

bool
XPWGSLGraph::testLink(XPIPin* from, XPIPin* to)
{
    XP_UNUSED(from)
    XP_UNUSED(to)

    return false;
}

XPILink*
XPWGSLGraph::addLink(XPIPin* from, XPIPin* to)
{
    XP_UNUSED(from)
    XP_UNUSED(to)

    return nullptr;
}

void
XPWGSLGraph::removeLink(uint32_t linkId){ XP_UNUSED(linkId) }

XPIContext* XPWGSLGraph::getContext() const
{
    return _context;
}

const std::unordered_map<uint32_t, std::unique_ptr<XPIBlock>>&
XPWGSLGraph::getBlocks() const
{
    return _blocks;
}

const std::unordered_map<uint32_t, std::unique_ptr<XPIBlock>>&
XPWGSLGraph::getPrototypes() const
{
    return _prototypes;
}

const std::unordered_map<uint32_t, std::unique_ptr<XPILink>>&
XPWGSLGraph::getLinks() const
{
    return _links;
}
