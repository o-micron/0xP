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
#include "uart.h"

#include <errno.h> // errno
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

XPEXTERN char _start_heap;
XPEXTERN char _end_heap;

static char* heap_ptr = &_start_heap;

XPEXTERN uint32_t
riscv_syscall1(uint32_t num, uint32_t arg0)
{
    uint32_t ret;
    __asm__ volatile("mv a7, %[syscall_num] \n" // Move syscall number into a7
                     "mv a0, %[arg0]        \n" // Argument 0 -> a0
                     "ecall                 \n" // Execute syscall
                     "mv %[retval], a0      \n" // Retrieve return value
                     : [retval] "=r"(ret)
                     : [syscall_num] "r"(num), [arg0] "r"(arg0)
                     : "a0", "a7");
    return ret;
}

XPEXTERN uint32_t
riscv_syscall2(uint32_t num, uint32_t arg0, uint32_t arg1)
{
    uint32_t ret;
    asm volatile("mv a7, %[syscall_num] \n" // Move syscall number into a7
                 "mv a0, %[arg0]        \n" // Argument 0 -> a0
                 "mv a1, %[arg1]        \n" // Argument 1 -> a1
                 "ecall                 \n" // Execute syscall
                 "mv %[retval], a0      \n" // Retrieve return value
                 : [retval] "=r"(ret)
                 : [syscall_num] "r"(num), [arg0] "r"(arg0), [arg1] "r"(arg1)
                 : "a0", "a1", "a7");
    return ret;
}

XPEXTERN uint32_t
riscv_syscall3(uint32_t num, uint32_t arg0, uint32_t arg1, uint32_t arg2)
{
    long ret;
    asm volatile("mv a7, %[syscall_num] \n" // Move syscall number into a7
                 "mv a0, %[arg0]        \n" // Argument 0 -> a0
                 "mv a1, %[arg1]        \n" // Argument 1 -> a1
                 "mv a2, %[arg2]        \n" // Argument 2 -> a2
                 "ecall                 \n" // Execute syscall
                 "mv %[retval], a0      \n" // Retrieve return value
                 : [retval] "=r"(ret)
                 : [syscall_num] "r"(num), [arg0] "r"(arg0), [arg1] "r"(arg1), [arg2] "r"(arg2)
                 : "a0", "a1", "a2", "a7");
    return ret;
}

XPEXTERN uint32_t
riscv_syscall4(uint32_t num, uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
    long ret;
    asm volatile("mv a7, %[syscall_num] \n" // Move syscall number into a7
                 "mv a0, %[arg0]        \n" // Argument 0 -> a0
                 "mv a1, %[arg1]        \n" // Argument 1 -> a1
                 "mv a2, %[arg2]        \n" // Argument 2 -> a2
                 "mv a3, %[arg3]        \n" // Argument 3 -> a3
                 "ecall                 \n" // Execute syscall
                 "mv %[retval], a0      \n" // Retrieve return value
                 : [retval] "=r"(ret)
                 : [syscall_num] "r"(num), [arg0] "r"(arg0), [arg1] "r"(arg1), [arg2] "r"(arg2), [arg3] "r"(arg3)
                 : "a0", "a1", "a2", "a3", "a7");
    return ret;
}

XPEXTERN void
__dso_handle()
{
}

XPEXTERN int
_close(int __fd)
{
    DBG_PRNT("_close");

    int fd = riscv_syscall1(SYSCALL_CLOSE, __fd);

    return fd;
}
XPEXTERN int
_execve(const char* __f, char* const* __arg, char* const* __env)
{
    DBG_PRNT("_execve");
    errno = ENOSYS;
    return -1;
}
XPEXTERN int
_fcntl(int __fd, int __cmd, int __arg)
{
    DBG_PRNT("_fcntl");
    errno = ENOSYS;
    return -1;
}
XPEXTERN int
_fork()
{
    DBG_PRNT("_fork");
    errno = ENOSYS;
    return -1;
}
XPEXTERN int
_fstat(int __fdes, struct stat* __stat)
{
    DBG_PRNT("_fstat");
    if (__fdes == STDOUT_FILENO || __fdes == STDERR_FILENO) { // stdout or stderr
        __stat->st_mode = S_IFCHR;                            // Character device (terminal)
        return 0;
    }

    return riscv_syscall2(SYSCALL_FSTAT, __fdes, (uint32_t)__stat);
}
XPEXTERN int
_getpid()
{
    DBG_PRNT("_getpid");
    return 1;
}
XPEXTERN int
_isatty(int __desc)
{
    DBG_PRNT("_isatty");
    return 1;
}
XPEXTERN int
_kill(int __pid, int __signal)
{
    DBG_PRNT("_kill");
    errno = ENOSYS;
    return -1;
}
XPEXTERN int
_link(const char* __oldpath, const char* __newpath)
{
    DBG_PRNT("_link");
    errno = ENOSYS;
    return -1;
}
XPEXTERN _off_t
_lseek(int __fdes, _off_t __off, int __w)
{
    DBG_PRNT("_lseek");
    register off_t result asm("a0");                      // Syscall return value
    register int   syscall_num asm("a7") = SYSCALL_LSEEK; // Syscall number

    asm volatile("ecall"
                 : "=r"(result)                                        // Output: result in a0
                 : "r"(__fdes), "r"(__off), "r"(__w), "r"(syscall_num) // Inputs
                 : "memory");

    return result;
}
XPEXTERN int
_mkdir(const char* __path, int __m)
{
    DBG_PRNT("_mkdir");
    errno = ENOSYS;
    return -1;
}
XPEXTERN int
_open(const char* __path, int __flag, int __m)
{
    DBG_PRNT("_open");
    // assuming -100 -> AT_FDCWD (not really used in this emulator)
    int fd = riscv_syscall4(SYSCALL_OPEN, -100, (uint32_t)__path, (uint32_t)__flag, (uint32_t)__m);
    return fd;
}
XPEXTERN _ssize_t
_read(int __fd, void* __buff, size_t __cnt)
{
    DBG_PRNT("_read");

    if (__fd == STDIN_FILENO) {
        DBG_PRNT("_read: handling stdin\n");
        int i;
        for (i = 0; i < __cnt; ++i) {
            ((char*)__buff)[i] = uart_getc();
            if (((char*)__buff)[i] == '\r' || ((char*)__buff)[i] == '\n' || ((char*)__buff)[i] == '\0') {
                ++i;
                break;
            }
        }
        return i;
    }

    return riscv_syscall3(SYSCALL_READ, __fd, (uint32_t)__buff, (uint32_t)__cnt);
}
XPEXTERN int
_rename(const char* __old, const char* __new)
{
    DBG_PRNT("_rename");
    errno = ENOSYS;
    return -1;
}
XPEXTERN void*
_sbrk(ptrdiff_t __incr)
{
    DBG_PRNT("_sbrk");
    if (heap_ptr + __incr > &_end_heap) {
        errno = ENOMEM;
        return (void*)-1;
    }
    char* prev_end_heap = heap_ptr;
    heap_ptr += __incr;
    return prev_end_heap;
}
XPEXTERN int
_stat(const char* __path, struct stat* __buff)
{
    DBG_PRNT("_stat");
    errno = ENOSYS;
    return -1;
}
XPEXTERN _CLOCK_T_
_times(struct tms* __time)
{
    DBG_PRNT("_times");
    errno = ENOSYS;
    return -1;
}
XPEXTERN int
_unlink(const char* __path)
{
    DBG_PRNT("_unlink");
    errno = ENOSYS;
    return -1;
}
XPEXTERN int
_wait(int* __status)
{
    DBG_PRNT("_wait");
    errno = ENOSYS;
    return -1;
}
XPEXTERN _ssize_t
_write(int __fd, const void* __buff, size_t __cnt)
{
    DBG_PRNT("_write");
    if (__fd != STDOUT_FILENO && __fd != STDERR_FILENO) {
        // handle writing to file
        return riscv_syscall3(SYSCALL_WRITE, __fd, (uint32_t)__buff, (uint32_t)__cnt);
    }

    // handle stdout and stderr
    for (int i = 0; i < __cnt; ++i) { uart_putc(((char*)__buff)[i]); }

    return __cnt;
}
XPEXTERN int
_getentropy(void* __tp, size_t __tzp)
{
    DBG_PRNT("_getentropy");
    errno = ENOSYS;
    return -1;
}
