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

#include <Renderer/WebGPU/WGSL/XPWGSLTextEditor.h>

XPWGSLTextEditor::XPWGSLTextEditor() {}

XPWGSLTextEditor::~XPWGSLTextEditor() {}

void
XPWGSLTextEditor::setText(std::string text)
{
    _text = text;
}

std::string
XPWGSLTextEditor::getText()
{
    return _text;
}

void
XPWGSLTextEditor::setCursorPosition(XPITextEditorCoordinates coordinates){ XP_UNUSED(coordinates) }

XPITextEditorCoordinates XPWGSLTextEditor::getCursorPosition()
{
    return { 0, 0 };
}

void
XPWGSLTextEditor::setLanguage(XPITextEditorLanguage lang){ XP_UNUSED(lang) }

XPITextEditorLanguage XPWGSLTextEditor::getLanguage()
{
    return XPITextEditorLanguage::MSL;
}

void
XPWGSLTextEditor::setColorHighlightingEnabled(bool isEnabled)
{
    XP_UNUSED(isEnabled)
}

bool
XPWGSLTextEditor::isColorHighlightingEnabled()
{
    return false;
}

void
XPWGSLTextEditor::setErrorMarkers(std::map<int, std::string> markers)
{
    XP_UNUSED(markers)
}

const std::map<int, std::string>&
XPWGSLTextEditor::getErrorMarkers()
{
    return _errorMarkers;
}

void
XPWGSLTextEditor::setBreakpoints(std::unordered_set<int> markers)
{
    XP_UNUSED(markers)
}

const std::unordered_set<int>&
XPWGSLTextEditor::getBreakpoints()
{
    return _breakpoints;
}

void
XPWGSLTextEditor::setPalette(XPITextEditorPalette palette)
{
    XP_UNUSED(palette)
}

void
XPWGSLTextEditor::setPaletteIndex(XPITextEditorPaletteIndex index, uint32_t value)
{
    XP_UNUSED(index)
    XP_UNUSED(value)
}

const XPITextEditorPalette&
XPWGSLTextEditor::getPalette()
{
    return _palette;
}

uint32_t
XPWGSLTextEditor::getPaletteIndex(XPITextEditorPaletteIndex index)
{
    XP_UNUSED(index)

    return 0;
}
