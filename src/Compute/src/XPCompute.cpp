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

#include <Compute/XPCompute.h>

#include <Compute/XPComputeElfLoader.h>
#include <Compute/XPImageWorks.h>
#include <Compute/shaders/shared/structures.h>

#include <assert.h>
#if defined(XP_USE_COMPUTE_CUDA)
    #include <cuda.h>
    #include <cuda_runtime.h>
#elif defined(XP_USE_COMPUTE_METAL)
    #include <Foundation/Foundation.hpp>
    #include <Metal/Metal.hpp>
    #include <QuartzCore/QuartzCore.hpp>
#endif
#include <fstream>
#include <inttypes.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#if defined(XP_USE_COMPUTE_CUDA)
void
load_and_run_cuda(const char* riscvCoreProgram);
#elif defined(XP_USE_COMPUTE_VULKAN)
void
load_and_run_vulkan(const char* riscvCoreProgram);
#elif defined(XP_USE_COMPUTE_METAL)
void
load_and_run_metal(const char* riscvCoreProgram);
#elif defined(XP_USE_COMPUTE_DX12)
void
load_and_run_dx12(const char* riscvCoreProgram);
#endif

XP_EXTERN void
xp_compute_load_and_run(const char* riscvCoreProgram)
{
#if defined(XP_USE_COMPUTE_CUDA)
    load_and_run_cuda(riscvCoreProgram);
#elif defined(XP_USE_COMPUTE_VULKAN)
    load_and_run_vulkan(riscvCoreProgram);
#elif defined(XP_USE_COMPUTE_METAL)
    load_and_run_metal(riscvCoreProgram);
#elif defined(XP_USE_COMPUTE_DX12)
    load_and_run_dx12(riscvCoreProgram);
#endif
}

#if defined(XP_USE_COMPUTE_CUDA)

    #define CUDA_CHECK(call)                                                                                           \
        do {                                                                                                           \
            cudaError_t err = call;                                                                                    \
            if (err != cudaSuccess) {                                                                                  \
                std::cerr << "CUDA error at " << __FILE__ << ":" << __LINE__ << " - " << cudaGetErrorString(err)       \
                          << std::endl;                                                                                \
                exit(1);                                                                                               \
            }                                                                                                          \
        } while (0)

    #define CU_CHECK(call)                                                                                             \
        do {                                                                                                           \
            CUresult err = call;                                                                                       \
            if (err != CUDA_SUCCESS) {                                                                                 \
                const char* errStr;                                                                                    \
                cuGetErrorString(err, &errStr);                                                                        \
                std::cerr << "CUDA driver error at " << __FILE__ << ":" << __LINE__ << " - " << errStr << std::endl;   \
                exit(1);                                                                                               \
            }                                                                                                          \
        } while (0)

std::string
loadPTXFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) { throw std::runtime_error("Failed to open PTX file: " + filename); }

    return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

void
load_and_run_cuda(const char* riscvCoreProgram)
{
    const int    N    = 144;
    const size_t size = N * sizeof(XPXPUProcessor);

    CU_CHECK(cuInit(0));

    CUdevice device;
    CU_CHECK(cuDeviceGet(&device, 0));

    CUcontext context;
    CU_CHECK(cuCtxCreate(&context, 0, device));

    std::string ptxCode = loadPTXFile("compute/compute.ptx");
    std::cout << "Loaded PTX file (" << ptxCode.size() << " bytes)\n";

    CUmodule module;
    CU_CHECK(cuModuleLoadData(&module, ptxCode.c_str()));

    CUfunction kernel;
    CU_CHECK(cuModuleGetFunction(&kernel, module, "computeMain"));

    std::vector<XPXPUProcessor> h_processors(N);

    memset(h_processors[0].bus.memory.flash, 0, XP_XPU_CONFIG_MEMORY_FLASH_SIZE);
    memset(h_processors[0].bus.memory.ram, 0, XP_XPU_CONFIG_MEMORY_RAM_SIZE);
    memset(h_processors[0].bus.memory.heap, 0, XP_XPU_CONFIG_MEMORY_HEAP_SIZE);
    memset(h_processors[0].bus.hostMappedMemory.data, 0, XP_XPU_CONFIG_HMM_SIZE);

    RiscvElfLoader* loader = xp_xpu_elf_loader_load(riscvCoreProgram);
    if (loader == NULL) {
        printf("ELF Program loading failed.\n");
        return;
    }

    assert(loader->segments[0].vaddr == XP_EMULATOR_CONFIG_MEMORY_FLASH_BASE);
    assert(loader->segments[1].vaddr == XP_EMULATOR_CONFIG_MEMORY_RAM_BASE);
    assert(loader->segments[2].vaddr == XP_EMULATOR_CONFIG_MEMORY_HEAP_BASE);

    h_processors[0].regs[int(XPXPUEReg::R0)] = 0;
    h_processors[0].regs[int(XPXPUEReg::R2)] = XP_XPU_CONFIG_HMM_TOP_STACK_PTR;
    h_processors[0].pc                       = 0;

    memcpy(h_processors[0].bus.memory.flash, loader->segments[0].data, loader->segments[0].size);
    memcpy(h_processors[0].bus.memory.ram, loader->segments[1].data, loader->segments[1].size);
    memcpy(h_processors[0].bus.memory.heap, loader->segments[2].data, loader->segments[2].size);

    h_processors[0].pc = loader->entry_point;

    for (size_t i = 1; i < N; ++i) { memcpy(&h_processors[i], &h_processors[0], sizeof(XPXPUProcessor)); }

    CUdeviceptr d_processors;
    CU_CHECK(cuMemAlloc(&d_processors, size));

    CU_CHECK(cuMemcpyHtoD(d_processors, h_processors.data(), size));

    void* kernelParams[] = { &d_processors, (void*)&N };

    unsigned int threadsPerBlock = 256;
    unsigned int blocksPerGrid   = (N + threadsPerBlock - 1) / threadsPerBlock;

    std::cout << "Launching kernel with " << blocksPerGrid << " blocks of " << threadsPerBlock << " threads\n";

    CU_CHECK(cuLaunchKernel(kernel, blocksPerGrid, 1, 1, threadsPerBlock, 1, 1, 0, 0, kernelParams, 0));
    CU_CHECK(cuCtxSynchronize());
    CU_CHECK(cuMemcpyDtoH(h_processors.data(), d_processors, size));

    for (size_t i = 0; i < N; ++i) {
        printf("XPU\n");
        printf("stackBottomAddress is %08" PRIx32 "\n", h_processors[i].bus.hostMappedMemory.deviceBottomStack);
        printf("stackTopAddress is %08" PRIx32 "\n", h_processors[i].bus.hostMappedMemory.deviceTopStack);
        printf("heapStartAddress is %08" PRIx32 "\n", h_processors[i].bus.hostMappedMemory.deviceStartHeap);
        printf("heapEndAddress is %08" PRIx32 "\n", h_processors[i].bus.hostMappedMemory.deviceEndHeap);
        printf("program.pc = %u\n", h_processors[i].pc);

        uint32_t fbAddr = h_processors[i].bus.hostMappedMemory.deviceStartHeap;
        if (fbAddr != 0) {
    #define WIDTH  120
    #define HEIGHT 120

            uint8_t  u8texture[WIDTH * HEIGHT * 3];
            uint32_t mem_address = fbAddr - XP_XPU_CONFIG_MEMORY_HEAP_BASE;
            uint8_t* etex        = (uint8_t*)&(h_processors[i].bus.memory.heap[mem_address]);
            for (int y = 0; y < HEIGHT; y++) {
                for (int x = 0; x < WIDTH; x++) {
                    int pixel_index = (y * WIDTH + x) * 3;

                    uint8_t rVal0 = etex[(pixel_index + 0) * 4];
                    uint8_t rVal1 = etex[(pixel_index + 0) * 4 + 1];
                    uint8_t rVal2 = etex[(pixel_index + 0) * 4 + 2];
                    uint8_t rVal3 = etex[(pixel_index + 0) * 4 + 3];
                    float   rVal  = (float)(uint32_t)((uint32_t)rVal3 | (uint32_t)rVal2 << 8 | (uint32_t)rVal1 << 16 |
                                                   (uint32_t)rVal0 << 24);

                    uint8_t gVal0 = etex[(pixel_index + 1) * 4];
                    uint8_t gVal1 = etex[(pixel_index + 1) * 4 + 1];
                    uint8_t gVal2 = etex[(pixel_index + 1) * 4 + 2];
                    uint8_t gVal3 = etex[(pixel_index + 1) * 4 + 3];
                    float   gVal  = (float)(uint32_t)((uint32_t)gVal3 | (uint32_t)gVal2 << 8 | (uint32_t)gVal1 << 16 |
                                                   (uint32_t)gVal0 << 24);

                    uint8_t bVal0 = etex[(pixel_index + 2) * 4];
                    uint8_t bVal1 = etex[(pixel_index + 2) * 4 + 1];
                    uint8_t bVal2 = etex[(pixel_index + 2) * 4 + 2];
                    uint8_t bVal3 = etex[(pixel_index + 2) * 4 + 3];
                    float   bVal  = (float)(uint32_t)((uint32_t)bVal3 | (uint32_t)bVal2 << 8 | (uint32_t)bVal1 << 16 |
                                                   (uint32_t)bVal0 << 24);

                    u8texture[pixel_index + 0] = (uint8_t)(rVal * 255.0f);
                    u8texture[pixel_index + 1] = (uint8_t)(gVal * 255.0f);
                    u8texture[pixel_index + 2] = (uint8_t)(bVal * 255.0f);
                }
            }

            std::stringstream ss;
            ss << "gpu_rt0_" << i << ".ppm";
            save_as_ppm(ss.str().c_str(), u8texture, (uint32_t)120, (uint32_t)120);
        }
    }

    cuMemFree(d_processors);
    cuModuleUnload(module);
    cuCtxDestroy(context);
}
#elif defined(XP_USE_COMPUTE_VULKAN)
void
load_and_run_vulkan(const char* riscvCoreProgram)
{
}
#elif defined(XP_USE_COMPUTE_METAL)
void
load_and_run_metal(const char* riscvCoreProgram)
{
    MTL::Device*       device       = MTL::CreateSystemDefaultDevice();
    MTL::CommandQueue* commandQueue = device->newCommandQueue();

    NS::Error*                 error           = nullptr;
    NS::String*                libPath         = NS::String::string("compute/compute.metallib", NS::UTF8StringEncoding);
    NS::URL*                   libraryURL      = NS::URL::fileURLWithPath(libPath);
    MTL::Library*              library         = device->newLibrary(libraryURL, &error);
    NS::String*                funcName        = NS::String::string("computeMain", NS::UTF8StringEncoding);
    MTL::Function*             computeFunction = library->newFunction(funcName);
    MTL::ComputePipelineState* computePipelineState = device->newComputePipelineState(computeFunction, &error);

    const int    N    = 144;
    const size_t size = N * sizeof(XPXPUProcessor);

    std::vector<XPXPUProcessor> h_processors(N);

    load_riscv_binary(&h_processors[0], riscvCoreProgram);
    for (size_t i = 1; i < N; ++i) { memcpy(&h_processors[i], &h_processors[0], sizeof(XPXPUProcessor)); }

    MTL::Buffer* inputBuffer = device->newBuffer(h_processors.data(), size, MTL::ResourceStorageModeShared);

    MTL::CommandBuffer* commandBuffer = commandQueue->commandBuffer();

    MTL::ComputeCommandEncoder* computeEncoder = commandBuffer->computeCommandEncoder();

    computeEncoder->setComputePipelineState(computePipelineState);

    computeEncoder->setBuffer(inputBuffer, 0, 0);
    computeEncoder->setBytes((void*)&N, sizeof(uint32_t), 1);

    MTL::Size threadsPerThreadgroup = MTL::Size::Make(256, 1, 1);
    MTL::Size threadgroupsPerGrid   = MTL::Size::Make((N + 255) / 256, 1, 1);

    computeEncoder->dispatchThreadgroups(threadgroupsPerGrid, threadsPerThreadgroup);

    computeEncoder->endEncoding();

    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();

    if (commandBuffer->error()) {
        std::cerr << "Command buffer error: " << commandBuffer->error()->localizedDescription()->utf8String()
                  << std::endl;
        return;
    }

    XPXPUProcessor* processors = (XPXPUProcessor*)inputBuffer->contents();

    uint8_t texture[120 * 120 * 3];
    for (size_t i = 0; i < N; ++i) {
        printf("XPU\n");
        printf("heapStartAddress is %08" PRIx32 "\n", processors[i].bus.hostMappedMemory.deviceHeapStart);
        printf("heapEndAddress is %08" PRIx32 "\n", processors[i].bus.hostMappedMemory.deviceHeapEnd);
        printf("stackBaseAddress is %08" PRIx32 "\n", processors[i].bus.hostMappedMemory.deviceStackBase);
        printf("stackTopAddress is %08" PRIx32 "\n", processors[i].bus.hostMappedMemory.deviceStackTop);
        printf("program.pc = %u\n", processors[i].pc);

        uint32_t fbAddr = processors[i].bus.hostMappedMemory.deviceStartHeap;
        if (fbAddr != 0) {
            uint32_t mem_address = fbAddr - XP_XPU_CONFIG_MEMORY_HEAP_BASE;
            uint8_t* etex        = (uint8_t*)&(processors[i].bus.memory.heap[mem_address]);
            for (int y = 0; y < 120; y++) {
                for (int x = 0; x < 120; x++) {
                    int pixel_index = (y * 120 + x) * 3;

                    uint8_t rVal0 = etex[(pixel_index + 0) * 4];
                    uint8_t rVal1 = etex[(pixel_index + 0) * 4 + 1];
                    uint8_t rVal2 = etex[(pixel_index + 0) * 4 + 2];
                    uint8_t rVal3 = etex[(pixel_index + 0) * 4 + 3];
                    float   rVal  = (float)(uint32_t)((uint32_t)rVal3 | (uint32_t)rVal2 << 8 | (uint32_t)rVal1 << 16 |
                                                   (uint32_t)rVal0 << 24);

                    uint8_t gVal0 = etex[(pixel_index + 1) * 4];
                    uint8_t gVal1 = etex[(pixel_index + 1) * 4 + 1];
                    uint8_t gVal2 = etex[(pixel_index + 1) * 4 + 2];
                    uint8_t gVal3 = etex[(pixel_index + 1) * 4 + 3];
                    float   gVal  = (float)(uint32_t)((uint32_t)gVal3 | (uint32_t)gVal2 << 8 | (uint32_t)gVal1 << 16 |
                                                   (uint32_t)gVal0 << 24);

                    uint8_t bVal0 = etex[(pixel_index + 2) * 4];
                    uint8_t bVal1 = etex[(pixel_index + 2) * 4 + 1];
                    uint8_t bVal2 = etex[(pixel_index + 2) * 4 + 2];
                    uint8_t bVal3 = etex[(pixel_index + 2) * 4 + 3];
                    float   bVal  = (float)(uint32_t)((uint32_t)bVal3 | (uint32_t)bVal2 << 8 | (uint32_t)bVal1 << 16 |
                                                   (uint32_t)bVal0 << 24);

                    texture[pixel_index + 0] = (uint8_t)(rVal * 255.0f);
                    texture[pixel_index + 1] = (uint8_t)(gVal * 255.0f);
                    texture[pixel_index + 2] = (uint8_t)(bVal * 255.0f);
                }
            }
            std::stringstream ss;
            ss << "gpu_rt0_" << i << ".ppm";
            save_as_ppm(ss.str().c_str(), texture, (uint32_t)120, (uint32_t)120);
        }
    }
}
#elif defined(XP_USE_COMPUTE_DX12)
void
load_and_run_dx12(const char* riscvCoreProgram)
{
}
#endif