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

#include <atomic>
#include <deque>
#include <functional>
#include <memory>

class XPRegistry;
class XPConsole;

/// @brief A class to represent the root of all engine structures
class XPEngine final
{
  public:
    XPEngine();
    ~XPEngine();

    /// @brief allocate everything
    void initialize();

    /// @brief deallocate everything
    void finalize();

    /// @brief Blocks and starts all threads in loops
    void run();

    /// @brief stops all threads and shuts down the engine
    void quit();

    /// @brief schedules a new task on the render thread
    void scheduleRenderTask(std::function<void()>&& task);

    /// @brief schedules a new task on the compute thread
    void scheduleComputeTask(std::function<void()>&& task);

    /// @brief schedules a new task on the physics thread
    void schedulePhysicsTask(std::function<void()>&& task);

    /// @brief schedules a new task on the ui thread
    void scheduleUITask(std::function<void()>&& task);

    /// @brief schedules a new task on the game thread
    void scheduleGameTask(std::function<void()>&& task);

    /// @brief run all the render queued thread tasks
    void runRenderTasks();

    /// @brief run all the compute queued thread tasks
    void runComputeTasks();

    /// @brief run all the physics queued thread tasks
    void runPhysicsTasks();

    /// @brief run all the ui thread queued tasks
    void runUITasks();

    /// @brief run all the game thread queued tasks
    void runGameTasks();

    /// @brief returns whether any of the thread queues have tasks
    bool hasTasks() const;

    void        setRegistry(std::unique_ptr<XPRegistry> registry);
    void        setConsole(std::unique_ptr<XPConsole> console);
    XPRegistry* getRegistry() const;
    XPConsole*  getConsole() const;

  private:
    std::unique_ptr<XPRegistry>       _registry;
    std::unique_ptr<XPConsole>        _console;
    std::atomic_bool                  _shouldQuitLock;
    std::deque<std::function<void()>> _renderThreadQueue;
    std::deque<std::function<void()>> _computeThreadQueue;
    std::deque<std::function<void()>> _physicsThreadQueue;
    std::deque<std::function<void()>> _uiThreadQueue;
    std::deque<std::function<void()>> _gameThreadQueue;
};
