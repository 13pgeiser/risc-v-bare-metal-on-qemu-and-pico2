#!/bin/bash
source "$(dirname "${BASH_SOURCE[0]}")/../sourceme"
cd "$(dirname "${BASH_SOURCE[0]}")"
riscv32-unknown-elf-gcc -o 05.elf -Os start.s main.c -nostartfiles -Wl,-Tlinker.ld
riscv32-unknown-elf-objdump -d -s -j .text 05.elf
echo ""
riscv32-unknown-elf-size 05.elf
qemu-system-riscv32 -M virt -nographic -kernel 05.elf -bios none -semihosting
# Should directly exit.
