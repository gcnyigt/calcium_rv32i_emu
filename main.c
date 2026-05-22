#include<stdio.h>
#include"include/rv32i.h"
#include "program.h"
#include <unistd.h>
#include <string.h>
void log_cpu_state(FILE *log_file, rv32i *cpu, word current_instr) {
    if (log_file == NULL) return;

    fprintf(log_file, "PC: 0x%08X | OPCODE: 0x%08X\n", cpu->pc, current_instr);
    for(int i = 0; i < 32; i += 4) {
        fprintf(log_file, "x%02d: 0x%08X | x%02d: 0x%08X | x%02d: 0x%08X | x%02d: 0x%08X\n",
               i, cpu->x[i],     i+1, cpu->x[i+1], 
               i+2, cpu->x[i+2], i+3, cpu->x[i+3]);
    }
    fprintf(log_file, "-------------------------------------------------------------------\n");
}
int main(int argc, char *argv[])
{
    int debug_mode = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--debug") == 0) {
            debug_mode = 1; 
        }
    }
    rv32i cpu;
    cpu.pc = 0;
    for (int i = 0; i < 32; i++)
    {
        cpu.x[i] = 0;
    }

    cpu.sys_err_table = 0;
    const word program_addr = 0x80000000;
    for (int i = 0; i < program_bin_len; i++)
    {
        write_byte_to_address_space(program_addr + i,program_bin[i],&cpu);
    }
    FILE *debug_log = NULL;
    if (debug_mode) {
        debug_log = fopen("cpu_debug.log", "w");
        if (debug_log != NULL) {
            fprintf(debug_log, "--- CALCIUM EXECUTION TRACE ---\n\n");
        } else {
            printf("Unable to initialize log file\n");
        }
    }

    printf("==================================\n");
    switch (debug_mode)
    {
    case 0:
        while (cpu.sys_err_table == 0)
        {
            decode(&cpu,fetch_instr(&cpu));
        }
        
        break;
    
    case 1:
        while (cpu.sys_err_table == 0)
        {        
        word instr = fetch_instr(&cpu);
        decode(&cpu,instr);
        if (debug_mode && debug_log != NULL) {
            log_cpu_state(debug_log, &cpu, instr);
        }    
        }
        break;
    default:
        break;
    }
    return cpu.sys_err_table;
}
