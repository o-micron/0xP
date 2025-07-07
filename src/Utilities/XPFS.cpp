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

#include "XPFS.h"

#if defined(XP_PLATFORM_WINDOWS)
    #include <Windows.h>
    #include <libloaderapi.h>
#elif defined(XP_PLATFORM_MACOS)
    #include <mach-o/dyld.h>
#endif

#include <assert.h>
#include <chrono>

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

time_t
XPFS::getTimestamp(const std::string& filepath)
{
#if defined(__EMSCRIPTEN__)
    return 0;
#elif defined(_WIN32)
    const std::filesystem::path p(filepath);
    if (!std::filesystem::exists(p)) return 0;
    struct _stat result = {};
    _stat(filepath.c_str(), &result);
    return result.st_mtime;
#elif defined(__APPLE__)
    const std::filesystem::path p(filepath);
    if (!std::filesystem::exists(p)) return 0;
    auto ftime = std::filesystem::last_write_time(p);
    auto sctp  = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
      ftime - decltype(ftime)::clock::now() + std::chrono::system_clock::now());
    return std::chrono::system_clock::to_time_t(sctp);
#elif defined(__linux__)
    const std::filesystem::path p(filepath);
    if (!std::filesystem::exists(p)) return 0;
    auto ftime = std::filesystem::last_write_time(p);
    auto sctp  = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
      ftime - decltype(ftime)::clock::now() + std::chrono::system_clock::now());
    return std::chrono::system_clock::to_time_t(sctp);
#endif
}

std::string
XPFS::getExecutablePath()
{
#if defined(XP_PLATFORM_EMSCRIPTEN)
    return "XPEngine.wasm";
#elif defined(XP_PLATFORM_MACOS)
    char     buf[PATH_MAX];
    uint32_t bufsize = PATH_MAX;
    if (!_NSGetExecutablePath(buf, &bufsize)) {
        std::string out = { buf };
        if (std::filesystem::path(out).has_parent_path()) {
            out = std::filesystem::path(out).parent_path().string() + "/";
            return out;
        }
        return out;
    }
#elif defined(XP_PLATFORM_WINDOWS)
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::string fullPath(path);

    // Find the last backslash to separate the directory
    size_t pos = fullPath.find_last_of("\\/");
    if (pos != std::string::npos) {
        std::string out = fullPath.substr(0, pos) + "/";
        return out;
    }
    return fullPath;
#elif defined(XP_PLATFORM_LINUX)
    char        result[PATH_MAX];
    ssize_t     count = readlink("/proc/self/exe", result, PATH_MAX);
    std::string out   = std::string(result, (count > 0) ? count : 0);
    if (std::filesystem::path(out).has_parent_path()) {
        out = std::filesystem::path(out).parent_path().string() + "/";
        return out;
    }
    return out;
#else
    #error "Platform not supported"
#endif
    return "";
}

std::string
XPFS::getExecutableDirectoryPath()
{
    static std::string exeDir = getExecutablePath();
    assert(exeDir[exeDir.size() - 1] == '/');
    return exeDir;
}

bool
XPFS::isFilePathContaining(const std::string& filepath, const std::string& substring)
{
    return filepath.find(substring) != std::string::npos;
}

bool
XPFS::isRelativeParentDirectory(std::filesystem::path directory, std::filesystem::path parentDirectory)
{
    if (directory.string().size() < parentDirectory.string().size()) return false;
    std::string substr = directory.string().substr(0, parentDirectory.string().size());
    return substr == parentDirectory.string();
}

bool
XPFS::areConceptuallySame(std::filesystem::path directory, std::filesystem::path parentDirectory)
{
    if (directory.string().size() > parentDirectory.string().size()) return false;
    std::string substr = parentDirectory.string().substr(parentDirectory.string().size() - directory.string().size());
    return substr == directory.string();
}

void
XPFS::createDirectory(const char* name)
{
    std::filesystem::create_directories(name);
}

bool
XPFS::isFile(const char* path)
{
    return std::filesystem::is_regular_file(std::filesystem::path(path));
}

bool
XPFS::isDirectory(const char* path)
{
    return std::filesystem::is_directory(std::filesystem::path(path));
}

void
XPFS::iterateFilesRecursivelyFromDirectory(const char* path, std::vector<std::filesystem::path>& files)
{
    std::filesystem::path p(path);
    if (std::filesystem::exists(p)) {
        for (const auto& entry : std::filesystem::directory_iterator(p)) {
            if (entry.is_directory()) {
                iterateFilesRecursivelyFromDirectory(entry.path().string().c_str(), files);
                continue;
            }
            if (!entry.is_regular_file()) { continue; }
            files.push_back(entry);
        }
    }
}

bool
XPFS::readFileLines(const char* path, std::vector<std::string>& lines)
{
    std::ifstream file(path);
    if (!file.is_open()) { return false; }
    for (std::string line; std::getline(file, line);) lines.push_back(line);
    file.close();
    return true;
}

bool
XPFS::readFileText(const char* path, std::string& text)
{
    std::ifstream file(path);
    if (!file.is_open()) { return false; }
    text = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    return true;
}

bool
XPFS::readFileBlock(const char* path, std::string& text)
{
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open()) { return false; }
    size_t fileSize = (size_t)file.tellg();
    text.resize(fileSize);
    file.seekg(0);
    file.read((char*)text.data(), fileSize);
    file.close();
    return true;
}

std::optional<std::vector<uint8_t>>
XPFS::readShaderFile(const char* path)
{
    std::vector<uint8_t> spirvBytes;
    std::ifstream        file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open()) { return {}; }
    size_t fileSize = (size_t)file.tellg();
    assert((fileSize % sizeof(uint32_t)) == 0);
    spirvBytes.resize(fileSize);
    file.seekg(0, std::ios::beg);
    file.read((char*)spirvBytes.data(), fileSize);
    file.close();
    return { spirvBytes };
}

void
XPFS::writeFileBlock(const char* path, const std::string& text)
{
    std::ofstream file(path, std::ios::ate | std::ios::binary);
    file << text;
    file.close();
}

void
XPFS::writeFileText(const char* path, const std::string& text)
{
    std::ofstream file(path);
    file << text;
    file.close();
}

void
XPFS::copyFile(const char* fromPath, const char* toPath)
{
    std::error_code ec;
    std::filesystem::copy(fromPath, toPath, std::filesystem::copy_options::overwrite_existing, ec);
}

const char*
XPFS::getMeshAssetsDirectory()
{
    static std::string p = std::format("{}meshes/", getExecutableDirectoryPath());
    return p.c_str();
}

const char*
XPFS::getSceneAssetsDirectory()
{
    static std::string p = std::format("{}scenes/", getExecutableDirectoryPath());
    return p.c_str();
}

const char*
XPFS::getShaderAssetsDirectory()
{
    static std::string p = std::format("{}shaders/", getExecutableDirectoryPath());
    return p.c_str();
}

const char*
XPFS::getTextureAssetsDirectory()
{
    static std::string p = std::format("{}textures/", getExecutableDirectoryPath());
    return p.c_str();
}

const char*
XPFS::getPrototypeAssetsDirectory()
{
    static std::string p = std::format("{}prototypes/", getExecutableDirectoryPath());
    return p.c_str();
}

const char*
XPFS::getPluginAssetsDirectory()
{
    static std::string p = std::format("{}plugins/", getExecutableDirectoryPath());
    return p.c_str();
}

const char*
XPFS::getFontAssetsDirectory()
{
    static std::string p = std::format("{}fonts/", getExecutableDirectoryPath());
    return p.c_str();
}

const char*
XPFS::getRiscvBinaryAssetsDirectory()
{
    static std::string p = std::format("{}riscv/", getExecutableDirectoryPath());
    return p.c_str();
}

const std::string
XPFS::buildMeshAssetsPath(std::string path)
{
    return fmt::format("{}{}", getMeshAssetsDirectory(), path);
}

const std::string
XPFS::buildShaderAssetsPath(std::string path)
{
    return fmt::format("{}{}", getShaderAssetsDirectory(), path);
}

const std::string
XPFS::buildTextureAssetsPath(std::string path)
{
    return fmt::format("{}{}", getTextureAssetsDirectory(), path);
}

const std::string
XPFS::buildPrototypeAssetsPath(std::string path)
{
    return fmt::format("{}{}", getPrototypeAssetsDirectory(), path);
}

const std::string
XPFS::buildPluginAssetsPath(std::string path)
{
    return fmt::format("{}{}", getPluginAssetsDirectory(), path);
}

const std::string
XPFS::buildFontAssetsPath(std::string path)
{
    return fmt::format("{}{}", getFontAssetsDirectory(), path);
}

const std::string
XPFS::buildSceneAssetsPath(std::string path)
{
    return fmt::format("{}{}", getSceneAssetsDirectory(), path);
}

const std::string
XPFS::buildRiscvBianryAssetsPath(std::string path)
{
    return fmt::format("{}{}", getRiscvBinaryAssetsDirectory(), path);
}
