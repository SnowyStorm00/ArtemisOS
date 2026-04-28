OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS
{
    . = 0xffffffff80000000;

    .text : { *(.text*) }
    .rodata : { *(.rodata*) }
    
    .data : {
        *(.limine_reqs)
        *(.data*)
    }

    .bss : { *(.bss*) }
}