cmake_minimum_required(VERSION 4.0)
if(EMSCRIPTEN)
    set(XP_EXE_NAME XPENGINE)
    project(${XP_EXE_NAME} VERSION 0.1.0 LANGUAGES C CXX)
elseif(WIN32)
    set(XP_EXE_NAME XPENGINE)
    set (CMAKE_SYSTEM_VERSION 10.0.22621.0)
    project(${XP_EXE_NAME} VERSION 0.1.0 LANGUAGES C CXX)
elseif(APPLE)
    set(XP_EXE_NAME XPENGINE)
    project(${XP_EXE_NAME} VERSION 0.1.0 LANGUAGES C CXX OBJC OBJCXX)
endif()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(XP_EDITOR_MODE)
    add_compile_definitions(XP_EDITOR_MODE)
endif()

if(XP_METAL_RENDERER_USE_SHADERS_FROM_SOURCE) 
    add_compile_definitions(XP_METAL_RENDERER_USE_SHADERS_FROM_SOURCE)
endif()
if(XP_VULKAN_RENDERER_USE_SHADERS_FROM_SOURCE)
    add_compile_definitions(XP_VULKAN_RENDERER_USE_SHADERS_FROM_SOURCE)
endif()
if(XP_VULKAN_VALIDATION_LAYERS)
    add_compile_definitions(XP_VULKAN_VALIDATION_LAYERS)
endif()
if (XP_VULKAN_DEBUG_UTILS)
    add_compile_definitions(XP_VULKAN_DEBUG_UTILS)
endif()

if(XP_RENDERER_DX12)
    add_compile_definitions(XP_RENDERER_DX12)
elseif(XP_RENDERER_METAL)
    add_compile_definitions(XP_RENDERER_METAL)
elseif(XP_RENDERER_VULKAN)
    add_compile_definitions(XP_RENDERER_VULKAN)
elseif(XP_RENDERER_WGPU)
    add_compile_definitions(XP_RENDERER_WGPU)
endif()
if(XP_RENDERER_SW)
    add_compile_definitions(XP_RENDERER_SW)
    if(XP_SW_USE_THREADS)
        add_compile_definitions(XP_SW_USE_THREADS)
    endif()
    if(XP_SW_RASTERIZER_ENABLE)
        add_compile_definitions(XP_SW_RASTERIZER_ENABLE)
    endif()
    if(XP_SW_RAYTRACER_ENABLE)
        add_compile_definitions(XP_SW_RAYTRACER_ENABLE)
    endif()
    if(XP_SW_RASTERIZER_RAYTRACED_SHADOWS)
        add_compile_definitions(XP_SW_RASTERIZER_RAYTRACED_SHADOWS)
    endif()
endif()
if(XP_USE_COMPUTE)
    add_compile_definitions(XP_USE_COMPUTE)
    if(XP_USE_COMPUTE_CUDA)
        add_compile_definitions(XP_USE_COMPUTE_CUDA)
    endif()
    if(XP_USE_COMPUTE_METAL)
        add_compile_definitions(XP_USE_COMPUTE_METAL)
    endif()
    if(XP_USE_COMPUTE_VULKAN)
        add_compile_definitions(XP_USE_COMPUTE_VULKAN)
    endif()
    if(XP_USE_COMPUTE_WGPU)
        add_compile_definitions(XP_USE_COMPUTE_WGPU)
    endif()
endif()
if(TODO_ENABLE_RENDER_TO_TEXTURES)
    add_compile_definitions(TODO_ENABLE_RENDER_TO_TEXTURES)
endif(TODO_ENABLE_RENDER_TO_TEXTURES)

add_compile_definitions(IMGUI_DEFINE_MATH_OPERATORS)
add_compile_definitions(IMGUI_DISABLE_OBSOLETE_FUNCTIONS)

if(XP_PHYSICS_BULLET)
    add_compile_definitions(XP_PHYSICS_BULLET)
elseif(XP_PHYSICS_JOLT)
    add_compile_definitions(XP_PHYSICS_JOLT)
    add_compile_definitions(JPH_ENABLE_ASSERTS)
    add_compile_definitions(JPH_OBJECT_STREAM)
    # IF DEBUGGING ON WINDOWS #
    # add_compile_definitions(JPH_FLOATING_POINT_EXCEPTIONS_ENABLED)
    # add_compile_definitions(JPH_DEBUG_RENDERER)
elseif(XP_PHYSICS_PHYSX4)
    add_compile_definitions(XP_PHYSICS_PHYSX4)
elseif(XP_PHYSICS_PHYSX5)
    add_compile_definitions(XP_PHYSICS_PHYSX5)
endif()
if(XP_DEBUG_PHYSX)
    add_compile_definitions(XP_DEBUG_PHYSX)
endif()
if(XP_MCP_SERVER)
    add_compile_definitions(XP_MCP_SERVER)
endif()

add_compile_definitions(XP_CONFIG_$<CONFIG>)

include(cmake/cross_platforms.cmake)
if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    include(cmake/annotations.cmake)
    if (XP_USE_COMPUTE)
        add_subdirectory(src/Compute)
    endif()
    include(cmake/emscripten.cmake)
elseif(APPLE)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    include(cmake/CPM.cmake)
    include(cmake/annotations.cmake)
    if (XP_USE_COMPUTE)
        add_subdirectory(src/Compute)
    endif()
    include(cmake/macOS.cmake)
    add_subdirectory(src/Emulator)
elseif(WIN32)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    include(cmake/CPM.cmake)
    include(cmake/annotations.cmake)
    if (XP_USE_COMPUTE)
        add_subdirectory(src/Compute)
    endif()
    include(cmake/windows.cmake)
    add_subdirectory(src/Emulator)
endif()
