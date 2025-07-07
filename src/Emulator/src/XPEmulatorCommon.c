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

#include <Emulator/XPEmulatorCommon.h>

#include <stdio.h>

#define ANSI_YELLOW "\x1b[33m"
#define ANSI_BLUE   "\x1b[31m"
#define ANSI_RESET  "\x1b[0m"

XP_EMULATOR_EXTERN void
xp_emulator_common_print_binary_u32(uint32_t val)
{
    for (int i = 0; i < 32; ++i) { ((val & (1 << (31 - i))) > 0) ? printf("1") : printf("0"); }
    printf("\n");
}

XP_EMULATOR_EXTERN void
xp_emulator_print_op(char* s)
{
#if defined(XP_EMULATOR_CONFIG_ENABLE_PRINT_OP)
    printf("%s%s%s ", ANSI_BLUE, s, ANSI_RESET);
#endif
}