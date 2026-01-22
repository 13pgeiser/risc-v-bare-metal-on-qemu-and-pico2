OUTPUT_FORMAT("elf32-littleriscv", "elf32-littleriscv", "elf32-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(_entry_point)

STACK_SIZE = 0x1000;

MEMORY
{
    flash  (rx) : ORIGIN = 0x10000000, LENGTH = 4M
    ram   (rwx) : ORIGIN = 0x20000000, LENGTH = 512k
}


SECTIONS
{
    .text : {
        KEEP (*(.reset))
        KEEP (*(.bootloader_metadata))
        . = ALIGN(4);
        *(.text.init) *(.text .text.*)
    } >flash

    .rodata : {
        . = ALIGN(4);
        *(.rodata .rodata.*)
    } >flash

    .data : {
        . = ALIGN(4);
        __data_start = .;
        *(.sdata .sdata.*) *(.data .data.*)
        . = ALIGN(4);
        __data_end = .;
    } >ram AT>flash

    __data_load_ptr = LOADADDR(.data);

    .bss (NOLOAD) : {
        . = ALIGN(4);
        __bss_start = .;
        *(.sbss .sbss.*) *(.bss .bss.*)
        . = ALIGN(4);
        __bss_end = .;
    } >ram

    stack_bottom =.;
    . += STACK_SIZE;
    stack_top = .;
    . = ALIGN(4096);
    _memory_start = .;
    PROVIDE(_memory_end = ORIGIN(ram) + LENGTH(ram));
}