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

#include <Lang/Interface/XPEnums.h>

#include <memory>
#include <set>
#include <string>
#include <vector>

class XPIBlock;
class XPIPinType;
class XPILink;

class XP_PURE_ABSTRACT XPIPin
{
  public:
    XPIPin(XPIBlock* const block) { XP_UNUSED(block) }
    virtual ~XPIPin() {}
    virtual uint32_t                  getId() const        = 0;
    virtual const std::string&        getName() const      = 0;
    virtual XPPinDirection            getDirection() const = 0;
    virtual XPIPinType*               getType() const      = 0;
    virtual XPIBlock*                 getBlock() const     = 0;
    virtual const std::set<XPILink*>& getLinks() const     = 0;
};
