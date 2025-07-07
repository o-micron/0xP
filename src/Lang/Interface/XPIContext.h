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

class XPILang;
class XPIGraph;
class XPITextEditor;

class XP_PURE_ABSTRACT XPIContext
{
  public:
    XPIContext(XPILang* const lang) { XP_UNUSED(lang) }
    virtual ~XPIContext() {}
    virtual void           setGraph(std::unique_ptr<XPIGraph> graph)                = 0;
    virtual void           setTextEditor(std::unique_ptr<XPITextEditor> textEditor) = 0;
    virtual XPILang*       getLang() const                                          = 0;
    virtual XPIGraph*      getGraph() const                                         = 0;
    virtual XPITextEditor* getTextEditor() const                                    = 0;
};
