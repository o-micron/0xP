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

#include <Emulator/XPEmulatorConfig.h>

#include <stdint.h>

XP_EMULATOR_EXTERN void
xp_emulator_common_print_binary_u32(uint32_t val);

XP_EMULATOR_EXTERN void
xp_emulator_print_op(char* s);

enum XP_EMULATOR_PROGRAM_EXIT_CODE
{
    XP_EMULATOR_PROGRAM_EXIT_CODE_SUCCESS                  = 0,
    XP_EMULATOR_PROGRAM_EXIT_CODE_UNDEFINED_INSTRUCTION    = -1,
    XP_EMULATOR_PROGRAM_EXIT_CODE_UNKNOWN                  = -2,
    XP_EMULATOR_PROGRAM_EXIT_CODE_EBREAK                   = -3,
    XP_EMULATOR_PROGRAM_EXIT_CODE_UNHANDLED_SYSCALL        = -4,
    XP_EMULATOR_PROGRAM_EXIT_CODE_FENCE_UNIMPLEMENTED      = -5,
    XP_EMULATOR_PROGRAM_EXIT_CODE_FLOATING_POINT_EXCEPTION = -6,
};

#define XP_EMULATOR_DBG_ASSERT(A, MSG)                                                                                 \
    if (!(A)) { printf(MSG "\n"); }
