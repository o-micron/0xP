if(XP_RENDERER_VULKAN)
    find_package(Vulkan REQUIRED)
endif()


# ---------------------------------------------------------------------------------------------------------------------------------------------------
# ASSETS
# ---------------------------------------------------------------------------------------------------------------------------------------------------
set(ASSETS_GLTF_SAMPLE_MODELS
)
set(ASSETS_GLTF_SAMPLE_MODEL_TEXTURES
)
file(GLOB_RECURSE ASSETS_FONTS ${CMAKE_SOURCE_DIR}/assets/fonts/*.ttf)
file(GLOB_RECURSE ASSETS_MESHES ${CMAKE_SOURCE_DIR}/assets/meshes/*)
file(GLOB_RECURSE ASSETS_SCENES ${CMAKE_SOURCE_DIR}/assets/scenes/*)
file(GLOB_RECURSE ASSETS_TEXTURES ${CMAKE_SOURCE_DIR}/assets/textures/*)
file(GLOB_RECURSE ASSETS_RISCV_BINARIES ${CMAKE_SOURCE_DIR}/src/Emulator/test/build/Debug/* ${CMAKE_SOURCE_DIR}/src/Compute/riscv/build/Debug/*)

set(GltfModelsAssetCommands )
set(GltfTexturesAssetCommands )
set(FontsAssetCommands )
set(MeshesAssetCommands )
set(ScenesAssetCommands )
set(TexturesAssetCommands )
set(RiscvBinariesAssetCommands )
foreach(ASSET_GLTF_SAMPLE_MODEL ${ASSETS_GLTF_SAMPLE_MODELS})
    list(APPEND GltfModelsAssetCommands COMMAND ${CMAKE_COMMAND} -E copy ${ASSET_GLTF_SAMPLE_MODEL} ${CMAKE_BINARY_DIR}/$<CONFIG>/meshes)
endforeach()
foreach(ASSET_GLTF_SAMPLE_MODEL_TEXTURE ${ASSETS_GLTF_SAMPLE_MODEL_TEXTURES})
    list(APPEND GltfTexturesAssetCommands COMMAND ${CMAKE_COMMAND} -E copy ${ASSET_GLTF_SAMPLE_MODEL_TEXTURE} ${CMAKE_BINARY_DIR}/$<CONFIG>/textures)
endforeach()
foreach(ASSET_FONT ${ASSETS_FONTS})
    list(APPEND FontsAssetCommands COMMAND ${CMAKE_COMMAND} -E copy ${ASSET_FONT} ${CMAKE_BINARY_DIR}/$<CONFIG>/fonts)
endforeach()
foreach(ASSET_MESH ${ASSETS_MESHES})
    list(APPEND MeshesAssetCommands COMMAND ${CMAKE_COMMAND} -E copy ${ASSET_MESH} ${CMAKE_BINARY_DIR}/$<CONFIG>/meshes)
endforeach()
foreach(ASSET_SCENE ${ASSETS_SCENES})
    list(APPEND ScenesAssetCommands COMMAND ${CMAKE_COMMAND} -E copy ${ASSET_SCENE} ${CMAKE_BINARY_DIR}/$<CONFIG>/scenes)
endforeach()
foreach(ASSET_TEXTURE ${ASSETS_TEXTURES})
    list(APPEND TexturesAssetCommands COMMAND ${CMAKE_COMMAND} -E copy ${ASSET_TEXTURE} ${CMAKE_BINARY_DIR}/$<CONFIG>/textures)
endforeach()
foreach(ASSET_RISCV_BINARY ${ASSETS_RISCV_BINARIES})
    list(APPEND RiscvBinariesAssetCommands COMMAND ${CMAKE_COMMAND} -E copy ${ASSET_RISCV_BINARY} ${CMAKE_BINARY_DIR}/$<CONFIG>/riscv)
endforeach()

add_custom_target(XPInstallMeshes
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/$<CONFIG>/meshes
    ${GltfModelsAssetCommands}
    ${MeshesAssetCommands}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "[INSTALLING MESHES]"
)
add_custom_target(XPInstallScenes
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/$<CONFIG>/scenes
    ${ScenesAssetCommands}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "[INSTALLING SCENES]"
)
add_custom_target(XPInstallTextures
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/$<CONFIG>/textures
    ${GltfTexturesAssetCommands}
    ${TexturesAssetCommands}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "[INSTALLING TEXTURES]"
)
add_custom_target(XPInstallFonts
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/$<CONFIG>/fonts
    ${FontsAssetCommands}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "[INSTALLING FONTS]"
)
add_custom_target(XPInstallRiscvBinaries
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/$<CONFIG>/riscv
    ${RiscvBinariesAssetCommands}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "[INSTALLING RISCV BINARIES]"
)
if (XP_USE_COMPUTE_CUDA)
    add_custom_target(XPXPUCompute
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/$<CONFIG>/compute
        COMMAND ${CMAKE_SOURCE_DIR}/artifacts/windows/bin/slangc.exe ${CMAKE_SOURCE_DIR}/src/Compute/shaders/compute.slang -I${CMAKE_SOURCE_DIR}/src/Compute/shaders/ -stage compute -capability cuda_sm_5_0 -profile sm_5_0 -entry computeMain -target cuda -O3 -g0 -o ${CMAKE_SOURCE_DIR}/src/Compute/shaders/compute.cu
        COMMAND nvcc -ptx ${CMAKE_SOURCE_DIR}/src/Compute/shaders/compute.cu -o ${CMAKE_SOURCE_DIR}/src/Compute/shaders/compute.ptx -diag-suppress 550
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/Compute/shaders/compute.ptx ${CMAKE_BINARY_DIR}/$<CONFIG>/compute/
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "[BUILDING XPU COMPUTE]"
    )
elseif(XP_USE_COMPUTE_METAL)
    add_custom_target(XPXPUCompute
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/$<CONFIG>/compute
        COMMAND ${CMAKE_SOURCE_DIR}/artifacts/macOS/bin/slangc -I${CMAKE_SOURCE_DIR}/src/Compute/shaders/ -stage compute -entry computeMain -target metal ${CMAKE_SOURCE_DIR}/src/Compute/shaders/compute.slang -O3 -g0 -o ${CMAKE_SOURCE_DIR}/src/Compute/shaders/compute.metal
        COMMAND xcrun -sdk macosx metal ${CMAKE_SOURCE_DIR}/src/Compute/shaders/compute.metal -o ${CMAKE_SOURCE_DIR}/src/Compute/shaders/compute.metallib
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/Compute/shaders/compute.metallib ${CMAKE_BINARY_DIR}/$<CONFIG>/compute/
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "[BUILDING XPU COMPUTE]"
    )
endif()
# ---------------------------------------------------------------------------------------------------------------------------------------------------


# ---------------------------------------------------------------------------------------------------------------------------------------------------
# SOURCES
# ---------------------------------------------------------------------------------------------------------------------------------------------------
set(XPENGINE_SOURCES_CONTROLLERS
    ${CMAKE_SOURCE_DIR}/src/Controllers/XPInput.cpp
)
set(XPENGINE_SOURCES_DATA_PIPELINE
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPAssimpModelLoader.cpp
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPFile.cpp
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPLightBuffer.cpp
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPMaterialBuffer.cpp
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPMeshBuffer.cpp
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPPreloadedAssets.cpp
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPSceneAsset.cpp
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPShaderBuffer.cpp
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPShaderLoader.cpp
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPStbTextureLoader.cpp
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPDataPipelineStore.cpp
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPTextureBuffer.cpp
)
set(XPENGINE_SOURCES_ENGINE
    ${CMAKE_SOURCE_DIR}/src/Engine/XPAllocators.cpp
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
set(XPENGINE_SOURCES_MCP
    ${CMAKE_SOURCE_DIR}/src/Mcp/XPMcpServer.cpp
)
set(XPENGINE_SOURCES_PAINTING
    ${CMAKE_SOURCE_DIR}/src/Painting/XPPainting.cpp
)
if(XP_PHYSICS_BULLET)
    set(XPENGINE_SOURCES_PHYSICS
        ${CMAKE_SOURCE_DIR}/src/Physics/Bullet/XPBulletPhysics.cpp
    )
elseif(XP_PHYSICS_JOLT)
    set(XPENGINE_SOURCES_PHYSICS
        ${CMAKE_SOURCE_DIR}/src/Physics/Jolt/XPJoltPhysics.cpp
    )
elseif(XP_PHYSICS_PHYSX4)
    set(XPENGINE_SOURCES_PHYSICS
        ${CMAKE_SOURCE_DIR}/src/Physics/PhysX4/XPPhysX4Physics.cpp
    )
elseif(XP_PHYSICS_PHYSX5)
    set(XPENGINE_SOURCES_PHYSICS
        ${CMAKE_SOURCE_DIR}/src/Physics/PhysX5/XPPhysX5Physics.cpp
    )
else()
    set(XPENGINE_SOURCES_PHYSICS
        ${CMAKE_SOURCE_DIR}/src/Physics/Empty/XPEmptyPhysics.cpp
    )
endif()
set(XPENGINE_SOURCES_PROFILER
    ${CMAKE_SOURCE_DIR}/src/Profiler/src/XPProfiler.cpp
)
set(XPENGINE_SOURCES_RENDERER
    ${CMAKE_SOURCE_DIR}/src/Renderer/Interface/XPRendererGraph.cpp
)
if(XP_RENDERER_DX12)
    set(XPENGINE_SOURCES_RENDERER ${XPENGINE_SOURCES_RENDERER}
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/dx12al/CommandList.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/dx12al/ConstantBuffer.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/dx12al/DescriptorHeap.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/dx12al/Device.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/dx12al/GraphicsPipeline.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/dx12al/PipelineStateObject.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/dx12al/Queue.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/dx12al/RootSignature.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/dx12al/Shader.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/dx12al/Swapchain.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/dx12al/Synchronization.cpp

        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/XPDX12GPUData.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/XPDX12Renderer.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/XPDX12Upscaler.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/XPDX12Window.cpp
    )
elseif(XP_RENDERER_METAL)
    set(XPENGINE_SOURCES_RENDERER ${XPENGINE_SOURCES_RENDERER}
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/MSL/XPMSLContext.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/MSL/XPMSLGraph.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/MSL/XPMSLLang.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/MSL/XPMSLParser.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/MSL/XPMSLTextEditor.cpp

        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/metal_cpp.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalBuffer.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalBufferManaged.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalBufferMemoryless.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalBufferPrivate.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalBufferShared.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalCommandBuffer.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalCommandQueue.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalEvent.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalGPUData.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalNN.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalCulling.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalRenderer.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalUpscaler.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalWindow.cpp
    )
elseif(XP_RENDERER_VULKAN)
    set(XPENGINE_SOURCES_RENDERER ${XPENGINE_SOURCES_RENDERER}
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/CommandBuffer.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/CommandPool.cpp    
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/Device.cpp    
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/FrameBuffer.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/FreeFunctions.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/GraphicsPipeline.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/Instance.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/PhysicalDevice.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/Queue.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/Shader.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/Swapchain.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/Synchronization.cpp

        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/XPVulkanGPUData.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/XPVulkanRenderer.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/XPVulkanUpscaler.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/XPVulkanWindow.cpp
    )
elseif(XP_RENDERER_WGPU)
    set(XPENGINE_SOURCES_RENDERER ${XPENGINE_SOURCES_RENDERER}
        ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/WGSL/XPWGSLContext.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/WGSL/XPWGSLGraph.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/WGSL/XPWGSLLang.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/WGSL/XPWGSLTextEditor.cpp

        ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/XPWGPUGPUData.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/XPWGPURenderer.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/XPWGPUUIImpl.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/WebGPU/XPWGPUWindow.cpp
    )
endif()
if (XP_RENDERER_SW)
    set(XPENGINE_SOURCES_RENDERER ${XPENGINE_SOURCES_RENDERER}
        ${CMAKE_SOURCE_DIR}/src/Renderer/SW/XPSWRenderer.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/SW/XPSWThirdParty.cpp
    )
endif()
set(XPENGINE_SOURCES_SCENE_DESCRIPTOR
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/Attachments/XPCollider.cpp
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/Attachments/XPRigidbody.cpp
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/Attachments/XPScript.cpp
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/Attachments/XPTransform.cpp
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPFilter.cpp
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPLayer.cpp
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPNode.cpp
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPScene.cpp
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPSceneStore.cpp
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPSceneDescriptorStore.cpp
)
set(XPENGINE_SOURCES_SHORTCUTS
    ${CMAKE_SOURCE_DIR}/src/Shortcuts/XPShortcuts.cpp
)
if(XP_EDITOR_MODE)
    set(XPENGINE_SOURCES_UI
        ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/XPImGUI.cpp
        ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/Assets.cpp
        ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/Console.cpp
        ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/EditorViewport.cpp
        ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/GameViewport.cpp
        ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/Emulator.cpp
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

        ${CMAKE_SOURCE_DIR}/src/UI/NodeEditor/XPNodeEditor.cpp
        ${CMAKE_SOURCE_DIR}/src/UI/NodeEditor/XPNodeEditorBlock.cpp
        ${CMAKE_SOURCE_DIR}/src/UI/NodeEditor/XPNodeEditorPin.cpp
        ${CMAKE_SOURCE_DIR}/src/UI/NodeEditor/XPNodeEditorPinLink.cpp
    )
else()
    set(XPENGINE_SOURCES_UI )
endif()
set(XPENGINE_SOURCES_UTILITIES
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPAnnotations.cpp
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPDiscarder.cpp
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPFreeCameraSystem.cpp
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPFS.cpp
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
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPIFileWatch.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPLightBuffer.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPLogic.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPMaterialAsset.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPMaterialBuffer.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPMeshAsset.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPMeshBuffer.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPPreloadedAssets.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPSceneAsset.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPShaderAsset.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPShaderBuffer.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPShaderLoader.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPStbTextureLoader.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPDataPipelineStore.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPTextureAsset.h
    ${CMAKE_SOURCE_DIR}/src/DataPipeline/XPTextureBuffer.h
)
set(XPENGINE_HEADERS_ENGINE
    ${CMAKE_SOURCE_DIR}/src/Engine/XPAllocators.h
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
set(XPENGINE_HEADERS_MCP
    ${CMAKE_SOURCE_DIR}/src/Mcp/XPMcpServer.h
)
set(XPENGINE_HEADERS_PAINTING
    ${CMAKE_SOURCE_DIR}/src/Painting/XPPainting.h
)
set(XPENGINE_HEADERS_PHYSICS
    ${CMAKE_SOURCE_DIR}/src/Physics/Interface/XPIPhysics.h
)
if(XP_PHYSICS_BULLET)
    set(XPENGINE_HEADERS_PHYSICS ${XPENGINE_HEADERS_PHYSICS}
        ${CMAKE_SOURCE_DIR}/src/Physics/Bullet/XPBulletPhysics.h
    )
elseif(XP_PHYSICS_JOLT)
    set(XPENGINE_HEADERS_PHYSICS ${XPENGINE_HEADERS_PHYSICS}
        ${CMAKE_SOURCE_DIR}/src/Physics/Jolt/XPJoltPhysics.h
    )
elseif(XP_PHYSICS_PHYSX4)
    set(XPENGINE_HEADERS_PHYSICS ${XPENGINE_HEADERS_PHYSICS}
        ${CMAKE_SOURCE_DIR}/src/Physics/PhysX4/XPPhysX4Physics.h
    )
elseif(XP_PHYSICS_PHYSX5)
    set(XPENGINE_HEADERS_PHYSICS ${XPENGINE_HEADERS_PHYSICS}
        ${CMAKE_SOURCE_DIR}/src/Physics/PhysX5/XPPhysX5Physics.h
    )
else()
    set(XPENGINE_HEADERS_PHYSICS ${XPENGINE_HEADERS_PHYSICS}
        ${CMAKE_SOURCE_DIR}/src/Physics/Empty/XPEmptyPhysics.h
    )
endif()
set(XPENGINE_HEADERS_PROFILER
    ${CMAKE_SOURCE_DIR}/src/Profiler/include/Profiler/XPProfiler.h
)
set(XPENGINE_HEADERS_RENDERER
    ${CMAKE_SOURCE_DIR}/src/Renderer/Interface/XPIRenderer.h
    ${CMAKE_SOURCE_DIR}/src/Renderer/Interface/XPIRendererBuffer.h
    ${CMAKE_SOURCE_DIR}/src/Renderer/Interface/XPIRendererCommandBuffer.h
    ${CMAKE_SOURCE_DIR}/src/Renderer/Interface/XPIRendererCommandQueue.h
    ${CMAKE_SOURCE_DIR}/src/Renderer/Interface/XPIRendererShaderCompute.h
    ${CMAKE_SOURCE_DIR}/src/Renderer/Interface/XPIRendererShaderSurface.h
    ${CMAKE_SOURCE_DIR}/src/Renderer/Interface/XPRendererGraph.h
)
if(XP_RENDERER_DX12)
    set(XPENGINE_HEADERS_RENDERER ${XPENGINE_HEADERS_RENDERER}
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/dx12al/CommandList.hpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/dx12al/ConstantBuffer.hpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/dx12al/DescriptorHeap.hpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/dx12al/Device.hpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/dx12al/Enums.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/dx12al/GraphicsPipeline.hpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/dx12al/PipelineStateObject.hpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/dx12al/Queue.hpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/dx12al/RootSignature.hpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/dx12al/Shader.hpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/dx12al/Swapchain.hpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/dx12al/Synchronization.hpp

        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/XPDX12.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/XPDX12GPUData.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/XPDX12Renderer.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/XPDX12Upscaler.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/XPDX12Window.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/XPShaderTypes.h
    )
elseif(XP_RENDERER_METAL)
    set(XPENGINE_HEADERS_RENDERER ${XPENGINE_HEADERS_RENDERER}
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/MSL/XPMSLContext.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/MSL/XPMSLGraph.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/MSL/XPMSLLang.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/MSL/XPMSLParser.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/MSL/XPMSLTextEditor.h

        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/metal_cpp.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetal.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalBuffer.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalBufferManaged.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalBufferMemoryless.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalBufferPrivate.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalBufferShared.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalCommandBuffer.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalCommandQueue.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalEvent.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalGPUData.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalNN.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalCulling.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalRenderer.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalUpscaler.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPMetalWindow.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/XPShaderTypes.h
    )
elseif(XP_RENDERER_VULKAN)
    set(XPENGINE_HEADERS_RENDERER ${XPENGINE_HEADERS_RENDERER}
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/Buffer.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/CommandBuffer.hpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/CommandPool.hpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/Defs.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/DescriptorSet.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/Device.hpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/Enums.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/FrameBuffer.hpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/FrameResources.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/FreeFunctions.hpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/GraphicsPipeline.hpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/Instance.hpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/PhysicalDevice.hpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/PushConstantData.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/Queue.hpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/Shader.hpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/Swapchain.hpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/Synchronization.hpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/Texture.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/val/val.h

        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/XPShaderTypes.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/XPVulkan.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/XPVulkanExt.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/XPVulkanGPUData.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/XPVulkanRenderer.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/XPVulkanUpscaler.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/XPVulkanWindow.h
    )
elseif(XP_RENDERER_WGPU)
    set(XPENGINE_HEADERS_RENDERER ${XPENGINE_HEADERS_RENDERER}
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
endif()
if (XP_RENDERER_SW)
    set(XPENGINE_HEADERS_RENDERER ${XPENGINE_HEADERS_RENDERER}
        ${CMAKE_SOURCE_DIR}/src/Renderer/SW/XPSWBVH.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/SW/XPSWImporter.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/SW/XPSWLogger.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/SW/XPSWMaths.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/SW/XPSWMemoryPool.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/SW/XPSWRasterizer.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/SW/XPSWRenderer.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/SW/XPSWRendererCommon.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/SW/XPSWSceneDescriptor.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/SW/XPSWTests.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/SW/XPSWTexture.h
        ${CMAKE_SOURCE_DIR}/src/Renderer/SW/XPSWThirdParty.h
    )
endif()
set(XPENGINE_HEADERS_SCENE_DESCRIPTOR
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/Attachments/XPCollider.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/Attachments/XPFreeCamera.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/Attachments/XPLogic.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/Attachments/XPMeshRenderer.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/Attachments/XPOrbitCamera.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/Attachments/XPRigidbody.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/Attachments/XPScript.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/Attachments/XPTransform.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPAttachments.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPEnums.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPFilter.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPLayer.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPNode.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPScene.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPSceneStore.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPSceneDescriptorStore.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPTypes.h
)
set(XPENGINE_HEADERS_SHORTCUTS
    ${CMAKE_SOURCE_DIR}/src/shortcuts/XPShortcuts.h
)
if(XP_EDITOR_MODE)
    set(XPENGINE_HEADERS_UI
        ${CMAKE_SOURCE_DIR}/src/UI/Interface/XPIUI.h
        ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/XPImGUI.h
        ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/Assets.h
        ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/Console.h
        ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/EditorViewport.h
        ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/GameViewport.h
        ${CMAKE_SOURCE_DIR}/src/UI/ImGUI/Tabs/Emulator.h
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

        ${CMAKE_SOURCE_DIR}/src/UI/NodeEditor/XPNodeEditor.h
        ${CMAKE_SOURCE_DIR}/src/UI/NodeEditor/XPNodeEditorBlock.h
        ${CMAKE_SOURCE_DIR}/src/UI/NodeEditor/XPNodeEditorPin.h
        ${CMAKE_SOURCE_DIR}/src/UI/NodeEditor/XPNodeEditorPinLink.h
    )
else()
    set(XPENGINE_HEADERS_UI )
endif()
set(XPENGINE_HEADERS_UTILITIES
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPAnnotations.h
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPBitArray.h
    ${CMAKE_SOURCE_DIR}/src/Utilities/XPBitFlag.h
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
if(XP_EDITOR_MODE)
    set(IMGUI_SOURCES
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/imgui_node_editor/crude_json.cpp
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/imgui_node_editor/imgui_canvas.cpp
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/imgui_node_editor/imgui_node_editor_api.cpp
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/imgui_node_editor/imgui_node_editor.cpp    

        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/ImGuiFileDialog/ImGuiFileDialog.cpp
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/ImGuiFileDialog/ImGuiFileDialog.h
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/ImGuiFileDialog/ImGuiFileDialogConfig.h

        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/client/Private/NetImgui_Api.cpp
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/client/Private/NetImgui_Client.cpp
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/client/Private/NetImgui_Client.h
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/client/Private/NetImgui_Client.inl
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/client/Private/NetImgui_CmdPackets_DrawFrame.cpp
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/client/Private/NetImgui_CmdPackets_DrawFrame.h
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/client/Private/NetImgui_CmdPackets.h
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/client/Private/NetImgui_CmdPackets.inl
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/client/Private/NetImgui_Network.h
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/client/Private/NetImgui_Shared.h
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/client/Private/NetImgui_Shared.inl
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/client/Private/NetImgui_WarningDisable.h
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/client/Private/NetImgui_WarningDisableImgui.h
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/client/Private/NetImgui_WarningDisableStd.h
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/client/Private/NetImgui_WarningReenable.h
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/client/NetImgui_Api.h
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/client/NetImgui_Config.h

        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/text_editor/TextEditor.cpp
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/text_editor/TextEditor.h

        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/imconfig.h
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/imgui.cpp
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/imgui.h
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/imgui_demo.cpp
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/imgui_draw.cpp
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/imgui_helper.h
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/imgui_internal.h
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/imgui_stdlib.cpp
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/imgui_stdlib.h
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/imgui_tables.cpp
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/imgui_widgets.cpp
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/imguizmo.cpp
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/imguizmo.h

        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/implot_internal.h
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/implot_items.cpp
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/implot.cpp
        ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/implot.h
    )
else()
    set(IMGUI_SOURCES )
endif()

if(XP_RENDERER_DX12)
    set(XPENGINE_SURFACE_SHADER_SOURCES
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/shaders/GBuffer.hlsl
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/shaders/Line.hlsl
        ${CMAKE_SOURCE_DIR}/src/Renderer/DX12/shaders/MBuffer.hlsl
    )
    set(XPENGINE_COMPUTE_SHADER_SOURCES
        
    )
elseif(XP_RENDERER_METAL)
    set(XPENGINE_SURFACE_SHADER_SOURCES
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/shaders/GBuffer.metal
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/shaders/Line.metal
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/shaders/MBuffer.metal
    )
    set(XPENGINE_COMPUTE_SHADER_SOURCES
        ${CMAKE_SOURCE_DIR}/src/Renderer/Metal/shaders/MainCompute.metal
    )
elseif(XP_RENDERER_VULKAN)
    set(XPENGINE_SURFACE_SHADER_SOURCES
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/shaders/GBuffer.vert
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/shaders/GBuffer.frag
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/shaders/Line.vert
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/shaders/Line.frag
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/shaders/MBuffer.vert
        ${CMAKE_SOURCE_DIR}/src/Renderer/Vulkan/shaders/MBuffer.frag
    )
    set(XPENGINE_COMPUTE_SHADER_SOURCES
        
    )
endif()

set(XPEDITOR_SOURCES
    ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/src/NetImguiServer_App_Custom.cpp
    ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/src/NetImguiServer_App_GlfwGL3.cpp
    ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/src/NetImguiServer_App.cpp
    ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/src/NetImguiServer_App.h
    ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/src/NetImguiServer_Config.cpp
    ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/src/NetImguiServer_HAL_GL3.cpp
    ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/src/NetImguiServer_HAL_Glfw.cpp    
    ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/src/NetImguiServer_Config.h
    ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/src/NetImguiServer_Network.cpp
    ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/src/NetImguiServer_Network.h
    ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/src/NetImguiServer_RemoteClient.cpp
    ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/src/NetImguiServer_RemoteClient.h
    ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/src/NetImguiServer_UI.cpp
    ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/src/NetImguiServer_UI.h
    ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/ThirdParty/DearImgui/imgui_demo.cpp
    ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/ThirdParty/DearImgui/imgui_draw.cpp
    ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/ThirdParty/DearImgui/imgui_tables.cpp
    ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/ThirdParty/DearImgui/imgui_widgets.cpp
    ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/server/ThirdParty/DearImgui/imgui.cpp
    ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/client/Private/NetImgui_Api.cpp
    ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/client/Private/NetImgui_Client.cpp
    ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src/netimgui/client/Private/NetImgui_CmdPackets_DrawFrame.cpp
)

set(XPTHIRDPARTY_LINK_DIRECTORIES
    ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/lib/
)

set(XPTHIRDPARTY_LINK_LIBRARIES
    LuaLib
    XPEmulator
)
if(XP_RENDERER_DX12)
    set(XPTHIRDPARTY_LINK_LIBRARIES ${XPTHIRDPARTY_LINK_LIBRARIES}
        d3d12
        dxgi
        d3dcompiler
    )
elseif(XP_RENDERER_METAL)
    set(XPTHIRDPARTY_LINK_LIBRARIES ${XPTHIRDPARTY_LINK_LIBRARIES}
        "-framework AudioToolbox"
        "-framework AVFoundation"
        "-framework Carbon"
        "-framework Cocoa"
        "-framework CoreAudio"
        "-framework CoreGraphics"
        "-framework CoreHaptics"
        "-framework CoreMotion"
        "-framework Foundation"
        "-framework ForceFeedback"
        "-framework GameController"
        "-framework ImageIO"
        "-framework IOKit"
        "-framework QuartzCore"
        "-framework Metal"
        "-framework ModelIO"
        "-framework MetalFX"
        "-framework MetalKit"
    )
elseif(XP_RENDERER_VULKAN)
    set(XPTHIRDPARTY_LINK_LIBRARIES ${XPTHIRDPARTY_LINK_LIBRARIES}
        Vulkan::Vulkan
    )
    if(APPLE)
        set(XPTHIRDPARTY_LINK_LIBRARIES ${XPTHIRDPARTY_LINK_LIBRARIES}
            "-framework AudioToolbox"
            "-framework AVFoundation"
            "-framework Carbon"
            "-framework Cocoa"
            "-framework CoreAudio"
            "-framework CoreGraphics"
            "-framework CoreHaptics"
            "-framework CoreMotion"
            "-framework Foundation"
            "-framework ForceFeedback"
            "-framework GameController"
            "-framework ImageIO"
            "-framework IOKit"
            "-framework QuartzCore"
            "-framework Metal"
            "-framework ModelIO"
            "-framework MetalFX"
            "-framework MetalKit"
        )
    endif()
endif()
if(XP_PHYSICS_BULLET)
    set(XPTHIRDPARTY_LINK_LIBRARIES ${XPTHIRDPARTY_LINK_LIBRARIES}
    )
elseif(XP_PHYSICS_JOLT)
    set(XPTHIRDPARTY_LINK_LIBRARIES ${XPTHIRDPARTY_LINK_LIBRARIES}
        Jolt
    )
elseif (XP_PHYSICS_PHYSX4)
    set(XPTHIRDPARTY_LINK_LIBRARIES ${XPTHIRDPARTY_LINK_LIBRARIES}
    )
elseif(XP_PHYSICS_PHYSX5)
    set(XPTHIRDPARTY_LINK_LIBRARIES ${XPTHIRDPARTY_LINK_LIBRARIES}
    )
else()
    set(XPTHIRDPARTY_LINK_LIBRARIES ${XPTHIRDPARTY_LINK_LIBRARIES}
    )
endif()
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
    ${CMAKE_SOURCE_DIR}/src/Emulator/include
    ${CMAKE_SOURCE_DIR}/src/Profiler/include
    ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/include
    ${XP_THIRDPARTY_PLATFORM_ARTIFACTS_PATH}/src
)
if(XP_RENDERER_METAL)
    set(XPTHIRDPARTY_INCLUDE_DIRECTORIES ${XPTHIRDPARTY_INCLUDE_DIRECTORIES}
        ${CMAKE_SOURCE_DIR}/thirdparty/metal-cpp
    )
endif()

# touch auto-generated files
set(AUTO_GENERATED_FILES
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPNode.cpp
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPSceneStore.cpp

    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPAttachments.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPNode.h
    ${CMAKE_SOURCE_DIR}/src/SceneDescriptor/XPSceneStore.h
)
foreach(AUTO_GENERATED_FILE ${AUTO_GENERATED_FILES})
    file(TOUCH ${AUTO_GENERATED_FILE})
endforeach()
# ---------------------------------------------------------------------------------------------------------------------------------------------------

