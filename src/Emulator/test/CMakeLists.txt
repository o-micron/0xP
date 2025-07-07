cmake_minimum_required(VERSION 3.9)
project(EmulatorPrograms VERSION 0.1.0 LANGUAGES C CXX ASM)

set(RV32_AS ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/riscv-gnu-toolchain/install32/bin/riscv32-unknown-elf-as)
set(RV32_GPP ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/riscv-gnu-toolchain/install32/bin/riscv32-unknown-elf-g++)
set(RV32_GCC ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/riscv-gnu-toolchain/install32/bin/riscv32-unknown-elf-gcc)
set(RV32_SIZE ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/riscv-gnu-toolchain/install32/bin/riscv32-unknown-elf-size)
set(RV32_OBJCOPY ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/riscv-gnu-toolchain/install32/bin/riscv32-unknown-elf-objcopy)
set(RV32_OBJDUMP ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/riscv-gnu-toolchain/install32/bin/riscv32-unknown-elf-objdump)

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# TEST C
# ---------------------------------------------------------------------------------------------------------------------------------------------------
add_executable(capp32 
    ${CMAKE_CURRENT_SOURCE_DIR}/common/crt0.S
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/syscalls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/uart.c
)

add_custom_command(TARGET capp32 POST_BUILD
    COMMAND bash -c "${RV32_SIZE} -A -x capp32"
    COMMAND bash -c "${RV32_OBJCOPY} capp32 -O binary capp32.bin"
    COMMAND bash -c "${RV32_OBJDUMP} -Mnumeric,no-aliases -dr capp32 >capp32.list"

    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>
)
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# TEST C++ (c++ std library headers are throwing errors for now)
# ---------------------------------------------------------------------------------------------------------------------------------------------------
add_executable(cppapp32 
    ${CMAKE_CURRENT_SOURCE_DIR}/common/crt0.S
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common/syscalls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/uart.c
)

add_custom_command(TARGET cppapp32 POST_BUILD
    COMMAND bash -c "${RV32_SIZE} -A -x cppapp32"
    COMMAND bash -c "${RV32_OBJCOPY} cppapp32 -O binary cppapp32.bin"
    COMMAND bash -c "${RV32_OBJDUMP} -Mnumeric,no-aliases -dr cppapp32 >cppapp32.list"

    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>
)
# ---------------------------------------------------------------------------------------------------------------------------------------------------


# ---------------------------------------------------------------------------------------------------------------------------------------------------
# TEST Rasterizer
# ---------------------------------------------------------------------------------------------------------------------------------------------------
add_executable(rasterizer32 
    ${CMAKE_CURRENT_SOURCE_DIR}/common/crt0.S
    ${CMAKE_CURRENT_SOURCE_DIR}/rasterizer/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rasterizer/Renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rasterizer/SceneDescription.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common/syscalls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/uart.c
)
target_include_directories(rasterizer32 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/rasterizer/artifacts/rv32/include/)
target_link_directories(rasterizer32 PRIVATE
    ${RISCV_TOOLCHAIN_PATH}/install32/lib/gcc/riscv32-unknown-elf/15.1.0/
    ${RISCV_TOOLCHAIN_PATH}/install32/risc32-unknown-elf/lib/
)
target_link_libraries(rasterizer32 PRIVATE m gcc)
add_custom_command(TARGET rasterizer32 POST_BUILD
    COMMAND bash -c "${RV32_SIZE} -A -x rasterizer32"
    COMMAND bash -c "${RV32_OBJCOPY} rasterizer32 -O binary rasterizer32.bin"
    COMMAND bash -c "${RV32_OBJDUMP} -Mnumeric,no-aliases -dr rasterizer32 >rasterizer32.list"

    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>
)
# ---------------------------------------------------------------------------------------------------------------------------------------------------


# ---------------------------------------------------------------------------------------------------------------------------------------------------
# TEST C ALL instructions
# ---------------------------------------------------------------------------------------------------------------------------------------------------
add_executable(all_instr 
    ${CMAKE_CURRENT_SOURCE_DIR}/common/crt0.S
    ${CMAKE_CURRENT_SOURCE_DIR}/common/syscalls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/uart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/all_instr/main.c
)
target_link_directories(all_instr PRIVATE
    ${RISCV_TOOLCHAIN_PATH}/install32/lib/gcc/riscv32-unknown-elf/15.1.0/
    ${RISCV_TOOLCHAIN_PATH}/install32/risc32-unknown-elf/lib/
)
target_link_libraries(all_instr PRIVATE m)
add_custom_command(TARGET all_instr POST_BUILD
    COMMAND bash -c "${RV32_SIZE} -A -x all_instr"
    COMMAND bash -c "${RV32_OBJCOPY} all_instr -O binary all_instr.bin"
    COMMAND bash -c "${RV32_OBJDUMP} -Mnumeric,no-aliases -dr all_instr >all_instr.list"

    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>
)
# ---------------------------------------------------------------------------------------------------------------------------------------------------


# ---------------------------------------------------------------------------------------------------------------------------------------------------
# TEST C (MAKEFILE)
# ---------------------------------------------------------------------------------------------------------------------------------------------------
# message(${CMAKE_BINARY_DIR})

# set(RV32_CFLAGS "-ffreestanding -fno-pic -DREENTRANT_SYSCALLS_PROVIDED -specs=nosys.specs -march=rv32im -mabi=ilp32")
# set(RV32_LIBS "")

# add_custom_command(TARGET cppapp32 POST_BUILD
#     COMMAND bash -c "${RV32_AS} -march=rv32im -mabi=ilp32 -o crt032.o ${CMAKE_CURRENT_SOURCE_DIR}/common/crt0.S"
#     COMMAND bash -c "${RV32_GCC} ${RV32_CFLAGS} ${RV32_LIBS} -O0 -g -gdwarf -g3 -c -o capp32.o ${CMAKE_CURRENT_SOURCE_DIR}/src/app.c"
#     COMMAND bash -c "${RV32_GCC} ${RV32_CFLAGS} ${RV32_LIBS} -O0 -g -gdwarf -g3 -c -o syscalls32.o ${CMAKE_CURRENT_SOURCE_DIR}/common/syscalls.c"
#     COMMAND bash -c "${RV32_GCC} ${RV32_CFLAGS} ${RV32_LIBS} -O0 -g -gdwarf -g3 -c -o uart32.o ${CMAKE_CURRENT_SOURCE_DIR}/common/uart.c"
#     COMMAND bash -c "${RV32_GCC} ${RV32_CFLAGS} ${RV32_LIBS} -O0 -g -gdwarf -g3 -nostartfiles -T ${CMAKE_CURRENT_SOURCE_DIR}/common/linker.ld -o capp32.elf crt032.o capp32.o syscalls32.o uart32.o"
#     # COMMAND bash -c "${RV32_LD} -b elf32-littleriscv -m elf32lriscv crt032.o app32.o -o app32.elf -L${RISCV_TOOLCHAIN_PATH}/install32/lib/gcc/riscv32-unknown-elf/14.2.0/rv32im/ilp32 -L${RISCV_TOOLCHAIN_PATH}/install32/risc32-unknown-elf/lib/rv32im/ilp32 -lc -lgcc -lm"
#     COMMAND bash -c "${RV32_SIZE} -A -x capp32.elf"
#     COMMAND bash -c "${RV32_OBJCOPY} capp32.elf -O binary capp32.bin"
#     COMMAND bash -c "${RV32_OBJDUMP} -Mnumeric,no-aliases -dr capp32.elf >capp32.list"

#     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>
# )
# ---------------------------------------------------------------------------------------------------------------------------------------------------
