cmake_minimum_required(VERSION 3.9)
project(XPUPrograms VERSION 0.1.0 LANGUAGES C CXX ASM)

set(RV32_AS ${CMAKE_CURRENT_SOURCE_DIR}/../../Emulator/thirdparty/riscv-gnu-toolchain/install32/bin/riscv32-unknown-elf-as)
set(RV32_GPP ${CMAKE_CURRENT_SOURCE_DIR}/../../Emulator/thirdparty/riscv-gnu-toolchain/install32/bin/riscv32-unknown-elf-g++)
set(RV32_GCC ${CMAKE_CURRENT_SOURCE_DIR}/../../Emulator/thirdparty/riscv-gnu-toolchain/install32/bin/riscv32-unknown-elf-gcc)
set(RV32_SIZE ${CMAKE_CURRENT_SOURCE_DIR}/../../Emulator/thirdparty/riscv-gnu-toolchain/install32/bin/riscv32-unknown-elf-size)
set(RV32_OBJCOPY ${CMAKE_CURRENT_SOURCE_DIR}/../../Emulator/thirdparty/riscv-gnu-toolchain/install32/bin/riscv32-unknown-elf-objcopy)
set(RV32_OBJDUMP ${CMAKE_CURRENT_SOURCE_DIR}/../../Emulator/thirdparty/riscv-gnu-toolchain/install32/bin/riscv32-unknown-elf-objdump)

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# TEST XPU
# ---------------------------------------------------------------------------------------------------------------------------------------------------
add_executable(xpu32 
    ${CMAKE_CURRENT_SOURCE_DIR}/common/crt0.S
    ${CMAKE_CURRENT_SOURCE_DIR}/common/syscalls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/uart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SceneDescription.cpp
)
target_include_directories(xpu32 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/artifacts/rv32/include/)
target_link_directories(xpu32 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Emulator/thirdparty/riscv-gnu-toolchain/install32/lib/gcc/riscv32-unknown-elf/15.1.0/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Emulator/thirdparty/riscv-gnu-toolchain/install32/risc32-unknown-elf/lib/
)
target_link_libraries(xpu32 PRIVATE m gcc)

add_custom_command(TARGET xpu32 POST_BUILD
    COMMAND bash -c "${RV32_SIZE} -A -x xpu32"
    COMMAND bash -c "${RV32_OBJCOPY} xpu32 -O binary xpu32.bin"
    COMMAND bash -c "${RV32_OBJDUMP} -Mnumeric,no-aliases -dr xpu32 >xpu32.list"

    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>
)
# ---------------------------------------------------------------------------------------------------------------------------------------------------
