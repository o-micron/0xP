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

#include <Engine/XPConsole.h>
#if defined(XP_EDITOR_MODE)
    #include <UI/Interface/XPIUI.h>
#endif
#include <sstream>

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wall"
    #pragma clang diagnostic ignored "-Weverything"
#endif
#define FMT_HEADER_ONLY
#include <fmt/format.h>
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

XPConsole::XPConsole(XPEngine* const engine)
  : _engine(engine)
  , _consoleVariables(
      { std::make_pair("stats", std::make_shared<XPConsoleVar<bool>>(true, "stats", [](XPRegistry* const, bool) {})),
        std::make_pair("stats_gpu",
                       std::make_shared<XPConsoleVar<bool>>(true, "stats_gpu", [](XPRegistry* const, bool) {})),

        // set the rendering resolution
        std::make_pair(
          "r.res",
          std::make_shared<XPConsoleVar<XPVec2<int>>>(XPVec2<int>(XP_INITIAL_WINDOW_WIDTH, XP_INITIAL_WINDOW_HEIGHT),
                                                      "r.res",
                                                      [](XPRegistry* const, XPVec2<int>) {})),
        // set the rendering scale, usually ranges from 0.5 to 2.0
        // 0.5 means render half resolution
        // 2.0 means render double the resolution
        std::make_pair("r.scale",
                       std::make_shared<XPConsoleVar<float>>(1.0f, "r.scale", [](XPRegistry* const, float) {})),

        // enable/disable frustum culling
        std::make_pair("r.frustumCulling",
                       std::make_shared<XPConsoleVar<bool>>(true, "r.frustumCulling", [](XPRegistry* const, bool) {})),

        // freeze the rendering camera, used to visualize frustum culling
        std::make_pair("r.freeze",
                       std::make_shared<XPConsoleVar<bool>>(false, "r.freeze", [](XPRegistry* const, bool) {})),

        // show/hide bounding boxes
        std::make_pair("r.bb", std::make_shared<XPConsoleVar<bool>>(false, "r.bb", [](XPRegistry* const, bool) {})),

        // capturing debug frames for metal
        std::make_pair(
          "r.captureDebugFrames",
          std::make_shared<XPConsoleVar<bool>>(false, "r.captureDebugFrames", [](XPRegistry* const registry, bool val) {
              if (val) { registry->getRenderer()->captureDebugFrames(); }
          })) })
{
}

XPConsole::~XPConsole() {}

void
XPConsole::renderVariablesUI(const char* filterBuff)
{
#if defined(XP_EDITOR_MODE)
    XPIUI* const ui          = _engine->getRegistry()->getUI();
    auto         headerFlags = ui->renderHeadless(nullptr, "Console variables");
    if ((headerFlags & XPEHeaderFlagsTableOpen) == XPEHeaderFlagsTableOpen) {
        for (auto& varPair : _consoleVariables) {
            if (strstr(varPair.second->getName(), filterBuff) != nullptr) { varPair.second->renderUI(ui, filterBuff); }
        }
    }
    ui->renderFootless(nullptr, "Console variables", headerFlags);
#endif
}

template<>
void
XPConsoleVar<bool>::setValueFromString(std::string str)
{
    if (str == "true" || str == "1") {
        _val = true;
    } else if (str == "false" || str == "0") {
        _val = false;
    }
}

template<>
std::string
XPConsoleVar<bool>::getValueAsString() const
{
    return fmt::format("{}", _val);
}

template<>
void
XPConsoleVar<bool>::renderUI(XPIUI* ui, const char* filterBuff)
{
#if defined(XP_EDITOR_MODE)
    XP_UNUSED(filterBuff)

    if (ui->renderField(nullptr, getHashName(), _uiVal) > 0) {
        _val = _uiVal;
        _cbfn(ui->getRegistry(), _val);
    }
#endif
}

template<>
void
XPConsoleVar<int>::setValueFromString(std::string str)
{
    std::stringstream ss;
    ss << str;
    ss >> _val;
}

template<>
std::string
XPConsoleVar<int>::getValueAsString() const
{
    return fmt::format("{}", _val);
}

template<>
void
XPConsoleVar<int>::renderUI(XPIUI* ui, const char* filterBuff)
{
#if defined(XP_EDITOR_MODE)
    XP_UNUSED(filterBuff)

    if (ui->renderField(nullptr, getHashName(), _uiVal) > 0) {
        _val = _uiVal;
        _cbfn(ui->getRegistry(), _val);
    }
#endif
}

template<>
void
XPConsoleVar<float>::setValueFromString(std::string str)
{
    std::stringstream ss;
    ss << str;
    ss >> _val;
}

template<>
std::string
XPConsoleVar<float>::getValueAsString() const
{
    return fmt::format("{}", _val);
}

template<>
void
XPConsoleVar<float>::renderUI(XPIUI* ui, const char* filterBuff)
{
#if defined(XP_EDITOR_MODE)
    XP_UNUSED(filterBuff)

    if (ui->renderField(nullptr, getHashName(), _uiVal) != 0) {
        _val = _uiVal;
        _cbfn(ui->getRegistry(), _val);
    }
#endif
}

template<>
void
XPConsoleVar<double>::setValueFromString(std::string str)
{
    std::stringstream ss;
    ss << str;
    ss >> _val;
}

template<>
std::string
XPConsoleVar<double>::getValueAsString() const
{
    return fmt::format("{}", _val);
}

template<>
void
XPConsoleVar<double>::renderUI(XPIUI* ui, const char* filterBuff)
{
#if defined(XP_EDITOR_MODE)
    XP_UNUSED(filterBuff)

    if (ui->renderField(nullptr, getHashName(), _uiVal) > 0) {
        _val = _uiVal;
        _cbfn(ui->getRegistry(), _val);
    }
#endif
}

template<>
void
XPConsoleVar<std::string>::setValueFromString(std::string str)
{
    _val = str;
}

template<>
std::string
XPConsoleVar<std::string>::getValueAsString() const
{
    return fmt::format("{}", _val);
}

template<>
void
XPConsoleVar<std::string>::renderUI(XPIUI* ui, const char* filterBuff)
{
#if defined(XP_EDITOR_MODE)
    XP_UNUSED(filterBuff)

    if (ui->renderField(nullptr, getHashName(), _uiVal) > 0) {
        _val = _uiVal;
        _cbfn(ui->getRegistry(), _val);
    }
#endif
}

template<>
void
XPConsoleVar<XPVec2<int>>::setValueFromString(std::string str)
{
    std::stringstream ss;
    ss << str;
    ss >> _val.x;
    ss >> _val.y;
}

template<>
std::string
XPConsoleVar<XPVec2<int>>::getValueAsString() const
{
    return fmt::format("{0} {1}", _val.x, _val.y);
}

template<>
void
XPConsoleVar<XPVec2<int>>::renderUI(XPIUI* ui, const char* filterBuff)
{
#if defined(XP_EDITOR_MODE)
    XP_UNUSED(filterBuff)

    if (ui->renderField(nullptr, getHashName(), _uiVal) > 0) {
        _val = _uiVal;
        _cbfn(ui->getRegistry(), _val);
    }
#endif
}
