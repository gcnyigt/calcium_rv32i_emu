#ifndef TYPES_H
#define TYPES_H
typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned short hword;
typedef unsigned long qword;
typedef struct 
{
word x[32];
word pc;
word addr_o;
word data_o;
word data_i;
word mtvec;
word mepc;
word mcause;
word mtval;
word mstatus;
int sys_err_table;
} rv32i;
typedef enum {
FETCH_ERR_MASK      = 0b00000001,
//WRITE_MASK        = 0b00000010,
UNDEFINED_OPCODE    = 0b10000000,
SLAVE_ERROR_MASK    = 0b00100000,
} bus_masks;
extern byte terminalbuffer[];
typedef enum{
WORD  = 4,
HWORD = 2,
BYTE  = 0,
}address_decoder_enums;
#endif
