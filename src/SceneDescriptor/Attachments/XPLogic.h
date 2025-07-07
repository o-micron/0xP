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

#ifndef XP_ATTACHMENT_LOGIC_H
#define XP_ATTACHMENT_LOGIC_H

#include <DataPipeline/XPLogic.h>
#include <Utilities/XPAnnotations.h>
#include <Utilities/XPMaths.h>
#include <Utilities/XPPlatforms.h>

#include <list>
#include <string>

class XPNode;
class XPIUI;

struct XPLogicSource
{
    XPLogic* logic;
};

struct XPAttachStruct Logic
{
    explicit Logic(XPNode* owner)
      : owner(owner)
      , sources{}
    {
    }

    void onChanged_sources() {}

    XPNode*       owner;
    XPAttachField std::list<XPLogicSource> sources;
};

[[maybe_unused]] static void
onTraitAttached(Logic* logic)
{
}

[[maybe_unused]] static void
onTraitDettached(Logic* logic)
{
}

[[maybe_unused]] static void
onRenderUI(Logic* logic, XPIUI* ui)
{
}

#endif