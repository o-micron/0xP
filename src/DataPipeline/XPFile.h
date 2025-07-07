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

#include <DataPipeline/XPEnums.h>
#include <Utilities/XPPlatforms.h>

#include <filesystem>
#include <stdint.h>
#include <string>
#include <unordered_set>

class XPDataPipelineStore;
class XPMeshAsset;
class XPShaderAsset;
class XPTextureAsset;
class XPRiscvBinaryAsset;
class XPScene;

class XPFile
{
  public:
    typedef void(XP_DYNAMIC_FN_CALL* onFileCreatedFn)(XPFile* file);
    typedef void(XP_DYNAMIC_FN_CALL* onFileModifiedFn)(XPFile* file);
    typedef void(XP_DYNAMIC_FN_CALL* onFileDeletedFn)(XPFile* file);
    typedef void(XP_DYNAMIC_FN_CALL* onMeshAssetStageChangeFn)(XPFile* file, XPMeshAsset* meshAsset);
    typedef void(XP_DYNAMIC_FN_CALL* onMeshAssetCommitChangeFn)(XPFile* file, XPMeshAsset* meshAsset);
    typedef void(XP_DYNAMIC_FN_CALL* onShaderAssetStageChangeFn)(XPFile* file, XPShaderAsset* shaderAsset);
    typedef void(XP_DYNAMIC_FN_CALL* onShaderAssetCommitChangeFn)(XPFile* file, XPShaderAsset* shaderAsset);
    typedef void(XP_DYNAMIC_FN_CALL* onTextureAssetStageChangeFn)(XPFile* file, XPTextureAsset* textureAsset);
    typedef void(XP_DYNAMIC_FN_CALL* onTextureAssetCommitChangeFn)(XPFile* file, XPTextureAsset* textureAsset);
    typedef void(XP_DYNAMIC_FN_CALL* onRiscvBinaryAssetStageChangeFn)(XPFile*             file,
                                                                      XPRiscvBinaryAsset* riscvBinaryAsset);
    typedef void(XP_DYNAMIC_FN_CALL* onRiscvBinaryAssetCommitChangeFn)(XPFile*             file,
                                                                       XPRiscvBinaryAsset* riscvBinaryAsset);

    XPFile(XPDataPipelineStore* const store, const std::string& path, uint32_t id, XPEFileResourceType type);
    ~XPFile();

    [[nodiscard]] XPDataPipelineStore* getDataPipelineStore() const;
    [[nodiscard]] uint32_t             getId() const;
    [[nodiscard]] const std::string&   getPath() const;
    [[nodiscard]] time_t               getTimestamp() const;
    [[nodiscard]] XPEFileResourceType  getResourceType() const;
    [[nodiscard]] bool                 hasChanges() const;
    [[nodiscard]] XPScene*             getScene() const;

    void                                           load();
    void                                           reload();
    void                                           stageChanges();
    void                                           commitChanges();
    void                                           addMeshAsset(XPMeshAsset* meshAsset);
    void                                           addShaderAsset(XPShaderAsset* shaderAsset);
    void                                           addTextureAsset(XPTextureAsset* textureAsset);
    void                                           addRiscvBinaryAsset(XPRiscvBinaryAsset* riscvBinaryAsset);
    void                                           removeMeshAsset(XPMeshAsset* meshAsset);
    void                                           removeShaderAsset(XPShaderAsset* shaderAsset);
    void                                           removeTextureAsset(XPTextureAsset* textureAsset);
    void                                           removeRiscvBinaryAsset(XPRiscvBinaryAsset* riscvBinaryAsset);
    const std::unordered_set<XPMeshAsset*>&        getMeshAssets() const;
    const std::unordered_set<XPShaderAsset*>&      getShaderAssets() const;
    const std::unordered_set<XPTextureAsset*>&     getTextureAssets() const;
    const std::unordered_set<XPRiscvBinaryAsset*>& getRiscvBinaryAssets() const;

    static void setOnFileCreated(onFileCreatedFn onFileCreated);
    static void setOnFileModified(onFileModifiedFn onFileModified);
    static void setOnFileDeleted(onFileDeletedFn onFileDeleted);
    static void setOnMeshAssetStageChange(onMeshAssetStageChangeFn onMeshAssetStageChange);
    static void setOnMeshAssetCommitChange(onMeshAssetCommitChangeFn onMeshAssetCommitChange);
    static void setOnShaderAssetStageChange(onShaderAssetStageChangeFn onShaderAssetStageChange);
    static void setOnShaderAssetCommitChange(onShaderAssetCommitChangeFn onShaderAssetCommitChange);
    static void setOnTextureAssetStageChange(onTextureAssetStageChangeFn onTextureAssetStageChange);
    static void setOnTextureAssetCommitChange(onTextureAssetCommitChangeFn onTextureAssetCommitChange);
    static void setOnRiscvBinaryAssetStageChange(onRiscvBinaryAssetStageChangeFn onRiscvBinaryAssetStageChange);
    static void setOnRiscvBinaryAssetCommitChange(onRiscvBinaryAssetCommitChangeFn onRiscvBinaryAssetCommitChange);

    static void onFileCreated(XPFile* file);
    static void onFileModified(XPFile* file);
    static void onFileDeleted(XPFile* file);
    static void onMeshAssetStageChange(XPFile* file, XPMeshAsset* meshAsset);
    static void onMeshAssetCommitChange(XPFile* file, XPMeshAsset* meshAsset);
    static void onShaderAssetStageChange(XPFile* file, XPShaderAsset* shaderAsset);
    static void onShaderAssetCommitChange(XPFile* file, XPShaderAsset* shaderAsset);
    static void onTextureAssetStageChange(XPFile* file, XPTextureAsset* textureAsset);
    static void onTextureAssetCommitChange(XPFile* file, XPTextureAsset* textureAsset);
    static void onRiscvBinaryAssetStageChange(XPFile* file, XPRiscvBinaryAsset* riscvBinaryAsset);
    static void onRiscvBinaryAssetCommitChange(XPFile* file, XPRiscvBinaryAsset* riscvBinaryAsset);

    static const std::unordered_set<std::string>& getAvailableMeshFileFormats();
    static const std::unordered_set<std::string>& getAvailableShaderFileFormats();
    static const std::unordered_set<std::string>& getAvailableTextureFileFormats();
    static const std::unordered_set<std::string>& getAvailableRiscvBinaryFileFormats();

    static bool isMeshFile(std::filesystem::path p);
    static bool isShaderFile(std::filesystem::path p);
    static bool isTextureFile(std::filesystem::path p);
    static bool isRiscvBinaryFile(std::filesystem::path p);

  private:
    void loadMeshFromMemory();
    void loadMeshFromDisk();
    void loadShaderFromDisk();
    void loadTextureFromDisk();
    void loadSceneFromDisk();
    void loadRiscvBinaryFromDisk();
    void reloadMeshFromMemory();
    void reloadMeshFromDisk();
    void reloadShaderFromDisk();
    void reloadTextureFromDisk();
    void reloadSceneFromDisk();
    void reloadRiscvBinaryFromDisk();

    static onFileCreatedFn                  _onFileCreated;
    static onFileModifiedFn                 _onFileModified;
    static onFileDeletedFn                  _onFileDeleted;
    static onMeshAssetStageChangeFn         _onMeshAssetStage;
    static onMeshAssetCommitChangeFn        _onMeshAssetCommit;
    static onShaderAssetStageChangeFn       _onShaderAssetStage;
    static onShaderAssetCommitChangeFn      _onShaderAssetCommit;
    static onTextureAssetStageChangeFn      _onTextureAssetStage;
    static onTextureAssetCommitChangeFn     _onTextureAssetCommit;
    static onRiscvBinaryAssetStageChangeFn  _onRiscvBinaryAssetStage;
    static onRiscvBinaryAssetCommitChangeFn _onRiscvBinaryAssetCommit;

    XPDataPipelineStore* const              _dataPipelineStore;
    const uint32_t                          _id;
    std::string                             _path;
    XPEFileResourceType                     _type;
    time_t                                  _timestamp;
    bool                                    _hasChanges;
    std::unordered_set<XPMeshAsset*>        _meshAssets;
    std::unordered_set<XPShaderAsset*>      _shaderAssets;
    std::unordered_set<XPTextureAsset*>     _textureAssets;
    std::unordered_set<XPRiscvBinaryAsset*> _riscvBinaryAssets;
    XPScene*                                _scene;
};
