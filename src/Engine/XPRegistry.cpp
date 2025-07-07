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

#include <Engine/XPRegistry.h>

#include <Engine/XPAllocators.h>
#include <Physics/Interface/XPIPhysics.h>
#include <Renderer/SW/XPSWRenderer.h>
#include <SceneDescriptor/XPScene.h>
#include <Utilities/XPLogger.h>

#include <thread>

XPRegistry::XPRegistry(XPEngine* const engine)
  : _engine(engine)
{
}

XPRegistry::~XPRegistry() {}

bool
XPRegistry::hasAnyChanges()
{
    bool hasAnyChanges = false;
#if defined(XP_RENDERER_SW)
    hasAnyChanges |= hasSWRendererChanges();
#endif
    hasAnyChanges |= hasRendererChanges();
    hasAnyChanges |= hasPhysicsChanges();
    hasAnyChanges |= hasUIChanges();
    hasAnyChanges |= hasDataPipelineStoreChanges();
    hasAnyChanges |= hasSceneChanges();
    hasAnyChanges |= hasFileWatchChanges();
    hasAnyChanges |= hasRendererGraphChanges();
    hasAnyChanges |= hasAllocatorsChanges();
#if defined(XP_MCP_SERVER)
    hasAnyChanges |= hasMcpServerChanges();
#endif
    return hasAnyChanges;
}

bool
XPRegistry::hasSWRendererChanges()
{
    return _tmpSWRenderer != nullptr;
}

bool
XPRegistry::hasRendererChanges()
{
    return _tmpRenderer != nullptr;
}

bool
XPRegistry::hasPhysicsChanges()
{
    return _tmpPhysics != nullptr;
}

bool
XPRegistry::hasUIChanges()
{
    return _tmpUi != nullptr;
}

bool
XPRegistry::hasDataPipelineStoreChanges()
{
    return _tmpDataPipelineStore != nullptr;
}

bool
XPRegistry::hasSceneChanges()
{
    return _tmpScene != nullptr;
}

bool
XPRegistry::hasFileWatchChanges()
{
    return _tmpFileWatch != nullptr;
}

bool
XPRegistry::hasRendererGraphChanges()
{
    return _tmpRendererGraph != nullptr;
}

bool
XPRegistry::hasAllocatorsChanges()
{
    return _tmpAllocators != nullptr;
}

bool
XPRegistry::hasMcpServerChanges()
{
    return _tmpMcpServer != nullptr;
}

XPProfilable void
XPRegistry::triggerAllChangesIfAny()
{
#if defined(XP_RENDERER_SW)
    triggerSWRendererChangesIfAny();
#endif
    triggerRendererChangesIfAny();
    triggerPhysicsChangesIfAny();
    triggerUIChangesIfAny();
    triggerDataPipelineStoreChangesIfAny();
    triggerSceneChangesIfAny();
    triggerFileWatchChangesIfAny();
    triggerRendererGraphChangesIfAny();
    triggerAllocatorsChangesIfAny();
#if defined(XP_MCP_SERVER)
    triggerMcpServerChangesIfAny();
#endif
}

void
XPRegistry::triggerSWRendererChangesIfAny()
{
    if (_tmpSWRenderer) {
        _swRenderer    = _tmpSWRenderer;
        _tmpSWRenderer = nullptr;
    }
}

void
XPRegistry::triggerRendererChangesIfAny()
{
    if (_tmpRenderer) {
        _renderer    = _tmpRenderer;
        _tmpRenderer = nullptr;
    }
}

void
XPRegistry::triggerPhysicsChangesIfAny()
{
    if (_tmpPhysics) {
        _physics    = _tmpPhysics;
        _tmpPhysics = nullptr;
    }
}

void
XPRegistry::triggerUIChangesIfAny()
{
    if (_tmpUi) {
        _ui    = _tmpUi;
        _tmpUi = nullptr;
    }
}

void
XPRegistry::triggerDataPipelineStoreChangesIfAny()
{
    if (_tmpDataPipelineStore) {
        _dataPipelineStore    = _tmpDataPipelineStore;
        _tmpDataPipelineStore = nullptr;
    }
}

void
XPRegistry::triggerSceneChangesIfAny()
{
    if (_tmpScene) {
        if (_scene) {
            XP_LOGV(XPLoggerSeverityInfo,
                    "Switching scenes from %s to %s",
                    _scene->getName().c_str(),
                    _tmpScene->getName().c_str());
        }
        _scene    = _tmpScene;
        _tmpScene = nullptr;
        _scene->addAttachmentChanges(UINT32_MAX, false);

        _physics->onSceneTraitsChanged();

        // attempt to load and render via sw renderer async
        static std::mutex mut;
        {
            std::lock_guard<std::mutex> l(mut);
            std::thread                 t([this] {
                _swRenderer->loadScene(_scene->getName());
                XPSWRasterizerEventListener listener;
                _swRenderer->render(listener);
            });
            t.detach();
        }
    }
}

void
XPRegistry::triggerFileWatchChangesIfAny()
{
    if (_tmpFileWatch) {
        _fileWatch    = _tmpFileWatch;
        _tmpFileWatch = nullptr;
    }
}

void
XPRegistry::triggerRendererGraphChangesIfAny()
{
    if (_tmpRendererGraph) {
        _rendererGraph    = _tmpRendererGraph;
        _tmpRendererGraph = nullptr;
    }
}

void
XPRegistry::triggerAllocatorsChangesIfAny()
{
    if (_tmpAllocators) {
        _allocators    = _tmpAllocators;
        _tmpAllocators = nullptr;
    }
}

void
XPRegistry::triggerMcpServerChangesIfAny()
{
    if (_tmpMcpServer) {
        _mcpServer    = _tmpMcpServer;
        _tmpMcpServer = nullptr;
    }
}

void
XPRegistry::setSWRendererBuffered(XPSWRenderer* swRenderer)
{
    _tmpSWRenderer = swRenderer;
}

void
XPRegistry::setRendererBuffered(XPIRenderer* renderer)
{
    _tmpRenderer = renderer;
}

void
XPRegistry::setPhysicsBuffered(XPIPhysics* physics)
{
    _tmpPhysics = physics;
}

void
XPRegistry::setUIBuffered(XPIUI* ui)
{
    _tmpUi = ui;
}

void
XPRegistry::setDataPipelineStoreBuffered(XPDataPipelineStore* dataPipelineStore)
{
    _tmpDataPipelineStore = dataPipelineStore;
}

void
XPRegistry::setSceneBuffered(XPScene* scene)
{
    _tmpScene = scene;
}

void
XPRegistry::setFileWatchBuffered(XPIFileWatch* fileWatch)
{
    _tmpFileWatch = fileWatch;
}

void
XPRegistry::setRendererGraphBuffered(XPRendererGraph* rendererGraph)
{
    _tmpRendererGraph = rendererGraph;
}

void
XPRegistry::setAllocatorsBuffered(XPAllocators* allocators)
{
    _tmpAllocators = allocators;
}

void
XPRegistry::setMcpServerBuffered(XPMcpServer* mcpServer)
{
    _tmpMcpServer = mcpServer;
}

XPEngine*
XPRegistry::getEngine() const
{
    return _engine;
}

XPSWRenderer*
XPRegistry::getSWRenderer() const
{
    return _swRenderer;
}

XPIRenderer*
XPRegistry::getRenderer() const
{
    return _renderer;
}

XPIPhysics*
XPRegistry::getPhysics() const
{
    return _physics;
}

XPIUI*
XPRegistry::getUI() const
{
    return _ui;
}

XPDataPipelineStore*
XPRegistry::getDataPipelineStore() const
{
    return _dataPipelineStore;
}

XPScene*
XPRegistry::getScene() const
{
    return _scene;
}

XPIFileWatch*
XPRegistry::getFileWatch() const
{
    return _fileWatch;
}

XPRendererGraph*
XPRegistry::getRendererGraph() const
{
    return _rendererGraph;
}

XPAllocators*
XPRegistry::getAllocators() const
{
    return _allocators;
}

XPMcpServer*
XPRegistry::getMcpServer() const
{
    return _mcpServer;
}