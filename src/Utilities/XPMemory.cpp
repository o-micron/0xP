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

#include <Utilities/XPMemory.h>

#ifdef __APPLE__
    #include <assert.h>
    #include <mach/mach_init.h>
    #include <mach/vm_map.h>
    #include <unistd.h>
#elif defined(__EMSCRIPTEN__)
    #include <assert.h>
    #include <sys/mman.h>
    #include <unistd.h>
#elif defined(WIN32)
    #include <Windows.h>
    #include <assert.h>
#else
    #error "Unknown Platform"
#endif

extern int
XPGetMemoryPageSize()
{
#ifdef __APPLE__
    return getpagesize();
#elif defined(__EMSCRIPTEN__)
    return sysconf(_SC_PAGESIZE);
#elif defined(WIN32)
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwPageSize;
#endif
}

extern void*
XPAllocateAlignedVirtualMemory(size_t numBytes)
{
    // In debug builds, check that we have
    // correct VM page alignment
    assert(numBytes != 0);
    assert((numBytes % XPGetMemoryPageSize()) == 0);

#ifdef __APPLE__
    char*         data;
    kern_return_t err;

    // Allocate directly from VM
    err = vm_allocate((vm_map_t)mach_task_self(), (vm_address_t*)&data, numBytes, VM_FLAGS_ANYWHERE);

    // Check errors
    assert(err == KERN_SUCCESS);
    if (err != KERN_SUCCESS) { data = NULL; }

    return data;
#elif defined(__EMSCRIPTEN__)
    void* data = mmap(NULL, numBytes, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    assert(data != nullptr);
    return data;
#elif defined(WIN32)
    void* data = nullptr;
    data       = VirtualAlloc(NULL,                                     // System selects address
                        numBytes,                                 // Size of allocation
                        MEM_RESERVE | MEM_COMMIT | MEM_64K_PAGES, // Allocate reserved pages
                        PAGE_READWRITE);
    assert(data != nullptr);
    memset(data, 0, numBytes);
    return data;
#endif
}

extern void
XPDeallocateAlignedVirtualMemory(void* address, size_t numBytes)
{
#ifdef __APPLE__
    kern_return_t err;
    err = vm_deallocate((vm_map_t)mach_task_self(), *(vm_address_t*)&address, numBytes);
    assert(err == KERN_SUCCESS);
#elif defined(__EMSCRIPTEN__)
    munmap(address, numBytes);
#elif defined(WIN32)
    VirtualFree(address,      // Base address of block
                numBytes,     // Bytes of committed pages
                MEM_RELEASE); // Decommit the pages
#endif
}

extern bool
XPIsAligned(const void* ptr, size_t alignment)
{
    return ((uintptr_t)ptr % alignment) == 0;
}

void*
XPAlignPointer(void* ptr, size_t alignment)
{
    uintptr_t addr         = (uintptr_t)ptr;
    uintptr_t aligned_addr = (addr + alignment - 1) & ~(alignment - 1);
    return (void*)aligned_addr;
}