.text
.global _start
.type _start, @function

_start:
    .option push
    .option norelax

setgp:
    auipc   gp, %pcrel_hi(__global_pointer$)
    addi    gp, gp, %pcrel_lo(setgp)
    .option pop

start_clear_bss:
    lui     a0, %hi(__bss_start)
    addi    a0, a0, %lo(__bss_start)
    lui     a1, %hi(__BSS_END__)
    addi    a1, a1, %lo(__BSS_END__)

clear_bss:
    bgeu    a0, a1, done_bss            
    sb      x0, 0(a0)                   
    addi    a0, a0, 1                   
    beq     x0, x0, clear_bss           
done_bss:
    la sp, _top_stack
    call main
    call __libc_fini_array
    mv a0, a0 
    li a7, 93
    ecall

shutdown:
    ebreak

    .size   _start, .-_start
