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
#include <Engine/XPRegistry.h>
#include <Utilities/XPMemoryPool.h>

#include <optional>
#include <unordered_map>
#include <vector>

class XPFile;
class XPMeshBuffer;
class XPShaderBuffer;
class XPTextureBuffer;
class XPMaterialBuffer;
class XPRiscvBinaryBuffer;
struct XPPreloadedMesh;
class XPRegistry;
class XPMeshAsset;
class XPShaderAsset;
class XPTextureAsset;
class XPMaterialAsset;
class XPRiscvBinaryAsset;

class XPDataPipelineStore
{
  public:
    XPDataPipelineStore(XPRegistry* const registry);
    ~XPDataPipelineStore();

    std::optional<XPFile*>              createFile(const std::string path, XPEFileResourceType type);
    std::optional<XPMeshAsset*>         createMeshAsset(XPFile* file);
    std::optional<XPShaderAsset*>       createShaderAsset(XPFile* file);
    std::optional<XPTextureAsset*>      createTextureAsset(XPFile* file);
    std::optional<XPMaterialAsset*>     createMaterialAsset(const std::string name);
    std::optional<XPRiscvBinaryAsset*>  createRiscvBinaryAsset(XPFile* file);
    std::optional<XPMeshBuffer*>        createMeshBuffer(XPMeshAsset* meshAsset);
    std::optional<XPShaderBuffer*>      createShaderBuffer(XPShaderAsset* shaderAsset);
    std::optional<XPTextureBuffer*>     createTextureBuffer(XPTextureAsset* textureAsset);
    std::optional<XPMaterialBuffer*>    createMaterialBuffer(XPMaterialAsset* materialAsset);
    std::optional<XPRiscvBinaryBuffer*> createRiscvBinaryBuffer(XPRiscvBinaryAsset* riscvBinaryAsset);

    [[nodiscard]] XPRegistry*                        getRegistry() const;
    [[nodiscard]] std::optional<XPFile*>             getFile(const std::string path, XPEFileResourceType type);
    [[nodiscard]] std::optional<XPMeshAsset*>        getMeshAsset(XPFile* file) const;
    [[nodiscard]] std::optional<XPShaderAsset*>      getShaderAsset(XPFile* file) const;
    [[nodiscard]] std::optional<XPTextureAsset*>     getTextureAsset(XPFile* file) const;
    [[nodiscard]] std::optional<XPMaterialAsset*>    getMaterialAsset(const std::string name) const;
    [[nodiscard]] std::optional<XPRiscvBinaryAsset*> getRiscvBinaryAsset(XPFile* file) const;

    [[nodiscard]] const std::unordered_map<XPEFileResourceType, std::unordered_map<std::string, XPFile*>>& getFiles()
      const;
    [[nodiscard]] const std::unordered_map<XPFile*, XPMeshAsset*>&                     getMeshAssets() const;
    [[nodiscard]] const std::unordered_map<XPFile*, XPShaderAsset*>&                   getShaderAssets() const;
    [[nodiscard]] const std::unordered_map<XPFile*, XPTextureAsset*>&                  getTextureAssets() const;
    [[nodiscard]] const std::unordered_map<std::string, XPMaterialAsset*>&             getMaterialAssets() const;
    [[nodiscard]] const std::unordered_map<XPFile*, XPRiscvBinaryAsset*>&              getRiscvBinaryAssets() const;
    [[nodiscard]] const std::unordered_map<XPMeshAsset*, XPMeshBuffer*>&               getMeshBuffers() const;
    [[nodiscard]] const std::unordered_map<XPShaderAsset*, XPShaderBuffer*>&           getShaderBuffers() const;
    [[nodiscard]] const std::unordered_map<XPTextureAsset*, XPTextureBuffer*>&         getTextureBuffers() const;
    [[nodiscard]] const std::unordered_map<XPMaterialAsset*, XPMaterialBuffer*>&       getMaterialBuffers() const;
    [[nodiscard]] const std::unordered_map<XPRiscvBinaryAsset*, XPRiscvBinaryBuffer*>& getRiscvBinaryBuffers() const;

    void destroyFile(const std::string path, XPEFileResourceType type);
    void destroyFile(XPFile* file);
    void destroyMeshAsset(XPMeshAsset* meshAsset);
    void destroyShaderAsset(XPShaderAsset* shaderAsset);
    void destroyTextureAsset(XPTextureAsset* textureAsset);
    void destroyMaterialAsset(XPMaterialAsset* materialAsset);
    void destroyRiscvBinaryAsset(XPRiscvBinaryAsset* riscvBinaryAsset);
    void destroyMeshBuffer(XPMeshBuffer* meshBuffer);
    void destroyShaderBuffer(XPShaderBuffer* shaderBuffer);
    void destroyTextureBuffer(XPTextureBuffer* textureBuffer);
    void destroyMaterialBuffer(XPMaterialBuffer* materialBuffer);
    void destroyRiscvBinaryBuffer(XPRiscvBinaryBuffer* riscvBinaryBuffer);

    bool isHasFilesNeedsReload() const;
    void setFilesNeedReload();
    void clearFilesNeedReload();

  private:
    XPRegistry* const _registry = nullptr;

    uint32_t _nextFileId;
    uint32_t _nextMeshAssetId;
    uint32_t _nextShaderAssetId;
    uint32_t _nextTextureAssetId;
    uint32_t _nextMaterialAssetId;
    uint32_t _nextRiscvBinaryAssetId;
    uint32_t _nextMeshBufferId;
    uint32_t _nextShaderBufferId;
    uint32_t _nextTextureBufferId;
    uint32_t _nextMaterialBufferId;
    uint32_t _nextRiscvBinaryBufferId;

    std::unordered_map<XPEFileResourceType, std::unordered_map<std::string, XPFile*>> _files;
    std::unordered_map<XPMeshAsset*, XPMeshBuffer*>                                   _meshBuffers;
    std::unordered_map<XPShaderAsset*, XPShaderBuffer*>                               _shaderBuffers;
    std::unordered_map<XPTextureAsset*, XPTextureBuffer*>                             _textureBuffers;
    std::unordered_map<XPMaterialAsset*, XPMaterialBuffer*>                           _materialBuffers;
    std::unordered_map<XPRiscvBinaryAsset*, XPRiscvBinaryBuffer*>                     _riscvBinaryBuffers;
    std::unordered_map<XPFile*, XPMeshAsset*>                                         _meshAssets;
    std::unordered_map<XPFile*, XPShaderAsset*>                                       _shaderAssets;
    std::unordered_map<XPFile*, XPTextureAsset*>                                      _textureAssets;
    std::unordered_map<std::string, XPMaterialAsset*>                                 _materialAssets;
    std::unordered_map<XPFile*, XPRiscvBinaryAsset*>                                  _riscvBinaryAssets;

    XPMemoryPool<XPFile>*              _filesPool;
    XPMemoryPool<XPMeshBuffer>*        _meshBuffersPool;
    XPMemoryPool<XPShaderBuffer>*      _shaderBuffersPool;
    XPMemoryPool<XPTextureBuffer>*     _textureBuffersPool;
    XPMemoryPool<XPMaterialBuffer>*    _materialBuffersPool;
    XPMemoryPool<XPRiscvBinaryBuffer>* _riscvBinaryBuffersPool;
    XPMemoryPool<XPMeshAsset>*         _meshAssetsPool;
    XPMemoryPool<XPShaderAsset>*       _shaderAssetsPool;
    XPMemoryPool<XPTextureAsset>*      _textureAssetsPool;
    XPMemoryPool<XPMaterialAsset>*     _materialAssetsPool;
    XPMemoryPool<XPRiscvBinaryAsset>*  _riscvBinaryAssetsPool;

    bool _hasFilesNeedsReload;
};
