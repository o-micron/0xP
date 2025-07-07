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

#include "XPScript.h"
#include <Engine/XPRegistry.h>
#include <SceneDescriptor/XPLayer.h>
#include <SceneDescriptor/XPNode.h>
#include <SceneDescriptor/XPScene.h>
#if defined(XP_EDITOR_MODE)
    #include <UI/ImGUI/Tabs/TextEditor.h>
    #include <UI/Interface/XPIUI.h>
#endif
#include <Compute/include/Compute/XPCompute.h>
#include <Emulator/XPEmulatorElfLoader.h>
#include <Emulator/XPEmulatorProcessor.h>
#include <Utilities/XPFS.h>

void
Script::onChanged_source()
{
}

void
Script::onChanged_program()
{
}

void
onTraitAttached(Script* script)
{
    script->processor = (XPEmulatorProcessor*)malloc(sizeof(XPEmulatorProcessor));
    script->program   = "";
    script->elfLoader = NULL;
    script->isLoaded.store(false);
    script->isRunning.store(false);
#if defined(XP_EDITOR_MODE)
// _textEditor->setBreakpoints();
#endif
}

void
onTraitDettached(Script* script)
{
    if (script->executionThread.joinable()) { script->executionThread.join(); }
    if (script->processor) {
        xp_emulator_processor_finalize(script->processor);
        free(script->processor);
    }
    if (script->elfLoader) { xp_emulator_elf_loader_unload((RiscvElfLoader*)script->elfLoader); }
}

void
onRenderUI(Script* script, XPIUI* ui)
{
#if defined(XP_EDITOR_MODE)
    if (ImGui::Button(ICON_FA_DOWNLOAD, ImVec2(25.0f, 25.0f))) {
        if (script->isLoaded.load() == false && script->isRunning == false) {
            const std::string fullProgramPath = XPFS::buildRiscvBianryAssetsPath(script->program);
            if (std::filesystem::exists(fullProgramPath)) {
                script->elfLoader = (void*)xp_emulator_elf_loader_load(fullProgramPath.c_str());
                if (script->elfLoader == NULL) { return; }

                script->processor = (XPEmulatorProcessor*)malloc(sizeof(XPEmulatorProcessor));
                xp_emulator_processor_initialize(script->processor);

                if (xp_emulator_processor_load_program(script->processor, ((RiscvElfLoader*)script->elfLoader)) == 0) {
                    std::vector<XPUITab*> tabs       = ui->getTabs();
                    XPTextEditorUITab* textEditorTab = static_cast<XPTextEditorUITab*>(tabs[XPUiViewMaskTextEditorBit]);
                    XPTextEditor*      textEditor    = textEditorTab->getTextEditor();

                    textEditor->setColorHighlightingEnabled(true);
                    textEditor->setFilepath(script->program);
                    script->isLoaded.store(true);
                    script->isRunning.store(false);
                }
            }
        }
    }
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_PLAY, ImVec2(25.0f, 25.0f))) {
        if (script->isLoaded.load() == true && script->isRunning.load() == false) {
            std::vector<XPUITab*> tabs          = ui->getTabs();
            XPTextEditorUITab*    textEditorTab = static_cast<XPTextEditorUITab*>(tabs[XPUiViewMaskTextEditorBit]);
            XPTextEditor*         textEditor    = textEditorTab->getTextEditor();

            script->isRunning.store(true);
            script->executionThread = std::thread([&]() {
                const std::string fullProgramPath = XPFS::buildRiscvBianryAssetsPath(script->program);
                xp_emulator_processor_run(script->processor);
    #if defined(XP_USE_COMPUTE)
                xp_compute_load_and_run(fullProgramPath.c_str());
    #endif
                script->isRunning.store(false);
            });
            script->executionThread.join();
        }
    }
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_STOP, ImVec2(25.0f, 25.0f))) {
        if (script->isLoaded.load() == true) {
            if (script->executionThread.joinable()) { script->executionThread.join(); }
            if (script->processor) {
                xp_emulator_processor_finalize(script->processor);
                free(script->processor);
            }
            if (script->elfLoader) { xp_emulator_elf_loader_unload((RiscvElfLoader*)script->elfLoader); }
            script->isLoaded.store(false);
            script->isRunning.store(false);
        }
    }
#endif
}