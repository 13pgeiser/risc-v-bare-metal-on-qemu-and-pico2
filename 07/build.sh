#!/bin/bash
source "$(dirname "${BASH_SOURCE[0]}")/../sourceme"
cd "$(dirname "${BASH_SOURCE[0]}")"
riscv32-unknown-elf-gcc -Os -o 07.elf start.s main.c -nostartfiles -Wl,-Tlinker.ld -nostdlib -lc
echo ""
riscv32-unknown-elf-size 07.elf
qemu-system-riscv32 -M virt -nographic -kernel 07.elf -bios none -semihosting
echo ""
riscv32-unknown-elf-gcc -Os -o 07.elf start.s main.c -nostartfiles -Wl,-Tlinker.ld -nostdlib -specs=nano.specs -lc_nano
echo ""
riscv32-unknown-elf-size 07.elf
qemu-system-riscv32 -M virt -nographic -kernel 07.elf -bios none -semihosting
