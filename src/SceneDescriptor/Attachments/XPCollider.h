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

#ifndef XP_ATTACHMENT_COLLIDER_H
#define XP_ATTACHMENT_COLLIDER_H

#include <SceneDescriptor/XPEnums.h>
#include <SceneDescriptor/XPTypes.h>
#include <Utilities/XPAnnotations.h>
#include <Utilities/XPMaths.h>

#include <string_view>

class XPNode;
class XPIUI;

struct ColliderParameters
{
    union
    {
        struct
        {
            float planeWidth;
            float __plane_pad1;
            float planeDepth;
        };
        struct
        {
            float boxWidth;
            float boxHeight;
            float boxDepth;
        };
        struct
        {
            float sphereRadius;
            float __sphere_pad1;
            float __sphere_pad2;
        };
        struct
        {
            float capsuleRadius;
            float capsuleHeight;
            float capsuleDensity;
        };
        struct
        {
            float cylinderRadius;
            float cylinderHeight;
            float __cylinder_pad2;
        };
    };
};

struct Collider;

struct XPAttachStructSecondary XPColliderInfo
{
    Collider*                         owner;
    const XPMeshBuffer*               meshBuffer;
    uint32_t                          meshBufferObjectIndex;
    void*                             shapeRef;
    XPAttachField XPColliderRefString shapeName;
    XPAttachField ColliderParameters  parameters;
    XPAttachField XPEColliderShape    shape;
};

struct XPAttachStruct Collider
{
    explicit Collider(XPNode* owner)
      : owner(owner)
    {
    }

    void onChanged_info();

    XPNode*       owner;
    XPAttachField std::vector<XPColliderInfo> info;
};

void
onTraitAttached(Collider* collider);

void
onTraitDettached(Collider* collider);

void
onRenderUI(Collider* collider, XPIUI* ui);

#endif