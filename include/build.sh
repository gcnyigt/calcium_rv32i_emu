#!/bin/bash
echo "Debugging C code"
riscv64-linux-gnu-gcc -march=rv32i_zicsr -mabi=ilp32 -ffreestanding -nostdlib -O1 \
-Wl,-Ttext=0x80000000 -Wl,--section-start=.rodata=0x80002000 -e main test.c printf.c -o test.elf

echo "Generating bin file"
riscv64-linux-gnu-objcopy -O binary -j .text -j .rodata test.elf mem.bin
truncate -s 32768 mem.bin
echo "generating mem.h"
xxd -i mem.bin > mem.h
echo "İşlem Tamam! mem.h 0x80000000 adresine hazır."
