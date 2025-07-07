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

#include <Renderer/Metal/MSL/XPMSLContext.h>
#include <Renderer/Metal/MSL/XPMSLLang.h>

XPMSLLang::XPMSLLang(XPRegistry* const registry)
  : XPILang(registry)
  , _registry(registry)
  , _currentContext(nullptr)
  , _contexts{}
{
}

XPMSLLang::~XPMSLLang() {}

void
XPMSLLang::run()
{
}

bool
XPMSLLang::compile()
{
    return true;
}

void
XPMSLLang::createContext(std::string name)
{
    if (_contexts.find(name) == _contexts.end()) { _contexts[name] = std::make_unique<XPMSLContext>(this); }
}

void
XPMSLLang::removeContext(std::string name)
{
    if (_contexts.find(name) != _contexts.end()) { _contexts.erase(name); }
}

void
XPMSLLang::switchContext(std::string name)
{
    if (_contexts.find(name) != _contexts.end()) { _currentContext = _contexts[name].get(); }
}

void
XPMSLLang::addPrototype(std::unique_ptr<XPIBlock> prototype)
{
    XP_UNUSED(prototype)
}

const std::string
XPMSLLang::getName()
{
    return "MSL";
}

std::string
XPMSLLang::getCompileErrors()
{
    return "";
}

std::string
XPMSLLang::getCompileWarnings()
{
    return "";
}

XPRegistry*
XPMSLLang::getRegistry()
{
    return _registry;
}

XPIContext*
XPMSLLang::getContext()
{
    return _currentContext;
}

const std::unordered_map<uint32_t, std::unique_ptr<XPIBlock>>&
XPMSLLang::getPrototypes() const
{
    static std::unordered_map<uint32_t, std::unique_ptr<XPIBlock>> p;
    return p;
}
