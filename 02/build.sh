#!/bin/bash
source "$(dirname "${BASH_SOURCE[0]}")/../sourceme"
cd "$(dirname "${BASH_SOURCE[0]}")"
riscv32-unknown-elf-gcc -o 02.elf start.s -nostartfiles -Wl,-Tlinker.ld
riscv32-unknown-elf-objdump -d -s -j .text 02.elf
riscv32-unknown-elf-size 02.elf