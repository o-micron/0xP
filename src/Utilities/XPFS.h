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

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

class XPFS
{
  public:
    static time_t      getTimestamp(const std::string& filepath);
    static std::string getExecutablePath();
    static std::string getExecutableDirectoryPath();
    static bool        isFilePathContaining(const std::string& filepath, const std::string& substring);
    static bool isRelativeParentDirectory(std::filesystem::path directory, std::filesystem::path parentDirectory);
    static bool areConceptuallySame(std::filesystem::path directory, std::filesystem::path parentDirectory);
    static void createDirectory(const char* name);
    static bool isFile(const char* path);
    static bool isDirectory(const char* path);
    static void iterateFilesRecursivelyFromDirectory(const char* path, std::vector<std::filesystem::path>& files);
    static bool readFileLines(const char* path, std::vector<std::string>& lines);
    static bool readFileText(const char* path, std::string& text);
    static bool readFileBlock(const char* path, std::string& text);
    static std::optional<std::vector<uint8_t>> readShaderFile(const char* path);
    static void                                writeFileBlock(const char* path, const std::string& text);
    static void                                writeFileText(const char* path, const std::string& text);
    static void                                copyFile(const char* fromPath, const char* toPath);
    static const char*                         getMeshAssetsDirectory();
    static const char*                         getSceneAssetsDirectory();
    static const char*                         getShaderAssetsDirectory();
    static const char*                         getTextureAssetsDirectory();
    static const char*                         getPrototypeAssetsDirectory();
    static const char*                         getPluginAssetsDirectory();
    static const char*                         getFontAssetsDirectory();
    static const char*                         getRiscvBinaryAssetsDirectory();
    static const std::string                   buildMeshAssetsPath(std::string path);
    static const std::string                   buildShaderAssetsPath(std::string path);
    static const std::string                   buildTextureAssetsPath(std::string path);
    static const std::string                   buildPrototypeAssetsPath(std::string path);
    static const std::string                   buildPluginAssetsPath(std::string path);
    static const std::string                   buildFontAssetsPath(std::string path);
    static const std::string                   buildSceneAssetsPath(std::string path);
    static const std::string                   buildRiscvBianryAssetsPath(std::string path);
};
