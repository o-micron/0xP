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

#define XP_XPU_SYSCALL_OPEN         1024
#define XP_XPU_SYSCALL_CLOCKGETTIME 403
#define XP_XPU_SYSCALL_BRK          214
#define XP_XPU_SYSCALL_GETTIMEOFDAY 169
#define XP_XPU_SYSCALL_EXIT         93
#define XP_XPU_SYSCALL_FSTAT        80
#define XP_XPU_SYSCALL_WRITE        64
#define XP_XPU_SYSCALL_READ         63
#define XP_XPU_SYSCALL_LSEEK        62
#define XP_XPU_SYSCALL_CLOSE        57

#define XP_XPU_STDIN_FILENO  0
#define XP_XPU_STDOUT_FILENO 1
#define XP_XPU_STDERR_FILENO 2