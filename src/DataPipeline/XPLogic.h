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

#include <Engine/XPRegistry.h>
#include <Utilities/XPMacros.h>
#include <Utilities/XPPlatforms.h>

#include <functional>
#include <string_view>

class XPNode;

enum XPELogicSourceType
{
    XPELogicSourceTypeUnknown,
    XPELogicSourceTypeDll,
    XPELogicSourceTypeDylib,
    XPELogicSourceTypeSo,
    XPELogicSourceTypeLua
};

XP_FOR_EACH_X(XP_STRINGIFY_ENUM_EXTENDED,
              XP_STRINGIFY_ENUM,
              XPELogicSourceTypeUnknown,
              XPELogicSourceTypeDll,
              XPELogicSourceTypeDylib,
              XPELogicSourceTypeSo,
              XPELogicSourceTypeLua);

enum XPEInputAction
{
    XPEInputActionPress,
    XPEInputActionRelease,
    XPEInputActionRepeat
};

enum XPEMouseInputButton
{
    XPEMouseInputButtonLeft,
    XPEMouseInputButtonMiddle,
    XPEMouseInputButtonRight
};

enum XPESpecialKeysDown : uint32_t
{
    XPESpecialKeysDownNone          = 0U,
    XPESpecialKeysDownKeyboardSuper = 1 << 1U,
    XPESpecialKeysDownKeyboardCtrl  = 1 << 2U,
    XPESpecialKeysDownKeyboardShift = 1 << 3U,
    XPESpecialKeysDownKeyboardAlt   = 1 << 4U,
};

typedef std::function<bool(XPRegistry&)>                                          LogicLoadFn;
typedef std::function<bool(XPRegistry&)>                                          LogicReloadFn;
typedef std::function<bool(XPNode*)>                                              LogicStartFn;
typedef std::function<bool(XPNode*)>                                              LogicUpdateFn;
typedef std::function<bool(XPNode*)>                                              LogicEndFn;
typedef std::function<bool()>                                                     LogicUnloadFn;
typedef std::function<void(XPNode*, XPEInputAction, XPEMouseInputButton)>         LogicOnMouseFn;
typedef std::function<void(XPNode*, double, double)>                              LogicOnMouseMoveFn;
typedef std::function<void(XPNode*, XPEMouseInputButton, double, double)>         LogicOnMouseDragFn;
typedef std::function<void(XPNode*, double, double)>                              LogicOnMouseScrollFn;
typedef std::function<void(XPNode*, XPEInputAction, int32_t, XPESpecialKeysDown)> LogicOnKeyboardFn;
typedef std::function<void(XPNode*, int32_t, int32_t)>                            LogicOnWindowResizeFn;
typedef std::function<void(XPNode*, int32_t, const char**)>                       LogicOnWindowDragDropFn;
typedef std::function<void(XPNode*)>                                              LogicOnWindowIconifyFn;
typedef std::function<void(XPNode*)>                                              LogicOnWindowIconifyRestoreFn;
typedef std::function<void(XPNode*)>                                              LogicOnWindowMaximizeFn;
typedef std::function<void(XPNode*)>                                              LogicOnWindowMaximizeRestoreFn;

class XP_PURE_ABSTRACT XPLogic
{
  public:
    virtual ~XPLogic()                                                          = default;
    virtual bool                           load()                               = 0;
    virtual bool                           reload()                             = 0;
    virtual void                           stageReload()                        = 0;
    virtual void                           commitReload()                       = 0;
    virtual bool                           unload()                             = 0;
    virtual std::string                    getName() const                      = 0;
    virtual std::string                    getPath() const                      = 0;
    virtual XPELogicSourceType             getType() const                      = 0;
    virtual time_t                         getTimeStamp() const                 = 0;
    virtual bool                           needsUpload() const                  = 0;
    virtual LogicLoadFn                    getLoadFn() const                    = 0;
    virtual LogicReloadFn                  getReloadFn() const                  = 0;
    virtual LogicStartFn                   getStartFn() const                   = 0;
    virtual LogicUpdateFn                  getUpdateFn() const                  = 0;
    virtual LogicEndFn                     getEndFn() const                     = 0;
    virtual LogicUnloadFn                  getUnloadFn() const                  = 0;
    virtual LogicOnMouseFn                 getOnMouseFn() const                 = 0;
    virtual LogicOnMouseMoveFn             getOnMouseMoveFn() const             = 0;
    virtual LogicOnMouseDragFn             getOnMouseDragFn() const             = 0;
    virtual LogicOnMouseScrollFn           getOnMouseScrollFn() const           = 0;
    virtual LogicOnKeyboardFn              getOnKeyboardFn() const              = 0;
    virtual LogicOnWindowResizeFn          getOnWindowResizeFn() const          = 0;
    virtual LogicOnWindowDragDropFn        getOnWindowDragDropFn() const        = 0;
    virtual LogicOnWindowIconifyFn         getOnWindowIconifyFn() const         = 0;
    virtual LogicOnWindowIconifyRestoreFn  getOnWindowIconifyRestoreFn() const  = 0;
    virtual LogicOnWindowMaximizeFn        getOnWindowMaximizeFn() const        = 0;
    virtual LogicOnWindowMaximizeRestoreFn getOnWindowMaximizeRestoreFn() const = 0;
};