OUTPUT_FORMAT("elf32-littleriscv", "elf32-littleriscv", "elf32-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(_start)

STACK_SIZE = 0x800;

MEMORY
{
    ram   (wxa!ri) : ORIGIN = 0x20000000, LENGTH = 512k
}

PHDRS
{
    text PT_LOAD;
    data PT_LOAD;
    bss PT_LOAD;
}

SECTIONS
{
    .text : {
        KEEP (*(.bootloader_metadata))
        *(.text.init) *(.text .text.*)
    } >ram AT>ram :text
    .rodata : {
        *(.rodata .rodata.*)
    } >ram AT>ram :text
    .data : {
        *(.sdata .sdata.*) *(.data .data.*)
    } >ram AT>ram :data
    .bss : {
        *(.sbss .sbss.*) *(.bss .bss.*)
    } >ram AT>ram :bss
    stack_bottom =.;
    . += STACK_SIZE;
    stack_top = .;
    . = ALIGN(4096);
    _memory_start = .;
    PROVIDE(_memory_end = ORIGIN(ram) + LENGTH(ram));
}