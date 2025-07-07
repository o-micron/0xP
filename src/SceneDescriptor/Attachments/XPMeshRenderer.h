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

#ifndef XP_ATTACHMENT_MESH_RENDERER_H
#define XP_ATTACHMENT_MESH_RENDERER_H

#include <SceneDescriptor/XPEnums.h>
#include <SceneDescriptor/XPTypes.h>
#include <Utilities/XPAnnotations.h>
#include <Utilities/XPMaths.h>
#include <Utilities/XPPlatforms.h>

#include <string>
#include <vector>

class XPNode;
class XPIUI;
class XPMeshBuffer;

struct XPMeshRendererInfo
{
    const XPMeshBuffer*        meshBuffer;
    uint32_t                   meshBufferObjectIndex;
    XPMeshRefString            mesh;
    XPMaterialRefString        material;
    XPEMeshRendererPolygonMode polygonMode;
};

struct XPAttachStruct MeshRenderer
{
    explicit MeshRenderer(XPNode* owner)
      : owner(owner)
      , info{}
      , isStatic(true)
    {
    }

    void onChanged_info() {}
    void onChanged_isStatic() {}

    XPNode*       owner;
    XPAttachField std::vector<XPMeshRendererInfo> info;
    XPAttachField bool                            isStatic;
};

[[maybe_unused]] static void
onTraitAttached(MeshRenderer* mr)
{
}

[[maybe_unused]] static void
onTraitDettached(MeshRenderer* mr)
{
}

[[maybe_unused]] static void
onRenderUI(MeshRenderer* mr, XPIUI* ui)
{
}

#endif