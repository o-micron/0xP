set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# SOURCES
# ---------------------------------------------------------------------------------------------------------------------------------------------------
set(XPENGINE_SOURCES_CONTROLLERS
    ${CMAKE_SOURCE_DIR}/src/Controllers/XPInput.cpp
)
set(XPENGINE_SOURCES_DATA_PIPELINE
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPAssimpModelLoader.cpp
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPFile.cpp
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPFileWatchEmscripten.cpp
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPMaterialBuffer.cpp
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPMeshBuffer.cpp
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPPreloadedAssets.cpp
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPShaderBuffer.cpp
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPStbTextureLoader.cpp
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPStore.cpp
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPTextureBuffer.cpp
)
set(XPENGINE_SOURCES_ENGINE
    ${CMAKE_SOURCE_DIR}/src/Engine/XPConsole.cpp
    ${CMAKE_SOURCE_DIR}/src/Engine/XPEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/Engine/XPRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/Engine/XPViewport.cpp
)
set(XPENGINE_SOURCES_GAME
    ${CMAKE_SOURCE_DIR}/src/Game/XPGame.cpp
)
set(XPENGINE_SOURCES_LANG
)
set(XPENGINE_SOURCES_PAINTING
    ${CMAKE_SOURCE_DIR}/src/Painting/XPPainting.cpp
)
set(XPENGINE_SOURCES_PHYSICS
    ${CMAKE_SOURCE_DIR}/src/Physics/PhysX4/XPPhysX4.cpp
)
set(XPENGINE_SOURCES_RENDERER
    ${CMAKE_SOURCE_DIR}/src/Renderer/Interface/XPRendererGraph.cpp

    ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/WGSL/XPWGSLContext.cpp
    ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/WGSL/XPWGSLGraph.cpp
    ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/WGSL/XPWGSLLang.cpp
    ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/WGSL/XPWGSLTextEditor.cpp
    
    ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/XPWGPUGPUData.cpp
    ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/XPWGPURenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/XPWGPUUIImpl.cpp
    ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/XPWGPUWindow.cpp
)
set(XPENGINE_SOURCES_SCENE_DESCRIPTOR
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPFilter.cpp
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPLayer.cpp
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPNode.cpp
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPScene.cpp
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPSceneStore.cpp
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPStore.cpp
)
set(XPENGINE_SOURCES_UI
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/XPImGUI.cpp
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/Assets.cpp
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/Console.cpp
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/EditorViewport.cpp
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/GameViewport.cpp
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/GCoreMemory.cpp
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/Hierarchy.cpp
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/Logs.cpp
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/MaterialEditor.cpp
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/Paint.cpp
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/Profiler.cpp
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/Properties.cpp
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/Settings.cpp
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/TextEditor.cpp
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/TextureEditor.cpp
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/Traits.cpp
)
set(XPENGINE_SOURCES_UTILITIES
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPAnnotations.cpp
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPCrossShaderCompiler.cpp
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPDiscarder.cpp
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPFreeCameraSystem.cpp
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPLocker.cpp
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPMemory.cpp
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPProfiler.cpp
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPRecorder.cpp
)
set(XPENGINE_HEADERS_COMPILERS
    ${CMAKE_SOURCE_DIR}/src/Compilers/Material/XPMaterial.h
    ${CMAKE_SOURCE_DIR}/src/Compilers/Script/XPScript.h
)
set(XPENGINE_HEADERS_CONTROLLERS
    ${CMAKE_SOURCE_DIR}/src/Controllers/XPInput.h
)
set(XPENGINE_HEADERS_DATA_PIPELINE
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPAssimpModelLoader.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPEnums.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPFile.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPFileWatchEmscripten.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPIFileWatch.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPLogic.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPMaterialAsset.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPMaterialBuffer.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPMeshAsset.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPMeshBuffer.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPPreloadedAssets.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPShaderAsset.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPShaderBuffer.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPStbTextureLoader.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPStore.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPTextureAsset.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPTextureBuffer.h
)
set(XPENGINE_HEADERS_ENGINE
    ${CMAKE_SOURCE_DIR}/src/Engine/XPConsole.h
    ${CMAKE_SOURCE_DIR}/src/Engine/XPEngine.h
    ${CMAKE_SOURCE_DIR}/src/Engine/XPRegistry.h
    ${CMAKE_SOURCE_DIR}/src/Engine/XPViewport.h
)
set(XPENGINE_HEADERS_GAME
    ${CMAKE_SOURCE_DIR}/src/Game/XPGame.h
)
set(XPENGINE_HEADERS_LANG
    ${CMAKE_SOURCE_DIR}/src/Lang/Interface/XPEnums.h
    ${CMAKE_SOURCE_DIR}/src/Lang/Interface/XPIBlock.h
    ${CMAKE_SOURCE_DIR}/src/Lang/Interface/XPIContext.h
    ${CMAKE_SOURCE_DIR}/src/Lang/Interface/XPIGraph.h
    ${CMAKE_SOURCE_DIR}/src/Lang/Interface/XPILang.h
    ${CMAKE_SOURCE_DIR}/src/Lang/Interface/XPILink.h
    ${CMAKE_SOURCE_DIR}/src/Lang/Interface/XPIPin.h
    ${CMAKE_SOURCE_DIR}/src/Lang/Interface/XPIPinType.h
    ${CMAKE_SOURCE_DIR}/src/Lang/Interface/XPITextEditor.h
)
set(XPENGINE_HEADERS_PAINTING
    ${CMAKE_SOURCE_DIR}/src/Painting/XPPainting.h
)
set(XPENGINE_HEADERS_PHYSICS
    ${CMAKE_SOURCE_DIR}/src/Physics/Interface/XPIPhysics.h
    ${CMAKE_SOURCE_DIR}/src/Physics/Bullet/XPBullet.h
    ${CMAKE_SOURCE_DIR}/src/Physics/Jolt/XPJolt.h
    ${CMAKE_SOURCE_DIR}/src/Physics/PhysX4/XPPhysX4.h
    ${CMAKE_SOURCE_DIR}/src/Physics/PhysX5/XPPhysX5.h
)
set(XPENGINE_HEADERS_RENDERER
    ${CMAKE_SOURCE_DIR}/src/Renderer/Interface/XPRendererGraph.h

    ${CMAKE_SOURCE_DIR}/src/Renderer/Interface/XPIRenderer.h
    ${CMAKE_SOURCE_DIR}/src/Renderer/Interface/XPIRendererBuffer.h
    ${CMAKE_SOURCE_DIR}/src/Renderer/Interface/XPIRendererCommandBuffer.h
    ${CMAKE_SOURCE_DIR}/src/Renderer/Interface/XPIRendererCommandQueue.h
    ${CMAKE_SOURCE_DIR}/src/Renderer/Interface/XPIRendererShaderCompute.h
    ${CMAKE_SOURCE_DIR}/src/Renderer/Interface/XPIRendererShaderSurface.h
    ${CMAKE_SOURCE_DIR}/src/Renderer/Interface/XPRendererGraph.h
    
    ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/WGSL/XPWGSLContext.h
    ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/WGSL/XPWGSLGraph.h
    ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/WGSL/XPWGSLLang.h
    ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/WGSL/XPWGSLTextEditor.h

    ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/wgpu_cpp.h
    ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/XPShaderTypes.h
    ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/XPWGPU.h
    ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/XPWGPUGPUData.h
    ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/XPWGPURenderer.h
    ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/XPWGPUUIImpl.h
    ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/XPWGPUWindow.h
)
set(XPENGINE_HEADERS_SCENE_DESCRIPTOR
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/Attachments/XPCollider.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/Attachments/XPFreeCamera.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/Attachments/XPLogic.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/Attachments/XPMeshRenderer.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/Attachments/XPOrbitCamera.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/Attachments/XPRigidbody.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/Attachments/XPTransform.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPAttachments.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPEnums.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPFilter.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPLayer.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPNode.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPScene.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPSceneStore.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPStore.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPTypes.h
)
set(XPENGINE_HEADERS_UI
    ${CMAKE_SOURCE_DIR}/src/UI/Interface/XPIUI.h
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/XPImGUI.h
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/Assets.h
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/Console.h
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/EditorViewport.h
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/GameViewport.h
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/GCoreMemory.h
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/Hierarchy.h
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/Logs.h
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/MaterialEditor.h
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/Paint.h
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/Profiler.h
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/Properties.h
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/Settings.h
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/Tabs.h
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/TextEditor.h
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/TextureEditor.h
    ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/Traits.h
)
set(XPENGINE_HEADERS_UTILITIES
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPAnnotations.h
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPBitArray.h
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPBitFlag.h
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPCrossShaderCompiler.h
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPDiscarder.h
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPFreeCameraSystem.h
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPFS.h
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPLocker.h
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPLogger.h
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPMacros.h
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPMaths.h
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPMemory.h
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPMemoryPool.h
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPPlatforms.h
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPProfiler.h
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPRecorder.h
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPThreadPool.h
)
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
    ${XPENGINE_HEADERS_UI}
    ${XPENGINE_HEADERS_UTILITIES}
)
set(IMGUI_SOURCES
    ${CMAKE_SOURCE_DIR}/thirdparty/artifacts/emscripten/src/ImGuiFileDialog/ImGuiFileDialog.cpp
    ${CMAKE_SOURCE_DIR}/thirdparty/artifacts/emscripten/src/ImGuiFileDialog/ImGuiFileDialog.h
    ${CMAKE_SOURCE_DIR}/thirdparty/artifacts/emscripten/src/ImGuiFileDialog/ImGuiFileDialogConfig.h

    ${CMAKE_SOURCE_DIR}/thirdparty/artifacts/macOS/src/text_editor/TextEditor.cpp
    ${CMAKE_SOURCE_DIR}/thirdparty/artifacts/macOS/src/text_editor/TextEditor.h

    ${CMAKE_SOURCE_DIR}/thirdparty/artifacts/emscripten/src/imconfig.h
    ${CMAKE_SOURCE_DIR}/thirdparty/artifacts/emscripten/src/imgui.cpp
    ${CMAKE_SOURCE_DIR}/thirdparty/artifacts/emscripten/src/imgui.h
    ${CMAKE_SOURCE_DIR}/thirdparty/artifacts/emscripten/src/imgui_demo.cpp
    ${CMAKE_SOURCE_DIR}/thirdparty/artifacts/emscripten/src/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/thirdparty/artifacts/emscripten/src/imgui_helper.h
    ${CMAKE_SOURCE_DIR}/thirdparty/artifacts/emscripten/src/imgui_internal.h
    ${CMAKE_SOURCE_DIR}/thirdparty/artifacts/emscripten/src/imgui_stdlib.cpp
    ${CMAKE_SOURCE_DIR}/thirdparty/artifacts/emscripten/src/imgui_stdlib.h
    ${CMAKE_SOURCE_DIR}/thirdparty/artifacts/emscripten/src/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/thirdparty/artifacts/emscripten/src/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/thirdparty/artifacts/emscripten/src/imguizmo.cpp
    ${CMAKE_SOURCE_DIR}/thirdparty/artifacts/emscripten/src/imguizmo.h

    ${CMAKE_SOURCE_DIR}/thirdparty/implot/implot_internal.h
    ${CMAKE_SOURCE_DIR}/thirdparty/implot/implot_items.cpp
    ${CMAKE_SOURCE_DIR}/thirdparty/implot/implot.cpp
    ${CMAKE_SOURCE_DIR}/thirdparty/implot/implot.h
)
set(IMGUI_GFX_API_SOURCES
    ${CMAKE_SOURCE_DIR}/thirdparty/artifacts/emscripten/src/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/thirdparty/artifacts/emscripten/src/imgui_impl_glfw.h
    ${CMAKE_SOURCE_DIR}/thirdparty/artifacts/emscripten/src/imgui_impl_wgpu.cpp
    ${CMAKE_SOURCE_DIR}/thirdparty/artifacts/emscripten/src/imgui_impl_wgpu.h
)
set(XPENGINE_SHADER_SOURCES
    ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/shaders/MainCompute.wgsl
    ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/shaders/GBuffer.wgsl
    ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/shaders/Line.wgsl
    ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/shaders/MBuffer.wgsl
)
set(XPTHIRDPARTY_LINK_DIRECTORIES
    ${CMAKE_SOURCE_DIR}/thirdparty/assimp/emscripten/lib
)
set(XPTHIRDPARTY_LINK_LIBRARIES
    assimp
    gcore
)
set(XPENGINE_HEADER_TOOL_FILES
    ${CMAKE_SOURCE_DIR}/src/Templates/XPAttachments.h.in
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPAttachments.h

    ${CMAKE_SOURCE_DIR}/src/Templates/XPNode.cpp.in
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPNode.cpp

    ${CMAKE_SOURCE_DIR}/src/Templates/XPNode.h.in
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPNode.h

    ${CMAKE_SOURCE_DIR}/src/Templates/XPSceneStore.cpp.in
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPSceneStore.cpp

    ${CMAKE_SOURCE_DIR}/src/Templates/XPSceneStore.h.in
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPSceneStore.h
)
set(XPTHIRDPARTY_INCLUDE_DIRECTORIES
    ${CMAKE_SOURCE_DIR}/src/
    ${CMAKE_SOURCE_DIR}/src/Scripting/gcore/include
    ${CMAKE_SOURCE_DIR}/thirdparty/artifacts/emscripten/include
    ${CMAKE_SOURCE_DIR}/thirdparty/artifacts/emscripten/src
)
# ---------------------------------------------------------------------------------------------------------------------------------------------------


# ---------------------------------------------------------------------------------------------------------------------------------------------------
# TARGET
# ---------------------------------------------------------------------------------------------------------------------------------------------------
add_executable(${XP_EXE_NAME} 
    ${CMAKE_SOURCE_DIR}/src/Editor/main.cpp
    ${XPENGINE_SOURCES} ${XPENGINE_HEADERS}
    ${IMGUI_SOURCES} ${IMGUI_GFX_API_SOURCES}
)
target_include_directories(${XP_EXE_NAME} PRIVATE ${XPTHIRDPARTY_INCLUDE_DIRECTORIES})
target_link_directories(${XP_EXE_NAME} PRIVATE ${XPTHIRDPARTY_LINK_DIRECTORIES})
target_link_libraries(${XP_EXE_NAME} PRIVATE ${XPTHIRDPARTY_LINK_LIBRARIES})

set(EMSCRIPTEN_ARGS "-s WASM=1 -s USE_GLFW=3 -s USE_WEBGPU=1")
# set(EMSCRIPTEN_ARGS "${EMSCRIPTEN_ARGS} -s PTHREAD_POOL_SIZE=2")
set(EMSCRIPTEN_ARGS "${EMSCRIPTEN_ARGS} -s NO_DISABLE_EXCEPTION_CATCHING")
# set(EMSCRIPTEN_ARGS "${EMSCRIPTEN_ARGS} -s LLD_REPORT_UNDEFINED -s ERROR_ON_UNDEFINED_SYMBOLS=0")
set(EMSCRIPTEN_ARGS "${EMSCRIPTEN_ARGS} -s NO_EXIT_RUNTIME=0 -s ASSERTIONS=1")
set(EMSCRIPTEN_ARGS "${EMSCRIPTEN_ARGS} -s ALLOW_MEMORY_GROWTH=1")
set(EMSCRIPTEN_ARGS "${EMSCRIPTEN_ARGS} --preload-file ${CMAKE_SOURCE_DIR}/assets/fonts@/fonts/")
# set(EMSCRIPTEN_ARGS "${EMSCRIPTEN_ARGS} --preload-file ${CMAKE_SOURCE_DIR}/assets/meshes@/meshes/")
# set(EMSCRIPTEN_ARGS "${EMSCRIPTEN_ARGS} --preload-file ${CMAKE_SOURCE_DIR}/assets/scenes@/scenes/")
set(EMSCRIPTEN_ARGS "${EMSCRIPTEN_ARGS} --preload-file ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/shaders@/shaders/")
# set(EMSCRIPTEN_ARGS "${EMSCRIPTEN_ARGS} --preload-file ${CMAKE_SOURCE_DIR}/assets/textures@/textures/")
set(EMSCRIPTEN_ARGS "${EMSCRIPTEN_ARGS} -s EXPORTED_FUNCTIONS=_main,_c_resize_browser_window,_c_on_paste,_c_on_copy")
set(EMSCRIPTEN_ARGS "${EMSCRIPTEN_ARGS} -s EXPORTED_RUNTIME_METHODS=ccall,cwrap")
# set(EMSCRIPTEN_ARGS "${EMSCRIPTEN_ARGS} -s MINIMAL_RUNTIME=2 -s TEXTDECODER=2 -s ABORTING_MALLOC=0 -s SUPPORT_ERRNO=0")
# set(EMSCRIPTEN_ARGS "${EMSCRIPTEN_ARGS} -s ENVIRONMENT=web -s MIN_FIREFOX_VERSION=87 -s MIN_SAFARI_VERSION=140000 -s MIN_EDGE_VERSION=90 -s MIN_CHROME_VERSION=90")
# set(EMSCRIPTEN_ARGS "${EMSCRIPTEN_ARGS} -s TOTAL_STACK=16KB -s INITIAL_MEMORY=128KB")
# set(EMSCRIPTEN_ARGS "${EMSCRIPTEN_ARGS} -s ALLOW_MEMORY_GROWTH=1")

set_target_properties(${XP_EXE_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LINK_FLAGS ${EMSCRIPTEN_ARGS}
)
# ---------------------------------------------------------------------------------------------------------------------------------------------------


# ---------------------------------------------------------------------------------------------------------------------------------------------------
# HEADER TOOL
# ---------------------------------------------------------------------------------------------------------------------------------------------------
file(GLOB XP_ATTACHMENT_HEADERS ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/Attachments/*.h)
set(XP_HEADER_TOOL_COMMAND /Users/$ENV{USER}/Library/Developer/Toolchains/LLVM19.1.6.xctoolchain/usr/bin/xp-header-tool)
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
    list(APPEND XP_FORMAT_COMMANDS COMMAND /Users/$ENV{USER}/Library/Developer/Toolchains/LLVM19.1.6.xctoolchain/usr/bin/clang-format -style=file -i ${VALUE})
endforeach()
foreach(VALUE ${XPENGINE_HEADERS})
    list(APPEND XP_FORMAT_COMMANDS COMMAND /Users/$ENV{USER}/Library/Developer/Toolchains/LLVM19.1.6.xctoolchain/usr/bin/clang-format -style=file -i ${VALUE})
endforeach()
foreach(VALUE ${XPENGINE_HEADER_TOOL_FILES})
    list(APPEND XP_FORMAT_COMMANDS COMMAND /Users/$ENV{USER}/Library/Developer/Toolchains/LLVM19.1.6.xctoolchain/usr/bin/clang-format -style=file -i ${VALUE})
endforeach()
add_custom_target(XPFormat
    ${XP_FORMAT_COMMANDS}
    COMMENT "[Running XPFormat]"
    DEPENDS XPHeaderTool
)
add_dependencies(${XP_EXE_NAME} XPFormat XPHeaderTool)
# ---------------------------------------------------------------------------------------------------------------------------------------------------