#!/bin/bash
source "$(dirname "${BASH_SOURCE[0]}")/../sourceme"
cd "$(dirname "${BASH_SOURCE[0]}")"
riscv32-unknown-elf-gcc -Os -march=rv32imacd_zicsr_zifencei_zba_zbb_zbkb_zbs -o 09.elf start.s main.c -nostartfiles -Wl,-Tlinker.ld -nostdlib -specs=nano.specs -lc_nano
echo ""
riscv32-unknown-elf-size 09.elf
# convert to uf2, thanks to https://github.com/dougsummerville/Bare-Metal-Raspberry-Pi-Pico-2
# picotool would work too.
if [ ! -e elf2uf2.py ]; then
    wget https://raw.githubusercontent.com/dougsummerville/Bare-Metal-Raspberry-Pi-Pico-2/refs/heads/dot/tools/elf2uf2.py
    sed -i 's/elf_file_hdr.e_machine != 0x28/elf_file_hdr.e_machine != 0xf3/g' elf2uf2.py
fi
python3 elf2uf2.py 09.elf -o 09.uf2 -vv
