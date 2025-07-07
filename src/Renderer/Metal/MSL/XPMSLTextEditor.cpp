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

#include <Renderer/Metal/MSL/XPMSLTextEditor.h>

XPMSLTextEditor::XPMSLTextEditor() {}

XPMSLTextEditor::~XPMSLTextEditor() {}

void
XPMSLTextEditor::setText(std::string text)
{
    _text = text;
}

std::string
XPMSLTextEditor::getText()
{
    return _text;
}

void
XPMSLTextEditor::setCursorPosition(XPITextEditorCoordinates coordinates){ XP_UNUSED(coordinates) }

XPITextEditorCoordinates XPMSLTextEditor::getCursorPosition()
{
    return { 0, 0 };
}

void
XPMSLTextEditor::setLanguage(XPITextEditorLanguage lang){ XP_UNUSED(lang) }

XPITextEditorLanguage XPMSLTextEditor::getLanguage()
{
    return XPITextEditorLanguage::MSL;
}

void
XPMSLTextEditor::setColorHighlightingEnabled(bool isEnabled)
{
    XP_UNUSED(isEnabled)
}

bool
XPMSLTextEditor::isColorHighlightingEnabled()
{
    return false;
}

void
XPMSLTextEditor::setErrorMarkers(std::map<int, std::string> markers)
{
    XP_UNUSED(markers)
}

const std::map<int, std::string>&
XPMSLTextEditor::getErrorMarkers()
{
    return _errorMarkers;
}

void
XPMSLTextEditor::setBreakpoints(std::unordered_set<int> markers)
{
    XP_UNUSED(markers)
}

const std::unordered_set<int>&
XPMSLTextEditor::getBreakpoints()
{
    return _breakpoints;
}

void
XPMSLTextEditor::setPalette(XPITextEditorPalette palette)
{
    XP_UNUSED(palette)
}

void
XPMSLTextEditor::setPaletteIndex(XPITextEditorPaletteIndex index, uint32_t value)
{
    XP_UNUSED(index)
    XP_UNUSED(value)
}

const XPITextEditorPalette&
XPMSLTextEditor::getPalette()
{
    return _palette;
}

uint32_t
XPMSLTextEditor::getPaletteIndex(XPITextEditorPaletteIndex index)
{
    XP_UNUSED(index)

    return 0;
}
