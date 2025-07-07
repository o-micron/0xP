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

#include <Lang/Interface/XPITextEditor.h>

class XPMSLTextEditor final : public XPITextEditor
{
  public:
    XPMSLTextEditor();
    ~XPMSLTextEditor() final;
    void                              setText(std::string text) final;
    std::string                       getText() final;
    void                              setCursorPosition(XPITextEditorCoordinates coordinates) final;
    XPITextEditorCoordinates          getCursorPosition() final;
    void                              setLanguage(XPITextEditorLanguage lang) final;
    XPITextEditorLanguage             getLanguage() final;
    void                              setColorHighlightingEnabled(bool isEnabled) final;
    bool                              isColorHighlightingEnabled() final;
    void                              setErrorMarkers(std::map<int, std::string> markers) final;
    const std::map<int, std::string>& getErrorMarkers() final;
    void                              setBreakpoints(std::unordered_set<int> markers) final;
    const std::unordered_set<int>&    getBreakpoints() final;
    void                              setPalette(XPITextEditorPalette palette) final;
    void                              setPaletteIndex(XPITextEditorPaletteIndex index, uint32_t value) final;
    const XPITextEditorPalette&       getPalette() final;
    uint32_t                          getPaletteIndex(XPITextEditorPaletteIndex index) final;

  private:
    std::string                _text;
    std::map<int, std::string> _errorMarkers;
    std::unordered_set<int>    _breakpoints;
    XPITextEditorPalette       _palette;
};
