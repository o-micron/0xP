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

#include <Lang/Interface/XPILang.h>

#include <map>
#include <memory>

class XPIContext;

class XPMSLLang final : public XPILang
{
  public:
    XPMSLLang(XPRegistry* const registry);
    ~XPMSLLang() final;
    void              run() final;
    bool              compile() final;
    void              createContext(std::string name) final;
    void              removeContext(std::string name) final;
    void              switchContext(std::string name) final;
    void              addPrototype(std::unique_ptr<XPIBlock> prototype) final;
    const std::string getName() final;
    std::string       getCompileErrors() final;
    std::string       getCompileWarnings() final;
    XPRegistry*       getRegistry() final;
    XPIContext*       getContext() final;
    const std::unordered_map<uint32_t, std::unique_ptr<XPIBlock>>& getPrototypes() const final;

  private:
    XPRegistry* const                                  _registry;
    XPIContext*                                        _currentContext;
    std::map<std::string, std::unique_ptr<XPIContext>> _contexts;
};
