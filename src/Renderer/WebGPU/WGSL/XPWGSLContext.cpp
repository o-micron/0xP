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
#include <Renderer/WebGPU/WGSL/XPWGSLGraph.h>
#include <Renderer/WebGPU/WGSL/XPWGSLLang.h>
#include <Renderer/WebGPU/WGSL/XPWGSLTextEditor.h>

XPWGSLContext::XPWGSLContext(XPILang* const lang)
  : XPIContext(lang)
  , _lang(lang)
  , _graph(nullptr)
  , _textEditor(nullptr)
{
}

XPWGSLContext::~XPWGSLContext() {}

void
XPWGSLContext::setGraph(std::unique_ptr<XPIGraph> graph)
{
    _graph = std::move(graph);
}

void
XPWGSLContext::setTextEditor(std::unique_ptr<XPITextEditor> textEditor)
{
    _textEditor = std::move(textEditor);
}

XPILang*
XPWGSLContext::getLang() const
{
    return _lang;
}

XPIGraph*
XPWGSLContext::getGraph() const
{
    return _graph.get();
}

XPITextEditor*
XPWGSLContext::getTextEditor() const
{
    return _textEditor.get();
}
