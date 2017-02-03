#include <stdio.h>
#include <string.h>

int decode(unsigned int buff);

int main()
{
	FILE *iImage, *dImage, *result, *error;
	int iMemory[256]={0}, dMemory[256]={0}, reg[32]={0}, PC=0, iTotal=0, dTotal=0, cycle=0;
    int opcode=0, funct=0, rs=0, rt=0, rd=0, shamt=0, imm=0, immu=0, immj=0, i=0, j=0;
    int IF=0, stall=0, stop=0, flush=0, IDfwd=0, IDfwd_Reg=0, EXfwd=0, EXfwd_Reg=0, WB_Value=0, ALU_Result=0, jumpPC=0, writeData_ID=0, writeData_EX=0, writeData_DM=0;
    int isWB_ID=0, isWB_EX=0, isWB_DM=0, isWB_WB=0, isWD_ID=0, isWD_EX=0, isWD_DM=0, isRD_ID=0, isRD_EX=0, isRD_DM=0;
    int WB_Reg_ID=0, WB_Reg_EX=0, WB_Reg_DM=0, WB_Reg_WB=0, data1=0, data2=0, ALUOP=0, data1_Reg=0, data2_Reg=0;
    char ID[10]="NOP", EX[10]="NOP", DM[10]="NOP", WB[10]="NOP";
    int write_zero, address_overflow, data_misaligned, number_overflow;
    unsigned int buff;

    iImage = fopen("iimage.bin", "rb");
    dImage = fopen("dimage.bin", "rb");
    result = fopen("snapshot.rpt", "w");
    error = fopen("error_dump.rpt", "w");

    //read input
    if(fread(&buff, sizeof(int), 1, iImage)) PC = decode(buff)/4;

    if(fread(&buff, sizeof(int), 1, dImage)) reg[29] = decode(buff);

    if(fread(&buff, sizeof(int), 1, iImage)) iTotal = decode(buff);

    if(fread(&buff, sizeof(int), 1, dImage)) dTotal = decode(buff);

    for(i=PC; i<PC+iTotal; i++) if(fread(&buff, sizeof(int), 1, iImage)) iMemory[i] = decode(buff);

    for(i=0; i<dTotal; i++) if(fread(&buff, sizeof(int), 1, dImage)) dMemory[i] = decode(buff);

    fclose(iImage); //remember to close the file
    fclose(dImage);

    while(1)
    {
        //print cycle, reg and PC
        fprintf(result, "cycle %d\n", cycle);
        for(i=0; i<32; i++) fprintf(result, "$%02d: 0x%08X\n", i, reg[i]);
        fprintf(result, "PC: 0x%08X\n", PC*4);
        cycle++;

        //set error to 0
        write_zero = 0;
        address_overflow = 0;
        data_misaligned = 0;
        number_overflow = 0;

        //WB stage
        isWB_WB = isWB_DM;
        WB_Reg_WB = WB_Reg_DM;
        strcpy(WB, DM);
        if(isWB_WB)
        {
            if(WB_Reg_WB == 0) write_zero = 1;
            else reg[WB_Reg_WB] = WB_Value;
        }

        //DM stage
        isWB_DM = isWB_EX;
        isWD_DM = isWD_EX;
        isRD_DM = isRD_EX;
        writeData_DM = writeData_EX;
        WB_Reg_DM = WB_Reg_EX;
        strcpy(DM, EX);
        i = ALU_Result/4;
        j = ALU_Result%4;

        if(isWD_DM != 0) //sb, sh, sw
        {
            if(isWD_DM == 1) //sb
            {
                if(ALU_Result<0 || ALU_Result>1023) address_overflow = 1;
                else
                {
                    if(j==3)
                    {
                        dMemory[i] &= 0xFFFFFF00;
                        dMemory[i] |= writeData_DM&0xFF;
                    }
                    else if(j==2)
                    {
                        dMemory[i] &= 0xFFFF00FF;
                        dMemory[i] |= writeData_DM<<8&0xFF00;
                    }
                    else if(j==1)
                    {
                        dMemory[i] &= 0xFF00FFFF;
                        dMemory[i] |= writeData_DM<<16&0xFF0000;
                    }
                    else
                    {
                        dMemory[i] &= 0xFFFFFF;
                        dMemory[i] |= writeData_DM<<24;
                    }
                }
            }
            else if(isWD_DM == 2) //sh
            {
                if(ALU_Result<0 || ALU_Result>1022) address_overflow = 1;
                if(ALU_Result%2!=0) data_misaligned = 1;
                if(!address_overflow && !data_misaligned)
                {
                    if(j==2)
                    {
                        dMemory[i] &= 0xFFFF0000;
                        dMemory[i] |= writeData_DM&0xFFFF;
                    }
                    else
                    {
                        dMemory[i] &= 0xFFFF;
                        dMemory[i] |= writeData_DM<<16;
                    }
                }
            }
            else //sw
            {
                if(ALU_Result<0 || ALU_Result>1020) address_overflow = 1;
                if(ALU_Result%4!=0) data_misaligned = 1;
                if(!address_overflow && !data_misaligned) dMemory[i] = writeData_DM;
            }
        }

        else if(isRD_DM != 0) //lb, lh, lw, lbu, lhu
        {
            if(isRD_DM == 1) //lb
            {
                if(ALU_Result<0 || ALU_Result>1023) address_overflow = 1;
                else
                {
                    if(j==3) WB_Value = dMemory[i]<<24>>24;
                    else if(j==2) WB_Value = dMemory[i]<<16>>24;
                    else if(j==1) WB_Value = dMemory[i]<<8>>24;
                    else WB_Value = dMemory[i]>>24;
                }
            }
            else if(isRD_DM == 2) //lh
            {
                if(ALU_Result<0 || ALU_Result>1022) address_overflow = 1;
                if(ALU_Result%2!=0) data_misaligned = 1;
                if(!address_overflow && !data_misaligned)
                {
                    if(j==2) WB_Value = dMemory[i]<<16>>16;
                    else WB_Value = dMemory[i]>>16;
                }
            }
            else if(isRD_DM == 3) //lw
            {
                if(ALU_Result<0 || ALU_Result>1020) address_overflow = 1;
                if(ALU_Result%4!=0) data_misaligned = 1;
                if(!address_overflow && !data_misaligned) WB_Value = dMemory[i];
            }
            else if(isRD_DM == 4) //lbu
            {
                if(ALU_Result<0 || ALU_Result>1023) address_overflow = 1;
                else
                {
                    if(j==3) WB_Value = dMemory[i]&0xFF;
                    else if(j==2) WB_Value = dMemory[i]>>8&0xFF;
                    else if(j==1) WB_Value = dMemory[i]>>16&0xFF;
                    else WB_Value = (unsigned int)dMemory[i]>>24;
                }
            }
            else //lhu
            {
                if(ALU_Result<0 || ALU_Result>1022) address_overflow = 1;
                if(ALU_Result%2!=0) data_misaligned = 1;
                if(!address_overflow && !data_misaligned)
                {
                    if(j==2) WB_Value = dMemory[i]&0xFFFF;
                    else WB_Value = (unsigned int)dMemory[i]>>16;
                }
            }
        }

        else WB_Value = ALU_Result; //not memory related instruction

        //EX stage
        EXfwd = 0;
        if(!stop)
        {
            isWB_EX = isWB_ID;
            isWD_EX = isWD_ID;
            isRD_EX = isRD_ID;
            writeData_EX = writeData_ID;
            WB_Reg_EX = WB_Reg_ID;
            strcpy(EX, ID);
        }
        else
        {
            ALUOP = 0;
            isWB_EX = 0;
            isWD_EX = 0;
            isRD_EX = 0;
            WB_Reg_EX = 0;
            strcpy(EX, "NOP");
        }
        if(ALUOP != 0) //not nop, jr, beq, bne, bgtz, j
        {
            //deal with forward
            if(data1_Reg!=0 && data1_Reg==WB_Reg_DM)
            {
                EXfwd++;
                EXfwd_Reg = data1_Reg;
                data1 = WB_Value;
            }
            if(data2_Reg!=0 && data2_Reg==WB_Reg_DM)
            {
                EXfwd += 2;
                EXfwd_Reg = data2_Reg;
                if(isWD_EX != 0) writeData_EX = WB_Value;
                else data2 = WB_Value;
            }

            if(ALUOP == 1) //add, addi, lb, lh, lw, lbu, lhu, sb, sh, sw
            {
                ALU_Result = data1 + data2;
                if(data1>0 && data2>0 && ALU_Result<=0) number_overflow = 1;
                else if(data1<0 && data2<0 && ALU_Result>=0) number_overflow = 1;
            }
            else if(ALUOP == 2) ALU_Result = data1 + data2; //addu, addiu, jal
            else if(ALUOP == 3) //sub
            {
                data2 = ~data2 + 1;
                ALU_Result = data1 + data2;
                if(data1>0 && data2>0 && ALU_Result<=0) number_overflow = 1;
                else if(data1<0 && data2<0 && ALU_Result>=0) number_overflow = 1;
            }
            else if(ALUOP == 4) ALU_Result = data1 & data2; //and, andi
            else if(ALUOP == 5) ALU_Result = data1 | data2; //or, ori
            else if(ALUOP == 6) ALU_Result = data1 ^ data2; //xor
            else if(ALUOP == 7) ALU_Result = ~(data1 | data2); //nor, nori
            else if(ALUOP == 8) ALU_Result = ~(data1 & data2); //nand
            else if(ALUOP == 9) ALU_Result = data1 < data2; //slt, slti
            else if(ALUOP == 10) ALU_Result = data2 << data1; //sll, lui
            else if(ALUOP == 11) ALU_Result = (unsigned int)data2 >> data1; //srl
            else ALU_Result = data2 >> data1; //sra
        }

        //ID stage
        IDfwd = 0;
        stall = 0;
        flush = 0;
        ALUOP = 0;
        isWB_ID = 0;
        isWD_ID = 0;
        isRD_ID = 0;
        data1_Reg = 0;
        data2_Reg = 0;
        WB_Reg_ID = 0;

        if(!stop) //not stall, get fragments of instruction
        {
            opcode = (unsigned int)IF>>26;
            funct = IF&0x3F;
            rs = IF>>21&0x1F;
            rt = IF>>16&0x1F;
            rd = IF>>11&0x1F;
            shamt = IF>>6&0x1F;
            imm = IF<<16>>16;
            immu = IF&0xFFFF;
            immj = IF&0x03FFFFFF;
        }

        if(opcode==0x00) //R type
        {
            if(funct < 5) //nop, sll, srl, sra
            {
                if(funct || rt || rd || shamt) //not nop
                {
                    isWB_ID = 1;
                    data1 = shamt;
                    data2 = reg[rt];
                    data2_Reg = rt;
                    WB_Reg_ID = rd;
                    if(rt!=0 && WB_Reg_EX==rt && isRD_EX!=0) stall = 1;
                    else if(rt!=0 && WB_Reg_DM==rt && WB_Reg_EX!=rt) stall = 1;

                    if(funct == 0x00) //sll
                    {
                        ALUOP = 10;
                        strcpy(ID, "SLL");
                    }
                    else if(funct == 0x02) //srl
                    {
                        ALUOP = 11;
                        strcpy(ID, "SRL");
                    }
                    else //sra
                    {
                        ALUOP = 12;
                        strcpy(ID, "SRA");
                    }
                }
                else strcpy(ID, "NOP");
            }
            else if(funct < 10) //jr
            {
                data1 = reg[rs];
                if(rs!=0 && WB_Reg_EX==rs) stall = 1;
                else if(rs!=0 && WB_Reg_DM==rs)
                {
                    if(isRD_DM != 0) stall = 1;
                    else
                    {
                        IDfwd++;
                        IDfwd_Reg = rs;
                        data1 = WB_Value;
                    }
                }
                if(!stall)
                {
                    flush = 1;
                    jumpPC = data1/4;
                }
                strcpy(ID, "JR");
            }
            else //add, addu, sub, and, or, xor, nor, nand, slt
            {
                isWB_ID = 1;
                data1 = reg[rs];
                data2 = reg[rt];
                data1_Reg = rs;
                data2_Reg = rt;
                WB_Reg_ID = rd;
                if(rs!=0 && WB_Reg_EX==rs && isRD_EX!=0) stall = 1;
                else if(rt!=0 && WB_Reg_EX==rt && isRD_EX!=0) stall = 1;
                else if(rs!=0 && WB_Reg_DM==rs && WB_Reg_EX!=rs) stall = 1;
                else if(rt!=0 && WB_Reg_DM==rt && WB_Reg_EX!=rt) stall = 1;

                if(funct == 0x20) //add
                {
                    ALUOP = 1;
                    strcpy(ID, "ADD");
                }
                else if(funct == 0x21) //addu
                {
                    ALUOP = 2;
                    strcpy(ID, "ADDU");
                }
                else if(funct == 0x22) //sub
                {
                    ALUOP = 3;
                    strcpy(ID, "SUB");
                }
                else if(funct == 0x24) //and
                {
                    ALUOP = 4;
                    strcpy(ID, "AND");
                }
                else if(funct == 0x25) //or
                {
                    ALUOP = 5;
                    strcpy(ID, "OR");
                }
                else if(funct == 0x26) //xor
                {
                    ALUOP = 6;
                    strcpy(ID, "XOR");
                }
                else if(funct == 0x27) //nor
                {
                    ALUOP = 7;
                    strcpy(ID, "NOR");
                }
                else if(funct == 0x28) //nand
                {
                    ALUOP = 8;
                    strcpy(ID, "NAND");
                }
                else //slt
                {
                    ALUOP = 9;
                    strcpy(ID, "SLT");
                }
            }
        }

        else //I J S type
        {
            if(opcode < 4) //j, jal
            {
                flush = 1;
                jumpPC = immj;
                if(opcode == 0x02) strcpy(ID, "J"); //j
                else //jal
                {
                    ALUOP = 2;
                    isWB_ID = 1;
                    data1 = PC*4;
                    data2 = 0;
                    WB_Reg_ID = 31;
                    strcpy(ID, "JAL");
                }
            }
            else if(opcode < 7) //beq, bne
            {
                data1 = reg[rs];
                data2 = reg[rt];
                if(rs!=0 && WB_Reg_EX==rs) stall = 1;
                else if(rt!=0 && WB_Reg_EX==rt) stall = 1;
                else
                {
                    if(rs!=0 && WB_Reg_DM==rs)
                    {
                        if(isRD_DM != 0) stall = 1;
                        else
                        {
                            IDfwd++;
                            IDfwd_Reg = rs;
                            data1 = WB_Value;
                        }
                    }
                    if(rt!=0 && WB_Reg_DM==rt)
                    {
                        if(isRD_DM != 0) stall = 1;
                        else
                        {
                            IDfwd += 2;
                            IDfwd_Reg = rt;
                            data2 = WB_Value;
                        }
                    }
                }
                if(opcode == 0x04) //beq
                {
                    if(!stall)
                    {
                        if(data1==data2)
                        {
                            flush = 1;
                            jumpPC = PC + imm;
                        }
                    }
                    strcpy(ID, "BEQ");
                }
                else //bne
                {
                    if(!stall)
                    {
                        if(data1!=data2)
                        {
                            flush = 1;
                            jumpPC = PC + imm;
                        }
                    }
                    strcpy(ID, "BNE");
                }
            }
            else if(opcode < 8) //bgtz
            {

                data1 = reg[rs];
                if(rs!=0 && WB_Reg_EX==rs) stall = 1;
                else if(rs!=0 && WB_Reg_DM==rs)
                {
                    if(isRD_DM != 0) stall = 1;
                    else
                    {
                        IDfwd++;
                        IDfwd_Reg = rs;
                        data1 = WB_Value;
                    }
                }
                if(!stall)
                {
                    if(data1>0)
                    {
                        flush = 1;
                        jumpPC = PC + imm;
                    }
                }
                strcpy(ID, "BGTZ");
            }
            else if(opcode < 15) //addi, addiu, slti, andi, ori, nori
            {
                isWB_ID = 1;
                data1 = reg[rs];
                if(opcode < 11) data2 = imm;
                else data2 = immu;
                data1_Reg = rs;
                WB_Reg_ID = rt;
                if(rs!=0 && WB_Reg_EX==rs && isRD_EX!=0) stall = 1;
                else if(rs!=0 && WB_Reg_DM==rs && WB_Reg_EX!=rs) stall = 1;

                if(opcode == 0x08) //addi
                {
                    ALUOP = 1;
                    strcpy(ID, "ADDI");
                }
                else if(opcode == 0x09) //addiu
                {
                    ALUOP = 2;
                    strcpy(ID, "ADDIU");
                }
                else if(opcode == 0x0A) //slti
                {
                    ALUOP = 9;
                    strcpy(ID, "SLTI");
                }
                else if(opcode == 0x0C) //andi
                {
                    ALUOP = 4;
                    strcpy(ID, "ANDI");
                }
                else if(opcode == 0x0D) //ori
                {
                    ALUOP = 5;
                    strcpy(ID, "ORI");
                }
                else //nori
                {
                    ALUOP = 7;
                    strcpy(ID, "NORI");
                }
            }
            else if(opcode < 16) //lui
            {
                ALUOP = 10;
                isWB_ID = 1;
                data1 = 16;
                data2 = imm;
                WB_Reg_ID = rt;
                strcpy(ID, "LUI");
            }
            else if(opcode < 40) //lb, lh, lw, lbu, lhu
            {
                ALUOP = 1;
                isWB_ID = 1;
                data1 = reg[rs];
                data2 = imm;
                data1_Reg = rs;
                WB_Reg_ID = rt;
                if(rs!=0 && WB_Reg_EX==rs && isRD_EX!=0) stall = 1;
                else if(rs!=0 && WB_Reg_DM==rs && WB_Reg_EX!=rs) stall = 1;

                if(opcode == 0x20) //lb
                {
                    isRD_ID = 1;
                    strcpy(ID, "LB");
                }
                else if(opcode == 0x21) //lh
                {
                    isRD_ID = 2;
                    strcpy(ID, "LH");
                }
                else if(opcode == 0x23) //lw
                {
                    isRD_ID = 3;
                    strcpy(ID, "LW");
                }
                else if(opcode == 0x24) //lbu
                {
                    isRD_ID = 4;
                    strcpy(ID, "LBU");
                }
                else //lhu
                {
                    isRD_ID = 5;
                    strcpy(ID, "LHU");
                }
            }
            else if(opcode < 50) //sb, sh, sw
            {
                ALUOP = 1;
                data1 = reg[rs];
                data2 = imm;
                writeData_ID = reg[rt];
                data1_Reg = rs;
                data2_Reg = rt;
                if(rs!=0 && WB_Reg_EX==rs && isRD_EX!=0) stall = 1;
                else if(rt!=0 && WB_Reg_EX==rt && isRD_EX!=0) stall = 1;
                else if(rs!=0 && WB_Reg_DM==rs && WB_Reg_EX!=rs) stall = 1;
                else if(rt!=0 && WB_Reg_DM==rt && WB_Reg_EX!=rt) stall = 1;

                if(opcode == 0x28) //sb
                {
                    isWD_ID = 1;
                    strcpy(ID, "SB");
                }
                else if(opcode == 0x29) //sh
                {
                    isWD_ID = 2;
                    strcpy(ID, "SH");
                }
                else //sw
                {
                    isWD_ID = 3;
                    strcpy(ID, "SW");
                }
            }
            else strcpy(ID, "HALT"); //halt
        }

        //IF stage
        if(!stop) IF = iMemory[PC];
        else stop = 0;

        //print IF
        fprintf(result, "IF: 0x%08X", IF);
        if(stall)
        {
            fprintf(result, " to_be_stalled");
            stop = 1;
        }
        else if(flush)
        {
            fprintf(result, " to_be_flushed");
            IF = 0;
            PC = jumpPC;
        }
        else PC++;
        fprintf(result, "\n");

        //print ID
        fprintf(result, "ID: %s", ID);
        if(stall) fprintf(result, " to_be_stalled");
        if(IDfwd==1 || IDfwd==3)
        {
            fprintf(result, " fwd_EX-DM_");
            fprintf(result, "rs_");
            fprintf(result, "$%d", IDfwd_Reg);
        }
        if(IDfwd>=2)
        {
            fprintf(result, " fwd_EX-DM_");
            fprintf(result, "rt_");
            fprintf(result, "$%d", IDfwd_Reg);
        }
        fprintf(result, "\n");

        //print EX
        fprintf(result, "EX: %s", EX);
        if(EXfwd==1 || EXfwd==3)
        {
            fprintf(result, " fwd_EX-DM_");
            fprintf(result, "rs_");
            fprintf(result, "$%d", EXfwd_Reg);
        }
        if(EXfwd>=2)
        {
            fprintf(result, " fwd_EX-DM_");
            fprintf(result, "rt_");
            fprintf(result, "$%d", EXfwd_Reg);
        }
        fprintf(result, "\n");

        //print DM
        fprintf(result, "DM: %s\n", DM);

        //print WB
        fprintf(result, "WB: %s\n\n\n", WB);

        if(write_zero) fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
        if(address_overflow) fprintf(error, "In cycle %d: Address Overflow\n", cycle);
        if(data_misaligned) fprintf(error, "In cycle %d: Misalignment Error\n", cycle);
        if(number_overflow) fprintf(error, "In cycle %d: Number Overflow\n", cycle);

        if(address_overflow || data_misaligned) break;
        if(((unsigned int)IF>>26==0x3F) && ID[0]=='H' && EX[0]=='H' && DM[0]=='H' && WB[0]=='H') break;
    }

    fclose(result);
    fclose(error);
    return 0;
}

int decode(unsigned int buff)
{
    int answer = 0;
    answer |= buff<<24;
    answer |= buff<<8&0xFF0000;
    answer |= buff>>8&0xFF00;
    answer |= buff>>24;
    return answer;
}
