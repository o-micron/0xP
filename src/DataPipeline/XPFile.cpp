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

#include <DataPipeline/XPAssimpModelLoader.h>
#include <DataPipeline/XPDataPipelineStore.h>
#include <DataPipeline/XPFile.h>
#include <DataPipeline/XPMeshAsset.h>
#include <DataPipeline/XPPreloadedAssets.h>
#include <DataPipeline/XPShaderAsset.h>
#include <DataPipeline/XPShaderLoader.h>
#include <DataPipeline/XPStbTextureLoader.h>
#include <DataPipeline/XPTextureAsset.h>
#include <Physics/Interface/XPIPhysics.h>
#include <Renderer/Interface/XPIRenderer.h>
#include <SceneDescriptor/XPScene.h>
#include <SceneDescriptor/XPSceneDescriptorStore.h>
#include <Utilities/XPFS.h>

XPFile::onFileCreatedFn                  XPFile::_onFileCreated            = nullptr;
XPFile::onFileModifiedFn                 XPFile::_onFileModified           = nullptr;
XPFile::onFileDeletedFn                  XPFile::_onFileDeleted            = nullptr;
XPFile::onMeshAssetStageChangeFn         XPFile::_onMeshAssetStage         = nullptr;
XPFile::onMeshAssetCommitChangeFn        XPFile::_onMeshAssetCommit        = nullptr;
XPFile::onShaderAssetStageChangeFn       XPFile::_onShaderAssetStage       = nullptr;
XPFile::onShaderAssetCommitChangeFn      XPFile::_onShaderAssetCommit      = nullptr;
XPFile::onTextureAssetStageChangeFn      XPFile::_onTextureAssetStage      = nullptr;
XPFile::onTextureAssetCommitChangeFn     XPFile::_onTextureAssetCommit     = nullptr;
XPFile::onRiscvBinaryAssetStageChangeFn  XPFile::_onRiscvBinaryAssetStage  = nullptr;
XPFile::onRiscvBinaryAssetCommitChangeFn XPFile::_onRiscvBinaryAssetCommit = nullptr;

static int64_t FileRefCounter = 0;

XPFile::XPFile(XPDataPipelineStore* const dataPipeline, const std::string& path, uint32_t id, XPEFileResourceType type)
  : _dataPipelineStore(dataPipeline)
  , _id(id)
  , _path(path)
  , _type(type)
  , _hasChanges(false)
  , _scene(nullptr)
{
    ++FileRefCounter;
    if (_onFileCreated) {
        switch (_type) {
            case XPEFileResourceType::PreloadedMesh:
            case XPEFileResourceType::Mesh: {
                auto optMeshAsset = _dataPipelineStore->createMeshAsset(this);
                if (optMeshAsset.has_value()) {
                    _meshAssets.insert(optMeshAsset.value());
                    load();
                }
                _onFileCreated(this);
            } break;
            case XPEFileResourceType::Shader: {
                auto optShaderAsset = _dataPipelineStore->createShaderAsset(this);
                if (optShaderAsset.has_value()) {
                    _shaderAssets.insert(optShaderAsset.value());
                    load();
                }
                _onFileCreated(this);
            } break;
            case XPEFileResourceType::Texture: {
                auto optTextureAsset = _dataPipelineStore->createTextureAsset(this);
                if (optTextureAsset.has_value()) {
                    _textureAssets.insert(optTextureAsset.value());
                    load();
                }
                _onFileCreated(this);
            } break;
            case XPEFileResourceType::Scene: {
                auto registry = _dataPipelineStore->getRegistry();
                auto optScene = registry->getScene()->getSceneDescriptorStore()->createScene(_path);
                if (optScene.has_value()) {
                    _scene = optScene.value();
                    _dataPipelineStore->getRegistry()->setSceneBuffered(_scene);
                    auto optMeshAsset = _dataPipelineStore->createMeshAsset(this);
                    if (optMeshAsset.has_value()) {
                        _meshAssets.insert(optMeshAsset.value());
                        load();
                    }
                    _onFileCreated(this);
                } else {
                    XP_LOG(XPLoggerSeverityFatal, "Failing to create a new scene");
                }
            } break;
            case XPEFileResourceType::RiscvBinary: {
                auto optRiscvBinaryAsset = _dataPipelineStore->createRiscvBinaryAsset(this);
                if (optRiscvBinaryAsset.has_value()) {
                    _riscvBinaryAssets.insert(optRiscvBinaryAsset.value());
                    load();
                }
                _onFileCreated(this);
            } break;

            case XPEFileResourceType::Plugin:
            case XPEFileResourceType::Unknown:
            case XPEFileResourceType::Count: break;
        }
    }
}

XPFile::~XPFile()
{
    if (_onFileDeleted) { _onFileDeleted(this); }
    --FileRefCounter;
    std::cout << "FileRefCounter: " << FileRefCounter << "\n";
}

XPDataPipelineStore*
XPFile::getDataPipelineStore() const
{
    return _dataPipelineStore;
}

uint32_t
XPFile::getId() const
{
    return _id;
}

const std::string&
XPFile::getPath() const
{
    return _path;
}

time_t
XPFile::getTimestamp() const
{
    return _timestamp;
}

XPEFileResourceType
XPFile::getResourceType() const
{
    return _type;
}

bool
XPFile::hasChanges() const
{
    return _hasChanges;
}

XPScene*
XPFile::getScene() const
{
    return _scene;
}

void
XPFile::load()
{
    switch (_type) {
        case XPEFileResourceType::Unknown:
        case XPEFileResourceType::Count: break;
        case XPEFileResourceType::PreloadedMesh: loadMeshFromMemory(); break;
        case XPEFileResourceType::Mesh: loadMeshFromDisk(); break;
        case XPEFileResourceType::Shader: loadShaderFromDisk(); break;
        case XPEFileResourceType::Texture: loadTextureFromDisk(); break;
        case XPEFileResourceType::Plugin: break;
        case XPEFileResourceType::Scene: loadSceneFromDisk(); break;
        case XPEFileResourceType::RiscvBinary: break;
    }
}

void
XPFile::reload()
{
    switch (_type) {
        case XPEFileResourceType::Unknown:
        case XPEFileResourceType::Count: break;

        case XPEFileResourceType::PreloadedMesh: break;
        case XPEFileResourceType::Mesh: reloadMeshFromDisk(); break;
        case XPEFileResourceType::Shader: reloadShaderFromDisk(); break;
        case XPEFileResourceType::Texture: reloadTextureFromDisk(); break;
        case XPEFileResourceType::Plugin: break;
        case XPEFileResourceType::Scene: reloadSceneFromDisk(); break;
        case XPEFileResourceType::RiscvBinary: break;
    }
}

void
XPFile::stageChanges()
{
    _timestamp  = getTimestamp();
    _hasChanges = true;
    if (_onMeshAssetStage) {
        for (XPMeshAsset* meshAsset : _meshAssets) { _onMeshAssetStage(this, meshAsset); }
    }
    if (_onShaderAssetStage) {
        for (XPShaderAsset* shaderAsset : _shaderAssets) { _onShaderAssetStage(this, shaderAsset); }
    }
    if (_onTextureAssetStage) {
        for (XPTextureAsset* textureAsset : _textureAssets) { _onTextureAssetStage(this, textureAsset); }
    }
    _dataPipelineStore->setFilesNeedReload();
}

void
XPFile::commitChanges()
{
    if (_hasChanges) {
        if (_onMeshAssetCommit) {
            for (XPMeshAsset* meshAsset : _meshAssets) { _onMeshAssetCommit(this, meshAsset); }
        }
        if (_onShaderAssetCommit) {
            for (XPShaderAsset* shaderAsset : _shaderAssets) { _onShaderAssetCommit(this, shaderAsset); }
        }
        if (_onTextureAssetCommit) {
            for (XPTextureAsset* textureAsset : _textureAssets) { _onTextureAssetCommit(this, textureAsset); }
        }
        _hasChanges = false;
    }
}

void
XPFile::addMeshAsset(XPMeshAsset* meshAsset)
{
    if (_meshAssets.find(meshAsset) == _meshAssets.end()) { _meshAssets.insert(meshAsset); }
}

void
XPFile::addShaderAsset(XPShaderAsset* shaderAsset)
{
    if (_shaderAssets.find(shaderAsset) == _shaderAssets.end()) { _shaderAssets.insert(shaderAsset); }
}

void
XPFile::addTextureAsset(XPTextureAsset* textureAsset)
{
    if (_textureAssets.find(textureAsset) == _textureAssets.end()) { _textureAssets.insert(textureAsset); }
}

void
XPFile::addRiscvBinaryAsset(XPRiscvBinaryAsset* riscvBinaryAsset)
{
    if (_riscvBinaryAssets.find(riscvBinaryAsset) == _riscvBinaryAssets.end()) {
        _riscvBinaryAssets.insert(riscvBinaryAsset);
    }
}

void
XPFile::removeMeshAsset(XPMeshAsset* meshAsset)
{
    _meshAssets.erase(meshAsset);
}

void
XPFile::removeShaderAsset(XPShaderAsset* shaderAsset)
{
    _shaderAssets.erase(shaderAsset);
}

void
XPFile::removeTextureAsset(XPTextureAsset* textureAsset)
{
    _textureAssets.erase(textureAsset);
}

void
XPFile::removeRiscvBinaryAsset(XPRiscvBinaryAsset* riscvBinaryAsset)
{
    _riscvBinaryAssets.erase(riscvBinaryAsset);
}

const std::unordered_set<XPMeshAsset*>&
XPFile::getMeshAssets() const
{
    return _meshAssets;
}

const std::unordered_set<XPShaderAsset*>&
XPFile::getShaderAssets() const
{
    return _shaderAssets;
}

const std::unordered_set<XPTextureAsset*>&
XPFile::getTextureAssets() const
{
    return _textureAssets;
}

const std::unordered_set<XPRiscvBinaryAsset*>&
XPFile::getRiscvBinaryAssets() const
{
    return _riscvBinaryAssets;
}

void
XPFile::setOnFileCreated(onFileCreatedFn onFileCreated)
{
    _onFileCreated = onFileCreated;
}

void
XPFile::setOnFileModified(onFileModifiedFn onFileModified)
{
    _onFileModified = onFileModified;
}

void
XPFile::setOnFileDeleted(onFileDeletedFn onFileDeleted)
{
    _onFileDeleted = onFileDeleted;
}

void
XPFile::setOnMeshAssetStageChange(onMeshAssetStageChangeFn onMeshAssetStageChange)
{
    _onMeshAssetStage = onMeshAssetStageChange;
}

void
XPFile::setOnMeshAssetCommitChange(onMeshAssetCommitChangeFn onMeshAssetCommitChange)
{
    _onMeshAssetCommit = onMeshAssetCommitChange;
}

void
XPFile::setOnShaderAssetStageChange(onShaderAssetStageChangeFn onShaderAssetStageChange)
{
    _onShaderAssetStage = onShaderAssetStageChange;
}

void
XPFile::setOnShaderAssetCommitChange(onShaderAssetCommitChangeFn onShaderAssetCommitChange)
{
    _onShaderAssetCommit = onShaderAssetCommitChange;
}

void
XPFile::setOnTextureAssetStageChange(onTextureAssetStageChangeFn onTextureAssetStageChange)
{
    _onTextureAssetStage = onTextureAssetStageChange;
}

void
XPFile::setOnTextureAssetCommitChange(onTextureAssetCommitChangeFn onTextureAssetCommitChange)
{
    _onTextureAssetCommit = onTextureAssetCommitChange;
}

void
XPFile::setOnRiscvBinaryAssetStageChange(onRiscvBinaryAssetStageChangeFn onRiscvBinaryAssetStageChange)
{
    _onRiscvBinaryAssetStage = onRiscvBinaryAssetStageChange;
}

void
XPFile::setOnRiscvBinaryAssetCommitChange(onRiscvBinaryAssetCommitChangeFn onRiscvBinaryAssetCommitChange)
{
    _onRiscvBinaryAssetCommit = onRiscvBinaryAssetCommitChange;
}

void
XPFile::onFileCreated(XPFile* file)
{
    if (_onFileCreated) { _onFileCreated(file); }
}

void
XPFile::onFileModified(XPFile* file)
{
    if (_onFileModified) { _onFileModified(file); }
}

void
XPFile::onFileDeleted(XPFile* file)
{
    if (_onFileDeleted) { _onFileDeleted(file); }
}

void
XPFile::onMeshAssetStageChange(XPFile* file, XPMeshAsset* meshAsset)
{
    if (_onMeshAssetStage) { _onMeshAssetStage(file, meshAsset); }
}

void
XPFile::onMeshAssetCommitChange(XPFile* file, XPMeshAsset* meshAsset)
{
    if (_onMeshAssetCommit) { _onMeshAssetCommit(file, meshAsset); }
}

void
XPFile::onShaderAssetStageChange(XPFile* file, XPShaderAsset* shaderAsset)
{
    if (_onShaderAssetStage) { _onShaderAssetStage(file, shaderAsset); }
}

void
XPFile::onShaderAssetCommitChange(XPFile* file, XPShaderAsset* shaderAsset)
{
    if (_onShaderAssetCommit) { _onShaderAssetCommit(file, shaderAsset); }
}

void
XPFile::onTextureAssetStageChange(XPFile* file, XPTextureAsset* textureAsset)
{
    if (_onTextureAssetStage) { _onTextureAssetStage(file, textureAsset); }
}

void
XPFile::onTextureAssetCommitChange(XPFile* file, XPTextureAsset* textureAsset)
{
    if (_onTextureAssetCommit) { _onTextureAssetCommit(file, textureAsset); }
}

void
XPFile::onRiscvBinaryAssetStageChange(XPFile* file, XPRiscvBinaryAsset* riscvBinaryAsset)
{
    if (_onRiscvBinaryAssetStage) { _onRiscvBinaryAssetStage(file, riscvBinaryAsset); }
}

void
XPFile::onRiscvBinaryAssetCommitChange(XPFile* file, XPRiscvBinaryAsset* riscvBinaryAsset)
{
    if (_onRiscvBinaryAssetCommit) { _onRiscvBinaryAssetCommit(file, riscvBinaryAsset); }
}

const std::unordered_set<std::string>&
XPFile::getAvailableMeshFileFormats()
{
    static const std::unordered_set<std::string> available = { ".obj", ".gltf", ".fbx", ".dae", ".glb" };
    return available;
}

const std::unordered_set<std::string>&
XPFile::getAvailableShaderFileFormats()
{
    static const std::unordered_set<std::string> available = {
#if defined(XP_RENDERER_METAL)
        ".metal"
#elif defined(XP_RENDERER_VULKAN)
        ".compute.spv", ".surface.spv"
#endif
    };
    return available;
}

const std::unordered_set<std::string>&
XPFile::getAvailableTextureFileFormats()
{
    static const std::unordered_set<std::string> available = { ".jpg", ".png" };
    return available;
}

const std::unordered_set<std::string>&
XPFile::getAvailableRiscvBinaryFileFormats()
{
    static const std::unordered_set<std::string> available = { ".bin" };
    return available;
}

bool
XPFile::isMeshFile(std::filesystem::path p)
{
    const std::unordered_set<std::string>& validMeshExtensions = getAvailableMeshFileFormats();
    auto                                   extension           = p.extension().string();
    return validMeshExtensions.find(extension) != validMeshExtensions.end();
}

bool
XPFile::isShaderFile(std::filesystem::path p)
{
    const std::unordered_set<std::string>& validShaderExtensions = getAvailableShaderFileFormats();
    for (const auto& shaderExtension : validShaderExtensions) {
        if (p.string().ends_with(shaderExtension)) { return true; }
    }
    return false;
}

bool
XPFile::isTextureFile(std::filesystem::path p)
{
    const std::unordered_set<std::string>& validTextureExtensions = getAvailableTextureFileFormats();
    auto                                   extension              = p.extension().string();
    return validTextureExtensions.find(extension) != validTextureExtensions.end();
}

bool
XPFile::isRiscvBinaryFile(std::filesystem::path p)
{
    const std::unordered_set<std::string>& validRiscvBinaryExtensions = getAvailableRiscvBinaryFileFormats();
    auto                                   extension                  = p.extension().string();
    return validRiscvBinaryExtensions.find(extension) != validRiscvBinaryExtensions.end();
}

void
XPFile::loadMeshFromMemory()
{
    if (_type == XPEFileResourceType::PreloadedMesh && !_meshAssets.empty()) {
        auto optMeshAsset = _dataPipelineStore->getMeshAsset(this);
        if (!optMeshAsset.has_value()) {
            optMeshAsset = _dataPipelineStore->createMeshAsset(this);
            if (!optMeshAsset.has_value()) {
                XP_LOG(XPLoggerSeverityFatal, "Couldn't create a new mesh asset");
                return;
            }
        }

        XPMeshAsset* meshAsset = optMeshAsset.value();

        auto optMeshBuffer = _dataPipelineStore->createMeshBuffer(meshAsset);
        if (!optMeshBuffer.has_value()) {
            XP_LOG(XPLoggerSeverityFatal, "Couldn't create a new mesh buffer");
            return;
        }

        XPMeshBuffer* meshBuffer = optMeshBuffer.value();
        meshAsset->setMeshBuffer(meshBuffer);

        const auto& preloadedMesh = XPPreloadedAssets::meshes().find(_path)->second;

        meshBuffer->deallocateResources();
        meshBuffer->setPositionsCount(preloadedMesh.positions.size());
        meshBuffer->setNormalsCount(preloadedMesh.normals.size());
        meshBuffer->setTexcoordsCount(preloadedMesh.texcoords.size());
        meshBuffer->setIndicesCount(preloadedMesh.indices.size());
        meshBuffer->setObjectsCount(1);
        meshBuffer->allocateForResources();

        memcpy(&meshBuffer->positionAtIndex(0).x,
               &preloadedMesh.positions[0].x,
               preloadedMesh.positions.size() * XPMeshBuffer::sizeofPositionsType());
        memcpy(&meshBuffer->normalAtIndex(0).x,
               &preloadedMesh.normals[0].x,
               preloadedMesh.normals.size() * XPMeshBuffer::sizeofNormalsType());
        memcpy(&meshBuffer->texcoordAtIndex(0).x,
               &preloadedMesh.texcoords[0].x,
               preloadedMesh.texcoords.size() * XPMeshBuffer::sizeofTexcoordsType());
        memcpy(&meshBuffer->indexAtIndex(0),
               &preloadedMesh.indices[0],
               preloadedMesh.indices.size() * XPMeshBuffer::sizeofIndicesType());

        XPMeshBufferObject& meshBufferObject = meshBuffer->objectAtIndex(0);
        meshBufferObject.name                = std::string(_path);
        meshBufferObject.meshBuffer          = meshBuffer;
        meshBufferObject.materialBuffer      = nullptr;
        meshBufferObject.vertexOffset        = 0;
        meshBufferObject.indexOffset         = 0;
        meshBufferObject.numIndices          = static_cast<uint32_t>(preloadedMesh.indices.size());
        meshBufferObject.boundingBox         = preloadedMesh.boundingBox;

        _dataPipelineStore->getRegistry()->getRenderer()->beginUploadMeshAssets();
        _dataPipelineStore->getRegistry()->getPhysics()->beginUploadMeshAssets();
        for (auto& meshAsset : _meshAssets) {
            _dataPipelineStore->getRegistry()->getRenderer()->uploadMeshAsset(meshAsset);
            _dataPipelineStore->getRegistry()->getPhysics()->uploadMeshAsset(meshAsset);
        }
        _dataPipelineStore->getRegistry()->getRenderer()->endUploadMeshAssets();
        _dataPipelineStore->getRegistry()->getPhysics()->endUploadMeshAssets();
    }
}

void
XPFile::loadMeshFromDisk()
{
    if (_type == XPEFileResourceType::Mesh && !_meshAssets.empty()) {
        XPMeshAsset* meshAsset = *_meshAssets.begin();
        if (meshAsset->getMeshBuffer() == nullptr) {
            auto optMeshBuffer = _dataPipelineStore->createMeshBuffer(meshAsset);
            if (optMeshBuffer.has_value()) {
                meshAsset->setMeshBuffer(optMeshBuffer.value());
                XPAssimpModelLoader::loadModel(meshAsset, *_dataPipelineStore);
                _dataPipelineStore->getRegistry()->getRenderer()->beginUploadMeshAssets();
                _dataPipelineStore->getRegistry()->getPhysics()->beginUploadMeshAssets();
                for (auto& meshAsset : _meshAssets) {
                    _dataPipelineStore->getRegistry()->getRenderer()->uploadMeshAsset(meshAsset);
                    _dataPipelineStore->getRegistry()->getPhysics()->uploadMeshAsset(meshAsset);
                }
                _dataPipelineStore->getRegistry()->getRenderer()->endUploadMeshAssets();
                _dataPipelineStore->getRegistry()->getPhysics()->endUploadMeshAssets();
            }
        }
    }
}

void
XPFile::loadShaderFromDisk()
{
    if (_type == XPEFileResourceType::Shader && !_shaderAssets.empty()) {
        _dataPipelineStore->getRegistry()->getRenderer()->beginUploadShaderAssets();
        for (auto& shaderAsset : _shaderAssets) {
            auto optShaderBuffer = _dataPipelineStore->createShaderBuffer(shaderAsset);
            if (optShaderBuffer.has_value()) {
                XPShaderBuffer* shaderBuffer = optShaderBuffer.value();
                shaderAsset->setShaderBuffer(shaderBuffer);
                XPShaderLoader::loadShader(shaderAsset, *_dataPipelineStore);
                _dataPipelineStore->getRegistry()->getRenderer()->uploadShaderAsset(shaderAsset);
            }
        }
        _dataPipelineStore->getRegistry()->getRenderer()->endUploadShaderAssets();
    }
}

void
XPFile::loadTextureFromDisk()
{
    if (_type == XPEFileResourceType::Texture && !_textureAssets.empty()) {
        XPTextureAsset* textureAsset = *_textureAssets.begin();
        if (textureAsset->getTextureBuffer() == nullptr) {
            auto optTextureBuffer = _dataPipelineStore->createTextureBuffer(textureAsset);
            if (optTextureBuffer.has_value()) {
                textureAsset->setTextureBuffer(optTextureBuffer.value());
                XPStbTextureLoader::load(textureAsset, *_dataPipelineStore);
                _dataPipelineStore->getRegistry()->getRenderer()->beginUploadTextureAssets();
                for (auto& textureAsset : _textureAssets) {
                    _dataPipelineStore->getRegistry()->getRenderer()->uploadTextureAsset(textureAsset);
                }
                _dataPipelineStore->getRegistry()->getRenderer()->endUploadTextureAssets();
            }
        }
    }
}

void
XPFile::loadSceneFromDisk()
{
    if (_type == XPEFileResourceType::Scene && !_meshAssets.empty()) {
        XPMeshAsset* meshAsset = *_meshAssets.begin();
        if (meshAsset->getMeshBuffer() == nullptr) {
            auto optMeshBuffer = _dataPipelineStore->createMeshBuffer(meshAsset);
            if (optMeshBuffer.has_value()) {
                meshAsset->setMeshBuffer(optMeshBuffer.value());
                XPAssimpModelLoader::loadModel(meshAsset, *_dataPipelineStore);
                XPAssimpModelLoader::loadScene(meshAsset, *_scene, *_dataPipelineStore);
                if (meshAsset->getMeshBuffer()->getPositionsCount() > 0 &&
                    meshAsset->getMeshBuffer()->getObjectsCount() > 0 &&
                    meshAsset->getMeshBuffer()->getIndicesCount() > 0) {
                    _dataPipelineStore->getRegistry()->getRenderer()->beginUploadMeshAssets();
                    _dataPipelineStore->getRegistry()->getPhysics()->beginUploadMeshAssets();

                    _dataPipelineStore->getRegistry()->getRenderer()->uploadMeshAsset(meshAsset);
                    _dataPipelineStore->getRegistry()->getPhysics()->uploadMeshAsset(meshAsset);

                    _dataPipelineStore->getRegistry()->getRenderer()->endUploadMeshAssets();
                    _dataPipelineStore->getRegistry()->getPhysics()->endUploadMeshAssets();
                } else {
                    for (auto& asset : _meshAssets) {
                        if (asset->getMeshBuffer()) { _dataPipelineStore->destroyMeshBuffer(asset->getMeshBuffer()); }
                        _dataPipelineStore->destroyMeshAsset(asset);
                    }
                    _dataPipelineStore->destroyFile(_path, _type);
                }
            }
        }
    }
}

void
XPFile::reloadMeshFromMemory()
{
    if (_type == XPEFileResourceType::PreloadedMesh && !_meshAssets.empty()) {
        auto optMeshAsset = _dataPipelineStore->getMeshAsset(this);
        if (!optMeshAsset.has_value()) {
            optMeshAsset = _dataPipelineStore->createMeshAsset(this);
            if (!optMeshAsset.has_value()) {
                XP_LOG(XPLoggerSeverityFatal, "Couldn't create a new mesh asset");
                return;
            }
        }

        XPMeshAsset*  meshAsset  = optMeshAsset.value();
        XPMeshBuffer* meshBuffer = meshAsset->getMeshBuffer();

        const auto& preloadedMesh = XPPreloadedAssets::meshes().find(_path)->second;

        meshBuffer->deallocateResources();
        meshBuffer->setPositionsCount(preloadedMesh.positions.size());
        meshBuffer->setNormalsCount(preloadedMesh.normals.size());
        meshBuffer->setTexcoordsCount(preloadedMesh.texcoords.size());
        meshBuffer->setIndicesCount(preloadedMesh.indices.size());
        meshBuffer->setObjectsCount(1);
        meshBuffer->allocateForResources();

        XPMeshBufferObject& meshBufferObject = meshBuffer->objectAtIndex(0);
        meshBufferObject.name                = _path;
        meshBufferObject.meshBuffer          = meshBuffer;
        meshBufferObject.materialBuffer      = nullptr;
        meshBufferObject.vertexOffset        = 0;
        meshBufferObject.indexOffset         = 0;
        meshBufferObject.numIndices          = static_cast<uint32_t>(preloadedMesh.indices.size());
        meshBufferObject.boundingBox         = preloadedMesh.boundingBox;

        memcpy(&meshBuffer->positionAtIndex(0).x,
               &preloadedMesh.positions[0].x,
               preloadedMesh.positions.size() * XPMeshBuffer::sizeofPositionsType());
        memcpy(&meshBuffer->normalAtIndex(0).x,
               &preloadedMesh.normals[0].x,
               preloadedMesh.normals.size() * XPMeshBuffer::sizeofNormalsType());
        memcpy(&meshBuffer->texcoordAtIndex(0).x,
               &preloadedMesh.texcoords[0].x,
               preloadedMesh.texcoords.size() * XPMeshBuffer::sizeofTexcoordsType());
        memcpy(&meshBuffer->indexAtIndex(0),
               &preloadedMesh.indices[0],
               preloadedMesh.indices.size() * XPMeshBuffer::sizeofIndicesType());

        _dataPipelineStore->getRegistry()->getRenderer()->beginReUploadMeshAssets();
        for (auto& meshAsset : _meshAssets) {
            _dataPipelineStore->getRegistry()->getRenderer()->reUploadMeshAsset(meshAsset);
        }
        _dataPipelineStore->getRegistry()->getRenderer()->endReUploadMeshAssets();
    }
}

void
XPFile::reloadMeshFromDisk()
{
    if (_type == XPEFileResourceType::Mesh && !_meshAssets.empty()) {
        XPAssimpModelLoader::loadModel(*_meshAssets.begin(), *_dataPipelineStore);
        _dataPipelineStore->getRegistry()->getRenderer()->beginReUploadMeshAssets();
        _dataPipelineStore->getRegistry()->getPhysics()->beginReUploadMeshAssets();
        for (auto& meshAsset : _meshAssets) {
            _dataPipelineStore->getRegistry()->getRenderer()->reUploadMeshAsset(meshAsset);
            _dataPipelineStore->getRegistry()->getPhysics()->reUploadMeshAsset(meshAsset);
        }
        _dataPipelineStore->getRegistry()->getRenderer()->endReUploadMeshAssets();
        _dataPipelineStore->getRegistry()->getPhysics()->endReUploadMeshAssets();
    }
}

void
XPFile::reloadShaderFromDisk()
{
    if (_type == XPEFileResourceType::Shader && !_shaderAssets.empty()) {
        _dataPipelineStore->getRegistry()->getRenderer()->beginReUploadShaderAssets();
        for (auto& shaderAsset : _shaderAssets) {
            XPShaderLoader::reloadShader(shaderAsset, *_dataPipelineStore);
            _dataPipelineStore->getRegistry()->getRenderer()->reUploadShaderAsset(shaderAsset);
        }
        _dataPipelineStore->getRegistry()->getRenderer()->endReUploadShaderAssets();
    }
}

void
XPFile::reloadTextureFromDisk()
{
    if (_type == XPEFileResourceType::Texture && !_textureAssets.empty()) {
        XPStbTextureLoader::load(*_textureAssets.begin(), *_dataPipelineStore);
        _dataPipelineStore->getRegistry()->getRenderer()->beginReUploadTextureAssets();
        for (auto& textureAsset : _textureAssets) {
            _dataPipelineStore->getRegistry()->getRenderer()->reUploadTextureAsset(textureAsset);
        }
        _dataPipelineStore->getRegistry()->getRenderer()->endReUploadTextureAssets();
    }
}

void
XPFile::reloadSceneFromDisk()
{
    if (_type == XPEFileResourceType::Scene && !_meshAssets.empty()) {
        _scene->destroyAllLayers();
        XPAssimpModelLoader::loadModel(*_meshAssets.begin(), *_dataPipelineStore);
        XPAssimpModelLoader::loadScene(*_meshAssets.begin(), *_scene, *_dataPipelineStore);
        for (auto& meshAsset : _meshAssets) {
            _dataPipelineStore->getRegistry()->getRenderer()->reUploadMeshAsset(meshAsset);
            _dataPipelineStore->getRegistry()->getPhysics()->reUploadMeshAsset(meshAsset);
        }
    }
}
