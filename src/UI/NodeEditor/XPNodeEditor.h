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

#pragma once

#include <stdint.h>

#include <list>
#include <memory>
#include <string>
#include <vector>

struct XPNodeEditorBlock;
struct XPNodeEditorPinLink;

struct XPNodeEditorContext
{
    uint32_t                                          id;
    std::string                                       name;
    std::vector<std::shared_ptr<XPNodeEditorBlock>>   blocks;
    std::vector<std::shared_ptr<XPNodeEditorPinLink>> links;
};

struct XPNodeEditor
{
    std::weak_ptr<XPNodeEditorContext>              currentContext;
    std::list<std::shared_ptr<XPNodeEditorContext>> contexts;
};