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

#include <Lang/Interface/XPIContext.h>

class XPILang;
class XPIGraph;
class XPITextEditor;

class XPMSLContext final : public XPIContext
{
  public:
    XPMSLContext(XPILang* const lang);
    ~XPMSLContext() final;
    void           setGraph(std::unique_ptr<XPIGraph> graph) final;
    void           setTextEditor(std::unique_ptr<XPITextEditor> textEditor) final;
    XPILang*       getLang() const final;
    XPIGraph*      getGraph() const final;
    XPITextEditor* getTextEditor() const final;

  private:
    XPILang* const                 _lang;
    std::unique_ptr<XPIGraph>      _graph;
    std::unique_ptr<XPITextEditor> _textEditor;
};
