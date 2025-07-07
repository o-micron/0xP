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

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wall"
    #pragma clang diagnostic ignored "-Weverything"
#endif
#include <boost/pool/object_pool.hpp>
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

#define XP_MPL_MEMORY_POOL(T) friend class boost::object_pool<T>;

// MemoryPool that has:
// next_size Number of chunks to request from the system the next time that object needs to allocate system memory
// (default 32). next_size != 0. max_size Maximum number of chunks to ever request from the system - this puts a cap on
// the doubling algorithm used by the underlying pool.
template<typename T>
struct XPMemoryPool
{
    friend class Store;
    friend struct SceneStore;

    // testing
    friend struct XPMemoryPoolObject;
    friend class XPMemoryPoolTests;

    // allocates memory then calls constructor for T
    template<typename... ARGS>
    T* create(ARGS... args)
    {
        return pool.construct(args...);
    }

    // calls destructor of T then deallocates memory
    void destroy(T* item) { pool.destroy(item); }

    // only allocates memory, doesn't call constructor for T
    T* allocate() { return pool.malloc(); }

    // doesn't call destructor of T, only deallocates memory
    void deallocate(T* item) { pool.free(item); }

    // get the reserved chunk size
    size_t getNextChunkSize() const { return pool.get_next_size(); }

    XPMemoryPool(size_t nextChunkSize, size_t maxNextChunkSize)
      : pool(nextChunkSize, maxNextChunkSize)
    {
    }

    ~XPMemoryPool() = default;

  private:
    boost::object_pool<T> pool;
};
