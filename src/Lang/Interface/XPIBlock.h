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

#include <memory>
#include <string>
#include <vector>

class XPIGraph;
class XPIPin;
class XPILang;

class XP_PURE_ABSTRACT XPIBlock
{
  public:
    XPIBlock(XPIGraph* const graph) { XP_UNUSED(graph) }
    virtual ~XPIBlock() {}
    virtual uint32_t                                    getId() const            = 0;
    virtual const std::string&                          getName() const          = 0;
    virtual const std::string&                          getCodegenFormat() const = 0;
    virtual const std::vector<std::shared_ptr<XPIPin>>& getInput() const         = 0;
    virtual const std::vector<std::shared_ptr<XPIPin>>& getOutput() const        = 0;
    virtual XPIGraph*                                   getGraph() const         = 0;
};
