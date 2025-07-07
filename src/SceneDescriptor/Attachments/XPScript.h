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

#ifndef XP_ATTACHMENT_SCRIPT_H
#define XP_ATTACHMENT_SCRIPT_H

#include <Utilities/XPAnnotations.h>
#include <Utilities/XPMaths.h>

#include <atomic>
#include <string>
#include <thread>

class XPNode;
class XPIUI;
struct XPEmulatorProcessor;

struct XPAttachStruct Script
{
    explicit Script(XPNode* owner)
      : owner(owner)
    {
    }

    void onChanged_source();
    void onChanged_program();

    XPNode*              owner;
    XPEmulatorProcessor* processor;
    XPAttachField std::string program;
    void*                     elfLoader;
    // XPAttachField std::string debug_info;
    std::atomic<bool> isLoaded;
    std::atomic<bool> isRunning;
    std::thread       executionThread;
};

void
onTraitAttached(Script* script);

void
onTraitDettached(Script* script);

void
onRenderUI(Script* script, XPIUI* ui);

#endif