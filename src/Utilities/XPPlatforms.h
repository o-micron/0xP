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

// ------------------------------------------------------------------
// BUILD VARIANTS
// ------------------------------------------------------------------
// - Release
// - Debug
// - RelWithDebInfo
// - MinSizeRel
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// SUPPORTED PLATFORMS
// ------------------------------------------------------------------
// XP_PLATFORM_WINDOWS             - Windows
// XP_PLATFORM_MACOS               - Mac OS X
// XP_PLATFORM_LINUX               - Linux
// XP_PLATFORM_EMSCRIPTEN          - WebGL 2.0
// ------------------------------------------------------------------

#if defined(XP_CONFIG_Release)
    #define XP_CONFIG_NAME "Release"
    #define XP_BUILD_RELEASE
#elif defined(XP_CONFIG_Debug)
    #define XP_CONFIG_NAME "Debug"
    #define XP_BUILD_DEBUG
#elif defined(XP_CONFIG_RelWithDebInfo)
    #define XP_CONFIG_NAME "RelWithDebInfo"
    #define XP_BUILD_RELWITHDEBINFO
#elif defined(XP_CONFIG_MinSizeRel)
    #define XP_CONFIG_NAME "MinSizeRel"
    #define XP_BUILD_MINSIZEREL
#else
    #error "Build config not supported"
#endif

#if defined(__EMSCRIPTEN__)
    #define XP_PLATFORM_NAME "EMSCRIPTEN"
    #define XP_PLATFORM_EMSCRIPTEN
    #define XP_FORCE_INLINE inline
    #define XP_FORCE_NO_INLINE
    #define XP_DYNAMIC_LIBRARY_PREFIX    "lib"
    #define XP_DYNAMIC_LIBRARY_EXTENSION ".so"
    #define XP_DYNAMIC_FN_CALL
    #define XP_DLL_HANDLE_TYPE            void*
    #define XP_DLL_OPEN(A, B)             dlopen(A, B)
    #define XP_DLL_GET_PROC_ADDRESS(A, B) dlsym(A, B)
    #define XP_DLL_CLOSE(A)               dlclose(A)
    #define XP_FUNCTION_NAME              __func__
    #define XP_PURE_ABSTRACT
    #define XP_NEW    new
    #define XP_DELETE delete
#elif defined(_WIN32) || defined(_WIN64)
    #define XP_PLATFORM_NAME "WINDOWS"
    #define XP_PLATFORM_WINDOWS
    #define XP_FORCE_INLINE               __forceinline
    #define XP_FORCE_NO_INLINE            _declspec(noinline)
    #define XP_DYNAMIC_LIBRARY_PREFIX     ""
    #define XP_DYNAMIC_LIBRARY_EXTENSION  ".dll"
    #define XP_DYNAMIC_FN_CALL            __stdcall
    #define XP_DLL_HANDLE_TYPE            HINSTANCE
    #define XP_DLL_OPEN(A, B)             LoadLibrary(A)
    #define XP_DLL_GET_PROC_ADDRESS(A, B) GetProcAddress(A, B)
    #define XP_DLL_CLOSE(A)               FreeLibrary(A)
    #define XP_FUNCTION_NAME              __FUNCTION__
    #define XP_PURE_ABSTRACT              __declspec(novtable)
    #define XP_NEW                        new
    #define XP_DELETE                     delete
    #if defined(XP_BUILD_RELEASE) || defined(XP_BUILD_RELWITHDEBINFO) || defined(XP_BUILD_MINSIZEREL)
        #ifndef NDEBUG
            #define NDEBUG
        #endif
    #elif defined(XP_BUILD_DEBUG)
        #ifndef _DEBUG
            #define _DEBUG
        #endif
    #endif
#elif defined(__linux__)
    #define XP_PLATFORM_NAME "LINUX"
    #define XP_PLATFORM_LINUX
    #define XP_FORCE_INLINE inline
    #define XP_FORCE_NO_INLINE
    #define XP_DYNAMIC_LIBRARY_PREFIX    "lib"
    #define XP_DYNAMIC_LIBRARY_EXTENSION ".so"
    #define XP_DYNAMIC_FN_CALL
    #define XP_DLL_HANDLE_TYPE            void*
    #define XP_DLL_OPEN(A, B)             dlopen(A, B)
    #define XP_DLL_GET_PROC_ADDRESS(A, B) dlsym(A, B)
    #define XP_DLL_CLOSE(A)               dlclose(A)
    #define XP_FUNCTION_NAME              __func__
    #define XP_PURE_ABSTRACT
    #define XP_NEW    new
    #define XP_DELETE delete
#elif defined(__APPLE__)
    #define XP_PLATFORM_NAME "MACOS"
    #define XP_PLATFORM_MACOS
    #define XP_FORCE_INLINE inline
    #define XP_FORCE_NO_INLINE
    #define XP_DYNAMIC_LIBRARY_PREFIX    "lib"
    #define XP_DYNAMIC_LIBRARY_EXTENSION ".dylib"
    #define XP_DYNAMIC_FN_CALL
    #define XP_DLL_HANDLE_TYPE            void*
    #define XP_DLL_OPEN(A, B)             dlopen(A, B)
    #define XP_DLL_GET_PROC_ADDRESS(A, B) dlsym(A, B)
    #define XP_DLL_CLOSE(A)               dlclose(A)
    #define XP_FUNCTION_NAME              __func__
    #define XP_PURE_ABSTRACT
    #define XP_NEW    new
    #define XP_DELETE delete
    #if defined(XP_BUILD_RELEASE) || defined(XP_BUILD_RELWITHDEBINFO) || defined(XP_BUILD_MINSIZEREL)
        #ifndef NDEBUG
            #define NDEBUG
        #endif
    #elif defined(XP_BUILD_DEBUG)
        #ifndef _DEBUG
            #define _DEBUG
        #endif
    #endif
#else
    #error "Platform not supported."
#endif

#if defined(__cplusplus)
    #define XP_EXTERN extern "C"
#else
    #define XP_EXTERN extern
#endif

#if defined(_MSC_VER)
    #include <intrin.h>
    #define XPDebugBreak() __debugbreak();
#else
    #define XPDebugBreak() __builtin_trap();
#endif

#if defined(XP_PLATFORM_EMSCRIPTEN) && defined(XP_ENGINE_PROFILER_EXPORT_DYNAMIC)
    #define XP_ENGINE_PROFILER_API __attribute__((visibility("default")))
#elif defined(XP_PLATFORM_WINDOWS) && defined(XP_ENGINE_PROFILER_EXPORT_DYNAMIC)
    #define XP_ENGINE_PROFILER_API __declspec(dllexport)
#elif defined(XP_PLATFORM_WINDOWS) && defined(XP_ENGINE_IMPORT_DYNAMIC)
    #define XP_ENGINE_PROFILER_API __declspec(dllimport)
#elif defined(XP_PLATFORM_MACOS) && defined(XP_ENGINE_PROFILER_EXPORT_DYNAMIC)
    #define XP_ENGINE_PROFILER_API __attribute__((visibility("default")))
#elif defined(XP_PLATFORM_LINUX) && defined(XP_ENGINE_PROFILER_EXPORT_DYNAMIC)
    #define XP_ENGINE_PROFILER_API __attribute__((visibility("default")))
#else
    #define XP_ENGINE_PROFILER_API
#endif