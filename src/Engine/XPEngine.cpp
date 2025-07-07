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

#include <Engine/XPConsole.h>
#include <Engine/XPEngine.h>
#include <Engine/XPRegistry.h>
#include <Physics/Interface/XPIPhysics.h>
#include <Renderer/Interface/XPIRenderer.h>
#if defined(XP_RENDERER_SW)
    #include <Renderer/SW/XPSWRenderer.h>
#endif
#include <SceneDescriptor/XPScene.h>
#if defined(XP_EDITOR_MODE)
    #include <UI/Interface/XPIUI.h>
#endif
#include <Mcp/XPMcpServer.h>
#include <SceneDescriptor/XPSceneDescriptorStore.h>
#include <Utilities/XPLogger.h>
#include <Utilities/XPProfiler.h>

#include <array>
#include <chrono>
#include <mutex>
#include <semaphore>
#include <thread>

#if defined(__EMSCRIPTEN__)
    #include <Renderer/WebGPU/wgpu_cpp.h>
#endif

XPEngine::XPEngine() { _shouldQuitLock.store(false); }

XPEngine::~XPEngine()
{
    _renderThreadQueue.clear();
    _computeThreadQueue.clear();
    _physicsThreadQueue.clear();
    _uiThreadQueue.clear();
    _gameThreadQueue.clear();
}

XPProfilable void
XPEngine::initialize()
{
#if defined(XP_MCP_SERVER)
    _registry->getMcpServer()->initialize();
#endif
#if defined(XP_RENDERER_SW)
    _registry->getSWRenderer()->initialize();
#endif
    _registry->getRenderer()->initialize();
    _registry->getPhysics()->initialize();
#if defined(XP_EDITOR_MODE)
    _registry->getUI()->initialize();
#endif
}

XPProfilable void
XPEngine::finalize()
{
#if defined(XP_EDITOR_MODE)
    _registry->getUI()->finalize();
#endif
    _registry->getPhysics()->finalize();
    _registry->getRenderer()->finalize();
#if defined(XP_RENDERER_SW)
    _registry->getSWRenderer()->finalize();
#endif
#if defined(XP_MCP_SERVER)
    _registry->getMcpServer()->finalize();
#endif
}

#if defined(__EMSCRIPTEN__)
static void
main_loop(void* userData)
{
    XPEngine*   engine   = static_cast<XPEngine*>(userData);
    XPRegistry* registry = engine->getRegistry();

    XPProfiler::instance().next();

    registry->triggerAllChangesIfAny();

    registry->getRenderer()->update();
    #if defined(XP_EDITOR_MODE)
    registry->getUI()->update(_registry->getRenderer()->getDeltaTime());
    #endif
}
#endif

XPProfilable void
XPEngine::run()
{
#if defined(__EMSCRIPTEN__)
    emscripten_set_main_loop_arg(main_loop, this, 0, 1);
#else
    XP_LOG(XPLoggerSeverityInfo, "StartThreads");

    XPScene*            scene  = _registry->getScene();
    std::list<XPScene*> scenes = _registry->getScene()->getSceneDescriptorStore()->getScenes();
    auto                it     = scenes.begin();
    ++it;
    _registry->setSceneBuffered(*it);

    while (!_shouldQuitLock.load()) {
        XPProfiler::instance().next();
        bool physicsShouldStep = true;
    #if defined(XP_EDITOR_MODE)
        physicsShouldStep = _registry->getUI()->isPhysicsPlaying();
    #endif
        _registry->triggerAllChangesIfAny();
        if (physicsShouldStep) { _registry->getPhysics()->update(); }
        _registry->getRenderer()->update();
    #if defined(XP_EDITOR_MODE)
        _registry->getUI()->update(_registry->getRenderer()->getDeltaTime());
        physicsShouldStep = _registry->getUI()->isPhysicsPlaying();
    #endif
    }
    XP_LOG(XPLoggerSeverityInfo, "EndThreads");
#endif
}

XPProfilable void
XPEngine::quit()
{
    _shouldQuitLock.store(true);
}

XPProfilable void
XPEngine::scheduleRenderTask(std::function<void()>&& task)
{
    _renderThreadQueue.emplace_back(std::move(task));
}

XPProfilable void
XPEngine::scheduleComputeTask(std::function<void()>&& task)
{
    _computeThreadQueue.emplace_back(std::move(task));
}

XPProfilable void
XPEngine::schedulePhysicsTask(std::function<void()>&& task)
{
    _physicsThreadQueue.emplace_back(std::move(task));
}

XPProfilable void
XPEngine::scheduleUITask(std::function<void()>&& task)
{
    _uiThreadQueue.emplace_back(std::move(task));
}

XPProfilable void
XPEngine::scheduleGameTask(std::function<void()>&& task)
{
    _gameThreadQueue.emplace_back(std::move(task));
}

XPProfilable void
XPEngine::runRenderTasks()
{
    while (!_renderThreadQueue.empty()) {
        auto& task = _renderThreadQueue.front();
        task();
        _renderThreadQueue.pop_front();
    }
}

XPProfilable void
XPEngine::runComputeTasks()
{
    while (!_computeThreadQueue.empty()) {
        auto& task = _computeThreadQueue.front();
        task();
        _computeThreadQueue.pop_front();
    }
}

XPProfilable void
XPEngine::runPhysicsTasks()
{
    while (!_physicsThreadQueue.empty()) {
        auto&& task = std::move(_physicsThreadQueue.front());
        _physicsThreadQueue.pop_front();
        task();
    }
}

XPProfilable void
XPEngine::runUITasks()
{
    // only execute a subset of the scheduled work to make sure that we don't block main thread
    auto       start   = std::chrono::steady_clock::now();
    const auto timeout = std::chrono::milliseconds(5);

    while (!_uiThreadQueue.empty()) {
        auto& task = _uiThreadQueue.front();
        task();
        _uiThreadQueue.pop_front();

        // break and rely on subsequent cycles to run the rest of the queue tasks ..
        auto elapsed = std::chrono::steady_clock::now() - start;
        if (elapsed >= timeout) { break; }
    }
}

XPProfilable void
XPEngine::runGameTasks()
{
    // only execute a subset of the scheduled work to make sure that we don't block main thread
    auto       start   = std::chrono::steady_clock::now();
    const auto timeout = std::chrono::milliseconds(5);

    while (!_gameThreadQueue.empty()) {
        auto&& task = std::move(_gameThreadQueue.front());
        _gameThreadQueue.pop_front();
        task();

        // break and rely on subsequent cycles to run the rest of the queue tasks ..
        auto elapsed = std::chrono::steady_clock::now() - start;
        if (elapsed >= timeout) { break; }
    }
}

XPProfilable bool
XPEngine::hasTasks() const
{
    return !_renderThreadQueue.empty() || !_computeThreadQueue.empty() || !_physicsThreadQueue.empty() ||
           !_uiThreadQueue.empty() || !_gameThreadQueue.empty();
}

void
XPEngine::setRegistry(std::unique_ptr<XPRegistry> registry)
{
    _registry = std::move(registry);
}

void
XPEngine::setConsole(std::unique_ptr<XPConsole> console)
{
    _console = std::move(console);
}

XPRegistry*
XPEngine::getRegistry() const
{
    return _registry.get();
}

XPConsole*
XPEngine::getConsole() const
{
    return _console.get();
}
