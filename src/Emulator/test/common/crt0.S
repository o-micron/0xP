# Freestanding program
# Ran through C pre-processor before it's assembled

# All data and instructions put in in the TEXT region of the address space
.text

# Make it global function, ie: void _start() { ... }
.global _start

# Message for debugger that this is a function ..
.type _start, @function


# Initialize global pointer ---------------------------------------------------------------------------------
_start:
    .option push
    # Don't do optimization or it will optimize the upcoming code and remove it away ..
    .option norelax

setgp:
    # Load gp register to initialize, symbol provided by the link editor to tell it where to point
    # Program counter relative to the symbol from the current executed instruction and put it in gp register
    auipc   gp, %pcrel_hi(__global_pointer$)
    addi    gp, gp, %pcrel_lo(setgp)

    .option pop
# -----------------------------------------------------------------------------------------------------------

start_clear_bss:
    # Clear the BSS segment starting from __bss_start to __BSS_END__-1 ---------------------------------------
    #  a0 = __bss_start
    lui     a0, %hi(__bss_start)
    addi    a0, a0, %lo(__bss_start)

    # a1 = __bss_end
    lui     a1, %hi(__BSS_END__)
    addi    a1, a1, %lo(__BSS_END__)

clear_bss:
    bgeu    a0, a1, done_bss            # while (!(a0 >= a1)) {
    sb      x0, 0(a0)                   #       *a0 = 0;
    addi    a0, a0, 1                   #        ++a0;
    beq     x0, x0, clear_bss           # }
done_bss:
    # Set up stack (adjust as needed)
    la sp, _top_stack

    # Call main
    call main

    call __libc_fini_array

    # Capture the return value of main() (stored in a0)
    mv a0, a0  # Return value already in a0

    # Exit after main returns
    li a7, 93       # Exit syscall number (93)
    ecall           # Perform syscall

shutdown:
    # Abort execution
    ebreak

    .size   _start, .-_start
# -----------------------------------------------------------------------------------------------------------
