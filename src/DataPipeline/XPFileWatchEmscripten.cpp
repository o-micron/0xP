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

#include <DataPipeline/XPFile.h>
#include <DataPipeline/XPFileWatchEmscripten.h>
#include <DataPipeline/XPIFileWatch.h>
#include <DataPipeline/XPMeshAsset.h>
#include <DataPipeline/XPMeshBuffer.h>
#include <DataPipeline/XPPreloadedAssets.h>
#include <DataPipeline/XPShaderAsset.h>
#include <DataPipeline/XPShaderBuffer.h>
#include <DataPipeline/XPDataPipelineStore.h>
#include <DataPipeline/XPTextureAsset.h>
#include <DataPipeline/XPTextureBuffer.h>

#include <Utilities/XPFS.h>
#include <Utilities/XPLogger.h>

#include <sstream>
#include <thread>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Weverything"
#endif
#define FMT_HEADER_ONLY
#include <fmt/format.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

XPFileWatchEmscripten::XPFileWatchEmscripten(XPRegistry* const registry)
  : XPIFileWatch(registry)
  , _registry(registry)
{
}

XPFileWatchEmscripten::~XPFileWatchEmscripten() {}

void
XPFileWatchEmscripten::start(XPDPStore*         store,
                             const std::string& rootPath,
                             const std::string& meshesDirectory,
                             const std::string& scenesDirectory,
                             const std::string& shadersDirectory,
                             const std::string& texturesDirectory,
                             const std::string& prototypesDirectory,
                             const std::string& pluginsDirectory)
{
    _store          = store;
    _rootPath       = rootPath;
    _meshesPath     = meshesDirectory;
    _scenesPath     = scenesDirectory;
    _shadersPath    = shadersDirectory;
    _texturesPath   = texturesDirectory;
    _prototypesPath = prototypesDirectory;
    _pluginsPath    = pluginsDirectory;

    // get assets directories
    // if (!std::filesystem::exists(_meshesPath)) {
    //     XP_LOG(XPLoggerSeverityFatal, "Failed to find a valid meshes directory !");
    //     return;
    // }
    // if (!std::filesystem::exists(_shadersPath)) {
    //     XP_LOG(XPLoggerSeverityFatal, "Failed to find a valid shaders directory !");
    //     return;
    // }
    // if (!std::filesystem::exists(_texturesPath)) {
    //     XP_LOG(XPLoggerSeverityFatal, "Failed to find a valid textures directory !");
    //     return;
    // }

    // load preloaded assets
    // {
    //     _store->createFile("TRIANGLE", XPEFileResourceType::PreloadedMesh);
    //     _store->createFile("QUAD", XPEFileResourceType::PreloadedMesh);
    //     _store->createFile("CUBE", XPEFileResourceType::PreloadedMesh);
    // }

    // loop over the directories and load the scenes
    // for (const auto& sceneEntry : std::filesystem::directory_iterator(_scenesPath)) {
    //     if (!sceneEntry.is_regular_file()) { continue; }
    //     auto fullPath = sceneEntry.path().string();
    //     if (XPFile::isMeshFile(fullPath)) { _store->createFile(fullPath, XPEFileResourceType::Scene); }
    // }

    // loop over the directories and load the assets
    // for (const auto& meshEntry : std::filesystem::directory_iterator(_meshesPath)) {
    //     if (!meshEntry.is_regular_file()) { continue; }
    //     auto fullPath = meshEntry.path().string();
    //     if (XPFile::isMeshFile(fullPath)) { _store->createFile(fullPath, XPEFileResourceType::Mesh); }
    // }
    // for (const auto& shaderEntry : std::filesystem::directory_iterator(_shadersPath)) {
    //     if (!shaderEntry.is_regular_file()) { continue; }
    //     auto fullPath = shaderEntry.path().string();
    //     if (XPFile::isShaderFile(fullPath) && shaderEntry.path().extension() == ".metal") {
    //         _store->createFile(fullPath, XPEFileResourceType::Shader);
    //     }
    // }
    // for (const auto& textureEntry : std::filesystem::directory_iterator(_texturesPath)) {
    //     if (!textureEntry.is_regular_file()) { continue; }
    //     auto fullPath = textureEntry.path().string();
    //     if (XPFile::isTextureFile(fullPath)) { _store->createFile(fullPath, XPEFileResourceType::Texture); }
    // }
}

void
XPFileWatchEmscripten::stop()
{
}

XPDPStore&
XPFileWatchEmscripten::getStore() const
{
    return *_store;
}

const std::string&
XPFileWatchEmscripten::getRootPath() const
{
    return _rootPath;
}

const std::string&
XPFileWatchEmscripten::getMeshesPath() const
{
    return _meshesPath;
}

const std::string&
XPFileWatchEmscripten::getScenesPath() const
{
    return _scenesPath;
}

const std::string&
XPFileWatchEmscripten::getShadersPath() const
{
    return _shadersPath;
}

const std::string&
XPFileWatchEmscripten::getTexturesPath() const
{
    return _texturesPath;
}

const std::string&
XPFileWatchEmscripten::getPrototypesPath() const
{
    return _prototypesPath;
}

const std::string&
XPFileWatchEmscripten::getPluginsPath() const
{
    return _pluginsPath;
}

void
XPFileWatchEmscripten::refreshDirectories(std::vector<std::string> directories)
{
    if (directories.empty()) return;

    std::filesystem::path executableParentDirectory = XPFS::getExecutableDirectoryPath();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    XP_LOG(XPLoggerSeverityInfo, std::string(60, '=').c_str());
    XP_LOG(XPLoggerSeverityInfo, "CHANGED DIRECTORIES");
    XP_LOG(XPLoggerSeverityInfo, std::string(60, '=').c_str());

    std::filesystem::path const_assets_dir  = (executableParentDirectory / _rootPath).make_preferred();
    std::filesystem::path const_mesh_dir    = (executableParentDirectory / _rootPath / _meshesPath).make_preferred();
    std::filesystem::path const_scene_dir   = (executableParentDirectory / _rootPath / _scenesPath).make_preferred();
    std::filesystem::path const_shader_dir  = (executableParentDirectory / _rootPath / _shadersPath).make_preferred();
    std::filesystem::path const_texture_dir = (executableParentDirectory / _rootPath / _texturesPath).make_preferred();
    std::filesystem::path const_plugin_dir  = (executableParentDirectory / _rootPath / _pluginsPath).make_preferred();

    for (auto& directory : directories) {
        XP_LOG(XPLoggerSeverityInfo, fmt::format("Directory ({}) changed.", directory).c_str());
        std::filesystem::path directory_path = std::filesystem::path(directory).make_preferred();
        if (!XPFS::isRelativeParentDirectory(directory_path, const_assets_dir)) { continue; }
        if (XPFS::isRelativeParentDirectory(directory_path, const_mesh_dir) && XPFile::isMeshFile(directory_path)) {
            std::string meshFilename = directory_path.string().substr(const_mesh_dir.string().size());
            XP_LOG(XPLoggerSeverityInfo, fmt::format("MeshAsset <{}>", meshFilename).c_str());
            auto optFile = _store->getFile(directory_path, XPEFileResourceType::Scene);
            if (optFile.has_value()) {
                auto optMeshAsset = _store->getMeshAsset(optFile.value());
                if (optMeshAsset.has_value()) {
                    auto meshAsset = optMeshAsset.value();
                    if (XPFS::getTimestamp(meshAsset->getFile()->getPath()) != meshAsset->getFile()->getTimestamp()) {
                        XPFile::onFileModified(meshAsset->getFile());
                    }
                }
            } else {
                _store->createFile(directory_path, XPEFileResourceType::Scene);
            }
        } else if (XPFS::isRelativeParentDirectory(directory_path, const_scene_dir) &&
                   XPFile::isMeshFile(directory_path)) {
            std::string sceneFilename = directory_path.string().substr(const_scene_dir.string().size());
            XP_LOG(XPLoggerSeverityInfo, fmt::format("SceneAsset <{}>", sceneFilename).c_str());
            auto optFile = _store->getFile(directory_path, XPEFileResourceType::Scene);
            if (optFile.has_value()) {
                auto optSceneAsset = _store->getMeshAsset(optFile.value());
                if (optSceneAsset.has_value()) {
                    auto sceneAsset = optSceneAsset.value();
                    if (XPFS::getTimestamp(sceneAsset->getFile()->getPath()) != sceneAsset->getFile()->getTimestamp()) {
                        XPFile::onFileModified(sceneAsset->getFile());
                    }
                }
            } else {
                _store->createFile(directory_path, XPEFileResourceType::Scene);
            }
        } else if (XPFS::isRelativeParentDirectory(directory_path, const_shader_dir) &&
                   XPFile::isShaderFile(directory_path)) {
            std::string shaderFilename = directory_path.parent_path().filename().string();
            XP_LOG(XPLoggerSeverityInfo, fmt::format("ShaderAsset <{}>", shaderFilename).c_str());
            auto optFile = _store->getFile(directory_path, XPEFileResourceType::Shader);
            if (optFile.has_value()) {
                auto optShaderAsset = _store->getShaderAsset(optFile.value());
                if (optShaderAsset.has_value()) {
                    auto shaderAsset = optShaderAsset.value();
                    if (XPFS::getTimestamp(shaderAsset->getFile()->getPath()) !=
                        shaderAsset->getFile()->getTimestamp()) {
                        XPFile::onFileModified(shaderAsset->getFile());
                    }
                }
            } else {
                _store->createFile(directory_path, XPEFileResourceType::Shader);
            }
        } else if (XPFS::isRelativeParentDirectory(directory_path, const_texture_dir) &&
                   XPFile::isTextureFile(directory_path)) {
            std::string textureFilename = directory_path.string().substr(const_texture_dir.string().size());
            puts(fmt::format("TextureAsset <{}>", textureFilename).c_str());
            auto optFile = _store->getFile(directory_path, XPEFileResourceType::Texture);
            if (optFile.has_value()) {
                auto optTextureAsset = _store->getTextureAsset(optFile.value());
                if (optTextureAsset.has_value()) {
                    auto textureAsset = optTextureAsset.value();
                    if (XPFS::getTimestamp(textureAsset->getFile()->getPath()) !=
                        textureAsset->getFile()->getTimestamp()) {
                        XPFile::onFileModified(textureAsset->getFile());
                    }
                }
            } else {
                _store->createFile(directory_path, XPEFileResourceType::Texture);
            }
        }
    }
}
