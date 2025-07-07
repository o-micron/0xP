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

#include <Lang/Interface/XPIBlock.h>

#include <memory>
#include <unordered_map>

class XPRegistry;
class XPIContext;
class XPIBlock;

class XP_PURE_ABSTRACT XPILang
{
  public:
    XPILang(XPRegistry* const registry) { XP_UNUSED(registry) }
    virtual ~XPILang() {}
    virtual void              run()                                                              = 0;
    virtual bool              compile()                                                          = 0;
    virtual void              createContext(std::string name)                                    = 0;
    virtual void              removeContext(std::string name)                                    = 0;
    virtual void              switchContext(std::string name)                                    = 0;
    virtual void              addPrototype(std::unique_ptr<XPIBlock> prototype)                  = 0;
    virtual const std::string getName()                                                          = 0;
    virtual std::string       getCompileErrors()                                                 = 0;
    virtual std::string       getCompileWarnings()                                               = 0;
    virtual XPRegistry*       getRegistry()                                                      = 0;
    virtual XPIContext*       getContext()                                                       = 0;
    virtual const std::unordered_map<uint32_t, std::unique_ptr<XPIBlock>>& getPrototypes() const = 0;
};
