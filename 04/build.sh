#!/bin/bash
source "$(dirname "${BASH_SOURCE[0]}")/../sourceme"
cd "$(dirname "${BASH_SOURCE[0]}")"
riscv32-unknown-elf-gcc -o 04.elf start.s -nostartfiles -Wl,-Tlinker.ld
riscv32-unknown-elf-objdump -d -s -j .text 04.elf
echo ""
riscv32-unknown-elf-size 04.elf
qemu-system-riscv32 -M virt -nographic -kernel 04.elf -bios none -semihosting
# Should directly exit.
