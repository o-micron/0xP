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

#include <Utilities/XPAnnotations.h>
#include <Utilities/XPMaths.h>

#include <UI/ImGUI/Tabs/Tabs.h>

#include <array>
#include <map>
#include <string>
#include <unordered_set>

enum class XPTextEditorLanguage
{
    C,
    CPP,
    GLSL,
    LUA
};

struct XPTextEditorCoordinates
{
    uint32_t line;
    uint32_t column;
};

enum class XPTextEditorPaletteIndex
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

typedef std::array<uint32_t, (unsigned)XPTextEditorPaletteIndex::Max> XPTextEditorPalette;

struct XPTextEditor
{
    XPTextEditor();

    ~XPTextEditor();

    // render the text editor
    void render(const char* label, XPVec2<float> size, void* font);

    // sets the path of file
    void setFilepath(const std::string& filepath);

    // sets the content in the text editor
    void setText(std::string text);

    // returns the text content in the text editor
    std::string getText();

    // sets the cursor position
    void setCursorPosition(XPTextEditorCoordinates coordinates);

    // returns the cursor position
    XPTextEditorCoordinates getCursorPosition();

    // sets the text highlighting language
    void setLanguage(XPTextEditorLanguage lang);

    // returns the text highlighting language
    XPTextEditorLanguage getLanguage();

    // set the text color highlighting enabled
    void setColorHighlightingEnabled(bool isEnabled);

    // returns true if text color highlighting is enabled
    bool isColorHighlightingEnabled();

    // marks errors on lines with strings or the errors
    void setErrorMarkers(const std::map<int, std::string>& markers);

    // returns the errors marks
    const std::map<int, std::string>& getErrorMarkers();

    // marks breakpoints on lines
    void setBreakpoints(const std::unordered_set<int>& markers);

    // returns the breakpoints marks
    const std::unordered_set<int>& getBreakpoints();

    // set the palette
    void setPalette(XPTextEditorPalette palette);

    // set a single value in the palette by index
    void setPaletteIndex(XPTextEditorPaletteIndex index, uint32_t value);

    // return the palette
    const XPTextEditorPalette& getPalette();

    // return a single value from the palette index
    uint32_t getPaletteIndex(XPTextEditorPaletteIndex index);

  private:
    TextEditor* _textEditor;
    std::string _filepath;
};

class XPTextEditorUITab final : public XPUITab
{
  public:
    XPTextEditorUITab(XPRegistry* const registry);
    ~XPTextEditorUITab() final;
    void             renderView(XPScene* scene, uint32_t& openViewsMask, float deltaTime) final;
    void             onEvent(XPScene* scene, SDL_Event* event) final;
    uint32_t         getViewMask() const final;
    ImGuiWindowFlags getWindowFlags() const final;
    const char*      getTitle() const final;
    ImVec2           getWindowPadding() const;
    XPTextEditor*    getTextEditor();
    void             setText(const char* code);
    void             setBreakpoints(const std::unordered_set<int>& lines);

  private:
    XPTextEditor* _textEditor;
};
