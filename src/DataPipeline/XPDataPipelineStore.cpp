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

#include <DataPipeline/XPDataPipelineStore.h>

#include <DataPipeline/XPFile.h>
#include <DataPipeline/XPMaterialAsset.h>
#include <DataPipeline/XPMaterialBuffer.h>
#include <DataPipeline/XPMeshAsset.h>
#include <DataPipeline/XPMeshBuffer.h>
#include <DataPipeline/XPPreloadedAssets.h>
#include <DataPipeline/XPRiscvBinaryAsset.h>
#include <DataPipeline/XPRiscvBinaryBuffer.h>
#include <DataPipeline/XPShaderAsset.h>
#include <DataPipeline/XPShaderBuffer.h>
#include <DataPipeline/XPTextureAsset.h>
#include <DataPipeline/XPTextureBuffer.h>

#include <optional>
#include <unordered_map>

XPDataPipelineStore::XPDataPipelineStore(XPRegistry* const registry)
  : _registry(registry)
  , _hasFilesNeedsReload(false)
{
    _filesPool              = XP_NEW              XPMemoryPool<XPFile>(32, 64);
    _meshAssetsPool         = XP_NEW         XPMemoryPool<XPMeshAsset>(32, 64);
    _shaderAssetsPool       = XP_NEW       XPMemoryPool<XPShaderAsset>(32, 64);
    _textureAssetsPool      = XP_NEW      XPMemoryPool<XPTextureAsset>(32, 64);
    _materialAssetsPool     = XP_NEW     XPMemoryPool<XPMaterialAsset>(32, 64);
    _riscvBinaryAssetsPool  = XP_NEW  XPMemoryPool<XPRiscvBinaryAsset>(32, 64);
    _meshBuffersPool        = XP_NEW        XPMemoryPool<XPMeshBuffer>(32, 64);
    _shaderBuffersPool      = XP_NEW      XPMemoryPool<XPShaderBuffer>(32, 64);
    _textureBuffersPool     = XP_NEW     XPMemoryPool<XPTextureBuffer>(32, 64);
    _materialBuffersPool    = XP_NEW    XPMemoryPool<XPMaterialBuffer>(32, 64);
    _riscvBinaryBuffersPool = XP_NEW XPMemoryPool<XPRiscvBinaryBuffer>(32, 64);

    _nextFileId              = 0;
    _nextMeshAssetId         = 0;
    _nextShaderAssetId       = 0;
    _nextTextureAssetId      = 0;
    _nextMaterialAssetId     = 0;
    _nextRiscvBinaryAssetId  = 0;
    _nextMeshBufferId        = 0;
    _nextShaderBufferId      = 0;
    _nextTextureBufferId     = 0;
    _nextRiscvBinaryBufferId = 0;
}

XPDataPipelineStore::~XPDataPipelineStore()
{
    delete _riscvBinaryAssetsPool;
    _riscvBinaryAssets.clear();
    delete _materialAssetsPool;
    _materialAssets.clear();
    delete _shaderAssetsPool;
    _shaderAssets.clear();
    delete _textureAssetsPool;
    _textureAssets.clear();
    delete _meshAssetsPool;
    _meshAssets.clear();
    delete _riscvBinaryBuffersPool;
    _riscvBinaryBuffers.clear();
    delete _materialBuffersPool;
    _materialBuffers.clear();
    delete _meshBuffersPool;
    _meshBuffers.clear();
    delete _shaderBuffersPool;
    _shaderBuffers.clear();
    delete _textureBuffersPool;
    _textureBuffers.clear();
    delete _filesPool;
    _files.clear();
}

std::optional<XPFile*>
XPDataPipelineStore::createFile(const std::string path, XPEFileResourceType type)
{
    if (_files[type].find(path) != _files[type].end()) { return std::nullopt; }
    XPFile* file       = _filesPool->create(this, path, ++_nextFileId, type);
    _files[type][path] = file;
    return file;
}

std::optional<XPMeshAsset*>
XPDataPipelineStore::createMeshAsset(XPFile* file)
{
    if (_meshAssets.find(file) != _meshAssets.end()) { return std::nullopt; }
    XPMeshAsset* meshAsset = _meshAssetsPool->create(file, ++_nextMeshAssetId);
    _meshAssets[file]      = meshAsset;
    return { meshAsset };
}

std::optional<XPShaderAsset*>
XPDataPipelineStore::createShaderAsset(XPFile* file)
{
    if (_meshAssets.find(file) != _meshAssets.end()) { return std::nullopt; }
    XPShaderAsset* shaderAsset = _shaderAssetsPool->create(file, ++_nextShaderAssetId);
    _shaderAssets[file]        = shaderAsset;
    return { shaderAsset };
}

std::optional<XPTextureAsset*>
XPDataPipelineStore::createTextureAsset(XPFile* file)
{
    if (_meshAssets.find(file) != _meshAssets.end()) { return std::nullopt; }
    XPTextureAsset* textureAsset = _textureAssetsPool->create(file, ++_nextTextureAssetId);
    _textureAssets[file]         = textureAsset;
    return { textureAsset };
}

std::optional<XPMaterialAsset*>
XPDataPipelineStore::createMaterialAsset(const std::string name)
{
    if (_materialAssets.find(name) != _materialAssets.end()) { return std::nullopt; }
    XPMaterialAsset* materialAsset = _materialAssetsPool->create(name, ++_nextMaterialAssetId);
    _materialAssets[name]          = materialAsset;
    return { materialAsset };
}

std::optional<XPRiscvBinaryAsset*>
XPDataPipelineStore::createRiscvBinaryAsset(XPFile* file)
{
    if (_riscvBinaryAssets.find(file) != _riscvBinaryAssets.end()) { return std::nullopt; }
    XPRiscvBinaryAsset* riscvBinaryAsset = _riscvBinaryAssetsPool->create(file, ++_nextRiscvBinaryAssetId);
    _riscvBinaryAssets[file]             = riscvBinaryAsset;
    return { riscvBinaryAsset };
}

std::optional<XPMeshBuffer*>
XPDataPipelineStore::createMeshBuffer(XPMeshAsset* meshAssset)
{
    if (_meshBuffers.find(meshAssset) != _meshBuffers.end()) { return std::nullopt; }
    XPMeshBuffer* meshBuffer = _meshBuffersPool->create(meshAssset, ++_nextMeshBufferId);
    _meshBuffers[meshAssset] = meshBuffer;
    return meshBuffer;
}

std::optional<XPShaderBuffer*>
XPDataPipelineStore::createShaderBuffer(XPShaderAsset* shaderAsset)
{
    if (_shaderBuffers.find(shaderAsset) != _shaderBuffers.end()) { return std::nullopt; }
    XPShaderBuffer* shaderBuffer = _shaderBuffersPool->create(shaderAsset, ++_nextShaderBufferId);
    _shaderBuffers[shaderAsset]  = shaderBuffer;
    return shaderBuffer;
}

std::optional<XPTextureBuffer*>
XPDataPipelineStore::createTextureBuffer(XPTextureAsset* textureAsset)
{
    if (_textureBuffers.find(textureAsset) != _textureBuffers.end()) { return std::nullopt; }
    XPTextureBuffer* textureBuffer = _textureBuffersPool->create(textureAsset, ++_nextTextureBufferId);
    _textureBuffers[textureAsset]  = textureBuffer;
    return textureBuffer;
}

std::optional<XPMaterialBuffer*>
XPDataPipelineStore::createMaterialBuffer(XPMaterialAsset* materialAsset)
{
    if (_materialBuffers.find(materialAsset) != _materialBuffers.end()) { return std::nullopt; }
    XPMaterialBuffer* materialBuffer = _materialBuffersPool->create(materialAsset, ++_nextMaterialBufferId);
    _materialBuffers[materialAsset]  = materialBuffer;
    return materialBuffer;
}

std::optional<XPRiscvBinaryBuffer*>
XPDataPipelineStore::createRiscvBinaryBuffer(XPRiscvBinaryAsset* riscvBinaryAsset)
{
    if (_riscvBinaryBuffers.find(riscvBinaryAsset) != _riscvBinaryBuffers.end()) { return std::nullopt; }
    XPRiscvBinaryBuffer* riscvBinaryBuffer =
      _riscvBinaryBuffersPool->create(riscvBinaryAsset, ++_nextRiscvBinaryBufferId);
    _riscvBinaryBuffers[riscvBinaryAsset] = riscvBinaryBuffer;
    return riscvBinaryBuffer;
}

XPRegistry*
XPDataPipelineStore::getRegistry() const
{
    return _registry;
}

std::optional<XPFile*>
XPDataPipelineStore::getFile(const std::string path, XPEFileResourceType type)
{
    const auto& typedFiles = _files[type];
    auto        it         = typedFiles.find(path);
    if (it != typedFiles.end()) { return { it->second }; }
    return std::nullopt;
}

std::optional<XPMeshAsset*>
XPDataPipelineStore::getMeshAsset(XPFile* file) const
{
    auto it = _meshAssets.find(file);
    if (it != _meshAssets.end()) { return { it->second }; }
    return std::nullopt;
}

std::optional<XPShaderAsset*>
XPDataPipelineStore::getShaderAsset(XPFile* file) const
{
    auto it = _shaderAssets.find(file);
    if (it != _shaderAssets.end()) { return { it->second }; }
    return std::nullopt;
}

std::optional<XPTextureAsset*>
XPDataPipelineStore::getTextureAsset(XPFile* file) const
{
    auto it = _textureAssets.find(file);
    if (it != _textureAssets.end()) { return { it->second }; }
    return std::nullopt;
}

std::optional<XPMaterialAsset*>
XPDataPipelineStore::getMaterialAsset(const std::string name) const
{
    auto it = _materialAssets.find(name);
    if (it != _materialAssets.end()) { return { it->second }; }
    return std::nullopt;
}

std::optional<XPRiscvBinaryAsset*>
XPDataPipelineStore::getRiscvBinaryAsset(XPFile* file) const
{
    auto it = _riscvBinaryAssets.find(file);
    if (it != _riscvBinaryAssets.end()) { return { it->second }; }
    return std::nullopt;
}

const std::unordered_map<XPEFileResourceType, std::unordered_map<std::string, XPFile*>>&
XPDataPipelineStore::getFiles() const
{
    return _files;
}

const std::unordered_map<XPMeshAsset*, XPMeshBuffer*>&
XPDataPipelineStore::getMeshBuffers() const
{
    return _meshBuffers;
}

const std::unordered_map<XPShaderAsset*, XPShaderBuffer*>&
XPDataPipelineStore::getShaderBuffers() const
{
    return _shaderBuffers;
}

const std::unordered_map<XPTextureAsset*, XPTextureBuffer*>&
XPDataPipelineStore::getTextureBuffers() const
{
    return _textureBuffers;
}

const std::unordered_map<XPMaterialAsset*, XPMaterialBuffer*>&
XPDataPipelineStore::getMaterialBuffers() const
{
    return _materialBuffers;
}

const std::unordered_map<XPRiscvBinaryAsset*, XPRiscvBinaryBuffer*>&
XPDataPipelineStore::getRiscvBinaryBuffers() const
{
    return _riscvBinaryBuffers;
}

const std::unordered_map<XPFile*, XPMeshAsset*>&
XPDataPipelineStore::getMeshAssets() const
{
    return _meshAssets;
}

const std::unordered_map<XPFile*, XPShaderAsset*>&
XPDataPipelineStore::getShaderAssets() const
{
    return _shaderAssets;
}

const std::unordered_map<XPFile*, XPTextureAsset*>&
XPDataPipelineStore::getTextureAssets() const
{
    return _textureAssets;
}

const std::unordered_map<std::string, XPMaterialAsset*>&
XPDataPipelineStore::getMaterialAssets() const
{
    return _materialAssets;
}

const std::unordered_map<XPFile*, XPRiscvBinaryAsset*>&
XPDataPipelineStore::getRiscvBinaryAssets() const
{
    return _riscvBinaryAssets;
}

void
XPDataPipelineStore::destroyFile(const std::string path, XPEFileResourceType type)
{
    auto& typedFiles = _files[type];
    auto  it         = typedFiles.find(path);
    if (it != typedFiles.end()) {
        typedFiles.erase(it);
        _filesPool->destroy(it->second);
    }
}

void
XPDataPipelineStore::destroyFile(XPFile* file)
{
    auto& typedFiles = _files[file->getResourceType()];
    auto  it         = typedFiles.find(file->getPath());
    if (it != typedFiles.end()) {
        typedFiles.erase(it);
        _filesPool->destroy(it->second);
    }
}

void
XPDataPipelineStore::destroyMeshAsset(XPMeshAsset* meshAsset)
{
    auto it = _meshAssets.find(meshAsset->getFile());
    if (it != _meshAssets.end()) {
        _meshAssets.erase(it);
        _meshAssetsPool->destroy(meshAsset);
    }
}

void
XPDataPipelineStore::destroyShaderAsset(XPShaderAsset* shaderAsset)
{
    auto it = _shaderAssets.find(shaderAsset->getFile());
    if (it != _shaderAssets.end()) {
        _shaderAssets.erase(it);
        _shaderAssetsPool->destroy(shaderAsset);
    }
}

void
XPDataPipelineStore::destroyTextureAsset(XPTextureAsset* textureAsset)
{
    auto it = _textureAssets.find(textureAsset->getFile());
    if (it != _textureAssets.end()) {
        _textureAssets.erase(it);
        _textureAssetsPool->destroy(textureAsset);
    }
}

void
XPDataPipelineStore::destroyMaterialAsset(XPMaterialAsset* materialAsset)
{
    auto it = _materialAssets.find(materialAsset->getName());
    if (it != _materialAssets.end()) {
        _materialAssets.erase(it);
        _materialAssetsPool->destroy(materialAsset);
    }
}

void
XPDataPipelineStore::destroyRiscvBinaryAsset(XPRiscvBinaryAsset* riscvBinaryAsset)
{
    auto it = _riscvBinaryAssets.find(riscvBinaryAsset->getFile());
    if (it != _riscvBinaryAssets.end()) {
        _riscvBinaryAssets.erase(it);
        _riscvBinaryAssetsPool->destroy(riscvBinaryAsset);
    }
}

void
XPDataPipelineStore::destroyMeshBuffer(XPMeshBuffer* meshBuffer)
{
    auto it = _meshBuffers.find(meshBuffer->getMeshAsset());
    if (it != _meshBuffers.end()) {
        _meshBuffers.erase(it);
        _meshBuffersPool->destroy(meshBuffer);
    }
}

void
XPDataPipelineStore::destroyShaderBuffer(XPShaderBuffer* shaderBuffer)
{
    auto it = _shaderBuffers.find(shaderBuffer->getShaderAsset());
    if (it != _shaderBuffers.end()) {
        _shaderBuffers.erase(it);
        _shaderBuffersPool->destroy(shaderBuffer);
    }
}

void
XPDataPipelineStore::destroyTextureBuffer(XPTextureBuffer* textureBuffer)
{
    auto it = _textureBuffers.find(textureBuffer->getTextureAsset());
    if (it != _textureBuffers.end()) {
        _textureBuffers.erase(it);
        _textureBuffersPool->destroy(textureBuffer);
    }
}

void
XPDataPipelineStore::destroyMaterialBuffer(XPMaterialBuffer* materialBuffer)
{
    auto it = _materialBuffers.find(materialBuffer->getMaterialAsset());
    if (it != _materialBuffers.end()) {
        _materialBuffers.erase(it);
        _materialBuffersPool->destroy(materialBuffer);
    }
}

void
XPDataPipelineStore::destroyRiscvBinaryBuffer(XPRiscvBinaryBuffer* riscvBinaryBuffer)
{
    auto it = _riscvBinaryBuffers.find(riscvBinaryBuffer->getRiscvBinaryAsset());
    if (it != _riscvBinaryBuffers.end()) {
        _riscvBinaryBuffers.erase(it);
        _riscvBinaryBuffersPool->destroy(riscvBinaryBuffer);
    }
}

bool
XPDataPipelineStore::isHasFilesNeedsReload() const
{
    return _hasFilesNeedsReload;
}

void
XPDataPipelineStore::setFilesNeedReload()
{
    _hasFilesNeedsReload = true;
}

void
XPDataPipelineStore::clearFilesNeedReload()
{
    _hasFilesNeedsReload = false;
}
