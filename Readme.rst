#######################################
Bare Metal RISC-V, QEMU, GCC and Pico 2
#######################################

:date: 2025-01-18 19:00
:modified: 2025-01-18 19:00
:tags: risc-v, qemu, gcc, pico2
:authors: Pascal Geiser
:summary: Minimal examples with GCC for RISC-V

.. contents::

************
Installation
************

Debian Linux
============

Install the following packages: `qemu-system-misc device-tree-compiler p7zip git`.


Assuming you've configured *sudo*:

.. code-block:: bash

    sudo apt install qemu-system-misc device-tree-compiler p7zip git

GCC
===

In order to simplify the installation slightly, the next steps have been automated in the file *sourceme*

It will download a usable gcc compiler for RISC-V architecture in the following repo:
 * https://github.com/13pgeiser/gcc-riscv32

****************************************
01: getting information about the target
****************************************

QEMU provides a generic virtual platform `virt <https://www.qemu.org/docs/master/system/riscv/virt.html>`__
In order to know more about the available peripherals and memories, the tool can be queried:

.. code-block:: bash

    qemu-system-riscv32 -machine virt -machine dumpdtb=riscv32-virt.dtb

This will provide the binary representation of the machine's data structure describing the HW components.

To have it in a more human readable form, *dtc* can convert it in dts format:

.. code-block:: bash

    dtc -I dtb -O dts -o riscv32-virt.dts riscv32-virt.dtb

Currently, the interesting section is:

.. code-block::

	memory@80000000 {
		device_type = "memory";
		reg = <0x00 0x80000000 0x00 0x8000000>;
	};

Finally, the linker can provide us a default (but too complex) version of the linker script:

.. code-block:: bash

    riscv32-unknown-elf-ld --verbose > qemu-riscv32-vrit.txt

***********************************************
02: compiling the smallest risc-v code
***********************************************

The smallest executable code is an infinite loop jumping to the first location in memory

.. code-block:: asm

            .text
            .global _start
    _start:
            j _start

To compile it, we need a small linker script that will explain to the linker where to put the compiled code.

Note that the ram section matches the memory discovered in the first step.

.. code-block::

    OUTPUT_FORMAT("elf32-littleriscv", "elf32-littleriscv", "elf32-littleriscv")
    OUTPUT_ARCH(riscv)
    ENTRY(_start)

    MEMORY
    {
        ram   (wxa!ri) : ORIGIN = 0x80000000, LENGTH = 128M
    }

    PHDRS
    {
        text PT_LOAD;
    }

    SECTIONS
    {
        .text : {
            *(.text.init) *(.text .text.*)
        } >ram AT>ram :text
    }

To create an application:

.. code-block:: bash

    riscv32-unknown-elf-gcc -o 02.elf start.s -nostartfiles -Wl,-Tlinker.ld

And to verify the result:

.. code-block:: bash

    riscv32-unknown-elf-objdump -d -s -j .text 02.elf
    riscv32-unknown-elf-size 02.elf

Which prints the following output:

.. code-block:: bash

    $ ./02/build.sh

    02.elf:     file format elf32-littleriscv

    Contents of section .text:
    80000000 01a0                                 ..              

    Disassembly of section .text:

    80000000 <_start>:
    80000000:       a001                    j       80000000 <_start>
    text    data     bss     dec     hex filename
        2       0       0       2       2 02.elf

Nice! 2 bytes only! ;-) But totally useless.


************************************
03: Running our (useless) executable
************************************

.. code-block:: bash

    source ../sourceme
    qemu-system-riscv32 -M virt -s -S -nographic -kernel 02.elf -bios none

This tell qemu to: (see https://www.qemu.org/docs/master/system/invocation.html for more information):
 * '-s': Shorthand for -gdb tcp::1234
 * '-S': Do not start CPU at startup
 * '-nographic': disable windowing system
 * '-kernel' 02.elf : loads our binary
 * '-bios none': get rid of the default bios

And in a second terminal:

.. code-block:: bash

    source ../sourceme
    riscv32-unknown-elf-gdb virt --eval-command="target remote :1234" --eval-command="x/8xw 0x80000000"

Which will connect with gdb to the stopped binary and dump the memory at 0x80000000 (RAM)

.. code-block::

    ...
    0x00001000 in ?? ()
    0x80000000:     0x0000a001      0x00000000      0x00000000      0x00000000
    0x80000010:     0x00000000      0x00000000      0x00000000      0x00000000

Then in the same gdb run:
 * 'c': to continue execution
 * 'ctrl-c': to break
 * 'info register pc' (or 'i r pc'): to show the current program counter

.. code-block::

    (gdb) c
    Continuing.

    Program received signal SIGINT, Interrupt.
    0x80000000 in ?? ()
    (gdb) info register pc
    pc             0x80000000       0x80000000
    (gdb)
