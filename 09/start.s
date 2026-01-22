        .text
        .global _start
_start:
        la sp, stack_top
        jal main
        li a0, 0x18 # SYS_EXIT
        li a1, 0
        jal sys_semihost

        .balign 16
        .option norvc
        .text
        .global sys_semihost
sys_semihost:
        slli zero, zero, 0x1f
        ebreak
        srai zero, zero, 0x7
        ret

.section .bootloader_metadata, "a"
bootloader_metadata:
.word 0xffffded3
.word 0x11010142
.word 0x00000344
.word _start	        # Initial PC address
.word stack_top 	# Initial SP address
.word 0x000004ff
.word 0x00000000
.word 0xab123579
