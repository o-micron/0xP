#include "R5RThreadPool.h"

R5RThreadPool::R5RThreadPool(uint8_t numThreads)
{
    _numThreads = numThreads;
    _numBusyThreads.store(0);
    _stopped.store(false);
    _threads       = new std::thread[numThreads];
    _threadsMemory = new R5RThreadPoolMemory[numThreads]{ 10 * 1024 * sizeof(uint8_t) };
    startAll();
}

R5RThreadPool::~R5RThreadPool()
{
    stopAll();
    delete[] _threads;
    delete[] _threadsMemory;
}

void
R5RThreadPool::submit(ThreadPoolTask task) noexcept
{
    {
        std::unique_lock<std::mutex> lock(_eventMutex);
        _tasks.emplace(std::move(task));
    }
    _eventVar.notify_one();
}

bool
R5RThreadPool::hasWork() noexcept
{
    std::unique_lock<std::mutex> lock(_eventMutex);
    return !_tasks.empty() || _numBusyThreads > 0;
}

uint8_t
R5RThreadPool::remainingTasks() noexcept
{
    std::unique_lock<std::mutex> lock(_eventMutex);
    return static_cast<uint8_t>(_tasks.size());
}

void
R5RThreadPool::waitForWork() noexcept
{
    while (hasWork()) {}
}

void
R5RThreadPool::startAll()
{
    for (size_t i = 0; i < _numThreads; ++i) {
        _threads[i] = std::thread([=]() {
            while (true) {
                ThreadPoolTask task;

                {
                    std::unique_lock<std::mutex> lock(_eventMutex);
                    _eventVar.wait(lock, [=]() { return _stopped.load() || !_tasks.empty(); });

                    if (_stopped.load()) { break; }

                    task = std::move(_tasks.front());
                    _tasks.pop();
                    _numBusyThreads.store(_numBusyThreads.load() + 1);
                }

                task(_threadsMemory[i]);

                {
                    std::unique_lock<std::mutex> lock(_eventMutex);
                    _numBusyThreads.store(_numBusyThreads.load() - 1);
                }
            }
        });
    }
}

void
R5RThreadPool::stopAll() noexcept
{
    {
        std::unique_lock<std::mutex> lock(_eventMutex);
        _stopped.store(true);
    }

    _eventVar.notify_all();

    for (size_t i = 0; i < _numThreads; ++i) {
        _threads[i].join();
        _threadsMemory[i].popAllFrameMemory();
    }
}