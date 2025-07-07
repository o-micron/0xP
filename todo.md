# Todo status
|✅|🚧|📋|⏸️|🛑|
|:--:|:--:|:--:|:--:|:--:|
|Done|In Progress|planned and ready to start|Not planned and needs more time|Not Planned, not scheduled or deprioritized|

|                                                 | MacOS | Linux | Windows | Web |
|-----------------------------------------------------------|:-----:|:-----:|:-------:|:---:|
| <font color="yellow">**Current needed improvements and Issues**</font>      |🚧|🚧|🚧|🚧|
| UI and Editor general bug fixing                                            |📋|📋|📋|📋|
| Put Scene Description on GPU and fix bugs                                   |📋|📋|📋|📋|
| <font color="yellow">**Editor integration**</font>                          |✅|✅|✅|✅|
| <font color="yellow">**Tracy Profiler integration**</font>                  |✅|✅|✅|✅|
| <font color="yellow">**Custom SW Rasterizer Subsystem**</font>                     |||||
| Rasterizer                                                                  |✅|✅|✅|✅|
| Raytraced Shadows only                                                      |✅|✅|✅|🚧|
| Full Raytracing                                                             |✅|✅|✅|🚧|
| Divide rasterization workloads on threads                                   |📋|📋|📋|📋|
| Use tiled architecture instead (tile partitioning)                          |📋|📋|📋|📋|
| Early depth workaround for supporting transparency and blending             |📋|📋|📋|📋|
| Optimize computations with SIMD                                             |📋|📋|📋|📋|
| GPU Compute Shader Based (CUDA)                                             |🛑|📋|📋|🛑|
| GPU Compute Shader Based (Metal Compute)                                    |📋|🛑|🛑|🛑|
| <font color="yellow">**GPU Rendering Subsystem**</font>          |Metal|Vulkan|Dx12|WEBGPU|
| RHI                                                                         |⏸️|⏸️|⏸️|⏸️|
| debugging                                |Xcode tools|RenderDoc|Nvidia Nsights GFX/COMPUTE|WGPU Inspector|
| Object Selection from Scene (for editor)                                    |✅|🚧|🛑|📋|
| GPU commands execution timing                                               |✅|🚧|🛑|📋|
| GPU compute based frustum culling                                           |✅|📋|🛑|📋|
| Visualise bounding boxes                                                    |✅|📋|🛑|📋|
| Visualise frustum culling with freeze                                       |✅|📋|🛑|📋|
| Shader reflection system                                                    |📋|✅|🛑|📋|
| Shader specialization                                                       |📋|📋|📋|📋|
| Unified Shading language across all APIs (Slang)                            |📋|📋|📋|📋|
| Node based material system                                                  |📋|📋|📋|📋|
| Upscaling                                                                   |⏸️|⏸️|⏸️|⏸️|
| Physically based rendering                                                  |⏸️|⏸️|⏸️|⏸️|
| Raytracing (GI/Shadows)                                                     |📋|📋|🛑|📋|
| GPU driven architecture                                                     |📋|📋|🛑|🛑|
| MeshShaders experiments                                                     |📋|📋|📋|🛑|
| Workgraph experiments                                                       |🛑|📋|📋|🛑|
| <font color="yellow">**Neural Rendering**</font>                            |📋|📋|📋|📋|
| <font color="yellow">**Physics Subsystem**</font>                                     |||||
| Jolt                                                                        |✅|✅|✅|🚧|
| Nvidia's PhysX 4                                                            |📋|📋|📋|📋|
| Nvidia's PhysX 5                                                            |⏸️|📋|📋|⏸️|
| Bullet Physics                                                              |🛑|🛑|🛑|🛑|
| <font color="yellow">**Data Pipeline Subsystem**</font>                               |||||
| File Watch                                                                  |✅|✅|✅|⏸️|
| Hot Reload                                                                  |✅|🚧|⏸️|⏸️|
| <font color="yellow">**Emulator Subsystem**</font>                                    |||||
| Platform Risc-V toolchain ready to build examples                           |✅|✅|🛑|🛑|
| Core Emulator                                                               |✅|✅|✅|✅|
| <font color="yellow">**UI Subsystem**</font>                                          |||||
| Remote UI                                                                   |✅|✅|✅|📋|
| Realtime modification of UI themes                                          |✅|✅|✅|✅|
| Adding more elegant design and layout (glass?)                              |📋|📋|📋|📋|
| <font color="yellow">**AI/MCPServer Subsystem**</font>                                |||||
| Controlling Actor/Entities creation/destruction and configuration           |🚧|🚧|🚧|🚧|
| <font color="yellow">**Scene Description Subsystem**</font>                           |||||
| Use attachments to describe components data                                 |✅|✅|✅|✅|
| Allow LLVM/Clang based code generation for scene description system         |✅|✅|✅|✅|
| Allow LLVM/Clang based UI code generation for attachments                   |✅|✅|✅|✅|
| Allow LLVM/Clang based new function interfaces                              |✅|✅|✅|✅|
| Add support for secondary attachments                                       |✅|✅|✅|✅|
| Add support for attachments members for unions and bitfields                |📋|📋|📋|📋|
| <font color="yellow">**Scripting Subsystem**</font>                                   |||||
| LUA                                                                         |✅|✅|✅|✅|
| Rust                                                                        |✅|✅|✅|🛑|
| C                                                                           |✅|✅|✅|🛑|
| C++                                                                         |✅|✅|✅|🛑|
| <font color="yellow">**Web based engine and editor**</font>                           |||||
| WebGPU                                                                      |🛑|🛑|🛑|✅|
| Use Wasm SIMD intrinsics                                                    |🛑|🛑|🛑|📋|
| Wasm64 experimental                                                         |🛑|🛑|🛑|📋|

