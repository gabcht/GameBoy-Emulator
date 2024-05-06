#include <opcode.h>
#include <bus.h>
#include <instructions.h>

void logs(Cpu *cpu)
{
    FILE *f= fopen("logs.txt", "a+");
	if (f==NULL)
	{
		printf("error: Couldn't open logs.txt\n");
		exit(1); 
	}

    uint8_t f_register = (cpu->cc.z << 7) + (cpu->cc.n << 6) + (cpu->cc.h << 5) + (cpu->cc.c << 4);
    uint8_t pcmem1 = busRead(cpu->pc);
    uint8_t pcmem2 = busRead(cpu->pc+1);
    uint8_t pcmem3 = busRead(cpu->pc+2);
    uint8_t pcmem4 = busRead(cpu->pc+3);
    fprintf(f, "A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X SP:%04X PC:%04X PCMEM:%02X,%02X,%02X,%02X\n\0", 
            cpu->a, f_register, cpu->b, cpu->c, cpu->d, cpu->e, cpu->h, cpu->l, cpu->sp, cpu->pc, pcmem1, pcmem2, pcmem3, pcmem4);
    fclose(f);
}

uint16_t get16Bits(uint8_t ho, uint8_t lo)
{
    return (uint16_t)((ho << 8) + lo);
}


// Tout les test OK sont à revoir
void emulate(Cpu *gb) // 1: enable 0: disable
{
    // system("cls");
    uint8_t opcode1 = busRead(gb->pc+1);
    uint8_t opcode2 = busRead(gb->pc+2);
    uint16_t registre16Bits;
    uint16_t registre16Bits2;
    uint16_t calc;
    // uint8_t ret_instruction = 0;
    uint8_t hl_memory_data = 0;
    uint16_t addr = 0;
    /*if (busRead(gb->pc) == 0xcd)
    {
        logs(gb);
    }

    if (busRead(gb->pc) == 0xc9)
    {
        logs(gb);
        ret_instruction = 1;
    }*/
    logs(gb);

    switch (busRead(gb->pc))
    {
        case 0x00: printf("[%04X] NOP\n", gb->pc); gb->pc += 1; break;                            // NOP (test OK)
        case 0x01:        
            gb->pc += 1;                                                           // LD BC d16
            printf("[%04X] LD BC, 0x%02X%02X\n", gb->pc, opcode2, opcode1);
            ld16(&gb->b, &gb->c, opcode2, opcode1);        
            gb->pc += 2;   
            break;
        case 0x02:                                                                   // LD (BC), A (test OK)
            printf("[%04X] LD (BC), A\n", gb->pc);
            gb->pc += 1;
            busWrite(get16Bits(gb->b, gb->c), gb->a);
            break;
        case 0x03:                                                                   // INC BC (test OK)
            printf("[%04X] INC BC\n", gb->pc);
            gb->pc += 1;
            inc16(&gb->b, &gb->c);
            break;
        case 0x04:                                                                   // INC B (test OK)
            printf("[%04X] INC B\n", gb->pc);
            gb->pc += 1;
            inc(gb, &gb->b);
            break;
        case 0x05:                                                                   // DEC B (test OK)
            printf("[%04X] DEC B\n", gb->pc);
            gb->pc += 1;
            dec(gb, &gb->b);
            break;        
        case 0x06:                         // LD B, d8
            printf("[%04X] LD B, %02X\n", gb->pc, opcode1);
            gb->pc += 1;
            ld(&gb->b, opcode1);
            gb->pc += 1;
            break;
        case 0x07:                         // RLCA a revoir
            printf("[%04X] RLCA\n", gb->pc);
            gb->pc += 1;
            gb->cc.z = 0;
            gb->cc.h = 0;
            gb->cc.n = 0;
            uint8_t cyBefore = gb->cc.c;
            calc = (gb->a << 1);
            gb->cc.c = (uint8_t)(calc & 0x100);
            gb->a = (calc | cyBefore) & 0xFF; 
            break;
        case 0x08:                                                      // LD (a16),SP pas sur
            registre16Bits = (opcode2 << 8) + opcode1;
            printf("[%04X] LD $%04X,SP\n", gb->pc, registre16Bits);
            gb->pc += 1;
            busWrite(registre16Bits, gb->sp & 0x00ff);
            busWrite(registre16Bits+1, (gb->sp & 0xff00) >> 8);
            gb->pc += 2;
            break;
        case 0x09:                                                      // ADD HL,BC
            printf("[%04X] ADD HL,BC\n", gb->pc);
            gb->pc += 1;
            add16(gb, &gb->h, &gb->l, get16Bits(gb->b, gb->c));
            break;
        case 0x0a:                                                      // LD A,(BC)
            printf("[%04X] LD A,(BC)\n", gb->pc);
            gb->pc += 1;      
            ld(&gb->a, busRead(get16Bits(gb->b, gb->c)));
            break;
        case 0x0b:                                                      // DEC BC
            printf("[%04X] DEC BC\n", gb->pc);
            gb->pc += 1;
            dec16(&gb->b, &gb->c);
            break;
        case 0x0c:                                                      // INC C (test OK)
            printf("[%04X] INC C\n", gb->pc);
            gb->pc += 1;
            inc(gb, &gb->c);
            break;
        case 0x0D:                                                      // DEC C (test OK)
            printf("[%04X] DEC C\n", gb->pc);
            gb->pc += 1;
            dec(gb, &gb->c);
            break;  
        case 0x0E:                                                      // LD C, d8
            printf("[%04X] LD C, %02X\n", gb->pc, opcode1);
            gb->pc += 1;
            ld(&gb->c, opcode1);
            gb->pc += 1;
            break;
        case 0x0F:                                                      // RRCA à revoir
            printf("[%04X] RRCA\n", gb->pc);
            gb->pc += 1;
            gb->cc.z = 0;
            gb->cc.h = 0;
            gb->cc.n = 0;
            uint8_t bitF = gb->a & 0x01;
            gb->a = (gb->a >> 1) | (gb->cc.c << 3);
            gb->cc.c = bitF;
            break;


        case 0x10: printf("Non Implementé\n"); gb->pc += 2; break;      // STOP 0
        case 0x11:                                                      // LD DE d16 (test OK)
            printf("[%04X] LD DE, 0x%02X%02X\n", gb->pc, opcode2, opcode1);
            gb->pc += 1;
            ld16(&gb->d, &gb->e, opcode2, opcode1);             
            gb->pc += 2;   
            break;
        case 0x12:                                                      // LD (DE), A (test OK)
            printf("[%04X] LD (BC), A\n", gb->pc);
            gb->pc += 1;
            busWrite(get16Bits(gb->d, gb->e), gb->a);
            break;
        case 0x13:                                                      // INC DE (test OK)
            printf("[%04X] INC DE\n", gb->pc);
            gb->pc += 1;
            inc16(&gb->d, &gb->e);
            break;
        case 0x14:                                                      // INC D (test OK)
            printf("[%04X] INC D\n", gb->pc);
            gb->pc += 1;
            inc(gb, &gb->d);
            break;
        case 0x15:                                                      // DEC D (test OK)
            printf("[%04X] DEC D\n", gb->pc);
            gb->pc += 1;
            dec(gb, &gb->d);
            break;        
        case 0x16:                                                      // LD D, d8
            printf("[%04X] LD D, %02X\n", gb->pc, opcode1);
            gb->pc += 1;
            ld(&gb->d, opcode1);
            gb->pc += 1;
            break;
        case 0x17:                                                      // RLA
            printf("[%04X] RLA\n", gb->pc);
            gb->pc += 1;
            break;
        case 0x18:                                                      // JR r8
            printf("[%04X] JR $%04X,SP\n", gb->pc, opcode1);
            gb->pc += 1;
            jr(gb, opcode1);
            gb->pc += 1;
            break;
        case 0x19:                                                      // ADD HL,DE
            printf("[%04X] ADD HL,DE\n", gb->pc);
            gb->pc += 1;
            add16(gb, &gb->h, &gb->l, get16Bits(gb->d, gb->e));
            break;
        case 0x1a:                                                      // LD A,(DE)
            printf("[%04X] LD A,(DE)\n", gb->pc);      
            gb->pc += 1;
            ld(&gb->a, busRead(get16Bits(gb->d, gb->e)));
            break;
        case 0x1b:                                                      // DEC DE
            printf("[%04X] DEC DE\n", gb->pc);
            gb->pc += 1;
            dec16(&gb->d, &gb->e);
            break;
        case 0x1c:                                                      // INC E (test OK)
            printf("[%04X] INC E\n", gb->pc);
            gb->pc += 1;
            inc(gb, &gb->e);
            break;
        case 0x1d:                                                      // DEC E (test OK)
            printf("[%04X] DEC E\n", gb->pc);
            gb->pc += 1;
            dec(gb, &gb->e);
            break;  
        case 0x1e:                                                      // LD E, d8
            printf("[%04X] LD E, %02X\n", gb->pc, opcode1);
            gb->pc += 1;
            ld(&gb->e, opcode1);
            gb->pc += 1;
            break;
        case 0x1f:                                                      // RRCA
            printf("[%04X] RRA\n", gb->pc);
            gb->pc += 1;
            rr(gb, &gb->a);
            break;


        case 0x20:                                                      // JR NZ, r8
            printf("[%04X] JR NZ $%02X\n", gb->pc, opcode1); 
            gb->pc += 1;
            jrNZ(gb, opcode1);
            gb->pc+=1;
            break;  
        case 0x21:                                                      // LD HL d16 (test OK)
            printf("[%04X] LD HL, 0x%02X%02X\n", gb->pc, opcode2, opcode1);
            gb->pc += 1;
            ld16(&gb->h,&gb->l, opcode2, opcode1);              
            gb->pc += 2;   
            break;
        case 0x22:
            printf("[%04X] LD (HL+), A\n", gb->pc);
            gb->pc += 1;
            busWrite(get16Bits(gb->h, gb->l), gb->a);
            inc16(&gb->h,&gb->l);
            break;
        case 0x23:
            printf("[%04X] INC HL\n", gb->pc);
            gb->pc += 1;
            inc16(&gb->h, &gb->l);
            break;
        case 0x24:
            printf("[%04X] INC H\n", gb->pc);
            gb->pc += 1;
            inc(gb, &gb->h);
            break;
        case 0x25:
            printf("[%04X] DEC H\n", gb->pc);
            gb->pc += 1;
            dec(gb, &gb->h);
            break;
        case 0x26:
            printf("[%04X] LD H, 0x%02X\n", gb->pc, opcode1);
            gb->pc += 1;
            ld(&gb->h, opcode1);
            gb->pc += 1;
            break;
        case 0x27:
            printf("[%04X] DAA\n", gb->pc);
            gb->pc += 1;
            daa(gb);
            break;
        case 0x28:
            printf("[%04X] JR Z, %02X\n", gb->pc, opcode1);
            gb->pc += 1;
            jrZ(gb, opcode1);
            gb->pc += 1;
            break;
        case 0x29:
            printf("[%04X] ADD HL, HL\n", gb->pc);
            gb->pc += 1;
            add16(gb, &gb->h, &gb->l, get16Bits(gb->h, gb->l));
            break;
        case 0x2a:
            printf("[%04X] LD A, (HL+)\n", gb->pc);
            gb->pc += 1;
            ld(&gb->a, busRead(get16Bits(gb->h, gb->l)));
            inc16(&gb->h,&gb->l);
            break;
        case 0x2b:
            printf("[%04X] DEC HL\n", gb->pc);
            gb->pc += 1;
            dec16(&gb->h, &gb->l);
            break;
        case 0x2c:
            printf("[%04X] INC L\n", gb->pc);
            gb->pc += 1;
            inc(gb, &gb->l);
            break;
        case 0x2d:
            printf("[%04X] DEC L\n", gb->pc);
            gb->pc += 1;
            dec(gb, &gb->l);
            break;
        case 0x2e:
            printf("[%04X] LD L, 0x%02X\n", gb->pc, opcode1);
            gb->pc += 1;
            ld(&gb->l, opcode1);
            gb->pc += 1;
            break;
        case 0x2f:
            printf("[%04X] CPL\n");
            gb->pc += 1;
            break;


        case 0x30: 
            printf("[%04X] JR NC $%02X\n", gb->pc, opcode1); 
            gb->pc += 1;
            jrNC(gb, opcode1);
            gb->pc+=1;
            break;
        case 0x31:                         // LD SP d16 (test OK)
            printf("[%04X] LD SP, 0x%02X%02X\n", gb->pc, opcode2, opcode1);
            gb->pc += 1;
            gb->sp = (opcode2 << 8) + opcode1;          
            gb->pc += 2;   
            break;
        case 0x32:
            printf("[%04X] LD (HL-), A\n", gb->pc);
            gb->pc += 1;
            busWrite(get16Bits(gb->h, gb->l), gb->a);
            dec16(&gb->h,&gb->l);
            break;
        case 0x33:
            printf("[%04X] INC SP\n", gb->pc);
            gb->pc += 1;
            gb->sp = gb->sp + 1;
            break;
        case 0x34:
            printf("[%04X] INC (HL)\n", gb->pc);
            gb->pc += 1;
            addr = get16Bits(gb->h, gb->l);
            hl_memory_data = busRead(get16Bits(gb->h, gb->l));
            inc(gb, &hl_memory_data);
            busWrite(addr, hl_memory_data);
            break;
        case 0x35:
            printf("[%04X] DEC (HL)\n", gb->pc);
            gb->pc += 1;
            addr = get16Bits(gb->h, gb->l);
            hl_memory_data = busRead(get16Bits(gb->h, gb->l));
            dec(gb, &hl_memory_data);
            busWrite(addr, hl_memory_data);
            break;
        case 0x36:
            printf("[%04X] LD (HL), 0x%02X\n", gb->pc, opcode1);
            gb->pc += 1;
            busWrite(get16Bits(gb->h, gb->l), opcode1);
            gb->pc += 1;
            break;
        case 0x37:
            printf("[%04X] SCF\n", gb->pc);
            gb->pc += 1;
            break;
        case 0x38:
            printf("[%04X] JR C, 0x%02X\n", gb->pc, opcode1);
            gb->pc += 1;
            jrC(gb, opcode1);
            gb->pc += 1;
            break;
        case 0x39:
            printf("[%04X] ADD HL, SP\n", gb->pc);
            gb->pc += 1;
            add16(gb, &gb->h,  &gb->l, gb->sp);
            break;
        case 0x3a:
            printf("[%04X] LD A, (HL-)\n", gb->pc);
            gb->pc += 1;
            ld(&gb->a, busRead(get16Bits(gb->h, gb->l)));
            dec16(&gb->h,&gb->l);
            break;
        case 0x3b:
            printf("[%04X] DEC SP\n", gb->pc);
            gb->pc += 1;
            gb->sp = gb->sp - 1;
            break;
        case 0x3c:
            printf("[%04X] INC A\n", gb->pc);
            gb->pc += 1;
            inc(gb, &gb->a);
            break;
        case 0x3d:
            printf("[%04X] DEC A\n", gb->pc);
            gb->pc += 1;
            dec(gb, &gb->a);
            break;
        case 0x3e:
            printf("[%04X] LD A, 0x%02X\n", gb->pc, opcode1);
            gb->pc += 1;
            ld(&gb->a, opcode1);
            gb->pc += 1;
            break;
        case 0x3f:
            printf("[%04X] CCF\n", gb->pc);
            gb->pc += 1;
            break;


        case 0x40: 
            printf("[%04X] LD B, B\n", gb->pc); 
            gb->pc += 1;
            ld(&gb->b, gb->b);
            break;
        case 0x41:                         
            printf("[%04X] LD B, C\n", gb->pc);
            gb->pc += 1;
            ld(&gb->b, gb->c);
            break;
        case 0x42:
            printf("[%04X] LD B, D\n", gb->pc);
            gb->pc += 1;
            ld(&gb->b, gb->d);
            break;
        case 0x43:
            printf("[%04X] LD B, E\n", gb->pc);
            gb->pc += 1;
            ld(&gb->b, gb->e);
            break;
        case 0x44:
            printf("[%04X] LD B, H\n", gb->pc);
            gb->pc += 1;
            ld(&gb->b, gb->h);
            break;
        case 0x45:
            printf("[%04X] LD B, L\n", gb->pc);
            gb->pc += 1;
            ld(&gb->b, gb->l);
            break;
        case 0x46:
            printf("[%04X] LD B, (HL)\n", gb->pc);
            gb->pc += 1;
            ld(&gb->b, busRead(get16Bits(gb->h, gb->l)));
            break;
        case 0x47:
            printf("[%04X] LD B, A\n", gb->pc);
            gb->pc += 1;
            ld(&gb->b, gb->a);
            break;
        case 0x48:
            printf("[%04X] LD C, B\n", gb->pc);
            gb->pc += 1;
            ld(&gb->c, gb->b);
            break;
        case 0x49:
            printf("[%04X] LD C, C\n", gb->pc);
            gb->pc += 1;
            ld(&gb->c, gb->c);
            break;
        case 0x4a:
            printf("[%04X] LD C, D\n", gb->pc);
            gb->pc += 1;
            ld(&gb->c, gb->d);
            break;
        case 0x4b:
            printf("[%04X] LD C, E\n", gb->pc);
            gb->pc += 1;
            ld(&gb->c, gb->e);
            break;
        case 0x4c:
            printf("[%04X] LD C, H\n", gb->pc);
            gb->pc += 1;
            ld(&gb->c, gb->h);
            break;
        case 0x4d:
            printf("[%04X] LD C, L\n", gb->pc);
            gb->pc += 1;
            ld(&gb->c, gb->l);
            break;
        case 0x4e:
            printf("[%04X] LD C, (HL)\n", gb->pc);
            gb->pc += 1;
            ld(&gb->c, busRead(get16Bits(gb->h, gb->l)));
            break;
        case 0x4f:
            printf("[%04X] LD C, A\n", gb->pc);
            gb->pc += 1;
            ld(&gb->c, gb->a);
            break;


        case 0x50: 
            printf("[%04X] LD D, B\n", gb->pc); 
            gb->pc += 1;
            ld(&gb->d, gb->b);
            break;
        case 0x51:                         
            printf("[%04X] LD D, C\n", gb->pc);
            gb->pc += 1;
            ld(&gb->d, gb->c);
            break;
        case 0x52:
            printf("[%04X] LD D, D\n", gb->pc);
            gb->pc += 1;
            ld(&gb->d, gb->d);
            break;
        case 0x53:
            printf("[%04X] LD D, E\n", gb->pc);
            gb->pc += 1;
            ld(&gb->d, gb->e);
            break;
        case 0x54:
            printf("[%04X] LD D, H\n", gb->pc);
            gb->pc += 1;
            ld(&gb->d, gb->h);
            break;
        case 0x55:
            printf("[%04X] LD D, L\n", gb->pc);
            gb->pc += 1;
            ld(&gb->d, gb->l);
            break;
        case 0x56:
            printf("[%04X] LD D, (HL)\n", gb->pc);
            gb->pc += 1;
            ld(&gb->d, busRead(get16Bits(gb->h, gb->l)));
            break;
        case 0x57:
            printf("[%04X] LD D, A\n", gb->pc);
            gb->pc += 1;
            ld(&gb->d, gb->a);
            break;
        case 0x58:
            printf("[%04X] LD E, B\n", gb->pc);
            gb->pc += 1;
            ld(&gb->e, gb->b);
            break;
        case 0x59:
            printf("[%04X] LD E, C\n", gb->pc);
            gb->pc += 1;
            ld(&gb->e, gb->c);
            break;
        case 0x5a:
            printf("[%04X] LD E, D\n", gb->pc);
            gb->pc += 1;
            ld(&gb->e, gb->d);
            break;
        case 0x5b:
            printf("[%04X] LD E, E\n", gb->pc);
            gb->pc += 1;
            ld(&gb->e, gb->e);
            break;
        case 0x5c:
            printf("[%04X] LD E, H\n", gb->pc);
            gb->pc += 1;
            ld(&gb->e, gb->h);
            break;
        case 0x5d:
            printf("[%04X] LD E, L\n", gb->pc);
            gb->pc += 1;
            ld(&gb->e, gb->l);
            break;
        case 0x5e:
            printf("[%04X] LD E, (HL)\n", gb->pc);
            gb->pc += 1;
            ld(&gb->e, busRead(get16Bits(gb->h, gb->l)));
            break;
        case 0x5f:
            printf("[%04X] LD E, A\n", gb->pc);
            gb->pc += 1;
            ld(&gb->e, gb->a);
            break;


        case 0x60: 
            printf("[%04X] LD H, B\n", gb->pc); 
            gb->pc += 1;
            ld(&gb->h, gb->b);
            break;
        case 0x61:                         
            printf("[%04X] LD H, C\n", gb->pc);
            gb->pc += 1;
            ld(&gb->h, gb->c);
            break;
        case 0x62:
            printf("[%04X] LD H, D\n", gb->pc);
            gb->pc += 1;
            ld(&gb->h, gb->d);
            break;
        case 0x63:
            printf("[%04X] LD H, E\n", gb->pc);
            gb->pc += 1;
            ld(&gb->h, gb->e);
            break;
        case 0x64:
            printf("[%04X] LD H, H\n", gb->pc);
            gb->pc += 1;
            ld(&gb->h, gb->h);
            break;
        case 0x65:
            printf("[%04X] LD H, L\n", gb->pc);
            gb->pc += 1;
            ld(&gb->h, gb->l);
            break;
        case 0x66:
            printf("[%04X] LD H, (HL)\n", gb->pc);
            gb->pc += 1;
            ld(&gb->h, busRead(get16Bits(gb->h, gb->l)));
            break;
        case 0x67:
            printf("[%04X] LD H, A\n", gb->pc);
            gb->pc += 1;
            ld(&gb->h, gb->a);
            break;
        case 0x68:
            printf("[%04X] LD L, B\n", gb->pc);
            gb->pc += 1;
            ld(&gb->l, gb->b);
            break;
        case 0x69:
            printf("[%04X] LD L, C\n", gb->pc);
            gb->pc += 1;
            ld(&gb->l, gb->c);
            break;
        case 0x6a:
            printf("[%04X] LD L, D\n", gb->pc);
            gb->pc += 1;
            ld(&gb->l, gb->d);
            break;
        case 0x6b:
            printf("[%04X] LD L, E\n", gb->pc);
            gb->pc += 1;
            ld(&gb->l, gb->e);
            break;
        case 0x6c:
            printf("[%04X] LD L, H\n", gb->pc);
            gb->pc += 1;
            ld(&gb->l, gb->h);
            break;
        case 0x6d:
            printf("[%04X] LD L, L\n", gb->pc);
            gb->pc += 1;
            ld(&gb->l, gb->l);
            break;
        case 0x6e:
            printf("[%04X] LD L, (HL)\n", gb->pc);
            gb->pc += 1;
            ld(&gb->l, busRead(get16Bits(gb->h, gb->l)));
            break;
        case 0x6f:
            printf("[%04X] LD L, A\n", gb->pc);
            gb->pc += 1;
            ld(&gb->l, gb->a);
            break;


        case 0x70: 
            printf("[%04X] LD (HL), B\n", gb->pc); 
            gb->pc += 1;
            busWrite(get16Bits(gb->h, gb->l), gb->b);
            break;
        case 0x71:                         
            printf("[%04X] LD (HL), C\n", gb->pc);
            gb->pc += 1;
            busWrite(get16Bits(gb->h, gb->l), gb->c);
            break;
        case 0x72:
            printf("[%04X] LD (HL), D\n", gb->pc);
            gb->pc += 1;
            busWrite(get16Bits(gb->h, gb->l), gb->d);
            break;
        case 0x73:
            printf("[%04X] LD (HL), E\n", gb->pc);
            gb->pc += 1;
            busWrite(get16Bits(gb->h, gb->l), gb->e);
            break;
        case 0x74:
            printf("[%04X] LD (HL), H\n", gb->pc);
            gb->pc += 1;
            busWrite(get16Bits(gb->h, gb->l), gb->h);
            break;
        case 0x75:
            printf("[%04X] LD (HL), L\n", gb->pc);
            gb->pc += 1;
            busWrite(get16Bits(gb->h, gb->l), gb->l);
            break;
        case 0x76:
            printf("[%04X] HALT\n", gb->pc);                    // PAS IMPLEMENTER
            gb->pc += 1;
            break;
        case 0x77:
            printf("[%04X] LD (HL), A\n", gb->pc);
            gb->pc += 1;
            busWrite(get16Bits(gb->h, gb->l), gb->a);
            break;
        case 0x78:
            printf("[%04X] LD A, B\n", gb->pc);
            gb->pc += 1;
            ld(&gb->a, gb->b);
            break;
        case 0x79:
            printf("[%04X] LD A, C\n", gb->pc);
            gb->pc += 1;
            ld(&gb->a, gb->c);
            break;
        case 0x7a:
            printf("[%04X] LD A, D\n", gb->pc);
            gb->pc += 1;
            ld(&gb->a, gb->d);
            break;
        case 0x7b:
            printf("[%04X] LD A, E\n", gb->pc);
            gb->pc += 1;
            ld(&gb->a, gb->e);
            break;
        case 0x7c:
            printf("[%04X] LD A, H\n", gb->pc);
            gb->pc += 1;
            ld(&gb->a, gb->h);
            break;
        case 0x7d:
            printf("[%04X] LD A, L\n", gb->pc);
            gb->pc += 1;
            ld(&gb->a, gb->l);
            break;
        case 0x7e:
            printf("[%04X] LD A, (HL)\n", gb->pc);
            gb->pc += 1;
            ld(&gb->a, busRead(get16Bits(gb->h, gb->l)));
            break;
        case 0x7f:
            printf("[%04X] LD A, A\n", gb->pc);
            gb->pc += 1;
            ld(&gb->a, gb->a);
            break;


        case 0x80: 
            printf("[%04X] ADD A, B\n", gb->pc); 
            gb->pc += 1;
            add(gb, gb->b);
            break;
        case 0x81:                         
            printf("[%04X] ADD A, C\n", gb->pc);
            gb->pc += 1;
            add(gb, gb->c);
            break;
        case 0x82:
            printf("[%04X] ADD A, D\n", gb->pc);
            gb->pc += 1;
            add(gb, gb->d);
            break;
        case 0x83:
            printf("[%04X] ADD A, E\n", gb->pc);
            gb->pc += 1;
            add(gb, gb->e);
            break;
        case 0x84:
            printf("[%04X] ADD A, H\n", gb->pc);
            gb->pc += 1;
            add(gb, gb->h);
            break;
        case 0x85:
            printf("[%04X] ADD A, L\n", gb->pc);
            gb->pc += 1;
            add(gb, gb->l);
            break;
        case 0x86:
            printf("[%04X] ADD A, (HL)\n", gb->pc);    
            gb->pc += 1;                
            add(gb, busRead(get16Bits(gb->h,gb->l)));
            break;
        case 0x87:
            printf("[%04X] ADD A, A\n", gb->pc);
            gb->pc += 1;
            add(gb, gb->a);
            break;
        case 0x88:
            printf("[%04X] ADC A, B\n", gb->pc);
            gb->pc += 1;
            adc(gb, gb->b);
            break;
        case 0x89:
            printf("[%04X] ADC A, C\n", gb->pc);
            gb->pc += 1;
            adc(gb, gb->c);
            break;
        case 0x8a:
            printf("[%04X] ADC A, D\n", gb->pc);
            gb->pc += 1;
            adc(gb, gb->d);
            break;
        case 0x8b:
            printf("[%04X] ADC A, E\n", gb->pc);
            gb->pc += 1;
            adc(gb, gb->e);
            break;
        case 0x8c:
            printf("[%04X] ADC A, H\n", gb->pc);
            gb->pc += 1;
            adc(gb, gb->h);
            break;
        case 0x8d:
            printf("[%04X] ADC A, L\n", gb->pc);
            gb->pc += 1;
            adc(gb, gb->l);
            break;
        case 0x8e:
            printf("[%04X] ADC A, (HL)\n", gb->pc);
            gb->pc += 1;
            adc(gb, busRead(get16Bits(gb->h,gb->l)));
            break;
        case 0x8f:
            printf("[%04X] ADC A, A\n", gb->pc);
            gb->pc += 1;
            adc(gb, gb->a);
            break;


        case 0x90: 
            printf("[%04X] SUB A, B\n", gb->pc); 
            gb->pc += 1;
            sub(gb, gb->b);
            break;
        case 0x91:                         
            printf("[%04X] SUB A, C\n", gb->pc);
            gb->pc += 1;
            sub(gb, gb->c);
            break;
        case 0x92:
            printf("[%04X] SUB A, D\n", gb->pc);
            gb->pc += 1;
            sub(gb, gb->d);
            break;
        case 0x93:
            printf("[%04X] SUB A, E\n", gb->pc);
            gb->pc += 1;
            sub(gb, gb->e);
            break;
        case 0x94:
            printf("[%04X] SUB A, H\n", gb->pc);
            gb->pc += 1;
            sub(gb, gb->h);
            break;
        case 0x95:
            printf("[%04X] SUB A, L\n", gb->pc);
            gb->pc += 1;
            sub(gb, gb->l);
            break;
        case 0x96:
            printf("[%04X] SUB A, (HL)\n", gb->pc);    
            gb->pc += 1;                
            sub(gb, busRead(get16Bits(gb->h,gb->l)));
            break;
        case 0x97:
            printf("[%04X] SUB A, A\n", gb->pc);
            gb->pc += 1;
            sub(gb, gb->a);
            break;
        case 0x98:
            printf("[%04X] SBC A, B\n", gb->pc);
            gb->pc += 1;
            sbc(gb, gb->b);
            break;
        case 0x99:
            printf("[%04X] SBC A, C\n", gb->pc);
            gb->pc += 1;
            sbc(gb, gb->c);
            break;
        case 0x9a:
            printf("[%04X] SBC A, D\n", gb->pc);
            gb->pc += 1;
            sbc(gb, gb->d);
            break;
        case 0x9b:
            printf("[%04X] SBC A, E\n", gb->pc);
            gb->pc += 1;
            sbc(gb, gb->e);
            break;
        case 0x9c:
            printf("[%04X] SBC A, H\n", gb->pc);
            gb->pc += 1;
            sbc(gb, gb->h);
            break;
        case 0x9d:
            printf("[%04X] SBC A, L\n", gb->pc);
            gb->pc += 1;
            sbc(gb, gb->l);
            break;
        case 0x9e:
            printf("[%04X] SBC A, (HL)\n", gb->pc);
            gb->pc += 1;
            sbc(gb, busRead(get16Bits(gb->h,gb->l)));
            break;
        case 0x9f:
            printf("[%04X] SBC A, A\n", gb->pc);
            gb->pc += 1;
            sbc(gb, gb->a);
            break;


        case 0xa0: 
            printf("[%04X] AND A, B\n", gb->pc); 
            gb->pc += 1;
            and(gb, gb->b);
            break;
        case 0xa1:                         
            printf("[%04X] AND A, C\n", gb->pc);
            gb->pc += 1;
            and(gb, gb->c);
            break;
        case 0xa2:
            printf("[%04X] AND A, D\n", gb->pc);
            gb->pc += 1;
            and(gb, gb->d);
            break;
        case 0xa3:
            printf("[%04X] AND A, E\n", gb->pc);
            gb->pc += 1;
            and(gb, gb->e);
            break;
        case 0xa4:
            printf("[%04X] AND A, H\n", gb->pc);
            gb->pc += 1;
            and(gb, gb->h);
            break;
        case 0xa5:
            printf("[%04X] AND A, L\n", gb->pc);
            gb->pc += 1;
            and(gb, gb->l);
            break;
        case 0xa6:
            printf("[%04X] AND A, (HL)\n", gb->pc);    
            gb->pc += 1;                
            and(gb, busRead(get16Bits(gb->h,gb->l)));
            break;
        case 0xa7:
            printf("[%04X] AND A, A\n", gb->pc);
            gb->pc += 1;
            and(gb, gb->a);
            break;
        case 0xa8:
            printf("[%04X] XOR A, B\n", gb->pc);
            gb->pc += 1;
            xor(gb, gb->b);
            break;
        case 0xa9:
            printf("[%04X] XOR A, C\n", gb->pc);
            gb->pc += 1;
            xor(gb, gb->c);
            break;
        case 0xaa:
            printf("[%04X] XOR A, D\n", gb->pc);
            gb->pc += 1;
            xor(gb, gb->d);
            break;
        case 0xab:
            printf("[%04X] XOR A, E\n", gb->pc);
            gb->pc += 1;
            xor(gb, gb->e);
            break;
        case 0xac:
            printf("[%04X] XOR A, H\n", gb->pc);
            gb->pc += 1;
            xor(gb, gb->h);
            break;
        case 0xad:
            printf("[%04X] XOR A, L\n", gb->pc);
            gb->pc += 1;
            xor(gb, gb->l);
            break;
        case 0xae:
            printf("[%04X] XOR A, (HL)\n", gb->pc);
            gb->pc += 1;
            printf("%02X", busRead(get16Bits(gb->h,gb->l)));
            xor(gb, busRead(get16Bits(gb->h,gb->l)));
            // exit(1);
            break;
        case 0xaf:
            printf("[%04X] XOR A, A\n", gb->pc);
            gb->pc += 1;
            xor(gb, gb->a);
            break;


        case 0xb0: 
            printf("[%04X] OR A, B\n", gb->pc); 
            gb->pc += 1;
            or(gb, gb->b);
            break;
        case 0xb1:                         
            printf("[%04X] OR A, C\n", gb->pc);
            gb->pc += 1;
            or(gb, gb->c);
            break;
        case 0xb2:
            printf("[%04X] OR A, D\n", gb->pc);
            gb->pc += 1;
            or(gb, gb->d);
            break;
        case 0xb3:
            printf("[%04X] OR A, E\n", gb->pc);
            gb->pc += 1;
            or(gb, gb->e);
            break;
        case 0xb4:
            printf("[%04X] OR A, H\n", gb->pc);
            gb->pc += 1;
            or(gb, gb->h);
            break;
        case 0xb5:
            printf("[%04X] OR A, L\n", gb->pc);
            gb->pc += 1;
            or(gb, gb->l);
            break;
        case 0xb6:
            printf("[%04X] OR A, (HL)\n", gb->pc);   
            gb->pc += 1;                 
            or(gb, busRead(get16Bits(gb->h,gb->l)));
            break;
        case 0xb7:
            printf("[%04X] OR A, A\n", gb->pc);
            gb->pc += 1;
            or(gb, gb->a);
            break;
        case 0xb8:
            printf("[%04X] CP B\n", gb->pc);
            gb->pc += 1;
            cp(gb, gb->b);
            break;
        case 0xb9:
            printf("[%04X] CP C\n", gb->pc);
            gb->pc += 1;
            cp(gb, gb->c);
            break;
        case 0xba:
            printf("[%04X] CP D\n", gb->pc);
            gb->pc += 1;
            cp(gb, gb->d);
            break;
        case 0xbb:
            printf("[%04X] CP E\n", gb->pc);
            gb->pc += 1;
            cp(gb, gb->e);
            break;
        case 0xbc:
            printf("[%04X] CP H\n", gb->pc);
            gb->pc += 1;
            cp(gb, gb->h);
            break;
        case 0xbd:
            printf("[%04X] CP L\n", gb->pc);
            gb->pc += 1;
            cp(gb, gb->l);
            break;
        case 0xbe:
            printf("[%04X] CP (HL)\n", gb->pc);
            gb->pc += 1;
            cp(gb, busRead(get16Bits(gb->h,gb->l)));
            break;
        case 0xbf:
            printf("[%04X] CP A\n", gb->pc);
            gb->pc += 1;
            cp(gb, gb->a);
            break;


        case 0xc0: 
            printf("[%04X] RET NZ\n", gb->pc);
            gb->pc += 1; 
            retNZ(gb);
            break;
        case 0xc1:                         
            printf("[%04X] POP BC\n", gb->pc);
            gb->pc += 1;
            pop(gb, &gb->b, &gb->c);
            break;
        case 0xc2:
            printf("[%04X] JP NZ, $%02x%02x\n", gb->pc, opcode2, opcode1);
            gb->pc += 1;
            jpNZ(gb, get16Bits(opcode2, opcode1));
            break;
        case 0xc3:                         // JP a16 (test OK)
            printf("[%04X] JP $%02x%02x\n", gb->pc, opcode2, opcode1);
            gb->pc += 1;
            jp(gb, get16Bits(opcode2, opcode1));
            break; 
        case 0xc4:
            printf("[%04X] CALL NZ, $%02x%02x\n", gb->pc, opcode2, opcode1);
            gb->pc += 1;
            callNZ(gb, get16Bits(opcode2, opcode1));
            break;
        case 0xc5:
            printf("[%04X] PUSH BC\n", gb->pc);
            gb->pc += 1;
            push(gb, gb->b, gb->c);
            break;
        case 0xc6:
            printf("[%04X] ADD A, %02x\n", gb->pc, opcode1);   
            gb->pc += 1;                 
            add(gb, opcode1);
            gb->pc += 1;
            break;
        case 0xc7:
            printf("[%04X] RST 00H\n", gb->pc); // Pas implementer
            gb->pc += 1;
            break;
        case 0xc8:
            printf("[%04X] RET Z\n", gb->pc);
            gb->pc += 1;
            retZ(gb);
            break;
        case 0xc9:
            printf("[%04X] RET\n", gb->pc);
            gb->pc += 1;
            ret(gb);
            break;
        case 0xca:
            printf("[%04X] JP Z, $%02x%02x\n", gb->pc, opcode2, opcode1);
            gb->pc += 1;
            jpZ(gb, get16Bits(opcode2, opcode1));
            break;
        case 0xcb:
            printf("[%04X] PREFIX CB\n", gb->pc); // Pas implementer
            gb->pc += 1;
            prefix_cb(gb, opcode1);
            gb->pc += 1;
            break;
        case 0xcc:
            printf("[%04X] CALL Z, $%02x%02x\n", gb->pc, opcode2, opcode1);
            gb->pc += 1;
            callZ(gb, get16Bits(opcode2, opcode1));
            break;
        case 0xcd:
            printf("[%04X] CALL, $%02x%02x\n", gb->pc, opcode2, opcode1);
            gb->pc += 1;
            call(gb, get16Bits(opcode2, opcode1));
            break;
        case 0xce:
            printf("[%04X] ADC A, 0x%02x\n", gb->pc, opcode1);
            gb->pc += 1;
            adc(gb, opcode1);
            gb->pc += 1;
            break;
        case 0xcf:
            printf("[%04X] RST 08H\n", gb->pc);         // Pas implementer 
            gb->pc += 1;
            break;



        case 0xd0: 
            printf("[%04X] RET NC\n", gb->pc); 
            gb->pc += 1;
            retNC(gb);
            break;
        case 0xd1:                         
            printf("[%04X] POP DE\n", gb->pc);
            gb->pc += 1;
            pop(gb, &gb->d, &gb->e);
            break;
        case 0xd2:
            printf("[%04X] JP NC, $%02x%02x\n", gb->pc, opcode2, opcode1);
            gb->pc += 1;
            jpNC(gb, get16Bits(opcode2, opcode1));
            break;
        case 0xd3:               // AUCUNE INSTRUCTION
            break; 
        case 0xd4:
            printf("[%04X] CALL NC, $%02x%02x\n", gb->pc, opcode2, opcode1);
            gb->pc += 1;
            callNC(gb, get16Bits(opcode2, opcode1));
            break;
        case 0xd5:
            printf("[%04X] PUSH DE\n", gb->pc);
            gb->pc += 1;
            push(gb, gb->d, gb->e);
            break;
        case 0xd6:
            printf("[%04X] SUB A, 0x%02x\n", gb->pc, opcode1);    
            gb->pc += 1;                
            sub(gb, opcode1);
            gb->pc += 1;
            break;
        case 0xd7:
            printf("[%04X] RST 10H\n", gb->pc); // Pas implementer
            gb->pc += 1;
            break;
        case 0xd8:
            printf("[%04X] RET C\n", gb->pc);
            gb->pc += 1;
            retC(gb);
            break;
        case 0xd9:
            printf("[%04X] RETI\n", gb->pc); // Pas implementer
            gb->pc += 1;
            break;
        case 0xda:
            printf("[%04X] JP C, $%02x%02x\n", gb->pc, opcode2, opcode1);
            gb->pc += 1;
            jpC(gb, get16Bits(opcode2, opcode1));
            break;
        case 0xdb:                                                  // Pas D'instruction
            break;
        case 0xdc:
            printf("[%04X] CALL C, $%02x%02x\n", gb->pc, opcode2, opcode1);
            gb->pc += 1;
            callC(gb, get16Bits(opcode2, opcode1));
            break;
        case 0xdd:
            break;
        case 0xde:
            printf("[%04X] SBC A, %02x\n", gb->pc, opcode1);
            gb->pc += 1;
            sbc(gb, opcode1);
            gb->pc += 1;
            break;
        case 0xdf:
            printf("[%04X] RST 18H\n", gb->pc);         // Pas implementer
            gb->pc += 1;
            break;



        case 0xe0: 
            printf("[%04X] LDH (%02x), A\n", gb->pc, opcode1); 
            gb->pc += 1;
            busWrite(0xFF00+opcode1, gb->a);
            gb->pc += 1;
            break;
        case 0xe1:                         
            printf("[%04X] POP HL\n", gb->pc);
            gb->pc += 1;
            pop(gb, &gb->h, &gb->l);
            break;
        case 0xe2:
            printf("[%04X] LD (C), A\n", gb->pc);
            gb->pc += 1;
            busWrite(0xFF00+gb->c, gb->a);
            break;
        case 0xe3:               // AUCUNE INSTRUCTION
            break; 
        case 0xe4:
            break;
        case 0xe5:
            printf("[%04X] PUSH HL\n", gb->pc);
            gb->pc += 1;
            push(gb, gb->h, gb->l);
            break;
        case 0xe6:
            printf("[%04X] AND A, 0x%02x\n", gb->pc, opcode1);   
            gb->pc += 1;                 
            and(gb, opcode1);
            gb->pc += 1;
            break;
        case 0xe7:
            printf("[%04X] RST 20H\n", gb->pc); // Pas implementer
            gb->pc += 1;
            break;
        case 0xe8:
            printf("[%04X] ADD SP, 0x%02x\n", gb->pc, opcode1);
            gb->pc += 1;
            break;
        case 0xe9:
            printf("[%04X] JP (HL)\n", gb->pc); 
            gb->pc += 1;
            jp(gb, get16Bits(gb->h, gb->l));
            break;
        case 0xea:
            printf("[%04X] LD $%02x%02x, A\n", gb->pc, opcode2, opcode1);
            gb->pc += 1;
            busWrite(get16Bits(opcode2,opcode1), gb->a);
            gb->pc += 2;
            break;
        case 0xeb:                                                  // Pas D'instruction
            break;
        case 0xec:
            break;
        case 0xed:
            break;
        case 0xee:
            printf("[%04X] XOR A, 0x%02x\n", gb->pc, opcode1); 
            gb->pc += 1;
            xor(gb, opcode1);
            gb->pc += 1;
            break;
        case 0xef:
            printf("[%04X] RST 28H\n", gb->pc);         // Pas implementer
            break;



        case 0xf0: 
            printf("[%04X] LDH A, (%02x)\n", gb->pc, opcode1); 
            gb->pc += 1;
            gb->a = busRead(0xFF00+opcode1);
            gb->pc += 1;
            break;
        case 0xf1:                         
            printf("[%04X] POP AF\n", gb->pc);
            gb->pc += 1;
            pop_af(gb, &gb->a, &gb->f);
            break;
        case 0xf2:
            printf("[%04X] LD A, (C)\n", gb->pc);
            gb->pc += 1;
            gb->a = busRead(0xFF00+gb->c);
            break;
        case 0xf3:               
            printf("[%04X] DI\n", gb->pc);
            gb->pc += 1;
            break; 
        case 0xf4:
            break;
        case 0xf5:
            printf("[%04X] PUSH AF\n", gb->pc);
            gb->pc += 1;
            push(gb, gb->a, gb->f);
            break;
        case 0xf6:
            printf("[%04X] OR A, 0x%02x\n", gb->pc, opcode1);     
            gb->pc += 1;               
            or(gb, opcode1);
            gb->pc += 1;
            break;
        case 0xf7:
            printf("[%04X] RST 30H\n", gb->pc); // Pas implementer
            gb->pc += 1;
            break;
        case 0xf8:
            printf("[%04X] LD HL, SP+0x%02x\n", gb->pc, opcode1);
            gb->pc += 1;
            break;
        case 0xf9:
            printf("[%04X] LD SP, HL\n", gb->pc); 
            gb->pc += 1;
            gb->sp = get16Bits(gb->h, gb->l);
            break;
        case 0xfa:
            printf("[%04X] LD A, $%02x%02x\n", gb->pc, opcode2, opcode1);
            gb->pc += 1;
            ld(&gb->a, busRead(get16Bits(opcode2,opcode1)));
            gb->pc += 2;
            break;
        case 0xfb:                                                  // Pas D'instruction
            printf("[%04X] EI\n", gb->pc);
            gb->pc += 1; 
            break;
        case 0xfc:
            break;
        case 0xfd:
            break;
        case 0xfe:
            printf("[%04X] CP A, 0x%02x\n", gb->pc, opcode1); 
            gb->pc += 1;
            cp(gb, opcode1);
            gb->pc += 1;
            break;
        case 0xff:
            printf("[%04X] RST 38H\n", gb->pc);         // Pas implementer
            gb->pc += 1;
            break;
    }
    
    gb->f = (gb->cc.z << 7) + (gb->cc.n << 6) + (gb->cc.h << 5) + (gb->cc.c << 4);
    printf("A=%02X, F=%02X, B=%02X, C=%02X, D=%02X, E=%02X, H=%02X, L=%02X, SP=%04X\n\n", gb->a, gb->f,gb->b, 
                                                                    gb->c, gb->d, gb->e, gb->h, gb->l, gb->sp);

}