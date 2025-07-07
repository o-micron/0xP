#pragma once

#include "R5RLogger.h"

#include <assert.h>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

struct R5RThreadPoolMemory
{
    R5RThreadPoolMemory(const size_t numBytes = 0)
    {
        // allocate memory to be used during frame rasterization
        frameMemoryEnd   = numBytes == 0 ? 4 * 1024 * 1024 * sizeof(uint8_t) : numBytes;
        frameMemory      = static_cast<uint8_t*>(malloc(frameMemoryEnd));
        frameMemoryStart = 0;
        if (frameMemory == nullptr) { LOGV_CRITICAL("Could not allocate thread pool memory of {} bytes", numBytes); }
    }
    R5RThreadPoolMemory(const R5RThreadPoolMemory&) = delete;
    R5RThreadPoolMemory(R5RThreadPoolMemory&&)      = delete;
    ~R5RThreadPoolMemory()
    {
        if (frameMemory) {
            assert(frameMemoryStart == 0 && "Memory pool wasn't properly cleared out");
            free(frameMemory);
            frameMemory    = nullptr;
            frameMemoryEnd = 0;
        }
    }
    [[nodiscard]] uint8_t* pushFrameMemory(size_t numBytes)
    {
        if (frameMemoryStart + numBytes <= frameMemoryEnd) {
            uint8_t* ptr = &frameMemory[frameMemoryStart];
            // memset(ptr, 0, numBytes);
            frameMemoryStart += static_cast<int64_t>(numBytes);
            return ptr;
        }
        assert(false && "Memory full, you need to expand the maximum size of a frame memory");
        return nullptr;
    }
    void popFrameMemory(size_t numBytes)
    {
        assert((frameMemoryStart - static_cast<int64_t>(numBytes)) >= 0 &&
               "You need to push memory first then return it back via calling this function");
        frameMemoryStart -= static_cast<int64_t>(numBytes);
    }
    void checkClear()
    {
        if (frameMemoryStart != 0) {
            LOGV_CRITICAL("ThreadPoolMemory is not clear here, frameMemoryStart = {}, frameMemoryEnd = {}",
                          frameMemoryStart,
                          frameMemoryEnd);
        }
    }
    void popAllFrameMemory() { frameMemoryStart = 0; }
    void memsetZeros() { memset(frameMemory, 0, frameMemoryEnd); }

    uint8_t* frameMemory;
    int64_t  frameMemoryStart;
    int64_t  frameMemoryEnd;
};

typedef std::function<void(R5RThreadPoolMemory&)> ThreadPoolTask;

struct R5RThreadPool
{
    explicit R5RThreadPool(uint8_t numThreads);
    ~R5RThreadPool();
    void    submit(ThreadPoolTask task) noexcept;
    bool    hasWork() noexcept;
    uint8_t remainingTasks() noexcept;
    void    waitForWork() noexcept;

  private:
    void startAll();
    void stopAll() noexcept;

    R5RThreadPoolMemory*       _threadsMemory;
    std::thread*               _threads;
    std::queue<ThreadPoolTask> _tasks;
    uint8_t                    _numThreads;
    std::atomic_uint8_t        _numBusyThreads;
    std::atomic_bool           _stopped;
    std::condition_variable    _eventVar;
    std::mutex                 _eventMutex;
};
