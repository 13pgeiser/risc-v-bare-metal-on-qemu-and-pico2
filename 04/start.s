        .text
        .global _start
_start:
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
