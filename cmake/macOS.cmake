# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(CMAKE_XCODE_GENERATE_SCHEME YES)
set(CMAKE_XCODE_ATTRIBUTE_COMPILER_INDEX_STORE_ENABLE NO)
if(XP_RENDERER_METAL)
    set(CMAKE_XCODE_SCHEME_ADDRESS_SANITIZER NO)
    set(CMAKE_XCODE_SCHEME_THREAD_SANITIZER NO)
    set(CMAKE_XCODE_SCHEME_UNDEFINED_BEHAVIOUR_SANITIZER NO)
    set(CMAKE_XCODE_SCHEME_ENABLE_GPU_FRAME_CAPTURE_MODE "Metal")
    set(CMAKE_XCODE_SCHEME_ENABLE_GPU_API_VALIDATION YES)
    set(CMAKE_XCODE_SCHEME_ENABLE_GPU_SHADER_VALIDATION YES)
endif()


# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# SOURCES
# ---------------------------------------------------------------------------------------------------------------------------------------------------
set(XPENGINE_SOURCES_DATA_PIPELINE ${XPENGINE_SOURCES_DATA_PIPELINE}
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPFileWatchUnix.cpp
)
set(XPENGINE_HEADERS_DATA_PIPELINE ${XPENGINE_HEADERS_DATA_PIPELINE}
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPFileWatchUnix.h
)
if(XP_EDITOR_MODE)
    set(IMGUI_SOURCES ${IMGUI_SOURCES}
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/client/Private/NetImgui_NetworkPosix.cpp
    )
endif()

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
    assimp # $<IF:$<CONFIG:Debug>,assimpd,assimp>
    SDL2 # $<IF:$<CONFIG:Debug>,SDL2d,SDL2>
    SDL2main # $<IF:$<CONFIG:Debug>,SDL2maind,SDL2main>
    iconv
    fswatch
    spirv-cross-c-shared
)
# ---------------------------------------------------------------------------------------------------------------------------------------------------


# ---------------------------------------------------------------------------------------------------------------------------------------------------
# TARGET
# ---------------------------------------------------------------------------------------------------------------------------------------------------
if (XP_CMAKE_XCODE)
    add_executable(${XP_EXE_NAME}
        ${CMAKE_SOURCE_DIR}/src/Editor/main.cpp
        ${XPENGINE_SOURCES} ${XPENGINE_HEADERS}
        ${IMGUI_SOURCES} ${IMGUI_GFX_API_SOURCES} ${SPIRV_REFLECT_SOURCES}
        ${XPENGINE_SURFACE_SHADER_SOURCES} ${XPENGINE_COMPUTE_SHADER_SOURCES}
        ${ASSETS_GLTF_SAMPLE_MODELS}
        ${ASSETS_GLTF_SAMPLE_MODEL_TEXTURES}
        ${ASSETS_FONTS}
        ${ASSETS_MESHES}
        ${ASSETS_SCENES}
        ${ASSETS_TEXTURES}
    )
else ()
    add_executable(${XP_EXE_NAME}
        ${CMAKE_SOURCE_DIR}/src/Editor/main.cpp
        ${XPENGINE_SOURCES} ${XPENGINE_HEADERS}
        ${IMGUI_SOURCES} ${IMGUI_GFX_API_SOURCES} ${SPIRV_REFLECT_SOURCES}
    )
endif ()
target_include_directories(${XP_EXE_NAME} PRIVATE ${XPTHIRDPARTY_INCLUDE_DIRECTORIES})
target_link_directories(${XP_EXE_NAME} PRIVATE ${XPTHIRDPARTY_LINK_DIRECTORIES})
target_link_libraries(${XP_EXE_NAME} PRIVATE ${XPTHIRDPARTY_LINK_LIBRARIES} XPProfiler)
if (XP_USE_COMPUTE)
    target_link_libraries(${XP_EXE_NAME} PRIVATE XPCompute)
endif()

if (XP_CMAKE_XCODE)
    GroupSourcesByFolder(${XP_EXE_NAME} "${XPENGINE_RESOURCE_FILES}")
    set_target_properties(
        ${XP_EXE_NAME}
        PROPERTIES
        # MACOSX_BUNDLE YES
        # MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_LIST_DIR}/plist.in"
        # XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
        # XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
        # XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
        # XCODE_ATTRIBUTE_INSTALL_PATH "${CMAKE_BINARY_DIR}/ProductRelease"
        # XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES "NO"
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
    add_executable(XPTests ${XP_TEST_SOURCES} ${XPENGINE_SOURCES} ${XPENGINE_HEADERS} ${IMGUI_SOURCES} ${IMGUI_GFX_API_SOURCES} ${SPIRV_REFLECT_SOURCES})
    if (XP_CMAKE_XCODE)
        GroupSourcesByFolder(XPTests)
    endif ()
    target_include_directories(XPTests PRIVATE ${XPTHIRDPARTY_INCLUDE_DIRECTORIES})
    target_link_directories(XPTests PRIVATE ${XPTHIRDPARTY_LINK_DIRECTORIES})
    target_link_libraries(XPTests PRIVATE GTest::gtest_main ${XPTHIRDPARTY_LINK_LIBRARIES})
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
    add_executable(XPBenchmarks ${CMAKE_SOURCE_DIR}/benchmarks/XPBenchmarks.cpp ${XPENGINE_SOURCES} ${XPENGINE_HEADERS} ${IMGUI_SOURCES} ${IMGUI_GFX_API_SOURCES} ${SPIRV_REFLECT_SOURCES})
    if (XP_CMAKE_XCODE)
        GroupSourcesByFolder(XPBenchmarks)
    endif()
    target_include_directories(XPBenchmarks PRIVATE ${XPTHIRDPARTY_INCLUDE_DIRECTORIES})
    target_link_directories(XPBenchmarks PRIVATE ${XPTHIRDPARTY_LINK_DIRECTORIES})
    target_link_libraries(XPBenchmarks PRIVATE benchmark::benchmark ${XPTHIRDPARTY_LINK_LIBRARIES})
endif(XP_BUILD_BENCHMARKS)
# ---------------------------------------------------------------------------------------------------------------------------------------------------


# ---------------------------------------------------------------------------------------------------------------------------------------------------
# HEADER TOOL
# ---------------------------------------------------------------------------------------------------------------------------------------------------
file(GLOB XP_ATTACHMENT_HEADERS ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/Attachments/*.h)
set(XP_HEADER_TOOL_COMMAND ${CMAKE_SOURCE_DIR}/artifacts/macOS/compiler/bin/xp-header-tool)
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
    list(APPEND XP_FORMAT_COMMANDS COMMAND ${CMAKE_SOURCE_DIR}/artifacts/macOS/compiler/bin/clang-format -style=file -i ${VALUE})
endforeach()
foreach(VALUE ${XPENGINE_HEADERS})
    list(APPEND XP_FORMAT_COMMANDS COMMAND ${CMAKE_SOURCE_DIR}/artifacts/macOS/compiler/bin/clang-format -style=file -i ${VALUE})
endforeach()
foreach(VALUE ${XPENGINE_HEADER_TOOL_FILES})
    list(APPEND XP_FORMAT_COMMANDS COMMAND ${CMAKE_SOURCE_DIR}/artifacts/macOS/compiler/bin/clang-format -style=file -i ${VALUE})
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
if (XP_USE_COMPUTE)
    add_dependencies(XPCompute XPXPUCompute)
    add_dependencies(XPInstallRiscvBinaries XPXPUCompute)
endif()
add_dependencies(${XP_EXE_NAME} XPInstallMeshes XPInstallScenes XPInstallTextures XPInstallFonts XPInstallRiscvBinaries)
if (XP_USE_COMPUTE)
    add_dependencies(${XP_EXE_NAME} XPXPUCompute)
endif()
# ---------------------------------------------------------------------------------------------------------------------------------------------------


# ---------------------------------------------------------------------------------------------------------------------------------------------------
# METAL SHADERS
# ---------------------------------------------------------------------------------------------------------------------------------------------------
if(XP_RENDERER_METAL)
    set(XPMetalShaders )
    set(XPMetalLibOutputDir ${CMAKE_BINARY_DIR}/$<CONFIG>/shaders)
    set(METAL_SURFACE_SHADERS ${XPENGINE_SURFACE_SHADER_SOURCES})
    foreach(METAL_SURFACE_SHADER ${METAL_SURFACE_SHADERS})
        get_filename_component(METAL_SURFACE_SHADER_FILENAME ${METAL_SURFACE_SHADER} NAME)
        get_filename_component(METAL_SURFACE_SHADER_WITHOUT_EXTENSION ${METAL_SURFACE_SHADER} NAME_WE)
        message(STATUS "[METAL] [${METAL_SURFACE_SHADER_WITHOUT_EXTENSION}] ${METAL_SURFACE_SHADER_FILENAME}")
        list(APPEND XPMetalShaders COMMAND ${CMAKE_COMMAND} -E copy ${METAL_SURFACE_SHADER} ${CMAKE_BINARY_DIR}/$<CONFIG>/shaders/${METAL_SURFACE_SHADER_FILENAME})
        list(APPEND XPMetalShaders COMMAND ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/scripts/compile_vertex_fragment.sh ${METAL_SURFACE_SHADER} ${METAL_SURFACE_SHADER_WITHOUT_EXTENSION} ${XPMetalLibOutputDir})
    endforeach()
    set(METAL_COMPUTE_SHADERS ${XPENGINE_COMPUTE_SHADER_SOURCES})
    foreach(METAL_COMPUTE_SHADER ${METAL_COMPUTE_SHADERS})
        get_filename_component(METAL_COMPUTE_SHADER_FILENAME ${METAL_COMPUTE_SHADER} NAME)
        get_filename_component(METAL_COMPUTE_SHADER_WITHOUT_EXTENSION ${METAL_COMPUTE_SHADER} NAME_WE)
        message(STATUS "[METAL] [${METAL_COMPUTE_SHADER_WITHOUT_EXTENSION}] ${METAL_COMPUTE_SHADER_FILENAME}")
        list(APPEND XPMetalShaders COMMAND ${CMAKE_COMMAND} -E copy ${METAL_COMPUTE_SHADER} ${CMAKE_BINARY_DIR}/$<CONFIG>/shaders/${METAL_COMPUTE_SHADER_FILENAME})
        list(APPEND XPMetalShaders COMMAND ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/scripts/compile_compute.sh ${METAL_COMPUTE_SHADER} ${METAL_COMPUTE_SHADER_WITHOUT_EXTENSION} ${XPMetalLibOutputDir})
    endforeach()
    add_custom_target(XPCompileMetalShaders
        COMMAND ${CMAKE_COMMAND} -E make_directory ${XPMetalLibOutputDir}
        ${XPMetalShaders}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPShaderTypes.h ${CMAKE_BINARY_DIR}/$<CONFIG>/shaders/XPShaderTypes.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "[COMPILING ALL METAL SHADERS]"
    )
    add_dependencies(${XP_EXE_NAME} XPCompileMetalShaders)
elseif(XP_RENDERER_VULKAN)
    set(XPVulkanShaders )
    set(XPVulkanLibOutputDir ${CMAKE_BINARY_DIR}/$<CONFIG>/shaders/)
    set(VULKAN_SURFACE_SHADERS ${XPENGINE_SURFACE_SHADER_SOURCES})
    foreach(VULKAN_SURFACE_SHADER ${VULKAN_SURFACE_SHADERS})
        get_filename_component(VULKAN_SURFACE_SHADER_DIR ${VULKAN_SURFACE_SHADER} DIRECTORY)    
        get_filename_component(VULKAN_SURFACE_SHADER_FILENAME ${VULKAN_SURFACE_SHADER} NAME)
        get_filename_component(VULKAN_SURFACE_SHADER_WITHOUT_EXTENSION ${VULKAN_SURFACE_SHADER} NAME_WE)
        message(STATUS "[VULKAN] [${VULKAN_SURFACE_SHADER_WITHOUT_EXTENSION}] ${VULKAN_SURFACE_SHADER_FILENAME}")
        list(APPEND XPVulkanShaders COMMAND ${CMAKE_COMMAND} -E copy ${VULKAN_SURFACE_SHADER} ${CMAKE_BINARY_DIR}/$<CONFIG>/shaders/${VULKAN_SURFACE_SHADER_FILENAME})
        list(APPEND XPVulkanShaders COMMAND ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/scripts/compile_vertex_fragment.sh ${VULKAN_SURFACE_SHADER_DIR} ${VULKAN_SURFACE_SHADER_WITHOUT_EXTENSION} ${XPVulkanLibOutputDir})
    endforeach()
    set(VULKAN_COMPUTE_SHADERS ${XPENGINE_COMPUTE_SHADER_SOURCES})
    foreach(VULKAN_COMPUTE_SHADER ${VULKAN_COMPUTE_SHADERS})
        get_filename_component(VULKAN_COMPUTE_SHADER_DIR ${VULKAN_COMPUTE_SHADER} DIRECTORY)
        get_filename_component(VULKAN_COMPUTE_SHADER_FILENAME ${VULKAN_COMPUTE_SHADER} NAME)
        get_filename_component(VULKAN_COMPUTE_SHADER_WITHOUT_EXTENSION ${VULKAN_COMPUTE_SHADER} NAME_WE)
        message(STATUS "[VULKAN] [${VULKAN_COMPUTE_SHADER_WITHOUT_EXTENSION}] ${VULKAN_COMPUTE_SHADER_FILENAME}")
        list(APPEND XPVulkanShaders COMMAND ${CMAKE_COMMAND} -E copy ${VULKAN_COMPUTE_SHADER} ${CMAKE_BINARY_DIR}/$<CONFIG>/shaders/${VULKAN_COMPUTE_SHADER_FILENAME})
        list(APPEND XPVulkanShaders COMMAND ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/scripts/compile_compute.sh ${VULKAN_COMPUTE_SHADER_DIR} ${VULKAN_COMPUTE_SHADER_WITHOUT_EXTENSION} ${XPVulkanLibOutputDir})
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
execute_process(
    COMMAND xcodebuild -toolchain "org.llvm.19.1.6" -target ${XP_EXE_NAME} -project ${CMAKE_BINARY_DIR}/${XP_EXE_NAME}.xcodeproj
    COMMAND xcpretty -r json-compilation-database --output ${CMAKE_BINARY_DIR}/compile_commands.json COMPILER_INDEX_STORE_ENABLE=NO
)
# ---------------------------------------------------------------------------------------------------------------------------------------------------

add_custom_command(TARGET ${XP_EXE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/bin/" ${CMAKE_BINARY_DIR}/$<CONFIG>/
)

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# XPEditor
# ---------------------------------------------------------------------------------------------------------------------------------------------------
set(XPEDITOR_PLATFORM_SPECIFIC_SOURCES ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/client/Private/NetImgui_NetworkPosix.cpp)

set(XPEDITOR_LINK_LIBRARIES libglfw3.a "-framework Cocoa" "-framework IOKit" "-framework CoreFoundation" "-framework CoreVideo")
set(XPEDITOR_LINK_DIRECTORIES ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/ThirdParty/glfw/lib-macos-universal/)

add_executable(XPEditor ${XPEDITOR_SOURCES} ${XPEDITOR_PLATFORM_SPECIFIC_SOURCES})
target_include_directories(XPEditor
    PRIVATE ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/ThirdParty/
    PRIVATE ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/ThirdParty/DearImgui/
    PRIVATE ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/ThirdParty/glfw/include/
)
target_link_directories(XPEditor PRIVATE ${XPEDITOR_LINK_DIRECTORIES})
target_link_libraries(XPEditor PRIVATE ${XPEDITOR_LINK_LIBRARIES})
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# XPPROFILER
# ---------------------------------------------------------------------------------------------------------------------------------------------------
add_library(XPProfiler STATIC ${XPENGINE_SOURCES_PROFILER} ${XPENGINE_HEADERS_PROFILER})
target_include_directories(XPProfiler PUBLIC ${XPTHIRDPARTY_INCLUDE_DIRECTORIES})
target_link_directories(XPProfiler PUBLIC ${XPTHIRDPARTY_LINK_DIRECTORIES})
target_link_libraries(XPProfiler PUBLIC ${XPTHIRDPARTY_LINK_LIBRARIES})

add_dependencies(${XP_EXE_NAME} XPProfiler)
# ---------------------------------------------------------------------------------------------------------------------------------------------------