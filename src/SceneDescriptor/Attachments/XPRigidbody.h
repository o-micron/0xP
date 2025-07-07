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

#ifndef XP_ATTACHMENT_RIGIDBODY_H
#define XP_ATTACHMENT_RIGIDBODY_H

#include <Utilities/XPAnnotations.h>
#include <Utilities/XPMaths.h>
#include <Utilities/XPPlatforms.h>

class XPNode;
class XPIUI;

struct XPAttachStruct Rigidbody
{
    explicit Rigidbody(XPNode* owner)
      : owner(owner)
      , simRef(nullptr)
      , linearVelocity(0.0f, 0.0f, 0.0f)
      , linearDamping(0.0f)
      , angularVelocity(0.0f, 0.0f, 0.0f)
      , angularDamping(0.0f)
      , mass(1.0f)
      , isLinearXLocked(false)
      , isLinearYLocked(false)
      , isLinearZLocked(false)
      , isAngularXLocked(false)
      , isAngularYLocked(false)
      , isAngularZLocked(false)
      , isStatic(false)
      , isTrigger(false)
    {
    }

    void onChanged_linearVelocity();
    void onChanged_linearDamping();
    void onChanged_angularVelocity();
    void onChanged_angularDamping();
    void onChanged_mass();
    void onChanged_isLinearXLocked();
    void onChanged_isLinearYLocked();
    void onChanged_isLinearZLocked();
    void onChanged_isAngularXLocked();
    void onChanged_isAngularYLocked();
    void onChanged_isAngularZLocked();
    void onChanged_isStatic();
    void onChanged_isTrigger();

    XPNode*       owner;
    void*         simRef;
    XPAttachField XPVec3<float> linearVelocity;
    XPAttachField float         linearDamping;
    XPAttachField XPVec3<float> angularVelocity;
    XPAttachField float         angularDamping;
    XPAttachField float         mass;
    XPAttachField bool          isLinearXLocked;
    XPAttachField bool          isLinearYLocked;
    XPAttachField bool          isLinearZLocked;
    XPAttachField bool          isAngularXLocked;
    XPAttachField bool          isAngularYLocked;
    XPAttachField bool          isAngularZLocked;
    XPAttachField bool          isStatic;
    XPAttachField bool          isTrigger;
};

void
onTraitAttached(Rigidbody* rigidbody);

void
onTraitDettached(Rigidbody* rigidbody);

void
onRenderUI(Rigidbody* rigidbody, XPIUI* ui);

#endif