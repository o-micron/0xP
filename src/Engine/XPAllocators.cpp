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

#include <Engine/XPAllocators.h>

#include <Utilities/XPLogger.h>
#include <Utilities/XPMemory.h>

#include <assert.h>

#if defined(XP_PLATFORM_MACOS)
    #include <errno.h>
    #include <signal.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <sys/mman.h>
    #include <unistd.h>
#elif defined(XP_PLATFORM_WINDOWS)
    #include <stdio.h>
    #include <stdlib.h>
    #include <tchar.h>
    #include <windows.h>
#else
    #error "Platform not supported"
#endif

#if defined(XP_PLATFORM_MACOS)
static void
handler(int sig, siginfo_t* si, void* unused)
{
    XP_LOGV(XPLoggerSeverityFatal, "Got SIGSEGV at address: 0x%lx\n", (long)si->si_addr);
}
#endif

XPAllocators::XPAllocators(size_t totalNumBytes)
{
    _bufferStart = nullptr;
    _buffer      = nullptr;
    _bufferEnd   = nullptr;

#if defined(XP_PLATFORM_MACOS)
    char*            p;
    int              pagesize;
    struct sigaction sa;

    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = handler;
    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        XP_LOG(XPLoggerSeverityFatal, "sigaction");
        return;
    }

    pagesize = sysconf(_SC_PAGE_SIZE);
    if (pagesize == -1) {
        XP_LOG(XPLoggerSeverityFatal, "sysconf");
        return;
    }

    unsigned int numPages = 0;
    size_t       numBytes = 0;
    while (numBytes < totalNumBytes) {
        numBytes += pagesize;
        ++numPages;
    }

    size_t allocatedBytes = numPages * pagesize;

    // Allocate a buffer aligned on a page boundary
    // Initial protection is PROT_READ | PROT_WRITE
    if (posix_memalign((void**)&_bufferStart, pagesize, allocatedBytes) != 0) {
        XP_LOG(XPLoggerSeverityFatal, "posix_memalign");
        return;
    }

    assert(_bufferStart != nullptr);

    _buffer    = _bufferStart;
    _bufferEnd = _bufferStart + allocatedBytes;
#elif defined(XP_PLATFORM_WINDOWS)
    int pageSize = XPGetMemoryPageSize();

    unsigned int numPages = 0;
    size_t       numBytes = 0;
    while (numBytes < totalNumBytes) {
        numBytes += pageSize;
        ++numPages;
    }

    size_t allocatedBytes = numPages * pageSize;
    _bufferStart          = (unsigned char*)XPAllocateAlignedVirtualMemory(allocatedBytes);

    if (_bufferStart == nullptr) {
        XP_LOG(XPLoggerSeverityFatal, "VirtualAlloc failed");
        return;
    }

    _buffer    = _bufferStart;
    _bufferEnd = _bufferStart + allocatedBytes;
#endif
}

XPAllocators::~XPAllocators()
{
#if defined(XP_PLATFORM_MACOS)
    if (_bufferStart) {
        free(_bufferStart);
        _bufferStart = nullptr;
        _buffer      = nullptr;
        _bufferEnd   = nullptr;
    }
#elif defined(XP_PLATFORM_WINDOWS)
    if (_bufferStart) {
        XPDeallocateAlignedVirtualMemory((void*)_bufferStart, _bufferEnd - _bufferStart);
        _bufferStart = nullptr;
        _buffer      = nullptr;
        _bufferEnd   = nullptr;
    }
#endif
}

unsigned char*
XPAllocators::allocate(size_t numBytes)
{
#if defined(XP_PLATFORM_MACOS)
    if (_buffer + numBytes > _bufferEnd) {
        XP_LOG(XPLoggerSeverityBreakPoint, "out of memory pages free memory");
        return nullptr;
    }
    unsigned char* out = _buffer;
    _buffer += numBytes;
    return out;
#elif defined(XP_PLATFORM_WINDOWS)
    if (_buffer + numBytes > _bufferEnd) {
        XP_LOG(XPLoggerSeverityBreakPoint, "out of memory pages free memory");
        return nullptr;
    }
    unsigned char* out = _buffer;
    _buffer += numBytes;
    return out;
#else
    #error "Unsupported platform"
#endif
}