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

#include <Utilities/XPMacros.h>
#include <Utilities/XPPlatforms.h>

#include <DataPipeline/XPFile.h>
#include <Engine/XPRegistry.h>

#include <filesystem>
#include <string>
#include <unordered_set>

class XPDataPipelineStore;
class XPRegistry;

class XP_PURE_ABSTRACT XPIFileWatch
{
  public:
    XPIFileWatch(XPRegistry* const registry) { XP_UNUSED(registry) }
    virtual ~XPIFileWatch() {}
    virtual void                 start(XPDataPipelineStore* dataPipelineStore,
                                       const std::string&   rootPath,
                                       const std::string&   meshesDirectory,
                                       const std::string&   scenesDirectory,
                                       const std::string&   shadersDirectory,
                                       const std::string&   texturesDirectory,
                                       const std::string&   prototypesDirectory,
                                       const std::string&   pluginsDirectory,
                                       const std::string&   riscvBinariesDirectory) = 0;
    virtual void                 stop()                                           = 0;
    virtual XPDataPipelineStore& getDataPipelineStore() const                     = 0;
    virtual const std::string&   getRootPath() const                              = 0;
    virtual const std::string&   getMeshesPath() const                            = 0;
    virtual const std::string&   getScenesPath() const                            = 0;
    virtual const std::string&   getShadersPath() const                           = 0;
    virtual const std::string&   getTexturesPath() const                          = 0;
    virtual const std::string&   getPrototypesPath() const                        = 0;
    virtual const std::string&   getPluginsPath() const                           = 0;
    virtual const std::string&   getRiscvBinariesPath() const                     = 0;
};
