#include"rv32i.h"
#include"bootrom/bootrom_hex.h"
byte mem[0x8000];
byte terminalbuffer[512];
word read_word_from_address_space(word address,rv32i *cpu){
    word aligned_addr;
    byte region = (address >> 28)&0xf;
    word internal_addr;
    switch (region)
    {
    word *ptr; 
        break;
    case 0x8:
        internal_addr = address & 0xFFFF;
        cpu->sys_err_table |= ((0x8000 - 0x4) < internal_addr)<<5;
        internal_addr = internal_addr *!(0x8000 - 0x4 < internal_addr);
        aligned_addr = internal_addr & ~0x3;
        ptr = (word *)&mem[aligned_addr];
        return *ptr;
        break;
    default:
        cpu->sys_err_table|=SLAVE_ERROR_MASK;
        return 0xFFFFFFFF;
        break;
    }
}
void write_word_to_address_space(word address,word data,rv32i *cpu){
    word aligned_addr;
    byte region = (address >> 28)&0xf;
    word internal_addr;
    word *ptr ;
    switch (region)
    {
    case 0x8:
        internal_addr = address & 0xFFFF;
        cpu->sys_err_table |= ((0x8000 - 0x4) < internal_addr)<<5;
        internal_addr = internal_addr *!(0x8000 - 0x4 < internal_addr);
        aligned_addr = internal_addr & ~0x3;
        ptr = (word *)&mem[aligned_addr];
        *ptr = data;
        break;
    default:
        cpu->sys_err_table|=SLAVE_ERROR_MASK;
        break;
    }
}
hword read_hword_from_address_space(word address,rv32i *cpu){
    byte region = (address >> 28)&0xf;
    word aligned_addr;
    word internal_addr;
    hword *ptr;
    switch (region)
    {
    case 0x8:
        internal_addr = address & 0xFFFF;
        cpu->sys_err_table |= ((0x8000 - 0x2) < internal_addr)<<5;
        internal_addr = internal_addr *!(0x8000 - 0x2 < internal_addr);
        aligned_addr = internal_addr & ~0x1;
        ptr = (hword *)&mem[aligned_addr];
        return *ptr;
        break;
    default:
        cpu->sys_err_table|=SLAVE_ERROR_MASK;
        return 0xFFFF;
        break;
    }
}
void write_hword_to_address_space(word address, hword data,rv32i *cpu){
    byte region = (address >> 28)&0xf;
    word internal_addr;
    word aligned_addr;
    hword *ptr;
    switch (region)
    {
    case 0x8:
        internal_addr = address & 0xFFFF;
        cpu->sys_err_table |= ((0x8000 - 0x2) < internal_addr)<<5;
        internal_addr = internal_addr *!(0x8000-0x2 < internal_addr);
        aligned_addr = internal_addr & ~0x1;
        ptr = (hword *)&mem[aligned_addr];
        *ptr = data;
        break;   
    default:
        cpu->sys_err_table|=SLAVE_ERROR_MASK;
        break;
    }
}
byte read_byte_from_address_space(word address,rv32i*cpu){
    byte region = (address >> 28)&0xf;
    word internal_addr;
    switch (region)
    {
    case 0x8:
        internal_addr = address & 0xFFFF;
        cpu->sys_err_table |= ((0x8000) < internal_addr)<<5;
        internal_addr = internal_addr *!(0x8000 < internal_addr);
        return mem[internal_addr] ;
        break;
    case 0x9:
        internal_addr = address & 0x7FFF;
        cpu->sys_err_table |= (512 < internal_addr)<<5;
        internal_addr = internal_addr *!(512 < internal_addr);
        return mem[internal_addr];
        break;
    default:
        cpu->sys_err_table|=SLAVE_ERROR_MASK;
        return 0xFF;
        break;
    }
}
void write_byte_to_address_space(word address,byte data,rv32i* cpu){
    byte region = (address >> 28)&0xf;
    word internal_addr;
    switch (region)
    {
    case 0x8:
        internal_addr = address & 0xFFFF;
        cpu->sys_err_table |= ((0x8000) < internal_addr)<<5;
        internal_addr = internal_addr *!(0x8000 < internal_addr);
        mem[internal_addr] = data;
        break;
    case 0x9:
        internal_addr = address & 0xFFFF;
        cpu->sys_err_table |= (512 < internal_addr)<<5;
        internal_addr = internal_addr *!(512 < internal_addr);
        terminal(internal_addr,data);
        break;
    
    default:
        cpu->sys_err_table|=SLAVE_ERROR_MASK;
        break;
    }
}
word fetch_instr(rv32i*cpu){
 word aligned_addr;
    byte region = (cpu->pc >> 28)&0xf;
    word internal_addr;
    switch (region)
    {
    word *ptr; 
    case 0x0:
        internal_addr = cpu->pc & 0xFFFF;
        cpu->sys_err_table |= (( bootrom_bin_len - 0x4) < internal_addr);//FETCH_ERR_MASK
        internal_addr = internal_addr *!(bootrom_bin_len - 0x4 < internal_addr);
        aligned_addr = internal_addr & ~0x3;
        ptr = (word *)&bootrom_bin[aligned_addr];
        return *ptr;
        break;
    case 0x8:
        internal_addr = cpu->pc & 0xFFFF;
        cpu->sys_err_table |= ((0x8000 - 0x4) < internal_addr);//FETCH_ERR_MASK
        internal_addr = internal_addr *!(0x8000 - 0x4 < internal_addr);
        aligned_addr = internal_addr & ~0x3;
        ptr = (word *)&mem[aligned_addr];
        return *ptr;
        break;
    case 0x9:
        internal_addr = cpu->pc & 0x7FFF;
        cpu->sys_err_table |= (252 < internal_addr);//FETCH_ERR_MASK
        internal_addr = internal_addr *!(252 < internal_addr);
        aligned_addr = internal_addr & ~0x3;
        ptr = (word *)&terminalbuffer[aligned_addr];
        return *ptr;
        break;
    default:
        cpu->sys_err_table|=FETCH_ERR_MASK;
        return 0xFFFFFFFF;
        break;
    }
}