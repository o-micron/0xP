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

#include <array>
#include <map>
#include <string>
#include <unordered_set>

enum class XPITextEditorLanguage
{
    C,
    CPP,
    GLSL,
    MSL,
    LUA
};

struct XPITextEditorCoordinates
{
    int line;
    int column;
};

enum class XPITextEditorPaletteIndex
{
    Default,
    Keyword,
    Number,
    String,
    CharLiteral,
    Punctuation,
    Preprocessor,
    Identifier,
    KnownIdentifier,
    PreprocIdentifier,
    Comment,
    MultiLineComment,
    Background,
    Cursor,
    Selection,
    ErrorMarker,
    Breakpoint,
    LineNumber,
    CurrentLineFill,
    CurrentLineFillInactive,
    CurrentLineEdge,
    Max
};

typedef std::array<uint32_t, (unsigned)XPITextEditorPaletteIndex::Max> XPITextEditorPalette;

class XP_PURE_ABSTRACT XPITextEditor
{
  public:
    XPITextEditor() {}
    virtual ~XPITextEditor() {}
    virtual void                              setText(std::string text)                                        = 0;
    virtual std::string                       getText()                                                        = 0;
    virtual void                              setCursorPosition(XPITextEditorCoordinates coordinates)          = 0;
    virtual XPITextEditorCoordinates          getCursorPosition()                                              = 0;
    virtual void                              setLanguage(XPITextEditorLanguage lang)                          = 0;
    virtual XPITextEditorLanguage             getLanguage()                                                    = 0;
    virtual void                              setColorHighlightingEnabled(bool isEnabled)                      = 0;
    virtual bool                              isColorHighlightingEnabled()                                     = 0;
    virtual void                              setErrorMarkers(std::map<int, std::string> markers)              = 0;
    virtual const std::map<int, std::string>& getErrorMarkers()                                                = 0;
    virtual void                              setBreakpoints(std::unordered_set<int> markers)                  = 0;
    virtual const std::unordered_set<int>&    getBreakpoints()                                                 = 0;
    virtual void                              setPalette(XPITextEditorPalette palette)                         = 0;
    virtual void                              setPaletteIndex(XPITextEditorPaletteIndex index, uint32_t value) = 0;
    virtual const XPITextEditorPalette&       getPalette()                                                     = 0;
    virtual uint32_t                          getPaletteIndex(XPITextEditorPaletteIndex index)                 = 0;
};