#include "include/rv32i.h"
int decode(rv32i *cpu,word instr){
    word imm = 0;
    word opcode = instr & 0x7F;
    word rd1;
    byte funct3; 
    word rs1;
    word rs2;
    byte func7;
    word address;
    switch (opcode)
    {              
        case 0b1100011: // B-Type
            rs1 = (instr >> 15) & 0x1F;
            rs2 = (instr >> 20) & 0x1F;
            funct3 = (instr >> 12) & 0x7;
            signed int imm_b = (((signed int)instr >> 31) << 12) | 
                               (((instr >> 7) & 0x1) << 11) | 
                               (((instr >> 25) & 0x3F) << 5) | 
                               (((instr >> 8) & 0xF) << 1);

            switch (funct3)
            {
                case 0b000: // BEQ
                    cpu->pc += (imm_b - 4) * (cpu->x[rs1] == cpu->x[rs2]);
                    break;

                case 0b001: // BNE
                    cpu->pc += (imm_b - 4) * (cpu->x[rs1] != cpu->x[rs2]);
                    break;

                case 0b100: // BLT
                    cpu->pc += (imm_b - 4) * ((signed int)cpu->x[rs1] < (signed int)cpu->x[rs2]);
                    break;

                case 0b101: // BGE
                    cpu->pc += (imm_b - 4) * ((signed int)cpu->x[rs1] >= (signed int)cpu->x[rs2]);
                    break;

                case 0b110: // BLTU
                    cpu->pc += (imm_b - 4) * (cpu->x[rs1] < cpu->x[rs2]);
                    break;

                case 0b111: // BGEU
                    cpu->pc += (imm_b - 4) * (cpu->x[rs1] >= cpu->x[rs2]);
                    break;
            
            default:
            cpu->sys_err_table|=UNDEFINED_OPCODE;
                break;
        }
        break;
        case 0b0001111: //FENCE
            break;
        case 0b1110011:
            word *target_csr = 0x0;
            word trap_address = cpu->mtvec & ~0x3;
            funct3 = (instr >> 12) & 0x7;
            rd1 = (instr >> 7) & 0x1F;
            rs1 = (instr >> 15) & 0x1F;
            word csr_addr = (instr >> 20) & 0xFFF;
            word temp;
            switch (funct3)
            {
                case 0b000:
                    switch (imm)
                    {
                    case 0://ecall
                        cpu->mepc = cpu->pc;
                        cpu->mcause = 11;
                        cpu->pc = trap_address - 4;
                        break;
                    case 0b000000000001://ecall
                        cpu->mepc = cpu->pc;
                        cpu->mcause = 3;
                        cpu->pc = trap_address - 4;
                        break;
                    case 0b001100000010: //mret
                        cpu->pc = cpu-> mepc -4;
                        break;
                    default:
                    cpu->sys_err_table|=UNDEFINED_OPCODE;
                        break;
                    }
                    break;

                case 0b001:
                target_csr = csr_addr_decoder(cpu,csr_addr);
                temp = *target_csr;
                *target_csr = cpu->x[rs1];
                cpu->x[rd1] = temp;
                    break;
                case 0b010:
                target_csr = csr_addr_decoder(cpu,csr_addr);
                temp = *target_csr;
                *target_csr |=cpu->x[rs1];
                    break; 
                case 0b011:
                target_csr = csr_addr_decoder(cpu,csr_addr);
                temp = *target_csr;
                *target_csr &= ~(cpu->x[rs1]);
                cpu->x[rd1] = temp;
                    break;
                case 0b101:
                target_csr = csr_addr_decoder(cpu,csr_addr);
                cpu->x[rd1] = *target_csr;
                *target_csr = rs1;
                    break;
                case 0b110:
                target_csr = csr_addr_decoder(cpu,csr_addr);
                cpu->x[rd1] = *target_csr;
                *target_csr |=rs1;
                    break;
                case 0b111:
                target_csr = csr_addr_decoder(cpu,csr_addr);
                cpu->x[rd1] = *target_csr;
                *target_csr &= ~rs1;
                    break;

                default:
                    cpu->sys_err_table |= 1; //Undefined opcode
                    break;
            }
        break;
        case 0b1101111: //JAL
            rd1 = (instr >> 7) & 0x1F;
            signed int imm_j = (((signed int)instr >> 31) << 20) | 
                            (((instr >> 21) & 0x3FF) << 1) |
                            (((instr >> 20) & 0x1) << 11) |
                            (((instr >> 12) & 0xFF) << 12);
            cpu->x[rd1] = cpu->pc + 4;
            cpu->pc = cpu->pc + imm_j - 4; 
            break;
        case 0b1100111://JALR
            rd1 = (instr >> 7) & 0x1F;
            rs1 = (instr >> 15) & 0x1F;
            funct3 = (instr >> 12) & 0x7;
            signed int imm_i = (signed int)instr >> 20;
            word target = (cpu->x[rs1] + imm_i) & ~1;
            cpu->x[rd1] = cpu->pc + 4;
            cpu->pc = target - 4;
            break;
        case 0b0110111://LUI
            rd1 = (instr >> 7) & 0x1F;
            cpu->x[rd1] = instr & 0xFFFFF000;
            break;
        case 0b0010111://AUIPC 
            rd1 = (instr >> 7) & 0x1F;
            cpu->x[rd1] = cpu->pc + (instr & 0xFFFFF000);
            break;
        case 0b0100011: //Store Instructions
            funct3 = (instr >> 12) & 0x7;
            rs1 = (instr >> 15) & 0x1F;
            rs2 = (instr >> 20) & 0x1F; 
            signed int imm_s = ((instr >> 25) << 5) | ((instr >> 7) & 0x1F);
            imm_s = (imm_s << 20) >> 20; 
            address = cpu->x[rs1] + imm_s;
            word data_to_store = cpu->x[rs2];
            switch (funct3) {
                case 0b000:
                    write_byte_to_address_space(address, (byte)data_to_store, cpu); //SB
                    break;
                case 0b001:
                    write_hword_to_address_space(address, (hword)data_to_store, cpu);//SH
                    break;

                case 0b010:
                    write_word_to_address_space(address, data_to_store, cpu);//SW
                    break;
                default:
                    cpu->sys_err_table|=UNDEFINED_OPCODE;
                    break;
            }
        break;
        case 0b0000011://Load Instructions
            rd1 = (instr >> 7)& 0x1F;
            funct3 = (instr >> 12)&0x7;
            rs1 = (instr >> 15)&0x1F;
            rs2 = (instr >> 20)&0x1F;
            imm = (signed int)instr >> 20;
            address = cpu->x[rs1] + imm;
            switch (funct3)
            {
            case 0b000://LB
                signed char sbyte_val = (signed char)read_byte_from_address_space(address,cpu);
                cpu->x[rd1] = (signed int)sbyte_val; 
                break;
            case 0b001://LH
                signed short shword_val = (signed short)read_hword_from_address_space(address,cpu);
                cpu->x[rd1] = (signed int)shword_val;           
                break;
            case 0b010://LW
                cpu->x[rd1] = (word)read_word_from_address_space(address,cpu); 
                break;
            case 0b100://LBU
                byte byte_val = (byte)read_byte_from_address_space(address,cpu);
                cpu->x[rd1] = (signed int)byte_val;
                break;
            case 0b101://LHU
                hword hword_val = (hword)read_hword_from_address_space(address,cpu);
                cpu->x[rd1] = (signed int)hword_val;
                break;     
            default:
                cpu->sys_err_table|=UNDEFINED_OPCODE;
                break;
            }
                
        break;
        case 0b0110011: // R type instructions
{
    rd1 = (instr >> 7) & 0x1F;
    funct3 = (instr >> 12) & 0x7;
    rs1 = (instr >> 15) & 0x1F;
    rs2 = (instr >> 20) & 0x1F;
    func7 = (instr >> 25) & 0x7F;
    word combined = (func7 << 3) | funct3;
    bool zero_err;
    signed int dividend,divisor;
    switch (combined)
    {
        case 0b0000000000:
            cpu->x[rd1] = cpu->x[rs1] + cpu->x[rs2]; // add
            break;
            
        case 0b0100000000:
            cpu->x[rd1] = cpu->x[rs1] - cpu->x[rs2]; // sub
            break;
            
        case 0b0000000001:
            cpu->x[rd1] = cpu->x[rs1] << (cpu->x[rs2] & 0x1F); // sll
            break;    
            
        case 0b0000000010:
            cpu->x[rd1] = ((signed int)cpu->x[rs1] < (signed int)cpu->x[rs2]) ? 1 : 0; // slt
            break;        
            
        case 0b0000000011:
            cpu->x[rd1] = (cpu->x[rs1] < cpu->x[rs2]) ? 1 : 0; // sltu
            break;
            
        case 0b0000000100:
            cpu->x[rd1] = cpu->x[rs1] ^ cpu->x[rs2]; // xor
            break;
            
        case 0b0000000101:
            cpu->x[rd1] = cpu->x[rs1] >> (cpu->x[rs2] & 0x1F); // srl
            break;
            
        case 0b0100000101:
            cpu->x[rd1] = (signed int)cpu->x[rs1] >> (cpu->x[rs2] & 0x1F); // sra
            break;
            
        case 0b0000000110:
            cpu->x[rd1] = cpu->x[rs1] | cpu->x[rs2]; // or
            break;
            
        case 0b0000000111:
            cpu->x[rd1] = cpu->x[rs1] & cpu->x[rs2]; // and
            break;
            //M Extension
        case 0b0000001000:
            cpu->x[rd1] = (signed int)cpu->x[rs1] * (signed int)cpu->x[rs2]; //mul
            break;
        case 0b0000001001:
            cpu->x[rd1] = emulate_mulh((signed int)cpu->x[rs1],(signed int)cpu->x[rs2]);  //mulh
            break;
        case 0b0000001011: 
        {
            cpu->x[rd1] = mulhu_32(cpu->x[rs1], cpu->x[rs2]); //mulhu
            break;
        }    
        case 0b0000001010:  //mulhsu
        {
            word u = cpu->x[rs1]; 
            word v = cpu->x[rs2]; 
            
            word hi = mulhu_32(u, v);

            if ((signed int)u < 0) {
                hi -= v;
            }
            
            cpu->x[rd1] = hi;
            break;
        }
        case 0b000001100://div
        zero_err = (cpu->x[rs2] == 0);
        cpu->sys_err_table |= (UNDEFINED_OPCODE*zero_err);
        divisor = cpu->x[rs2] + zero_err;
        dividend = cpu->x[rs1];
        if (dividend == (signed int)0x80000000 && divisor == -1) {
                cpu->x[rd1] = dividend; break; 
            }
        cpu->x[rd1] = ((signed int) dividend /(signed int) divisor )|(0xFFFFFFFF * zero_err);

        break;
        case 0b000001101://divu
        zero_err = (cpu->x[rs2] == 0);
        cpu->sys_err_table |= (UNDEFINED_OPCODE*zero_err);
        divisor = cpu->x[rs2] + zero_err;
        dividend = cpu->x[rs1];
        cpu->x[rd1] = ((word) dividend /( word )divisor|(0xFFFFFFFF * zero_err));
        break;
        case 0b0000001110: //rem
        {
            signed int dividend = (signed int)cpu->x[rs1];
            signed int rs2_val  = (signed int)cpu->x[rs2];
            bool zero_err = (rs2_val == 0);
            bool over_err = (dividend == (signed int)0x80000000 && rs2_val == -1);
            cpu->sys_err_table |= (UNDEFINED_OPCODE * zero_err); 
            signed int divisor = rs2_val + zero_err + (over_err * 2); 
            signed int raw_rem = dividend % divisor;
            cpu->x[rd1] = (raw_rem * !(zero_err | over_err)) + (dividend * zero_err);
            break;
        }
        case 0b0000001111: //remu
        {
            word dividend = (word)cpu->x[rs1];
            word rs2_val  = (word)cpu->x[rs2];          
            bool zero_err = (rs2_val == 0);
            cpu->sys_err_table |= (UNDEFINED_OPCODE * zero_err);
            word divisor = rs2_val + zero_err; 
            word raw_rem = dividend % divisor;
            cpu->x[rd1] = (raw_rem * !zero_err) + (dividend * zero_err);
            break;
        }
        default:
            cpu->sys_err_table |= UNDEFINED_OPCODE;
            break;
    }
    break;
}
        case 0b0010011://I type
            imm = (signed int)instr >> 20;
            rd1 = (instr >> 7)& 0x1F;
            funct3 = (instr >> 12)&0x7;
            rs1 = (instr >> 15)&0x1F;
            switch (funct3)
            {
            case 0b000:
                cpu->x[rd1] = cpu->x[rs1] + imm; //addi
                break;
            case 0b010:
                cpu->x[rd1] = (signed int)cpu->x[rs1] < (signed int)imm; //slti 
                break;
            case 0b011:
                cpu->x[rd1] = cpu->x[rs1] < imm; //sltui
                break;
            case 0b100:
                cpu->x[rd1] = cpu->x[rs1] ^ imm; //xori
                break;
            case 0b110:
                cpu->x[rd1] = cpu->x[rs1] | imm; //ori
                break;
            case 0b111:
                cpu->x[rd1] = cpu->x[rs1] & imm; //andi
                break;     
            case 0b001:
                cpu->x[rd1] = cpu->x[rs1] << imm; //slli
                break;     
            case 0b101:
                func7 = (instr >> 25)&0x7f;
                switch (func7)
                {
                case 0b0000000:
                    cpu->x[rd1] = cpu->x[rs1] >> (imm & 0x1F); //srli
                    break;
                case 0b0100000:
                    cpu->x[rd1] = (signed int)cpu->x[rs1] >> (imm & 0x1F); //srai
                    break;
                
                default:
                cpu->sys_err_table|=UNDEFINED_OPCODE;
                    break;
                }
                break;
            default:
                cpu->sys_err_table|=UNDEFINED_OPCODE;
                break;
            }


        break;
        default:
            cpu->sys_err_table |= UNDEFINED_OPCODE;
            cpu->pc = cpu->pc+4;
            break;
        };
        cpu->pc = cpu->pc+4;
        cpu->x[0] = 0;
        return cpu->sys_err_table;
}
word * csr_addr_decoder(rv32i*cpu,word csr_addr){
            switch (csr_addr) {
                    case 0x300: return &cpu->mstatus;
                    case 0x305: return &cpu->mtvec;
                    case 0x341: return &cpu->mepc;
                    case 0x342: return &cpu->mcause; 
                    case 0x343: return &cpu->mtval; 
                    default:
                        cpu->sys_err_table |= 1;
                        return 0; //Undefined opcode
                        break;
                }
}
signed int emulate_mulh(signed int a, signed int b) {
    word a_low = (word)a & 0xFFFF;
    signed int a_high = a >> 16;
    word b_low = (word)b & 0xFFFF;
    signed int b_high = b >> 16;
    word p0 = a_low * b_low;
    word p1 = a_low * b_high;
    word p2 = a_high * b_low;
    word p3 = a_high * b_high;
    word mid = (p0 >> 16) + (hword)p1 + (hword)p2;
    word high = p3 + (mid >> 16) + (p1 >> 16) + (p2 >> 16);

    return (signed int)high;
}
word mulhu_32(word u, word v) {
    word u0 = u & 0xFFFF, u1 = u >> 16;
    word v0 = v & 0xFFFF, v1 = v >> 16;
    
    // Çapraz çarpımlar ve taşıma (carry) hesaplamaları
    word p0 = u0 * v0;
    word p1 = u1 * v0 + (p0 >> 16);
    word p2 = u0 * v1 + (p1 & 0xFFFF);
    
    // Sonucun en üst 32-bitini döndür
    return (u1 * v1) + (p1 >> 16) + (p2 >> 16);
}