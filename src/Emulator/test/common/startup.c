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

#include "syscalls.h"

#ifdef __cplusplus
    #define XPEXTERN extern "C"
#else
    #define XPEXTERN extern
#endif

XPEXTERN int
main(int, char**);

XPEXTERN __attribute__((visibility("hidden"), used)) void
libc_start(int argc, char** argv)
{
    // zero-initialize .bss section
    // XPEXTERN char __bss_start;
    // XPEXTERN char __BSS_END__;
    // for (char* bss = &__bss_start; bss < &__BSS_END__; bss++) { *bss = 0; }

    // call global constructors
    {
        XPEXTERN void (*__init_array_start[])();
        XPEXTERN void (*__init_array_end[])();
        int count = __init_array_end - __init_array_start;
        for (int i = 0; i < count; i++) { __init_array_start[i](); }
    }

    // call main
    int ret = main(argc, argv);

    // call global destructors
    {
        XPEXTERN void (*__fini_array_start[])();
        XPEXTERN void (*__fini_array_end[])();
        int count = __fini_array_end - __fini_array_start;
        for (int i = 0; i < count; i++) { __fini_array_start[i](); }
    }

    // return code is then reflected in the exit syscall
    riscv_syscall1(SYSCALL_EXIT, ret);
}

// clang-format off
asm(
  "# All data and instructions put in in the TEXT region of the address space                                   \n"
  ".text                                                                                                        \n"
  "# Make it global function, ie: void _start() { ... }                                                         \n"
  ".global _start                                                                                               \n"
  "# Message for debugger that this is a function                                                               \n"
  ".type _start, @function                                                                                      \n"
  "                                                                                                             \n"
  "_start:                                                                                                      \n"
  "     .option   push;				                                                                            \n"
  "     # Don't do optimization or it'll optimize this and remove it away                                       \n"
  "     .option   norelax; 			                                                                            \n"
  "                                                                                                             \n"
  "# Load gp register to initialize, symbol provided by the link editor to tell it where to point               \n"
  "# Program counter relative to the symbol from the current executed instruction and put it in gp register     \n"
  "setgp:                                                                                                       \n"
  "     auipc     gp, %pcrel_hi(__global_pointer$);                                                             \n"
  "     addi      gp, gp, %pcrel_lo(setgp)                                                                      \n"
  "                                                                                                             \n"
  "     .option   pop					                                                                        \n"
  "                                                                                                             \n"
  "start_clear_bss:                                                                                             \n"
  "     # Clear the BSS segment starting from __bss_start to __BSS_END__-1                                      \n"
  "     #  a0 = __bss_start                                                                                     \n"
  "     lui     a0, %hi(__bss_start)                                                                            \n"
  "     addi    a0, a0, %lo(__bss_start)                                                                        \n"
  "     # a1 = __bss_end                                                                                        \n"
  "     lui     a1, %hi(__BSS_END__)                                                                            \n"
  "     addi    a1, a1, %lo(__BSS_END__)                                                                        \n"
  "                                                                                                             \n"
  "clear_bss:                                                                                                   \n"
  "     bgeu    a0, a1, done_bss            # while (!(a0 >= a1)) {                                             \n"
  "     sb      x0, 0(a0)                   #       *a0 = 0;                                                    \n"
  "     addi    a0, a0, 1                   #        ++a0;                                                      \n"
  "     beq     x0, x0, clear_bss           # }                                                                 \n"
  "                                                                                                             \n"
  "done_bss:                                                                                                    \n"
  "     # Set up stack (adjust as needed)                                                                       \n"
  "     la      sp, _stack_top                                                                                  \n"
  "                                                                                                             \n"
  "     call    libc_start				                                                                        \n"
  "");
// clang-format on
