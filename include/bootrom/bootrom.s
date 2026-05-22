.section .text
.globl _start
_start:
lui x1,0x80000
lui x2,0x80008
lui t0,0x10000
csrw mtvec,t0
jalr x0, 0(x1)


