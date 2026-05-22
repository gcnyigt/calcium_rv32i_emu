#include "addr_space.h"
#include "types.h"
#include "terminal.h"
#ifndef RV32I_H
#define RV32I_H
int decode(rv32i* ,word);
word * csr_addr_decoder(rv32i*,word);
#endif
