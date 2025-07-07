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

#include <UI/ImGUI/Tabs/Assets.h>

#include <DataPipeline/XPTextureAsset.h>
#include <DataPipeline/XPTextureBuffer.h>
#include <SceneDescriptor/XPSceneDescriptorStore.h>

#include <string.h>

XPAssetsUITab::XPAssetsUITab(XPRegistry* const registry)
  : XPUITab(registry)
{
}

void
XPAssetsUITab::XPAssetsUITab::renderView(XPScene* scene, uint32_t& openViewsMask, float deltaTime)
{
    XP_UNUSED(deltaTime)

    ImGui::PushID("TAB_ASSETS");

    static ImGuiTableFlags assetsFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
                                         ImGuiTableFlags_NoBordersInBodyUntilResize | ImGuiTableFlags_NoBordersInBody |
                                         ImGuiTableFlags_PreciseWidths;
    if (ImGui::BeginTable("##assets table", 2, assetsFlags)) {

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        static int currentAsset = 1;
        {
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            static ImGuiTableFlags assetsIndexFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody |
                                                      ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;
            if (ImGui::BeginTable("##assets index table", 1, assetsIndexFlags)) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                if (ImGui::Selectable("Colliders", currentAsset == 1, ImGuiSelectableFlags_SpanAllColumns)) {
                    currentAsset = 1;
                    openViewsMask |= XPUiViewMaskColliders;
                    openViewsMask &= ~XPUiViewMaskMaterials;
                    openViewsMask &= ~XPUiViewMaskTextures;
                    openViewsMask &= ~XPUiViewMaskShaders;
                    openViewsMask &= ~XPUiViewMaskMeshes;
                    openViewsMask &= ~XPUiViewMaskScenes;
                    openViewsMask &= ~XPUiViewMaskFramebuffers;
                    openViewsMask &= ~XPUiViewMaskScripts;
                    openViewsMask &= ~XPUiViewMaskRiscvBinaries;
                    openViewsMask &= ~XPUiViewMaskFiles;
                }

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                if (ImGui::Selectable("Framebuffers", currentAsset == 2, ImGuiSelectableFlags_SpanAllColumns)) {
                    currentAsset = 2;
                    openViewsMask &= ~XPUiViewMaskColliders;
                    openViewsMask &= ~XPUiViewMaskMaterials;
                    openViewsMask &= ~XPUiViewMaskTextures;
                    openViewsMask &= ~XPUiViewMaskShaders;
                    openViewsMask &= ~XPUiViewMaskMeshes;
                    openViewsMask &= ~XPUiViewMaskScenes;
                    openViewsMask |= XPUiViewMaskFramebuffers;
                    openViewsMask &= ~XPUiViewMaskScripts;
                    openViewsMask &= ~XPUiViewMaskRiscvBinaries;
                    openViewsMask &= ~XPUiViewMaskFiles;
                }

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                if (ImGui::Selectable("Materials", currentAsset == 3, ImGuiSelectableFlags_SpanAllColumns)) {
                    currentAsset = 3;
                    openViewsMask &= ~XPUiViewMaskColliders;
                    openViewsMask |= XPUiViewMaskMaterials;
                    openViewsMask &= ~XPUiViewMaskTextures;
                    openViewsMask &= ~XPUiViewMaskShaders;
                    openViewsMask &= ~XPUiViewMaskMeshes;
                    openViewsMask &= ~XPUiViewMaskScenes;
                    openViewsMask &= ~XPUiViewMaskFramebuffers;
                    openViewsMask &= ~XPUiViewMaskScripts;
                    openViewsMask &= ~XPUiViewMaskRiscvBinaries;
                    openViewsMask &= ~XPUiViewMaskFiles;
                }

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                if (ImGui::Selectable("Meshes", currentAsset == 4, ImGuiSelectableFlags_SpanAllColumns)) {
                    currentAsset = 4;
                    openViewsMask &= ~XPUiViewMaskColliders;
                    openViewsMask &= ~XPUiViewMaskMaterials;
                    openViewsMask &= ~XPUiViewMaskTextures;
                    openViewsMask &= ~XPUiViewMaskShaders;
                    openViewsMask |= XPUiViewMaskMeshes;
                    openViewsMask &= ~XPUiViewMaskScenes;
                    openViewsMask &= ~XPUiViewMaskFramebuffers;
                    openViewsMask &= ~XPUiViewMaskScripts;
                    openViewsMask &= ~XPUiViewMaskRiscvBinaries;
                    openViewsMask &= ~XPUiViewMaskFiles;
                }

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                if (ImGui::Selectable("Scenes", currentAsset == 5, ImGuiSelectableFlags_SpanAllColumns)) {
                    currentAsset = 5;
                    openViewsMask &= ~XPUiViewMaskColliders;
                    openViewsMask &= ~XPUiViewMaskMaterials;
                    openViewsMask &= ~XPUiViewMaskTextures;
                    openViewsMask &= ~XPUiViewMaskShaders;
                    openViewsMask &= ~XPUiViewMaskMeshes;
                    openViewsMask |= XPUiViewMaskScenes;
                    openViewsMask &= ~XPUiViewMaskFramebuffers;
                    openViewsMask &= ~XPUiViewMaskScripts;
                    openViewsMask &= ~XPUiViewMaskRiscvBinaries;
                    openViewsMask &= ~XPUiViewMaskFiles;
                }

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                if (ImGui::Selectable("Scripts", currentAsset == 6, ImGuiSelectableFlags_SpanAllColumns)) {
                    currentAsset = 6;
                    openViewsMask &= ~XPUiViewMaskColliders;
                    openViewsMask &= ~XPUiViewMaskMaterials;
                    openViewsMask &= ~XPUiViewMaskTextures;
                    openViewsMask &= ~XPUiViewMaskShaders;
                    openViewsMask &= ~XPUiViewMaskMeshes;
                    openViewsMask &= ~XPUiViewMaskScenes;
                    openViewsMask &= ~XPUiViewMaskFramebuffers;
                    openViewsMask |= XPUiViewMaskScripts;
                    openViewsMask &= ~XPUiViewMaskRiscvBinaries;
                    openViewsMask &= ~XPUiViewMaskFiles;
                }

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                if (ImGui::Selectable("Shaders", currentAsset == 7, ImGuiSelectableFlags_SpanAllColumns)) {
                    currentAsset = 7;
                    openViewsMask &= ~XPUiViewMaskColliders;
                    openViewsMask &= ~XPUiViewMaskMaterials;
                    openViewsMask &= ~XPUiViewMaskTextures;
                    openViewsMask |= XPUiViewMaskShaders;
                    openViewsMask &= ~XPUiViewMaskMeshes;
                    openViewsMask &= ~XPUiViewMaskScenes;
                    openViewsMask &= ~XPUiViewMaskFramebuffers;
                    openViewsMask &= ~XPUiViewMaskScripts;
                    openViewsMask &= ~XPUiViewMaskRiscvBinaries;
                    openViewsMask &= ~XPUiViewMaskFiles;
                }

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                if (ImGui::Selectable("Textures", currentAsset == 8, ImGuiSelectableFlags_SpanAllColumns)) {
                    currentAsset = 8;
                    openViewsMask &= ~XPUiViewMaskColliders;
                    openViewsMask &= ~XPUiViewMaskMaterials;
                    openViewsMask |= XPUiViewMaskTextures;
                    openViewsMask &= ~XPUiViewMaskShaders;
                    openViewsMask &= ~XPUiViewMaskMeshes;
                    openViewsMask &= ~XPUiViewMaskScenes;
                    openViewsMask &= ~XPUiViewMaskFramebuffers;
                    openViewsMask &= ~XPUiViewMaskScripts;
                    openViewsMask &= ~XPUiViewMaskRiscvBinaries;
                    openViewsMask &= ~XPUiViewMaskFiles;
                }

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                if (ImGui::Selectable("RiscvBinaries", currentAsset == 9, ImGuiSelectableFlags_SpanAllColumns)) {
                    currentAsset = 9;
                    openViewsMask &= ~XPUiViewMaskColliders;
                    openViewsMask &= ~XPUiViewMaskMaterials;
                    openViewsMask &= XPUiViewMaskTextures;
                    openViewsMask &= ~XPUiViewMaskShaders;
                    openViewsMask &= ~XPUiViewMaskMeshes;
                    openViewsMask &= ~XPUiViewMaskScenes;
                    openViewsMask &= ~XPUiViewMaskFramebuffers;
                    openViewsMask &= ~XPUiViewMaskScripts;
                    openViewsMask |= ~XPUiViewMaskRiscvBinaries;
                    openViewsMask &= ~XPUiViewMaskFiles;
                }

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                if (ImGui::Selectable("Files", currentAsset == 10, ImGuiSelectableFlags_SpanAllColumns)) {
                    currentAsset = 10;
                    openViewsMask &= ~XPUiViewMaskColliders;
                    openViewsMask &= ~XPUiViewMaskMaterials;
                    openViewsMask &= ~XPUiViewMaskTextures;
                    openViewsMask &= ~XPUiViewMaskShaders;
                    openViewsMask &= ~XPUiViewMaskMeshes;
                    openViewsMask &= ~XPUiViewMaskScenes;
                    openViewsMask &= ~XPUiViewMaskFramebuffers;
                    openViewsMask &= ~XPUiViewMaskScripts;
                    openViewsMask &= ~XPUiViewMaskRiscvBinaries;
                    openViewsMask |= XPUiViewMaskFiles;
                }

                ImGui::EndTable();
            }
            ImGui::TableSetColumnIndex(1);
            if (currentAsset == 0) {
                renderBlueprintsView(scene, openViewsMask);
            } else if (currentAsset == 1) {
                renderCollidersView(scene, openViewsMask);
            } else if (currentAsset == 2) {
                renderFramebuffersView(scene, openViewsMask);
            } else if (currentAsset == 3) {
                renderMaterialsView(scene, openViewsMask);
            } else if (currentAsset == 4) {
                renderMeshesView(scene, openViewsMask);
            } else if (currentAsset == 5) {
                renderScenesView(scene, openViewsMask);
            } else if (currentAsset == 6) {
                renderScriptsView(scene, openViewsMask);
            } else if (currentAsset == 7) {
                renderShadersView(scene, openViewsMask);
            } else if (currentAsset == 8) {
                renderTexturesView(scene, openViewsMask);
            } else if (currentAsset == 9) {
                renderRiscvBinariesView(scene, openViewsMask);
            } else if (currentAsset == 10) {
                renderFilesView(scene, openViewsMask);
            }
        }
        ImGui::EndTable();
    }

    ImGui::PopID();
}

void
XPAssetsUITab::onEvent(XPScene* scene, SDL_Event* event){ XP_UNUSED(scene) XP_UNUSED(event) }

uint32_t XPAssetsUITab::getViewMask() const
{
    return XPUiViewMaskAssets;
}

ImGuiWindowFlags
XPAssetsUITab::getWindowFlags() const
{
    return ImGuiWindowFlags_NoCollapse;
}

const char*
XPAssetsUITab::getTitle() const
{
    return XP_TAB_ASSETS_TITLE;
}

ImVec2
XPAssetsUITab::getWindowPadding() const
{
    return ImVec2(4.0f, 4.0f);
}

void
XPAssetsUITab::renderBlueprintsView(XPScene* scene, uint32_t& openViewsMask)
{
    XP_UNUSED(scene)
    XP_UNUSED(openViewsMask)

    ImGui::PushID("TAB_ASSETS_BLUEPRINTS");

    ImGui::PopID();
}

void
XPAssetsUITab::renderCollidersView(XPScene* scene, uint32_t& openViewsMask)
{
    XP_UNUSED(scene)
    XP_UNUSED(openViewsMask)

    ImGui::PushID("TAB_ASSETS_COLLIDERS");

    ImGui::PopID();
}

void
XPAssetsUITab::renderFramebuffersView(XPScene* scene, uint32_t& openViewsMask)
{
    XP_UNUSED(scene)
    XP_UNUSED(openViewsMask)

    ImGui::PushID("TAB_ASSETS_FRAMEBUFFERS");

    ImGui::PopID();
}

void
XPAssetsUITab::renderMaterialsView(XPScene* scene, uint32_t& openViewsMask)
{
    XP_UNUSED(scene)
    XP_UNUSED(openViewsMask)

    ImGui::PushID("TAB_ASSETS_MATERIALS");

    ImGui::PopID();
}

void
XPAssetsUITab::renderMeshesView(XPScene* scene, uint32_t& openViewsMask)
{
    XP_UNUSED(openViewsMask)

    ImGui::PushID("TAB_ASSETS_MESHES");

    static const std::function<void(const XPMeshBufferObject&)> fnMeshBufferObject =
      [&](const XPMeshBufferObject& meshBufferObject) {
          ImGuiTreeNodeFlags nodeTreeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth |
                                                 ImGuiTreeNodeFlags_NavLeftJumpsBackHere |
                                                 ImGuiTreeNodeFlags_NoAutoOpenOnLog |
                                                 ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Leaf;
          if (ImGui::TreeNodeEx(meshBufferObject.name.c_str(), nodeTreeNodeFlags)) { ImGui::TreePop(); }
      };
    static const std::function<void(const XPMeshAsset*)> fnMeshAsset = [&](const XPMeshAsset* meshAsset) {
        if (ImGui::TreeNode(meshAsset->getFile()->getPath().c_str())) {
            for (size_t mboi = 0; mboi < meshAsset->getMeshBuffer()->getObjectsCount(); ++mboi) {
                XPMeshBufferObject& meshBufferObject = meshAsset->getMeshBuffer()->objectAtIndex(mboi);
                fnMeshBufferObject(meshBufferObject);
            }
            ImGui::TreePop();
        }
    };

    static ImGuiTableFlags sceneFlags =
      ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;
    if (ImGui::BeginTable("##meshes table", 1, sceneFlags)) {
        for (const auto& meshAsset : scene->getRegistry()->getDataPipelineStore()->getMeshAssets()) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            fnMeshAsset(meshAsset.second);
        }
        ImGui::EndTable();
    }

    ImGui::PopID();
}

void
XPAssetsUITab::renderScenesView(XPScene* scene, uint32_t& openViewsMask)
{
    XP_UNUSED(openViewsMask)

    ImGui::PushID("TAB_ASSETS_SCENES");

    static ImGuiTableFlags sceneFlags =
      ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;
    if (ImGui::BeginTable("##scenes table", 1, sceneFlags)) {
        for (const auto& listedScene : scene->getSceneDescriptorStore()->getScenes()) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            ImGui::Text("%s %s", listedScene->getName().c_str(), (scene == listedScene ? "*" : ""));
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                scene->getRegistry()->setSceneBuffered(listedScene);
            }
        }
        ImGui::EndTable();
    }

    ImGui::PopID();
}

void
XPAssetsUITab::renderScriptsView(XPScene* scene, uint32_t& openViewsMask)
{
    XP_UNUSED(scene)
    XP_UNUSED(openViewsMask)

    ImGui::PushID("TAB_ASSETS_SCRIPTS");

    ImGui::PopID();
}

void
XPAssetsUITab::renderShadersView(XPScene* scene, uint32_t& openViewsMask)
{
    XP_UNUSED(scene)
    XP_UNUSED(openViewsMask)

    ImGui::PushID("TAB_ASSETS_SHADERS");

    ImGui::PopID();
}

void
XPAssetsUITab::renderTexturesView(XPScene* scene, uint32_t& openViewsMask)
{
    XP_UNUSED(scene)
    XP_UNUSED(openViewsMask)

    ImGui::PushID("TAB_ASSETS_TEXTURES");

    static ImGuiTableFlags texturesFlags =
      ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;
    if (ImGui::BeginTable("##textures table", 1, texturesFlags)) {
        for (const auto& textureAsset : scene->getRegistry()->getDataPipelineStore()->getTextureAssets()) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            ImGui::Text("%s", textureAsset.first->getPath().c_str());
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Image((ImTextureID)textureAsset.second->getGPURef(), ImVec2(200, 200));
                ImGui::EndTooltip();
            }
        }
        ImGui::EndTable();
    }

    ImGui::PopID();
}

void
XPAssetsUITab::renderRiscvBinariesView(XPScene* scene, uint32_t& openViewsMask)
{
    XP_UNUSED(scene)
    XP_UNUSED(openViewsMask)

    ImGui::PushID("TAB_ASSETS_RISCV_BINARIES");

    static ImGuiTableFlags flags =
      ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;
    if (ImGui::BeginTable("##riscv binaries table", 1, flags)) {
        for (const auto& riscvBinaryAsset : scene->getRegistry()->getDataPipelineStore()->getRiscvBinaryAssets()) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            ImGui::Text("%s", riscvBinaryAsset.first->getPath().c_str());
        }
        ImGui::EndTable();
    }

    ImGui::PopID();
}

void
XPAssetsUITab::renderFilesView(XPScene* scene, uint32_t& openViewsMask)
{
    XP_UNUSED(openViewsMask)

    ImGui::PushID("TAB_ASSETS_FILES");

    static std::list<std::string> output;
    static std::string            filterBuff = "";
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputText("%s", &filterBuff, 0);

    typedef std::pair<XPEFileResourceType, std::unordered_map<std::string, XPFile*>> FileTypeMapPair;

    static const std::function<void(const XPFile*)> fnFile = [&](const XPFile* file) {
        ImGuiTreeNodeFlags nodeTreeNodeFlags =
          ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_NavLeftJumpsBackHere |
          ImGuiTreeNodeFlags_NoAutoOpenOnLog | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Leaf;
        const char* filePath = file->getPath().c_str();
        if (strstr(filePath, filterBuff.c_str()) != nullptr) {
            if (ImGui::TreeNodeEx(file->getPath().c_str(), nodeTreeNodeFlags)) { ImGui::TreePop(); }
        }
    };
    static const std::function<void(const FileTypeMapPair&)> fnFileType = [&](const FileTypeMapPair& fileTypePair) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        const char* resourceTypeName = [](const FileTypeMapPair& fileTypePair) {
            switch (fileTypePair.first) {
                case XPEFileResourceType::Unknown: return "UnknownFiles";
                case XPEFileResourceType::PreloadedMesh: return "PreloadedMeshFiles";
                case XPEFileResourceType::Mesh: return "MeshFiles";
                case XPEFileResourceType::Shader: return "ShaderFiles";
                case XPEFileResourceType::Texture: return "TextureFiles";
                case XPEFileResourceType::Plugin: return "PluginFiles";
                case XPEFileResourceType::Scene: return "SceneFiles";
                case XPEFileResourceType::RiscvBinary: return "RiscvBinaryFiles";
                case XPEFileResourceType::Count: return "";
            };
            return "";
        }(fileTypePair);

        if (ImGui::TreeNode(resourceTypeName)) {
            for (const auto& filePair : fileTypePair.second) { fnFile(filePair.second); }
            ImGui::TreePop();
        }
    };

    static ImGuiTableFlags sceneFlags =
      ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;
    if (ImGui::BeginTable("##files table", 1, sceneFlags)) {
        for (const auto& fileTypePair : scene->getRegistry()->getDataPipelineStore()->getFiles()) {
            fnFileType(fileTypePair);
        }
        ImGui::EndTable();
    }

    ImGui::PopID();
}
