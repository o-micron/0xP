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
#include <Renderer/Metal/MSL/XPMSLGraph.h>
#include <Renderer/Metal/MSL/XPMSLLang.h>
#include <Renderer/Metal/MSL/XPMSLTextEditor.h>

XPMSLContext::XPMSLContext(XPILang* const lang)
  : XPIContext(lang)
  , _lang(lang)
  , _graph(nullptr)
  , _textEditor(nullptr)
{
}

XPMSLContext::~XPMSLContext() {}

void
XPMSLContext::setGraph(std::unique_ptr<XPIGraph> graph)
{
    _graph = std::move(graph);
}

void
XPMSLContext::setTextEditor(std::unique_ptr<XPITextEditor> textEditor)
{
    _textEditor = std::move(textEditor);
}

XPILang*
XPMSLContext::getLang() const
{
    return _lang;
}

XPIGraph*
XPMSLContext::getGraph() const
{
    return _graph.get();
}

XPITextEditor*
XPMSLContext::getTextEditor() const
{
    return _textEditor.get();
}
