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

#include <Utilities/XPPlatforms.h>

#include <Engine/XPEngine.h>
#include <Engine/XPRegistry.h>
#include <Renderer/Interface/XPIRenderer.h>

#include <functional>
#include <list>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>

class XPEngine;
class XPIUI;

enum class XPConsoleTextSourceType
{
    Input,
    Output
};

struct XPConsoleText
{
    XPConsoleText(std::string text, XPConsoleTextSourceType sourceType)
      : text(text)
      , sourceType(sourceType)
    {
    }

    std::string             text;
    XPConsoleTextSourceType sourceType;
};

class XPIConsoleVar
{
  public:
    XPIConsoleVar(const char* name)
      : _name(std::string("##").append(name))
    {
    }
    virtual void        setValueFromString(std::string str)         = 0;
    virtual std::string getValueAsString() const                    = 0;
    virtual void        renderUI(XPIUI* ui, const char* filterBuff) = 0;
    virtual void        onChange(XPRegistry* const registry)        = 0;
    virtual const char* getName() const { return _name.c_str() + 1; }
    virtual const char* getHashName() const { return _name.c_str(); }

  protected:
    const std::string _name;
};

template<typename T>
class XPConsoleVar final : public XPIConsoleVar
{
  public:
    XPConsoleVar(T val, const char* name, const std::function<void(XPRegistry* const, const T& val)>&& cbfn)
      : XPIConsoleVar(name)
      , _val(val)
      , _uiVal(val)
      , _cbfn(std::move(cbfn))
    {
    }
    void        setValueFromString(std::string str) final;
    std::string getValueAsString() const final;
    T           getValue() const { return _val; }
    void        renderUI(XPIUI* ui, const char* filterBuff) final;
    void        onChange(XPRegistry* const registry) final { _cbfn(registry, _val); }

  private:
    T                                                          _val;
    T                                                          _uiVal;
    const std::function<void(XPRegistry* const, const T& val)> _cbfn;
};

class XPConsole
{
  public:
    XPConsole(XPEngine* const engine);
    ~XPConsole();

    std::optional<std::string> getVariableAsString(std::string name) const
    {
        auto it = _consoleVariables.find(name);
        if (it != _consoleVariables.end()) { return { it->second->getValueAsString() }; }
        return std::nullopt;
    }

    template<typename T>
    T getVariableValue(std::string name) const
    {
        auto it = _consoleVariables.find(name);
        if (it != _consoleVariables.end()) {
            if (auto ptr = std::dynamic_pointer_cast<XPConsoleVar<T>>(it->second)) { return ptr->getValue(); }
        }
        exit(-666666);
    }

    std::optional<std::shared_ptr<XPIConsoleVar>> getVariable(std::string name) const
    {
        auto it = _consoleVariables.find(name);
        if (it != _consoleVariables.end()) { return { it->second }; }
        return std::nullopt;
    }

    void setVariableFromString(std::string name, std::string val) const
    {
        auto it = _consoleVariables.find(name);
        if (it != _consoleVariables.end()) {
            it->second->setValueFromString(val);
            it->second->onChange(_engine->getRegistry());
        }
    }

    void renderVariablesUI(const char* filterBuff);

  private:
    XPEngine* const                                                 _engine;
    std::unordered_map<std::string, std::shared_ptr<XPIConsoleVar>> _consoleVariables;
};
