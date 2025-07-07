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

#include <Emulator/XPEmulatorCommon.h>
#include <Emulator/XPEmulatorConfig.h>
#include <Emulator/XPEmulatorInstruction.h>

#include <stdint.h>

XP_EMULATOR_EXTERN struct XPEmulatorEncodedInstruction
xp_emulator_decoder_decode_instruction(uint32_t encodedInstruction);

//XP_EMULATOR_EXTERN uint32_t
//imm_I(struct XPEmulatorEncodedInstruction instr);

XP_EMULATOR_EXTERN uint32_t
imm_S(struct XPEmulatorEncodedInstruction instr);

XP_EMULATOR_EXTERN uint32_t
imm_B(struct XPEmulatorEncodedInstruction instr);

XP_EMULATOR_EXTERN uint32_t
imm_U(struct XPEmulatorEncodedInstruction instr);

XP_EMULATOR_EXTERN int32_t
imm_J(struct XPEmulatorEncodedInstruction instr);

//XP_EMULATOR_EXTERN uint32_t
//shamt(struct XPEmulatorEncodedInstruction instr);