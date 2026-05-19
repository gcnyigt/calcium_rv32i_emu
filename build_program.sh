#!/bin/bash
echo "Building program"
riscv64-linux-gnu-as -march=rv32i_zicsr -mabi=ilp32 program.s -o program.o
riscv64-linux-gnu-ld -m elf32lriscv -Ttext=0x8 program.o -o program.elf
echo "Generating hex code"
riscv64-linux-gnu-objcopy -O binary program.elf program.bin
xxd -i program.bin > program.h
