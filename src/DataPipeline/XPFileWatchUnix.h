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

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wall"
    #pragma clang diagnostic ignored "-Weverything"
#endif
#include <libfswatch/c/libfswatch.h>
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

class XPFileWatchUnix final : public XPIFileWatch
{
  public:
    XPFileWatchUnix(XPRegistry* const registry);
    ~XPFileWatchUnix() final;
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
    static void* fswatch_start_monitoring(void* param);
    static void  fswatch_callback(fsw_cevent const* const events, const unsigned int event_num, void* data);
    void         refreshDirectories(std::vector<std::string> directories);

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
    FSW_HANDLE                      _fswatch_handle;
    pthread_t                       _fswatch_thread;
    std::unordered_set<std::string> _directories;
};
