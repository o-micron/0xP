# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# set(CMAKE_XCODE_GENERATE_SCHEME YES)
# set(CMAKE_XCODE_SCHEME_ADDRESS_SANITIZER NO)
# set(CMAKE_XCODE_SCHEME_THREAD_SANITIZER NO)
# set(CMAKE_XCODE_SCHEME_UNDEFINED_BEHAVIOUR_SANITIZER NO)
# set(CMAKE_XCODE_SCHEME_ENABLE_GPU_FRAME_CAPTURE_MODE "Metal")
# set(CMAKE_XCODE_SCHEME_ENABLE_GPU_API_VALIDATION YES)
# set(CMAKE_XCODE_SCHEME_ENABLE_GPU_SHADER_VALIDATION YES)
# set(CMAKE_XCODE_ATTRIBUTE_COMPILER_INDEX_STORE_ENABLE NO)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

# add_compile_definitions(_ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH=0)
# add_compile_definitions(_HAS_ITERATOR_DEBUGGING=0)
# add_compile_definitions(_ITERATOR_DEBUG_LEVEL=0)
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MT")
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# SOURCES
# ---------------------------------------------------------------------------------------------------------------------------------------------------
set(XPENGINE_SOURCES_DATA_PIPELINE ${XPENGINE_SOURCES_DATA_PIPELINE}
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPFileWatchWindows.cpp
)
set(XPENGINE_HEADERS_DATA_PIPELINE ${XPENGINE_HEADERS_DATA_PIPELINE}
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPFileWatchWindows.h
)
if(XP_EDITOR_MODE)
    set(IMGUI_SOURCES ${IMGUI_SOURCES}
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/client/Private/NetImgui_NetworkWin32.cpp
    )
endif()
set_source_files_properties(${IMGUI_SOURCES} PROPERTIES COMPILE_FLAGS "-w")
set_source_files_properties(${XPEDITOR_SOURCES} PROPERTIES COMPILE_FLAGS "-w")
set(XPENGINE_SOURCES
    ${XPENGINE_SOURCES_CONTROLLERS}
    ${XPENGINE_SOURCES_DATA_PIPELINE}
    ${XPENGINE_SOURCES_ENGINE}
    ${XPENGINE_SOURCES_GAME}
    ${XPENGINE_SOURCES_LANG}
    ${XPENGINE_SOURCES_MCP}
    ${XPENGINE_SOURCES_PAINTING}
    ${XPENGINE_SOURCES_PHYSICS}
    ${XPENGINE_SOURCES_RENDERER}
    ${XPENGINE_SOURCES_SCENE_DESCRIPTOR}
    ${XPENGINE_SOURCES_SHORTCUTS}
    ${XPENGINE_SOURCES_UI}
    ${XPENGINE_SOURCES_UTILITIES}
)
set(XPENGINE_HEADERS
    ${XPENGINE_HEADERS_COMPILERS}
    ${XPENGINE_HEADERS_CONTROLLERS}
    ${XPENGINE_HEADERS_DATA_PIPELINE}
    ${XPENGINE_HEADERS_ENGINE}
    ${XPENGINE_HEADERS_GAME}
    ${XPENGINE_HEADERS_LANG}
    ${XPENGINE_HEADERS_MCP}
    ${XPENGINE_HEADERS_PAINTING}
    ${XPENGINE_HEADERS_PHYSICS}
    ${XPENGINE_HEADERS_RENDERER}
    ${XPENGINE_HEADERS_SCENE_DESCRIPTOR}
    ${XPENGINE_HEADERS_SHORTCUTS}
    ${XPENGINE_HEADERS_UI}
    ${XPENGINE_HEADERS_UTILITIES}
)
set(XPTHIRDPARTY_LINK_LIBRARIES ${XPTHIRDPARTY_LINK_LIBRARIES}
    assimp-vc143-mt # $<IF:$<CONFIG:Debug>,assimp-vc143-mtd,assimp-vc143-mt>
    SDL2-static # $<IF:$<CONFIG:Debug>,SDL2-staticd,SDL2-static>
    SDL2main # $<IF:$<CONFIG:Debug>,SDL2maind,SDL2main>
    spirv-cross-c-shared # $<IF:$<CONFIG:Debug>,spirv-cross-c-sharedd,spirv-cross-c-shared>
    Setupapi
    Version
    winmm
    imagehlp
    dinput8
    dxguid
    user32
    gdi32
    winmm
    imm32
    ole32
    oleaut32
    shell32
    uuid
    d3d12
    dxgi
    d3dcompiler
)
# ---------------------------------------------------------------------------------------------------------------------------------------------------


# ---------------------------------------------------------------------------------------------------------------------------------------------------
# TARGET
# ---------------------------------------------------------------------------------------------------------------------------------------------------
if (XP_CMAKE_VISUAL_STUDIO)
    add_executable(${XP_EXE_NAME}
        ${CMAKE_SOURCE_DIR}/src/Editor/main.cpp
        ${XPENGINE_SOURCES} ${XPENGINE_HEADERS}
        ${IMGUI_SOURCES} ${IMGUI_GFX_API_SOURCES}
    )
else ()
    add_executable(${XP_EXE_NAME}
        ${CMAKE_SOURCE_DIR}/src/Editor/main.cpp
        ${XPENGINE_SOURCES} ${XPENGINE_HEADERS}
        ${IMGUI_SOURCES} ${IMGUI_GFX_API_SOURCES}
    )
endif ()
target_include_directories(${XP_EXE_NAME} PRIVATE ${XPTHIRDPARTY_INCLUDE_DIRECTORIES})
target_link_directories(${XP_EXE_NAME} PRIVATE ${XPTHIRDPARTY_LINK_DIRECTORIES})
target_link_libraries(${XP_EXE_NAME} PRIVATE ${XPTHIRDPARTY_LINK_LIBRARIES} XPProfiler)
set_property(TARGET ${XP_EXE_NAME}
    PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

if (XP_USE_COMPUTE)
    target_link_libraries(${XP_EXE_NAME} PRIVATE XPCompute)
endif()

if (XP_CMAKE_VISUAL_STUDIO)
    GroupSourcesByFolder(${XP_EXE_NAME} "${XPENGINE_RESOURCE_FILES}")
    set_target_properties(
        ${XP_EXE_NAME}
        PROPERTIES
        RESOURCE "${XPENGINE_SURFACE_SHADER_SOURCES};${XPENGINE_COMPUTE_SHADER_SOURCES};${ASSETS_GLTF_SAMPLE_MODELS};${ASSETS_GLTF_SAMPLE_MODEL_TEXTURES};${ASSETS_FONTS};${ASSETS_MESHES};${ASSETS_SCENES};${ASSETS_TEXTURES}"
    )
endif ()
# ---------------------------------------------------------------------------------------------------------------------------------------------------


# ---------------------------------------------------------------------------------------------------------------------------------------------------
# TESTS
# ---------------------------------------------------------------------------------------------------------------------------------------------------
if(XP_BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG main
    )

    if(EMSCRIPTEN)
    elseif(WIN32)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()

    FetchContent_MakeAvailable(googletest)

    file(GLOB_RECURSE XP_TEST_SOURCES ${CMAKE_SOURCE_DIR}/tests/*.cpp)
    enable_testing()
    add_executable(XPTests ${XP_TEST_SOURCES} ${XPENGINE_SOURCES} ${XPENGINE_HEADERS} ${IMGUI_SOURCES} ${IMGUI_GFX_API_SOURCES})
    if (XP_CMAKE_VISUAL_STUDIO)
        GroupSourcesByFolder(XPTests)
    endif ()
    target_include_directories(XPTests PRIVATE ${XPTHIRDPARTY_INCLUDE_DIRECTORIES})
    target_link_directories(XPTests PRIVATE ${XPTHIRDPARTY_LINK_DIRECTORIES})
    target_link_libraries(XPTests PRIVATE GTest::gtest_main ${XPTHIRDPARTY_LINK_LIBRARIES})
    set_property(TARGET XPTests
        PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
    include(GoogleTest)
    gtest_discover_tests(XPTests)
endif(XP_BUILD_TESTS)
# ---------------------------------------------------------------------------------------------------------------------------------------------------


# ---------------------------------------------------------------------------------------------------------------------------------------------------
# BENCHMARKS
# ---------------------------------------------------------------------------------------------------------------------------------------------------
if(XP_BUILD_BENCHMARKS)
    set(BENCHMARK_DOWNLOAD_DEPENDENCIES ON)
    include(FetchContent)
    FetchContent_Declare(
        benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG main
    )
    set(BENCHMARK_ENABLE_TESTING OFF)
    FetchContent_MakeAvailable(benchmark)
    add_executable(XPBenchmarks ${CMAKE_SOURCE_DIR}/benchmarks/XPBenchmarks.cpp ${XPENGINE_SOURCES} ${XPENGINE_HEADERS} ${IMGUI_SOURCES} ${IMGUI_GFX_API_SOURCES})
    if (XP_CMAKE_VISUAL_STUDIO)
        GroupSourcesByFolder(XPBenchmarks)
    endif()
    target_include_directories(XPBenchmarks PRIVATE ${XPTHIRDPARTY_INCLUDE_DIRECTORIES})
    target_link_directories(XPBenchmarks PRIVATE ${XPTHIRDPARTY_LINK_DIRECTORIES})
    target_link_libraries(XPBenchmarks PRIVATE benchmark::benchmark ${XPTHIRDPARTY_LINK_LIBRARIES})
    set_property(TARGET XPBenchmarks
        PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
endif(XP_BUILD_BENCHMARKS)
# ---------------------------------------------------------------------------------------------------------------------------------------------------


# ---------------------------------------------------------------------------------------------------------------------------------------------------
# HEADER TOOL
# ---------------------------------------------------------------------------------------------------------------------------------------------------
file(GLOB XP_ATTACHMENT_HEADERS ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/Attachments/*.h)
set(XP_HEADER_TOOL_COMMAND ${CMAKE_SOURCE_DIR}/artifacts/windows/compiler/bin/xp-header-tool.exe)
JOIN("${XPENGINE_HEADER_TOOL_FILES}" "," XPENGINE_HEADER_TOOL_FILES_JOINED)
add_custom_target(XPHeaderTool
    COMMAND ${XP_HEADER_TOOL_COMMAND} --enable-xp-header-tool=1 --xp-header-tool-debug=0 --xp-header-tool-json-filename=${CMAKE_BINARY_DIR}/XPHeaderTool.json --xp-header-tool-template-and-output-files=${XPENGINE_HEADER_TOOL_FILES_JOINED} -p=${CMAKE_BINARY_DIR}/compile_commands.json ${XP_ATTACHMENT_HEADERS}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "[Running XPHeaderTool]"
)
# ---------------------------------------------------------------------------------------------------------------------------------------------------


# ---------------------------------------------------------------------------------------------------------------------------------------------------
# FORMAT SOURCE FILES
# ---------------------------------------------------------------------------------------------------------------------------------------------------
set(XP_FORMAT_COMMANDS )
foreach(VALUE ${XPENGINE_SOURCES})
    list(APPEND XP_FORMAT_COMMANDS COMMAND ${CMAKE_SOURCE_DIR}/artifacts/windows/compiler/bin/clang-format.exe -style=file -i ${VALUE})
endforeach()
foreach(VALUE ${XPENGINE_HEADERS})
    list(APPEND XP_FORMAT_COMMANDS COMMAND ${CMAKE_SOURCE_DIR}/artifacts/windows/compiler/bin/clang-format.exe -style=file -i ${VALUE})
endforeach()
foreach(VALUE ${XPENGINE_HEADER_TOOL_FILES})
    list(APPEND XP_FORMAT_COMMANDS COMMAND ${CMAKE_SOURCE_DIR}/artifacts/windows/compiler/bin/clang-format.exe -style=file -i ${VALUE})
endforeach()
add_custom_target(XPFormat
    ${XP_FORMAT_COMMANDS}
    COMMENT "[Running XPFormat]"
    DEPENDS XPHeaderTool
)
add_dependencies(${XP_EXE_NAME} XPFormat XPHeaderTool)
# ---------------------------------------------------------------------------------------------------------------------------------------------------


# ---------------------------------------------------------------------------------------------------------------------------------------------------
# COPY ASSETS
# ---------------------------------------------------------------------------------------------------------------------------------------------------
add_dependencies(XPCompute XPXPUCompute)
add_dependencies(XPInstallRiscvBinaries XPXPUCompute)
add_dependencies(${XP_EXE_NAME} XPInstallMeshes XPInstallScenes XPInstallTextures XPInstallFonts XPInstallRiscvBinaries XPXPUCompute)
# ---------------------------------------------------------------------------------------------------------------------------------------------------


# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Vulkan SHADERS
# ---------------------------------------------------------------------------------------------------------------------------------------------------
if(XP_RENDERER_VULKAN)
    set(XPVulkanShaders )
    set(XPVulkanLibOutputDir ${CMAKE_BINARY_DIR}/$<CONFIG>/shaders/)
    set(VULKAN_SURFACE_SHADERS ${XPENGINE_SURFACE_SHADER_SOURCES})
    foreach(VULKAN_SURFACE_SHADER ${VULKAN_SURFACE_SHADERS})
        get_filename_component(VULKAN_SURFACE_SHADER_DIR ${VULKAN_SURFACE_SHADER} DIRECTORY)    
        get_filename_component(VULKAN_SURFACE_SHADER_FILENAME ${VULKAN_SURFACE_SHADER} NAME)
        get_filename_component(VULKAN_SURFACE_SHADER_WITHOUT_EXTENSION ${VULKAN_SURFACE_SHADER} NAME_WE)
        message(STATUS "[VULKAN] [${VULKAN_SURFACE_SHADER_WITHOUT_EXTENSION}] ${VULKAN_SURFACE_SHADER_FILENAME}")
        list(APPEND XPVulkanShaders COMMAND ${CMAKE_COMMAND} -E copy ${VULKAN_SURFACE_SHADER} ${CMAKE_BINARY_DIR}/$<CONFIG>/shaders/${VULKAN_SURFACE_SHADER_FILENAME})
        list(APPEND XPVulkanShaders COMMAND ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/scripts/compile_vertex_fragment.bat ${VULKAN_SURFACE_SHADER_DIR} ${VULKAN_SURFACE_SHADER_WITHOUT_EXTENSION} ${XPVulkanLibOutputDir})
    endforeach()
    set(VULKAN_COMPUTE_SHADERS ${XPENGINE_COMPUTE_SHADER_SOURCES})
    foreach(VULKAN_COMPUTE_SHADER ${VULKAN_COMPUTE_SHADERS})
        get_filename_component(VULKAN_COMPUTE_SHADER_DIR ${VULKAN_COMPUTE_SHADER} DIRECTORY)
        get_filename_component(VULKAN_COMPUTE_SHADER_FILENAME ${VULKAN_COMPUTE_SHADER} NAME)
        get_filename_component(VULKAN_COMPUTE_SHADER_WITHOUT_EXTENSION ${VULKAN_COMPUTE_SHADER} NAME_WE)
        message(STATUS "[VULKAN] [${VULKAN_COMPUTE_SHADER_WITHOUT_EXTENSION}] ${VULKAN_COMPUTE_SHADER_FILENAME}")
        list(APPEND XPVulkanShaders COMMAND ${CMAKE_COMMAND} -E copy ${VULKAN_COMPUTE_SHADER} ${CMAKE_BINARY_DIR}/$<CONFIG>/shaders/${VULKAN_COMPUTE_SHADER_FILENAME})
        list(APPEND XPVulkanShaders COMMAND ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/scripts/compile_compute.bat ${VULKAN_COMPUTE_SHADER_DIR} ${VULKAN_COMPUTE_SHADER_WITHOUT_EXTENSION} ${XPVulkanLibOutputDir})
    endforeach()
    add_custom_target(XPCompileVulkanShaders
        ${XPVulkanShaders}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/XPShaderTypes.h ${CMAKE_BINARY_DIR}/$<CONFIG>/shaders/XPShaderTypes.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "[COMPILING ALL VULKAN SHADERS]"
    )
    add_dependencies(${XP_EXE_NAME} XPCompileVulkanShaders)
endif()
# ---------------------------------------------------------------------------------------------------------------------------------------------------


# ---------------------------------------------------------------------------------------------------------------------------------------------------
# DX12 SHADERS
# ---------------------------------------------------------------------------------------------------------------------------------------------------
if(XP_RENDERER_DX12)
    set(XPDX12Shaders )
    set(XPDX12LibOutputDir ${CMAKE_BINARY_DIR}/$<CONFIG>/shaders/)
    set(DX12_SURFACE_SHADERS ${XPENGINE_SURFACE_SHADER_SOURCES})
    foreach(DX12_SURFACE_SHADER ${DX12_SURFACE_SHADERS})
        get_filename_component(DX12_SURFACE_SHADER_DIR ${DX12_SURFACE_SHADER} DIRECTORY)    
        get_filename_component(DX12_SURFACE_SHADER_FILENAME ${DX12_SURFACE_SHADER} NAME)
        get_filename_component(DX12_SURFACE_SHADER_WITHOUT_EXTENSION ${DX12_SURFACE_SHADER} NAME_WE)
        message(STATUS "[DX12] [${DX12_SURFACE_SHADER_WITHOUT_EXTENSION}] ${DX12_SURFACE_SHADER_FILENAME}")
        list(APPEND XPDX12Shaders COMMAND ${CMAKE_COMMAND} -E copy ${DX12_SURFACE_SHADER} ${CMAKE_BINARY_DIR}/$<CONFIG>/shaders/${DX12_SURFACE_SHADER_FILENAME})
        list(APPEND XPDX12Shaders COMMAND ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/scripts/compile_vertex_fragment.bat ${DX12_SURFACE_SHADER_DIR} ${DX12_SURFACE_SHADER_WITHOUT_EXTENSION} ${XPDX12LibOutputDir})
    endforeach()
    set(DX12_COMPUTE_SHADERS ${XPENGINE_COMPUTE_SHADER_SOURCES})
    foreach(DX12_COMPUTE_SHADER ${DX12_COMPUTE_SHADERS})
        get_filename_component(DX12_COMPUTE_SHADER_DIR ${DX12_COMPUTE_SHADER} DIRECTORY)
        get_filename_component(DX12_COMPUTE_SHADER_FILENAME ${DX12_COMPUTE_SHADER} NAME)
        get_filename_component(DX12_COMPUTE_SHADER_WITHOUT_EXTENSION ${DX12_COMPUTE_SHADER} NAME_WE)
        message(STATUS "[DX12] [${DX12_COMPUTE_SHADER_WITHOUT_EXTENSION}] ${DX12_COMPUTE_SHADER_FILENAME}")
        list(APPEND XPDX12Shaders COMMAND ${CMAKE_COMMAND} -E copy ${DX12_COMPUTE_SHADER} ${CMAKE_BINARY_DIR}/$<CONFIG>/shaders/${DX12_COMPUTE_SHADER_FILENAME})
        list(APPEND XPDX12Shaders COMMAND ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/scripts/compile_compute.bat ${DX12_COMPUTE_SHADER_DIR} ${DX12_COMPUTE_SHADER_WITHOUT_EXTENSION} ${XPDX12LibOutputDir})
    endforeach()
    add_custom_target(XPCompileDX12Shaders
        ${XPDX12Shaders}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/XPShaderTypes.h ${CMAKE_BINARY_DIR}/$<CONFIG>/shaders/XPShaderTypes.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "[COMPILING ALL DX12 SHADERS]"
    )
    add_dependencies(${XP_EXE_NAME} XPCompileDX12Shaders)
endif()
# ---------------------------------------------------------------------------------------------------------------------------------------------------


# ---------------------------------------------------------------------------------------------------------------------------------------------------
# PROTOTYPE SHADERS
# ---------------------------------------------------------------------------------------------------------------------------------------------------
add_custom_target(XPInstallPrototypes
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/$<CONFIG>/prototypes/
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/prototypes/ ${CMAKE_BINARY_DIR}/$<CONFIG>/prototypes/
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "[INSTALLING PROTOTYPES]"
)
add_dependencies(${XP_EXE_NAME} XPInstallPrototypes)
# ---------------------------------------------------------------------------------------------------------------------------------------------------


# ---------------------------------------------------------------------------------------------------------------------------------------------------
# quick fix for COMPILE_COMMANDS.json database
# ---------------------------------------------------------------------------------------------------------------------------------------------------
# execute_process(
#     COMMAND xcodebuild -toolchain "org.llvm.19.1.6" -target ${XP_EXE_NAME} -project ${CMAKE_BINARY_DIR}/${XP_EXE_NAME}.xcodeproj
#     COMMAND xcpretty -r json-compilation-database --output ${CMAKE_BINARY_DIR}/compile_commands.json COMPILER_INDEX_STORE_ENABLE=NO
# )
# ---------------------------------------------------------------------------------------------------------------------------------------------------

add_custom_command(TARGET ${XP_EXE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/bin/" ${CMAKE_BINARY_DIR}/$<CONFIG>/
)


# ---------------------------------------------------------------------------------------------------------------------------------------------------
# XPEditor
# ---------------------------------------------------------------------------------------------------------------------------------------------------
set(XPEDITOR_PLATFORM_SPECIFIC_SOURCES ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/client/Private/NetImgui_NetworkWin32.cpp)
set(XPEDITOR_LINK_DIRECTORIES ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/ThirdParty/glfw/lib-vc2022-64/)
set(XPEDITOR_LINK_LIBRARIES glfw3_mt)
set_source_files_properties(${XPEDITOR_PLATFORM_SPECIFIC_SOURCES} PROPERTIES COMPILE_FLAGS "-w")

add_executable(XPEditor ${XPEDITOR_SOURCES} ${XPEDITOR_PLATFORM_SPECIFIC_SOURCES})
target_include_directories(XPEditor
    PRIVATE ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/ThirdParty/
    PRIVATE ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/ThirdParty/DearImgui/
    PRIVATE ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/ThirdParty/glfw/include/
)
target_link_directories(XPEditor PRIVATE ${XPEDITOR_LINK_DIRECTORIES})
target_link_libraries(XPEditor PRIVATE ${XPEDITOR_LINK_LIBRARIES})
set_property(TARGET XPEditor
    PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# XPPROFILER
# ---------------------------------------------------------------------------------------------------------------------------------------------------
add_library(XPProfiler STATIC ${XPENGINE_SOURCES_PROFILER} ${XPENGINE_HEADERS_PROFILER})
target_include_directories(XPProfiler PUBLIC ${XPTHIRDPARTY_INCLUDE_DIRECTORIES})
target_link_directories(XPProfiler PUBLIC ${XPTHIRDPARTY_LINK_DIRECTORIES})
target_link_libraries(XPProfiler PUBLIC ${XPTHIRDPARTY_LINK_LIBRARIES})
set_property(TARGET XPProfiler
    PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

add_dependencies(${XP_EXE_NAME} XPProfiler)
# ---------------------------------------------------------------------------------------------------------------------------------------------------