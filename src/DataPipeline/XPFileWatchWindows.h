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

#include <DataPipeline/XPIFileWatch.h>

#include <string>

class XPFileWatchWindows final : public XPIFileWatch
{
  public:
    XPFileWatchWindows(XPRegistry* const registry);
    ~XPFileWatchWindows() final;
    void                 start(XPDataPipelineStore* dataPipelineStore,
                               const std::string&   rootPath,
                               const std::string&   meshesDirectory,
                               const std::string&   scenesDirectory,
                               const std::string&   shadersDirectory,
                               const std::string&   texturesDirectory,
                               const std::string&   prototypesDirectory,
                               const std::string&   pluginsDirectory,
                               const std::string&   riscvBinariesDirectory) final;
    void                 stop() final;
    XPDataPipelineStore& getDataPipelineStore() const final;
    const std::string&   getRootPath() const final;
    const std::string&   getMeshesPath() const final;
    const std::string&   getScenesPath() const final;
    const std::string&   getShadersPath() const final;
    const std::string&   getTexturesPath() const final;
    const std::string&   getPrototypesPath() const final;
    const std::string&   getPluginsPath() const final;
    const std::string&   getRiscvBinariesPath() const final;

  private:
    void refreshDirectories(std::vector<std::string> directories);

    XPRegistry*                     _registry;
    XPDataPipelineStore*            _dataPipelineStore;
    std::string                     _rootPath;
    std::string                     _meshesPath;
    std::string                     _scenesPath;
    std::string                     _shadersPath;
    std::string                     _texturesPath;
    std::string                     _prototypesPath;
    std::string                     _pluginsPath;
    std::string                     _riscvBinariesPath;
    std::unordered_set<std::string> _directories;
};
