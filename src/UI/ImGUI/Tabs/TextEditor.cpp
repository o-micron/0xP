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

#include "TextEditor.h"

#include <Emulator/XPEmulatorProcessor.h>
#include <UI/ImGUI/XPImGUI.h>
#include <Utilities/XPFS.h>

XPTextEditor::XPTextEditor()
{
    _textEditor = XP_NEW TextEditor();
    _textEditor->SetText("");
    _textEditor->SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
    _textEditor->SetPalette(TextEditor::GetPrototypePalette());
    _textEditor->SetColorizerEnable(true);
}
XPTextEditor::~XPTextEditor() { XP_DELETE _textEditor; }
void
XPTextEditor::render(const char* label, XPVec2<float> size, void* font)
{
    ImGui::PushFont(static_cast<ImFont*>(font));
    _textEditor->Render(label, ImVec2(size.x, size.y));
    ImGui::PopFont();
}
void
XPTextEditor::setFilepath(const std::string& filepath)
{
    if (filepath != _filepath) {
        auto p = std::filesystem::path(filepath);
        if (std::filesystem::exists(p)) {
            std::string source = "";
            XPFS::readFileBlock(filepath.c_str(), source);
            setText(source);
            XPTextEditorCoordinates coordinates = {};
            coordinates.line                    = 1;
            coordinates.column                  = 1;
            setCursorPosition(coordinates);
            if (p.extension() == ".cpp" || p.extension() == ".hpp" || p.extension() == ".h") {
                setLanguage(XPTextEditorLanguage::CPP);
            } else if (p.extension() == ".c") {
                setLanguage(XPTextEditorLanguage::C);
            }
        }
        _filepath = filepath;
    }
}
void
XPTextEditor::setText(std::string text)
{
    _textEditor->SetText(text);
}
std::string
XPTextEditor::getText()
{
    return _textEditor->GetText();
}
void
XPTextEditor::setCursorPosition(XPTextEditorCoordinates coordinates)
{
    TextEditor::Coordinates editorCoordinates = {};
    editorCoordinates.mLine                   = coordinates.line;
    editorCoordinates.mColumn                 = coordinates.column;
    _textEditor->SetCursorPosition(editorCoordinates);
}
XPTextEditorCoordinates
XPTextEditor::getCursorPosition()
{
    XPTextEditorCoordinates coordinates = {};
    coordinates.line                    = _textEditor->GetCursorPosition().mLine;
    coordinates.column                  = _textEditor->GetCursorPosition().mColumn;
    return coordinates;
}
void
XPTextEditor::setLanguage(XPTextEditorLanguage lang)
{
    switch (lang) {
        case XPTextEditorLanguage::C: {
            _textEditor->SetLanguageDefinition(TextEditor::LanguageDefinition::C());
        } break;
        case XPTextEditorLanguage::CPP: {
            _textEditor->SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
        } break;
        case XPTextEditorLanguage::GLSL: {
            _textEditor->SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
        } break;
        case XPTextEditorLanguage::LUA: {
            _textEditor->SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
        } break;
    }
}
XPTextEditorLanguage
XPTextEditor::getLanguage()
{
    auto languageDefinition = _textEditor->GetLanguageDefinition();
    if (languageDefinition.mName == TextEditor::LanguageDefinition::C().mName) { return XPTextEditorLanguage::C; }
    if (languageDefinition.mName == TextEditor::LanguageDefinition::CPlusPlus().mName) {
        return XPTextEditorLanguage::CPP;
    }
    if (languageDefinition.mName == TextEditor::LanguageDefinition::GLSL().mName) { return XPTextEditorLanguage::GLSL; }
    if (languageDefinition.mName == TextEditor::LanguageDefinition::Lua().mName) { return XPTextEditorLanguage::LUA; }
    return XPTextEditorLanguage::GLSL;
}
void
XPTextEditor::setColorHighlightingEnabled(bool isEnabled)
{
    _textEditor->SetColorizerEnable(isEnabled);
}
bool
XPTextEditor::isColorHighlightingEnabled()
{
    return _textEditor->IsColorizerEnabled();
}
void
XPTextEditor::setErrorMarkers(const std::map<int, std::string>& markers)
{
    _textEditor->SetErrorMarkers(markers);
}
const std::map<int, std::string>&
XPTextEditor::getErrorMarkers()
{
    return _textEditor->GetErrorMarkers();
}
void
XPTextEditor::setBreakpoints(const std::unordered_set<int>& markers)
{
    _textEditor->SetBreakpoints(markers);
}
const std::unordered_set<int>&
XPTextEditor::getBreakpoints()
{
    return _textEditor->GetBreakpoints();
}
void
XPTextEditor::setPalette(XPTextEditorPalette palette)
{
    _textEditor->SetPalette(palette);
}
void
XPTextEditor::setPaletteIndex(XPTextEditorPaletteIndex index, uint32_t value)
{
    auto palette             = _textEditor->GetPalette();
    palette[(unsigned)index] = value;
    _textEditor->SetPalette(palette);
}
const XPTextEditorPalette&
XPTextEditor::getPalette()
{
    return _textEditor->GetPalette();
}
uint32_t
XPTextEditor::getPaletteIndex(XPTextEditorPaletteIndex index)
{
    return _textEditor->GetPalette()[(unsigned)index];
}

XPTextEditorUITab::XPTextEditorUITab(XPRegistry* const registry)
  : XPUITab(registry)
  , _textEditor(XP_NEW XPTextEditor())
{
}

XPTextEditorUITab::~XPTextEditorUITab() { XP_DELETE _textEditor; }

void
XPTextEditorUITab::renderView(XPScene* scene, uint32_t& openViewsMask, float deltaTime)
{
    XP_UNUSED(scene)
    XP_UNUSED(openViewsMask)
    XP_UNUSED(deltaTime)

    XPNode* selectedNode = nullptr;
    if (!scene->getSelectedNodes().empty()) { selectedNode = *scene->getSelectedNodes().begin(); }
    if (selectedNode == nullptr || !selectedNode->hasScriptAttachment()) { return; }

    Script* script = selectedNode->getScript();

    ImVec2 textEditorSize = ImGui::GetContentRegionAvail();
    textEditorSize.y -= ImGui::CalcTextSize("a").y;
    ImGui::PushFont(scene->getRegistry()->getUI()->getFontCode());
    _textEditor->render("##RiscvTextEditor", XPVec2<float>(textEditorSize.x, textEditorSize.y), nullptr);
    ImGui::PopFont();
}

void
XPTextEditorUITab::onEvent(XPScene* scene, SDL_Event* event){ XP_UNUSED(scene) XP_UNUSED(event) }

uint32_t XPTextEditorUITab::getViewMask() const
{
    return XPUiViewMaskTextEditor;
}

ImGuiWindowFlags
XPTextEditorUITab::getWindowFlags() const
{
    return ImGuiWindowFlags_NoCollapse;
}

const char*
XPTextEditorUITab::getTitle() const
{
    return XP_TAB_TEXT_EDITOR_TITLE;
}

ImVec2
XPTextEditorUITab::getWindowPadding() const
{
    return ImVec2(5.0f, 5.0f);
}

XPTextEditor*
XPTextEditorUITab::getTextEditor()
{
    return _textEditor;
}

void
XPTextEditorUITab::setText(const char* code)
{
    // editor set the syntax highlighting lang
    _textEditor->setLanguage(XPTextEditorLanguage::CPP);

    // set text from file
    _textEditor->setText(code);

    // set theme
    _textEditor->setColorHighlightingEnabled(true);
}

void
XPTextEditorUITab::setBreakpoints(const std::unordered_set<int>& lines)
{
    _textEditor->setBreakpoints(lines);
}
