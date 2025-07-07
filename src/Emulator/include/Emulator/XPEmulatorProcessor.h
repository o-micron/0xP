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

#include <Emulator/XPEmulatorBus.h>
#include <Emulator/XPEmulatorConfig.h>
#include <Emulator/XPEmulatorEnums.h>
#include <Emulator/XPEmulatorElfLoader.h>

#include <stdint.h>

typedef struct XPEmulatorProcessor
{
    struct XPEmulatorBus bus;
    uint32_t             regs[XPEmulatorEReg_Count];
    uint32_t             pc;
} XPEmulatorProcessor;

XP_EMULATOR_EXTERN void
xp_emulator_processor_initialize(XPEmulatorProcessor* processor);

XP_EMULATOR_EXTERN int
xp_emulator_processor_load_program(XPEmulatorProcessor* processor, RiscvElfLoader* loader);

XP_EMULATOR_EXTERN int
xp_emulator_processor_step(XPEmulatorProcessor* processor);

XP_EMULATOR_EXTERN void
xp_emulator_processor_run(XPEmulatorProcessor* processor);

XP_EMULATOR_EXTERN void
xp_emulator_processor_finalize(XPEmulatorProcessor* processor);
