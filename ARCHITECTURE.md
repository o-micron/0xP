# Architecture

The core structure that binds everything together is the `Registry` class. \
This class holds all instances used by the engine and it is configured at engine startup. \
The closest design pattern to this is the `dependency injection pattern`. \
Mainly I check the definitions coming from `CMAKE` on which backends to be used then I create instances for each subsystem accordingly. \
Once I load all instances I then launch the engine loop. \
The registry does not expect much changes of these instances on runtime, but in theory you can change them. \
So for example you can decide to switch from `Vulkan` to `Metal` mid the runtime of the engine. \
In theory as well with small changes you can run both `Vulkan` and `Metal` side by side to check rendering missmatches, but more on that later ..  

```mermaid
flowchart TB
Registry ---> Renderer
Registry ---> Physics
Registry ---> UI
Registry ---> DataPipelineStore
Registry ---> Scene
Registry ---> FileWatch
Registry ---> ..
```

---
<br>
<br>
<br>

# Data Pipeline
The 0xP engine loads assets fom disk then transforms the information into structures to then be consumed by the different subsystems. \
Also because the engine is having builtin hot-reload for almost all assets loaded from disk, it keeps track of filesystem changes related to the loaded assets.
This then allows you to press the button next to the play button in the editor to apply the changes immediately and do the hot-reload ..

For example loading a 3D model "cube.obj" from disk is then transformed into an `XPMeshAsset` and `XPMeshBuffer` to then be consumed by the `Renderer` to upload the [vertices, normals, ..] into the gpu buffers ..
For more on that, check the source code at `src/DataPipeline/`

Here's also some drawings to help you out understanding how it works.

```mermaid
flowchart TB
XPFileWatchUnix::start ---> LoadPreloadedAssets
XPFileWatchUnix::start ---> LoadAssets
XPFileWatchUnix::start ---> LaunchFileWatch

LoadAssets ---> LoadMeshes
LoadAssets ---> LoadShaders
LoadAssets ---> LoadTextures

LoadMeshes ---> id1["Store::CreateNewFile"] ---> id2["Store::CreateNewMeshBuffer"] ---> id3["OnFileCreated(BufferType)"]
LoadShaders ---> id1["Store::CreateNewFile"] ---> id5["Store::CreateNewShaderBuffer"] ---> id3["OnFileCreated(BufferType)"]
LoadTextures ---> id1["Store::CreateNewFile"] ---> id8["Store::CreateNewTextureBuffer"] ---> id3["OnFileCreated(BufferType)"]

id3["OnFileCreated(BufferType)"] ---> id10["LoadMeshBufferFromFile"]
id3["OnFileCreated(BufferType)"] ---> id11["LoadShaderBufferFromFile"]
id3["OnFileCreated(BufferType)"] ---> id12["LoadTextureBufferFromFile"]
```

<br />
<br />
<br />
<br />
<br />
<br />
<br />

# Mesh Hierarchy

```mermaid
flowchart TB
MeshAsset([MeshAsset]) ----> GPURef([GPURef-XPMetalMesh-])
MeshAsset([MeshAsset]) ---> MeshBuffer([MeshBuffer])

MeshBuffer([MeshBuffer]) ---> Id([Id])
MeshBuffer([MeshBuffer]) ---> File([File])
MeshBuffer([MeshBuffer]) ----> MeshBufferObject[(MeshBufferObject)]
MeshBuffer([MeshBuffer]) ---> Positions[(Positions)]
MeshBuffer([MeshBuffer]) ---> Normals[(Normals)]
MeshBuffer([MeshBuffer]) ---> Texcoords[(Texcoords)]


MeshBufferObject[(MeshBufferObject)] ---> Name([Name])
MeshBufferObject[(MeshBufferObject)] ---> VertexOffset([VertexOffset])
MeshBufferObject[(MeshBufferObject)] ---> IndexOffset([IndexOffset])
MeshBufferObject[(MeshBufferObject)] ---> NumIndices([NumIndices])
```

<br />
<br />
<br />
<br />
<br />
<br />
<br />

# File And Resources Creation pipeline

```mermaid
flowchart LR

id1["File::Create"] ---> id2["File::onCreated"] ---> id3["createResourceTypeBuffer"] ---> id4["loadResourceTypeFromDisk"]
id1["File::Create"] ---> id5["File::onReloaded"] --> id3 --> id4
```

<br>
<br>
<br>
<br>
<br>

