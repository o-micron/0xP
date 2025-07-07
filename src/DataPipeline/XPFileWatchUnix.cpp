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
#include <DataPipeline/XPFileWatchUnix.h>
#include <DataPipeline/XPIFileWatch.h>
#include <DataPipeline/XPMeshAsset.h>
#include <DataPipeline/XPMeshBuffer.h>
#include <DataPipeline/XPPreloadedAssets.h>
#include <DataPipeline/XPRiscvBinaryAsset.h>
#include <DataPipeline/XPRiscvBinaryBuffer.h>
#include <DataPipeline/XPShaderAsset.h>
#include <DataPipeline/XPShaderBuffer.h>
#include <DataPipeline/XPTextureAsset.h>
#include <DataPipeline/XPTextureBuffer.h>

#include <Utilities/XPFS.h>
#include <Utilities/XPLogger.h>

#include <chrono>
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

XPFileWatchUnix::XPFileWatchUnix(XPRegistry* const registry)
  : XPIFileWatch(registry)
  , _registry(registry)
{
}

XPFileWatchUnix::~XPFileWatchUnix() {}

void
XPFileWatchUnix::start(XPDataPipelineStore* dataPipelineStore,
                       const std::string&   rootPath,
                       const std::string&   meshesDirectory,
                       const std::string&   scenesDirectory,
                       const std::string&   shadersDirectory,
                       const std::string&   texturesDirectory,
                       const std::string&   prototypesDirectory,
                       const std::string&   pluginsDirectory,
                       const std::string&   riscvBinariesDirectory)
{
    _dataPipelineStore = dataPipelineStore;
    _rootPath          = rootPath;
    _meshesPath        = meshesDirectory;
    _scenesPath        = scenesDirectory;
    _shadersPath       = shadersDirectory;
    _texturesPath      = texturesDirectory;
    _prototypesPath    = prototypesDirectory;
    _pluginsPath       = pluginsDirectory;
    _riscvBinariesPath = riscvBinariesDirectory;

    // get assets directories
    if (!std::filesystem::exists(_meshesPath)) {
        XP_LOG(XPLoggerSeverityFatal, "Failed to find a valid meshes directory !");
        return;
    }
    if (!std::filesystem::exists(_shadersPath)) {
        XP_LOG(XPLoggerSeverityFatal, "Failed to find a valid shaders directory !");
        return;
    }
    if (!std::filesystem::exists(_texturesPath)) {
        XP_LOG(XPLoggerSeverityFatal, "Failed to find a valid textures directory !");
        return;
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    // load preloaded assets
    {
        _dataPipelineStore->createFile("TRIANGLE", XPEFileResourceType::PreloadedMesh);
        _dataPipelineStore->createFile("QUAD", XPEFileResourceType::PreloadedMesh);
        _dataPipelineStore->createFile("CUBE", XPEFileResourceType::PreloadedMesh);
    }

    // loop over the directories and load the scenes
    for (const auto& sceneEntry : std::filesystem::directory_iterator(_scenesPath)) {
        if (!sceneEntry.is_regular_file()) { continue; }
        auto fullPath = sceneEntry.path().string();
        if (XPFile::isMeshFile(fullPath)) { _dataPipelineStore->createFile(fullPath, XPEFileResourceType::Scene); }
    }

    // loop over the directories and load the riscv binaries
    for (const auto& riscvBinary : std::filesystem::directory_iterator(_riscvBinariesPath)) {
        if (!riscvBinary.is_regular_file()) { continue; }
        auto fullPath = riscvBinary.path().string();
        if (XPFile::isRiscvBinaryFile(fullPath)) {
            _dataPipelineStore->createFile(fullPath, XPEFileResourceType::RiscvBinary);
        }
    }

    // loop over the directories and load the assets
    // for (const auto& meshEntry : std::filesystem::directory_iterator(_meshesPath)) {
    //     if (!meshEntry.is_regular_file()) { continue; }
    //     auto fullPath = meshEntry.path().string();
    //     if (XPFile::isMeshFile(fullPath)) { _dataPipelineStore->createFile(fullPath, XPEFileResourceType::Mesh); }
    // }
    // for (const auto& shaderEntry : std::filesystem::directory_iterator(_shadersPath)) {
    //     if (!shaderEntry.is_regular_file()) { continue; }
    //     auto fullPath = shaderEntry.path().string();
    //     if (XPFile::isShaderFile(fullPath) && shaderEntry.path().extension() == ".metal") {
    //         _dataPipelineStore->createFile(fullPath, XPEFileResourceType::Shader);
    //     }
    // }
    for (const auto& textureEntry : std::filesystem::directory_iterator(_texturesPath)) {
        if (!textureEntry.is_regular_file()) { continue; }
        auto fullPath = textureEntry.path().string();
        if (XPFile::isTextureFile(fullPath)) { _dataPipelineStore->createFile(fullPath, XPEFileResourceType::Texture); }
    }

    if (FSW_OK != fsw_init_library()) {
        fsw_last_error();
        XP_LOG(XPLoggerSeverityFatal, "Failed to initialize file watcher !");
        return;
    }

    auto t2 = std::chrono::high_resolution_clock::now();

    /* Getting number of milliseconds as an integer. */
    auto ms_int = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);

    XP_LOGV(XPLoggerSeverityInfo, "Assets loading time is %u", ms_int);

#if defined(XP_PLATFORM_MACOS)
    _fswatch_handle = fsw_init_session(fsevents_monitor_type);
#elif defined(XP_PLATFORM_LINUX)
    _fswatch_handle = fsw_init_session(windows_monitor_type);
#endif

    if (!_fswatch_handle) {
        fsw_last_error();
        XP_LOG(XPLoggerSeverityFatal, "Invalid fswatch handle");
        return;
    }
    // add directories
    if (FSW_OK != fsw_add_path(_fswatch_handle, _rootPath.c_str())) { fsw_last_error(); }
    // register callback function
    if (FSW_OK != fsw_set_callback(_fswatch_handle, &XPFileWatchUnix::fswatch_callback, this)) { fsw_last_error(); }
    // set overflow
    fsw_set_allow_overflow(_fswatch_handle, false);
    // start monitor
    if (pthread_create(&_fswatch_thread, nullptr, fswatch_start_monitoring, (void*)&_fswatch_handle)) {
        XP_LOG(XPLoggerSeverityFatal, "Error creating thread");
        return;
    }
}

void
XPFileWatchUnix::stop()
{
    if (FSW_OK != fsw_stop_monitor(_fswatch_handle)) { XP_LOG(XPLoggerSeverityError, "Error stopping monitor"); }
    sleep(3);
    if (FSW_OK != fsw_destroy_session(_fswatch_handle)) { XP_LOG(XPLoggerSeverityError, "Error destroying session"); }
    if (pthread_join(_fswatch_thread, nullptr)) { XP_LOG(XPLoggerSeverityError, "Error joining monitor thread"); }
}

XPDataPipelineStore&
XPFileWatchUnix::getDataPipelineStore() const
{
    return *_dataPipelineStore;
}

const std::string&
XPFileWatchUnix::getRootPath() const
{
    return _rootPath;
}

const std::string&
XPFileWatchUnix::getMeshesPath() const
{
    return _meshesPath;
}

const std::string&
XPFileWatchUnix::getScenesPath() const
{
    return _scenesPath;
}

const std::string&
XPFileWatchUnix::getShadersPath() const
{
    return _shadersPath;
}

const std::string&
XPFileWatchUnix::getTexturesPath() const
{
    return _texturesPath;
}

const std::string&
XPFileWatchUnix::getPrototypesPath() const
{
    return _prototypesPath;
}

const std::string&
XPFileWatchUnix::getPluginsPath() const
{
    return _pluginsPath;
}

const std::string&
XPFileWatchUnix::getRiscvBinariesPath() const
{
    return _riscvBinariesPath;
}

void
XPFileWatchUnix::refreshDirectories(std::vector<std::string> directories)
{
    if (directories.empty()) return;

    std::filesystem::path executableParentDirectory = XPFS::getExecutableDirectoryPath();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    XP_LOG(XPLoggerSeverityInfo, std::string(60, '=').c_str());
    XP_LOG(XPLoggerSeverityInfo, "CHANGED DIRECTORIES");
    XP_LOG(XPLoggerSeverityInfo, std::string(60, '=').c_str());

    std::filesystem::path const_assets_dir      = (executableParentDirectory / _rootPath).make_preferred();
    std::filesystem::path const_mesh_dir        = (const_assets_dir / _meshesPath).make_preferred();
    std::filesystem::path const_scene_dir       = (const_assets_dir / _scenesPath).make_preferred();
    std::filesystem::path const_shader_dir      = (const_assets_dir / _shadersPath).make_preferred();
    std::filesystem::path const_texture_dir     = (const_assets_dir / _texturesPath).make_preferred();
    std::filesystem::path const_plugin_dir      = (const_assets_dir / _pluginsPath).make_preferred();
    std::filesystem::path const_riscvBinary_dir = (const_assets_dir / _riscvBinariesPath).make_preferred();

    for (auto& directory : directories) {
        XP_LOG(XPLoggerSeverityInfo, fmt::format("Directory ({}) changed.", directory).c_str());
        std::filesystem::path directory_path = std::filesystem::path(directory).make_preferred();
        if (!XPFS::isRelativeParentDirectory(directory_path, const_assets_dir)) { continue; }
        if (XPFS::isRelativeParentDirectory(directory_path, const_mesh_dir) && XPFile::isMeshFile(directory_path)) {
            std::string meshFilename = directory_path.string().substr(const_mesh_dir.string().size());
            XP_LOG(XPLoggerSeverityInfo, fmt::format("MeshAsset <{}>", meshFilename).c_str());
            auto optFile = _dataPipelineStore->getFile(directory_path, XPEFileResourceType::Scene);
            if (optFile.has_value()) {
                auto optMeshAsset = _dataPipelineStore->getMeshAsset(optFile.value());
                if (optMeshAsset.has_value()) {
                    auto meshAsset = optMeshAsset.value();
                    if (XPFS::getTimestamp(meshAsset->getFile()->getPath()) != meshAsset->getFile()->getTimestamp()) {
                        XPFile::onFileModified(meshAsset->getFile());
                    }
                }
            } else {
                _dataPipelineStore->createFile(directory_path, XPEFileResourceType::Scene);
            }
        } else if (XPFS::isRelativeParentDirectory(directory_path, const_scene_dir) &&
                   XPFile::isMeshFile(directory_path)) {
            std::string sceneFilename = directory_path.string().substr(const_scene_dir.string().size());
            XP_LOG(XPLoggerSeverityInfo, fmt::format("SceneAsset <{}>", sceneFilename).c_str());
            auto optFile = _dataPipelineStore->getFile(directory_path, XPEFileResourceType::Scene);
            if (optFile.has_value()) {
                auto optSceneAsset = _dataPipelineStore->getMeshAsset(optFile.value());
                if (optSceneAsset.has_value()) {
                    auto sceneAsset = optSceneAsset.value();
                    if (XPFS::getTimestamp(sceneAsset->getFile()->getPath()) != sceneAsset->getFile()->getTimestamp()) {
                        XPFile::onFileModified(sceneAsset->getFile());
                    }
                }
            } else {
                _dataPipelineStore->createFile(directory_path, XPEFileResourceType::Scene);
            }
        } else if (XPFS::isRelativeParentDirectory(directory_path, const_shader_dir) &&
                   XPFile::isShaderFile(directory_path)) {
            std::string shaderFilename = directory_path.parent_path().filename().string();
            XP_LOG(XPLoggerSeverityInfo, fmt::format("ShaderAsset <{}>", shaderFilename).c_str());
            auto optFile = _dataPipelineStore->getFile(directory_path, XPEFileResourceType::Shader);
            if (optFile.has_value()) {
                auto optShaderAsset = _dataPipelineStore->getShaderAsset(optFile.value());
                if (optShaderAsset.has_value()) {
                    auto shaderAsset = optShaderAsset.value();
                    if (XPFS::getTimestamp(shaderAsset->getFile()->getPath()) !=
                        shaderAsset->getFile()->getTimestamp()) {
                        XPFile::onFileModified(shaderAsset->getFile());
                    }
                }
            } else {
                _dataPipelineStore->createFile(directory_path, XPEFileResourceType::Shader);
            }
        } else if (XPFS::isRelativeParentDirectory(directory_path, const_texture_dir) &&
                   XPFile::isTextureFile(directory_path)) {
            std::string textureFilename = directory_path.string().substr(const_texture_dir.string().size());
            puts(fmt::format("TextureAsset <{}>", textureFilename).c_str());
            auto optFile = _dataPipelineStore->getFile(directory_path, XPEFileResourceType::Texture);
            if (optFile.has_value()) {
                auto optTextureAsset = _dataPipelineStore->getTextureAsset(optFile.value());
                if (optTextureAsset.has_value()) {
                    auto textureAsset = optTextureAsset.value();
                    if (XPFS::getTimestamp(textureAsset->getFile()->getPath()) !=
                        textureAsset->getFile()->getTimestamp()) {
                        XPFile::onFileModified(textureAsset->getFile());
                    }
                }
            } else {
                _dataPipelineStore->createFile(directory_path, XPEFileResourceType::Texture);
            }
        } else if (XPFS::isRelativeParentDirectory(directory_path, const_riscvBinary_dir) &&
                   XPFile::isRiscvBinaryFile(directory_path)) {
            std::string riscvBinaryFilename = directory_path.string().substr(const_riscvBinary_dir.string().size());
            puts(fmt::format("RiscvBinaryAsset <{}>", riscvBinaryFilename).c_str());
            auto optFile = _dataPipelineStore->getFile(directory_path.string(), XPEFileResourceType::RiscvBinary);
            if (optFile.has_value()) {
                auto optRiscvBinaryAsset = _dataPipelineStore->getRiscvBinaryAsset(optFile.value());
                if (optRiscvBinaryAsset.has_value()) {
                    auto riscvBinaryAsset = optRiscvBinaryAsset.value();
                    if (XPFS::getTimestamp(riscvBinaryAsset->getFile()->getPath()) !=
                        riscvBinaryAsset->getFile()->getTimestamp()) {
                        XPFile::onFileModified(riscvBinaryAsset->getFile());
                    }
                }
            } else {
                _dataPipelineStore->createFile(directory_path.string(), XPEFileResourceType::RiscvBinary);
            }
        }
    }
}

void*
XPFileWatchUnix::fswatch_start_monitoring(void* param)
{
    FSW_HANDLE* handle = (FSW_HANDLE*)param;
    if (FSW_OK != fsw_start_monitor(*handle)) {
        XP_LOG(XPLoggerSeverityError, "Error creating thread");
    } else {
        XP_LOG(XPLoggerSeverityError, "Monitor stopped");
    }
    return nullptr;
}

void
XPFileWatchUnix::fswatch_callback(fsw_cevent const* const events, const unsigned int event_num, void* data)
{
    // let's lock the next scope to prevent crashes when files are continuously being changed for any reason
    static std::mutex WATCH_LOCK_MUTEX;
    {
        // lock scope and do your processing safely
        std::lock_guard<std::mutex> SCOPED_LOCK(WATCH_LOCK_MUTEX);

        auto fileWatchUnix = static_cast<XPFileWatchUnix*>(data);
        for (unsigned int i = 0; i < event_num; ++i) {
            auto path = std::string(events[i].path);
            fileWatchUnix->_directories.insert(path);
            std::stringstream ss;
            for (unsigned int iflag = 0; iflag < events[i].flags_num; ++iflag) {
                ss << events[i].flags[iflag] << " ";
                switch (events[i].flags[iflag]) {
                    case NoOp: {
                        // puts(fmt::format("{} No event has occurred : {}", events[i].evt_time, path).c_str());
                    } break;
                    case PlatformSpecific: {
                        // puts(fmt::format("{} Platform-specific placeholder for event type that cannot currently be
                        // mapped : {}", events[i].evt_time, path).c_str());
                    } break;
                    case Created: {
                        // puts(fmt::format("{} An object was created : {}", events[i].evt_time, path).c_str());
                    } break;
                    case Updated: {
                        // puts(fmt::format("{} An object was updated : {}", events[i].evt_time, path).c_str());
                    } break;
                    case Removed: {
                        // puts(fmt::format("{} An object was removed : {}", events[i].evt_time, path).c_str());
                    } break;
                    case Renamed: {
                        // auto toPath = events[++i].path;
                        // puts(fmt::format("{} An object was renamed : from {} to {}", events[i].evt_time, path,
                        // toPath).c_str());
                    } break;
                    case OwnerModified: {
                        // puts(fmt::format("{} The owner of an object was modified : {}", events[i].evt_time,
                        // path).c_str());
                    } break;
                    case AttributeModified: {
                        // puts(fmt::format("{} The attributes of an object were modified : {}", events[i].evt_time,
                        // path).c_str());
                    } break;
                    case MovedFrom: {
                        // puts(fmt::format("{} An object was moved from this location : {}", events[i].evt_time,
                        // path).c_str());
                    } break;
                    case MovedTo: {
                        // puts(fmt::format("{} An object was moved to this location : {}", events[i].evt_time,
                        // path).c_str());
                    } break;
                    case IsFile: {
                        // puts(fmt::format("{} The object is a file : {}", events[i].evt_time, path).c_str());
                    } break;
                    case IsDir: {
                        // puts(fmt::format("{} The object is a directory : {}", events[i].evt_time, path).c_str());
                    } break;
                    case IsSymLink: {
                        // puts(fmt::format("{} The object is a symbolic link : {}", events[i].evt_time, path).c_str());
                    } break;
                    case Link: {
                        // puts(fmt::format("{} The link count of an object has changed : {}", events[i].evt_time,
                        // path).c_str());
                    } break;
                    case Overflow: {
                        // puts(fmt::format("{} The event queue has overflowed : {}", events[i].evt_time,
                        // path).c_str());
                    } break;
                }
            }
            puts(fmt::format("fswatch_callback: [{}] <{}>\n\n", ss.str().c_str(), events[i].path).c_str());
        }
        fileWatchUnix->refreshDirectories(
          std::vector<std::string>(fileWatchUnix->_directories.begin(), fileWatchUnix->_directories.end()));
        fileWatchUnix->_directories.clear();
    }
}
