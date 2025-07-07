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

#include <Renderer/WebGPU/WGSL/XPWGSLContext.h>
#include <Renderer/WebGPU/WGSL/XPWGSLLang.h>

XPWGSLLang::XPWGSLLang(XPRegistry* const registry)
  : XPILang(registry)
  , _registry(registry)
  , _currentContext(nullptr)
  , _contexts{}
{
}

XPWGSLLang::~XPWGSLLang() {}

void
XPWGSLLang::run()
{
}

bool
XPWGSLLang::compile()
{
    return true;
}

void
XPWGSLLang::createContext(std::string name)
{
    if (_contexts.find(name) == _contexts.end()) { _contexts[name] = std::make_unique<XPWGSLContext>(this); }
}

void
XPWGSLLang::removeContext(std::string name)
{
    if (_contexts.find(name) != _contexts.end()) { _contexts.erase(name); }
}

void
XPWGSLLang::switchContext(std::string name)
{
    if (_contexts.find(name) != _contexts.end()) { _currentContext = _contexts[name].get(); }
}

void
XPWGSLLang::addPrototype(std::unique_ptr<XPIBlock> prototype)
{
    XP_UNUSED(prototype)
}

const std::string
XPWGSLLang::getName()
{
    return "MSL";
}

std::string
XPWGSLLang::getCompileErrors()
{
    return "";
}

std::string
XPWGSLLang::getCompileWarnings()
{
    return "";
}

XPRegistry*
XPWGSLLang::getRegistry()
{
    return _registry;
}

XPIContext*
XPWGSLLang::getContext()
{
    return _currentContext;
}

const std::unordered_map<uint32_t, std::unique_ptr<XPIBlock>>&
XPWGSLLang::getPrototypes() const
{
    static std::unordered_map<uint32_t, std::unique_ptr<XPIBlock>> p;
    return p;
}
