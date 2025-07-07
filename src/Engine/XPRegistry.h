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

#include <Utilities/XPAnnotations.h>
#include <Utilities/XPPlatforms.h>

#include <functional>
#include <map>
#include <memory>
#include <string>

class XPEngine;
class XPDataPipelineStore;
class XPIRenderer;
class XPIPhysics;
class XPIUI;
class XPScene;
class XPIFileWatch;
struct XPRendererGraph;
class XPAllocators;
struct XPSWRenderer;
class XPMcpServer;

/// @brief A class to hold an instance for each subsystem
class XPRegistry final
{
  public:
    XPRegistry(XPEngine* const engine);
    ~XPRegistry();

    bool hasAnyChanges();
    bool hasSWRendererChanges();
    bool hasRendererChanges();
    bool hasPhysicsChanges();
    bool hasUIChanges();
    bool hasDataPipelineStoreChanges();
    bool hasSceneChanges();
    bool hasFileWatchChanges();
    bool hasRendererGraphChanges();
    bool hasAllocatorsChanges();
    bool hasMcpServerChanges();

    void triggerAllChangesIfAny();
    void triggerSWRendererChangesIfAny();
    void triggerRendererChangesIfAny();
    void triggerPhysicsChangesIfAny();
    void triggerUIChangesIfAny();
    void triggerDataPipelineStoreChangesIfAny();
    void triggerSceneChangesIfAny();
    void triggerFileWatchChangesIfAny();
    void triggerRendererGraphChangesIfAny();
    void triggerAllocatorsChangesIfAny();
    void triggerMcpServerChangesIfAny();

    void setSWRendererBuffered(XPSWRenderer* swRenderer);
    void setRendererBuffered(XPIRenderer* renderer);
    void setPhysicsBuffered(XPIPhysics* physics);
    void setUIBuffered(XPIUI* ui);
    void setDataPipelineStoreBuffered(XPDataPipelineStore* dataPipelineStore);
    void setSceneBuffered(XPScene* scene);
    void setFileWatchBuffered(XPIFileWatch* fileWatch);
    void setRendererGraphBuffered(XPRendererGraph* rendererGraph);
    void setAllocatorsBuffered(XPAllocators* allocators);
    void setMcpServerBuffered(XPMcpServer* mcpServer);

    XPEngine*            getEngine() const;
    XPSWRenderer*        getSWRenderer() const;
    XPIRenderer*         getRenderer() const;
    XPIPhysics*          getPhysics() const;
    XPIUI*               getUI() const;
    XPDataPipelineStore* getDataPipelineStore() const;
    XPScene*             getScene() const;
    XPIFileWatch*        getFileWatch() const;
    XPRendererGraph*     getRendererGraph() const;
    XPAllocators*        getAllocators() const;
    XPMcpServer*         getMcpServer() const;

  private:
    XPEngine* const      _engine            = nullptr;
    XPSWRenderer*        _swRenderer        = nullptr;
    XPIRenderer*         _renderer          = nullptr;
    XPIPhysics*          _physics           = nullptr;
    XPIUI*               _ui                = nullptr;
    XPDataPipelineStore* _dataPipelineStore = nullptr;
    XPScene*             _scene             = nullptr;
    XPIFileWatch*        _fileWatch         = nullptr;
    XPRendererGraph*     _rendererGraph     = nullptr;
    XPAllocators*        _allocators        = nullptr;
    XPMcpServer*         _mcpServer         = nullptr;
    // used only to buffer changes between engine cycles
    XPSWRenderer*        _tmpSWRenderer        = nullptr;
    XPIRenderer*         _tmpRenderer          = nullptr;
    XPIPhysics*          _tmpPhysics           = nullptr;
    XPIUI*               _tmpUi                = nullptr;
    XPDataPipelineStore* _tmpDataPipelineStore = nullptr;
    XPScene*             _tmpScene             = nullptr;
    XPIFileWatch*        _tmpFileWatch         = nullptr;
    XPRendererGraph*     _tmpRendererGraph     = nullptr;
    XPAllocators*        _tmpAllocators        = nullptr;
    XPMcpServer*         _tmpMcpServer         = nullptr;
};
