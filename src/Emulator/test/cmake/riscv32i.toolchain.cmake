set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv32)

# Set the toolchain prefix
set(RISCV_GCC_TOOLCHAIN_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/riscv-gnu-toolchain/install32/bin/riscv32-unknown-elf-")

# Specify compilers
set(CMAKE_C_COMPILER "${RISCV_GCC_TOOLCHAIN_PREFIX}gcc")
set(CMAKE_CXX_COMPILER "${RISCV_GCC_TOOLCHAIN_PREFIX}g++")
set(CMAKE_ASM_COMPILER "${RISCV_GCC_TOOLCHAIN_PREFIX}gcc")

# Set other tools
set(CMAKE_AR "${RISCV_GCC_TOOLCHAIN_PREFIX}ar")
set(CMAKE_RANLIB "${RISCV_GCC_TOOLCHAIN_PREFIX}ranlib")
set(CMAKE_LINKER "${RISCV_GCC_TOOLCHAIN_PREFIX}ld")

# Set compiler flags
set(CMAKE_C_FLAGS "-fno-exceptions -DREENTRANT_SYSCALLS_PROVIDED -specs=nosys.specs -march=rv32im -mabi=ilp32")
set(CMAKE_CXX_FLAGS "-fno-exceptions -fno-rtti -DREENTRANT_SYSCALLS_PROVIDED -specs=nosys.specs -march=rv32im -mabi=ilp32 -std=c++17")

# Use custom linker script
set(CMAKE_EXE_LINKER_FLAGS "-nostartfiles -T ${CMAKE_CURRENT_SOURCE_DIR}/common/linker.ld")

# for crt0.S
# enable_language(ASM)
