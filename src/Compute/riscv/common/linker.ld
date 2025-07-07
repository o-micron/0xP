OUTPUT_FORMAT("elf32-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY
{
    FLASH (rx) : ORIGIN = 0x4000000, LENGTH = 1M
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 4M
    HEAP (rw) : ORIGIN = 0x80400040, LENGTH = 2M
}

SECTIONS
{
    .text : {
        _start_text = .;
        *(.text*)                   
        *(.rodata*)                 
        . = ALIGN(4);
        _end_text = .;
    } > FLASH

    .init_array :
    {
        __init_array_start = .;
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        __init_array_end = .;
    } > FLASH

    .fini_array :
    {
        __fini_array_start = .;
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        __fini_array_end = .;
    } > FLASH

    . = ALIGN(16);
    PROVIDE(__global_pointer$ = .);

    .data : {
        _start_data = .;
        *(.data*)     
        *(.sdata*)    
        . = ALIGN(4);
        _end_data = .;
    } > RAM

    .bss : {
        __bss_start = .;
        *(.bss*)     
        *(.sbss*)    
        . = ALIGN(4);
        __BSS_END__ = .;
    } > RAM

    .stack : {
        . = ALIGN(16);
        _bottom_stack = .;
        . += 2M;
        _top_stack = .;
    } > RAM

    .heap : {
        . = ALIGN(8);
        _start_heap = .;
        . = ORIGIN(HEAP) + LENGTH(HEAP);
        _end_heap = .;
    } > HEAP

    /DISCARD/ :
    {
        *(.comment)
        *(.debug*)
        *(.eh_frame*)
    }

    _end = .;
}

__flash_start = ORIGIN(FLASH);
__flash_size = LENGTH(FLASH);
__ram_start = ORIGIN(RAM);
__ram_size = LENGTH(RAM);
__heap_start = ORIGIN(HEAP);
__heap_size = LENGTH(HEAP);

ASSERT(_end_data <= _bottom_stack, "Data section overlaps with stack")
ASSERT(__fini_array_end <= ORIGIN(FLASH) + LENGTH(FLASH), "Flash overflows")
ASSERT(_top_stack <= ORIGIN(RAM) + LENGTH(RAM), "Stack overflows RAM")
ASSERT(_end_heap <= ORIGIN(HEAP) + LENGTH(HEAP), "Heap overflows heap region")