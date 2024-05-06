#include <instructions.h>
#include <bus.h>

void ld16(uint8_t *destHo, uint8_t *destLo, uint8_t srcHo, uint8_t srcLo)
{
    *destHo = srcHo;
    *destLo = srcLo;
}

void ld(uint8_t *dest, uint8_t src)
{
    *dest = src;
}




void inc16(uint8_t *regHi, uint8_t *regLo)
{
    *regLo = *regLo + 1;
    if (*regLo == 0)
    {
        *regHi = *regHi + 1;
    }
}

void inc(Cpu *gb,uint8_t *reg)
{
    uint8_t calc = *reg + 1;
    if (calc == 0)
        gb->cc.z = 1;
    else
        gb->cc.z = 0;
    if (((1&0x0f) + (*reg&0x0f))&0x10)
        gb->cc.h = 1;
    else
        gb->cc.h = 0;
    gb->cc.n = 0;
    *reg = calc;
}

void dec(Cpu *gb,uint8_t *reg)
{
    uint8_t calc = *reg - 1;
    if (calc == 0)
        gb->cc.z = 1;
    else
        gb->cc.z = 0;

    if (((*reg&0x0f) - (1&0x0f))&0x10)
        gb->cc.h = 1;
    else
        gb->cc.h = 0;
    gb->cc.n = 1;
    *reg = calc;
}

void dec16(uint8_t *regHi, uint8_t *regLo)
{
    *regLo = *regLo - 1;
    if (*regLo == 0)
    {
        *regHi = *regHi -1;
    }
}

void add16(Cpu *gb, uint8_t *regDestHi, uint8_t *regDestLo, uint16_t regSrc)
{
    uint16_t regDest = (*regDestHi << 8) + *regDestLo;
    uint32_t res = regDest + regSrc;

    gb->cc.h = ((regDest & 0xFFF) + (regSrc & 0xFFF)) >> 12;

    if (res > 0xFFFF)
        gb->cc.c = 1;
    else
        gb->cc.c = 0;

    gb->cc.n = 0;
    
    gb->h = (res & 0x00FF00) >> 8;
    gb->l = (uint8_t)(res & 0x0000FF);
}


void jr(Cpu *gb, uint8_t value)
{
    if (value > 128)
    {
        uint8_t value_signed = 0xFF - (value-1);
        gb->pc -= value_signed;  
    }
    else
    {
        gb->pc += value;
    }
}

void jrC(Cpu *gb, uint8_t value)
{
    if (gb->cc.c == 1)
    {
        jr(gb, value);
    }
}

void jrNC(Cpu *gb, uint8_t value)
{
    if (gb->cc.c == 0)
    {
        jr(gb, value);
    }
}

void jrZ(Cpu *gb, uint8_t value)
{
    if (gb->cc.z == 1)
    {
        jr(gb, value);
    }
}

void jrNZ(Cpu *gb, uint8_t value)
{
    if (gb->cc.z == 0)
    {
        jr(gb, value);
    }
}



/*void jp(Cpu *gb, uint16_t value)
{
    gb->pc = value;
}
*/

void and(Cpu *gb, uint8_t reg)
{
    uint8_t res = gb->a & reg;
    if (res == 0)
        gb->cc.z = 1;
    else
        gb->cc.z = 0;
    gb->cc.c = 0;
    gb->cc.n = 0;
    gb->cc.h = 1;
    gb->a = res;
}

void or(Cpu *gb, uint8_t reg)
{
    uint8_t res = gb->a | reg;
    if (res == 0)
        gb->cc.z = 1;
    else
        gb->cc.z = 0;
    gb->cc.c = 0;
    gb->cc.n = 0;
    gb->cc.h = 0;
    gb->a = res;
}

void xor(Cpu *gb, uint8_t reg)
{
    uint8_t res = gb->a ^ reg;
    if (res == 0)
        gb->cc.z = 1;
    else
        gb->cc.z = 0;
    gb->cc.c = 0;
    gb->cc.n = 0;
    gb->cc.h = 0;
    gb->a = res;
}

// Mettre en place le h flag
void sbc(Cpu *gb, uint8_t reg)
{
    uint16_t res = gb->a - reg - gb->cc.c;
    //uint8_t signA = gb->a & 0x80;
    //uint8_t signReg = reg & 0x80;
    
    gb->cc.n = 1;

    if (res == 0)
        gb->cc.z = 1;
    else
        gb->cc.z = 0;

    if ((res & 0x100) == 0x100)
        gb->cc.c = 1;
    else 
        gb->cc.c = 0;
    
    if (((gb->a&0x0f) - (reg&0x0f))&0x10)
        gb->cc.h = 1;
    else
        gb->cc.h = 0;

    if ((res & 0x10) == 1)
        gb->cc.h = 1;
	else
        gb->cc.h = 0;

    gb->a = res & 0xFF; 
}

// Mettre en place le h flag
void adc(Cpu *gb, uint8_t reg)
{
    uint16_t res = gb->a + reg + gb->cc.c;
    //uint8_t signA = gb->a & 0x80;
    //uint8_t signReg = reg & 0x80;

    gb->cc.n = 0;

    if ((res & 0xff) == 0)
        gb->cc.z = 1;
    else
        gb->cc.z = 0;

    if ((res & 0x100) == 0x100)
        gb->cc.c = 1;
    else 
        gb->cc.c = 0;

    if (((gb->a&0x0f) + (reg&0x0f) + gb->cc.c)&0x10)
        gb->cc.h = 1;
    else
        gb->cc.h = 0;

    gb->a = res & 0xFF;    
}

// Mettre en place le h flag
void sub(Cpu *gb, uint8_t reg)
{
    uint16_t res = gb->a - reg;
    //uint8_t signA = gb->a & 0x80;
    //uint8_t signReg = reg & 0x80;

    gb->cc.n = 1;

    if ((res & 0xFF) == 0)
        gb->cc.z = 1;
    else
        gb->cc.z = 0;

    if ((res & 0x100) == 0x100)
        gb->cc.c = 1;
    else 
        gb->cc.c = 0;

    if (((gb->a&0x0f) - (reg&0x0f))&0x10)
        gb->cc.h = 1;
    else
        gb->cc.h = 0;

    gb->a = res & 0xFF;
}


void add(Cpu *gb, uint8_t reg)
{
    uint16_t res = gb->a + reg;

    gb->cc.n = 0;

    if ((res & 0xFF) == 0x00)
        gb->cc.z = 1;
    else
        gb->cc.z = 0;

    if ((res & 0x100) == 0x100)
        gb->cc.c = 1;
    else 
        gb->cc.c = 0;

    if (((gb->a&0x0f) + (reg&0x0f))&0x10)
        gb->cc.h = 1;
    else
        gb->cc.h = 0;

    gb->a = res & 0xFF;
}

void cp(Cpu *gb, uint8_t reg)
{
    uint8_t x = gb->a - reg;
	gb->cc.z = (x == 0);
    gb->cc.n = 1;
    if (((gb->a&0x0f) - (reg&0x0f))&0x10)
        gb->cc.h = 1;
    else
        gb->cc.h = 0;
    gb->cc.c = (gb->a < reg);
}


void push(Cpu *gb, uint8_t reg1, uint8_t reg2)
{
    busWrite(gb->sp-1, reg1);
    busWrite(gb->sp-2, reg2);
	gb->sp -= 2;
}

void pop(Cpu *gb, uint8_t *reg1, uint8_t *reg2)
{
    *reg2 = busRead(gb->sp);
	*reg1 = busRead(gb->sp+1);
	gb->sp += 2;
}

void pop_af(Cpu *gb, uint8_t *reg1, uint8_t *reg2)
{
    uint8_t flags = busRead(gb->sp);
    gb->cc.z = (flags & 0x80) >> 7; 
    gb->cc.n = (flags & 0x40) >> 6; 
    gb->cc.h = (flags & 0x20) >> 5;
    gb->cc.c = (flags & 0x10) >> 4; 
	*reg1 = busRead(gb->sp+1);
	gb->sp += 2;
}

void ret(Cpu *gb)
{
    //uint16_t test = ((busRead(gb->sp+1) << 8) | busRead(gb->sp));
    //printf("%04X\n", test);
    gb->pc = ((busRead(gb->sp+1) << 8) | busRead(gb->sp));
    //printf("%04X", gb->pc);
    gb->sp += 2;
    //exit(1);
}

void retZ(Cpu *gb)
{
    if (gb->cc.z == 1)
    {
        ret(gb);
    }
}

void retNZ(Cpu *gb)
{
    if (gb->cc.z == 0)
    {
        ret(gb);
    }
}

void retC(Cpu *gb)
{
    if (gb->cc.c == 1)
    {
        ret(gb);
    }
}

void retNC(Cpu *gb)
{
    if (gb->cc.c == 0)
    {
        ret(gb);
    }
}



void call(Cpu *gb, uint16_t addr)
{
    // uint16_t ret = 0;
    /*if ((gb->pc & 0x000F) == 0x000F)
    {
        ret = gb->pc+1;
    }
    else
    {*/
    uint16_t ret = gb->pc+2;
    //}
    busWrite(gb->sp-1, (ret >> 8) & 0x00ff);
    busWrite(gb->sp-2, (ret & 0x00ff));
	gb->sp = gb->sp - 2;
	gb->pc = addr;
}

void callZ(Cpu *gb, uint16_t addr)
{
    if (gb->cc.z == 1)
    {
        call(gb, addr);
    }
    else
    {
        gb->pc +=2;
    }
}

void callNZ(Cpu *gb, uint16_t addr)
{
    if (gb->cc.z == 0)
    {
        call(gb, addr);
    }
    else
    {
        gb->pc +=2;
    }
}

void callC(Cpu *gb, uint16_t addr)
{
    if (gb->cc.c == 1)
    {
        call(gb, addr);
    }
    else
    {
        gb->pc +=2;
    }
}

void callNC(Cpu *gb, uint16_t addr)
{
    if (gb->cc.c == 0)
    {
        call(gb, addr);
    }
    else
    {
        gb->pc +=2;
    }
}


void jp(Cpu *gb, uint16_t value)
{
    gb->pc = value;
}

void jpZ(Cpu *gb, uint16_t value)
{
    if (gb->cc.z == 1)
    {
        jp(gb, value);
    }
    else
    {
        gb->pc +=2;
    }
}

void jpNZ(Cpu *gb, uint16_t value)
{
    if (gb->cc.z == 0)
    {
        jp(gb, value);
    }
    else
    {
        gb->pc +=2;
    }
}

void jpC(Cpu *gb, uint16_t value)
{
    if (gb->cc.c == 1)
    {
        jp(gb, value);
    }
    else
    {
        gb->pc +=2;
    }
}

void jpNC(Cpu *gb, uint16_t value)
{
    if (gb->cc.c == 0)
    {
        jp(gb, value);
    }
    else
    {
        gb->pc +=2;
    }
}


void bit(Cpu *gb, uint8_t reg, uint8_t b)
{
    uint8_t mask = 0x01 << b;
    uint8_t calc = mask & reg;
    if (calc == 0)
    {
        gb->cc.z = 1;
    }
    else
    {
        gb->cc.z = 0;
    }
    gb->cc.h = 1;
    gb->cc.n = 0;
}

void set(uint8_t *reg, uint8_t b)
{
    uint8_t mask = 0x01 << b;
    *reg |= mask;
}

void res(uint8_t *reg, uint8_t b)
{
    uint8_t mask = 0x01 << b;
    *reg ^= mask;
}

void rlc(Cpu *gb, uint8_t *reg)
{
    uint16_t res = *reg << 1;
    gb->cc.c = (res & 0xF00) >> 8;
    res |= gb->cc.c;

    if ((res & 0xFF) == 0)
    {
        gb->cc.z = 1;
    }
    else
    {
        gb->cc.z = 0;
    }
    gb->cc.h = 0;
    gb->cc.n = 0;

    *reg = res & 0xFF; 
}

void rl(Cpu *gb, uint8_t *reg)
{
    uint16_t res = *reg << 1;
    gb->cc.c = (res & 0xF00) >> 8;

    if ((res & 0xFF) == 0)
    {
        gb->cc.z = 1;
    }
    else
    {
        gb->cc.z = 0;
    }
    gb->cc.h = 0;
    gb->cc.n = 0;
    *reg = res & 0xFF; 
}

void rrc(Cpu *gb, uint8_t *reg)
{
    gb->cc.c = *reg & 0x01;
    uint8_t res = (*reg >> 1) | (gb->cc.c << 7);

    if (res == 0)
    {
        gb->cc.z = 1;
    }
    else
    {
        gb->cc.z = 0;
    }
    gb->cc.h = 0;
    gb->cc.n = 0;
    *reg = res; 
}

void rr(Cpu *gb, uint8_t *reg)
{
    uint8_t carry = *reg & 0x01;
    uint8_t res = (*reg >> 1) | (gb->cc.c << 7);
    gb->cc.c = carry;
    if (res == 0)
    {
        gb->cc.z = 1;
    }
    else
    {
        gb->cc.z = 0;
    }
    gb->cc.h = 0;
    gb->cc.n = 0;
    *reg = res; 
}

void sla(Cpu *gb, uint8_t *reg)
{
    uint16_t res = *reg << 1;
    gb->cc.c = (res & 0xF00) >> 8;

    if ((res & 0xFF) == 0)
    {
        gb->cc.z = 1;
    }
    else
    {
        gb->cc.z = 0;
    }
    gb->cc.h = 0;
    gb->cc.n = 0;
    *reg = res & 0xff;
}

void sra(Cpu *gb, uint8_t *reg)
{
    gb->cc.c = *reg & 0x01;
    uint8_t bit7 = *reg & 0x80;
    uint8_t res = (*reg >> 1) | bit7;

    if (res == 0)
    {
        gb->cc.z = 1;
    }
    else
    {
        gb->cc.z = 0;
    }
    gb->cc.h = 0;
    gb->cc.n = 0;
    *reg = res;
}

void srl(Cpu *gb, uint8_t *reg)
{
    gb->cc.c = *reg & 0x01;
    uint8_t res = *reg >> 1;

    if (res == 0)
    {
        gb->cc.z = 1;
    }
    else
    {
        gb->cc.z = 0;
    }
    gb->cc.h = 0;
    gb->cc.n = 0;
    *reg = res;
}

void swap(Cpu *gb, uint8_t *reg)
{
    uint8_t ho = (*reg & 0xF0) >> 4;
    uint8_t lo = (*reg & 0x0F) << 4;
    uint8_t res = lo | ho;
    if (res == 0)
    {
        gb->cc.z = 1;
    }
    else
    {
        gb->cc.z = 0;
    }
    gb->cc.c = 0;
    gb->cc.h = 0;
    gb->cc.n = 0;
}


void prefix_cb(Cpu *gb, uint8_t opcode1)
{
    switch (opcode1)
    {
        case 0x00: rlc(gb, &gb->b); break;
        case 0x01: rlc(gb, &gb->c); break;
        case 0x02: rlc(gb, &gb->d); break;
        case 0x03: rlc(gb, &gb->e); break;
        case 0x04: rlc(gb, &gb->h); break;
        case 0x05: rlc(gb, &gb->l); break;
        case 0x06:  break;
        case 0x07: rlc(gb, &gb->a); break;
        case 0x08: rrc(gb, &gb->b); break;
        case 0x09: rrc(gb, &gb->c); break;
        case 0x0a: rrc(gb, &gb->d); break;
        case 0x0b: rrc(gb, &gb->e); break;
        case 0x0c: rrc(gb, &gb->h); break;
        case 0x0d: rrc(gb, &gb->l); break;
        case 0x0e:  break;
        case 0x0f: rrc(gb, &gb->a); break;


        case 0x10: rl(gb, &gb->b); break;
        case 0x11: rl(gb, &gb->c); break;
        case 0x12: rl(gb, &gb->d); break;
        case 0x13: rl(gb, &gb->e); break;
        case 0x14: rl(gb, &gb->h); break;
        case 0x15: rl(gb, &gb->l); break;
        case 0x16:  break;
        case 0x17: rl(gb, &gb->a); break;
        case 0x18: rr(gb, &gb->b); break;
        case 0x19: rr(gb, &gb->c); break;
        case 0x1a: rr(gb, &gb->d); break;
        case 0x1b: rr(gb, &gb->e); break;
        case 0x1c: rr(gb, &gb->h); break;
        case 0x1d: rr(gb, &gb->l); break;
        case 0x1e:  break;
        case 0x1f: rr(gb, &gb->a); break;


        case 0x20: sla(gb, &gb->b); break;
        case 0x21: sla(gb, &gb->c); break;
        case 0x22: sla(gb, &gb->d); break;
        case 0x23: sla(gb, &gb->e); break;
        case 0x24: sla(gb, &gb->h); break;
        case 0x25: sla(gb, &gb->l); break;
        case 0x26:  break;
        case 0x27: sla(gb, &gb->a); break;
        case 0x28: sra(gb, &gb->b); break;
        case 0x29: sra(gb, &gb->c); break;
        case 0x2a: sra(gb, &gb->d); break;
        case 0x2b: sra(gb, &gb->e); break;
        case 0x2c: sra(gb, &gb->h); break;
        case 0x2d: sra(gb, &gb->l); break;
        case 0x2e:  break;
        case 0x2f: sra(gb, &gb->a); break;


        case 0x30: swap(gb, &gb->b); break;
        case 0x31: swap(gb, &gb->c); break;
        case 0x32: swap(gb, &gb->d); break;
        case 0x33: swap(gb, &gb->e); break;
        case 0x34: swap(gb, &gb->h); break;
        case 0x35: swap(gb, &gb->l); break;
        case 0x36:  break;
        case 0x37: swap(gb, &gb->a); break;
        case 0x38: srl(gb, &gb->b); break;
        case 0x39: srl(gb, &gb->c); break;
        case 0x3a: srl(gb, &gb->d); break;
        case 0x3b: srl(gb, &gb->e); break;
        case 0x3c: srl(gb, &gb->h); break;
        case 0x3d: srl(gb, &gb->l); break;
        case 0x3e:  break;
        case 0x3f: srl(gb, &gb->a); break;


        case 0x40: bit(gb, gb->b, 0); break;
        case 0x41: bit(gb, gb->c, 0); break;
        case 0x42: bit(gb, gb->d, 0); break;
        case 0x43: bit(gb, gb->e, 0); break;
        case 0x44: bit(gb, gb->h, 0); break;
        case 0x45: bit(gb, gb->l, 0); break;
        case 0x46:  break;
        case 0x47: bit(gb, gb->a, 0); break;
        case 0x48: bit(gb, gb->b, 1); break;
        case 0x49: bit(gb, gb->c, 1); break;
        case 0x4a: bit(gb, gb->d, 1); break;
        case 0x4b: bit(gb, gb->e, 1); break;
        case 0x4c: bit(gb, gb->h, 1); break;
        case 0x4d: bit(gb, gb->l, 1); break;
        case 0x4e:  break;
        case 0x4f: bit(gb, gb->a, 1); break;


        case 0x50: bit(gb, gb->b, 2); break;
        case 0x51: bit(gb, gb->c, 2); break;
        case 0x52: bit(gb, gb->d, 2); break;
        case 0x53: bit(gb, gb->e, 2); break;
        case 0x54: bit(gb, gb->h, 2); break;
        case 0x55: bit(gb, gb->l, 2); break;
        case 0x56:  break;
        case 0x57: bit(gb, gb->a, 2); break;
        case 0x58: bit(gb, gb->b, 3); break;
        case 0x59: bit(gb, gb->c, 3); break;
        case 0x5a: bit(gb, gb->d, 3); break;
        case 0x5b: bit(gb, gb->e, 3); break;
        case 0x5c: bit(gb, gb->h, 3); break;
        case 0x5d: bit(gb, gb->l, 3); break;
        case 0x5e:  break;
        case 0x5f: bit(gb, gb->a, 3); break;


        case 0x60: bit(gb, gb->b, 4); break;
        case 0x61: bit(gb, gb->c, 4); break;
        case 0x62: bit(gb, gb->d, 4); break;
        case 0x63: bit(gb, gb->e, 4); break;
        case 0x64: bit(gb, gb->h, 4); break;
        case 0x65: bit(gb, gb->l, 4); break;
        case 0x66:  break;
        case 0x67: bit(gb, gb->a, 4); break;
        case 0x68: bit(gb, gb->b, 5); break;
        case 0x69: bit(gb, gb->c, 5); break;
        case 0x6a: bit(gb, gb->d, 5); break;
        case 0x6b: bit(gb, gb->e, 5); break;
        case 0x6c: bit(gb, gb->h, 5); break;
        case 0x6d: bit(gb, gb->l, 5); break;
        case 0x6e:  break;
        case 0x6f: bit(gb, gb->a, 5); break;


        case 0x70: bit(gb, gb->b, 6); break;
        case 0x71: bit(gb, gb->c, 6); break;
        case 0x72: bit(gb, gb->d, 6); break;
        case 0x73: bit(gb, gb->e, 6); break;
        case 0x74: bit(gb, gb->h, 6); break;
        case 0x75: bit(gb, gb->l, 6); break;
        case 0x76:  break;
        case 0x77: bit(gb, gb->a, 6); break;
        case 0x78: bit(gb, gb->b, 7); break;
        case 0x79: bit(gb, gb->c, 7); break;
        case 0x7a: bit(gb, gb->d, 7); break;
        case 0x7b: bit(gb, gb->e, 7); break;
        case 0x7c: bit(gb, gb->h, 7); break;
        case 0x7d: bit(gb, gb->l, 7); break;
        case 0x7e:  break;
        case 0x7f: bit(gb, gb->a, 7); break;


        case 0x80: res(&gb->b, 0); break;
        case 0x81: res(&gb->c, 0); break;
        case 0x82: res(&gb->d, 0); break;
        case 0x83: res(&gb->e, 0); break;
        case 0x84: res(&gb->h, 0); break;
        case 0x85: res(&gb->l, 0); break;
        case 0x86:  break;
        case 0x87: res(&gb->a, 0); break;
        case 0x88: res(&gb->b, 1); break;
        case 0x89: res(&gb->c, 1); break;
        case 0x8a: res(&gb->d, 1); break;
        case 0x8b: res(&gb->e, 1); break;
        case 0x8c: res(&gb->h, 1); break;
        case 0x8d: res(&gb->l, 1); break;
        case 0x8e:  break;
        case 0x8f: res(&gb->a, 1); break;


        case 0x90: res(&gb->b, 2); break;
        case 0x91: res(&gb->c, 2); break;
        case 0x92: res(&gb->d, 2); break;
        case 0x93: res(&gb->e, 2); break;
        case 0x94: res(&gb->h, 2); break;
        case 0x95: res(&gb->l, 2); break;
        case 0x96:  break;
        case 0x97: res(&gb->a, 2); break;
        case 0x98: res(&gb->b, 3); break;
        case 0x99: res(&gb->c, 3); break;
        case 0x9a: res(&gb->d, 3); break;
        case 0x9b: res(&gb->e, 3); break;
        case 0x9c: res(&gb->h, 3); break;
        case 0x9d: res(&gb->l, 3); break;
        case 0x9e:  break;
        case 0x9f: res(&gb->a, 3); break;


        case 0xa0: res(&gb->b, 4); break;
        case 0xa1: res(&gb->c, 4); break;
        case 0xa2: res(&gb->d, 4); break;
        case 0xa3: res(&gb->e, 4); break;
        case 0xa4: res(&gb->h, 4); break;
        case 0xa5: res(&gb->l, 4); break;
        case 0xa6:  break;
        case 0xa7: res(&gb->a, 4); break;
        case 0xa8: res(&gb->b, 5); break;
        case 0xa9: res(&gb->c, 5); break;
        case 0xaa: res(&gb->d, 5); break;
        case 0xab: res(&gb->e, 5); break;
        case 0xac: res(&gb->h, 5); break;
        case 0xad: res(&gb->l, 5); break;
        case 0xae:  break;
        case 0xaf: res(&gb->a, 5); break;


        case 0xb0: res(&gb->b, 6); break;
        case 0xb1: res(&gb->c, 6); break;
        case 0xb2: res(&gb->d, 6); break;
        case 0xb3: res(&gb->e, 6); break;
        case 0xb4: res(&gb->h, 6); break;
        case 0xb5: res(&gb->l, 6); break;
        case 0xb6:  break;
        case 0xb7: res(&gb->a, 6); break;
        case 0xb8: res(&gb->b, 7); break;
        case 0xb9: res(&gb->c, 7); break;
        case 0xba: res(&gb->d, 7); break;
        case 0xbb: res(&gb->e, 7); break;
        case 0xbc: res(&gb->h, 7); break;
        case 0xbd: res(&gb->l, 7); break;
        case 0xbe:  break;
        case 0xbf: res(&gb->a, 7); break;


        case 0xc0: set(&gb->b, 0); break;
        case 0xc1: set(&gb->c, 0); break;
        case 0xc2: set(&gb->d, 0); break;
        case 0xc3: set(&gb->e, 0); break;
        case 0xc4: set(&gb->h, 0); break;
        case 0xc5: set(&gb->l, 0); break;
        case 0xc6:  break;
        case 0xc7: set(&gb->a, 0); break;
        case 0xc8: set(&gb->b, 1); break;
        case 0xc9: set(&gb->c, 1); break;
        case 0xca: set(&gb->d, 1); break;
        case 0xcb: set(&gb->e, 1); break;
        case 0xcc: set(&gb->h, 1); break;
        case 0xcd: set(&gb->l, 1); break;
        case 0xce:  break;
        case 0xcf: set(&gb->a, 1); break;


        case 0xd0: set(&gb->b, 2); break;
        case 0xd1: set(&gb->c, 2); break;
        case 0xd2: set(&gb->d, 2); break;
        case 0xd3: set(&gb->e, 2); break;
        case 0xd4: set(&gb->h, 2); break;
        case 0xd5: set(&gb->l, 2); break;
        case 0xd6:  break;
        case 0xd7: set(&gb->a, 2); break;
        case 0xd8: set(&gb->b, 3); break;
        case 0xd9: set(&gb->c, 3); break;
        case 0xda: set(&gb->d, 3); break;
        case 0xdb: set(&gb->e, 3); break;
        case 0xdc: set(&gb->h, 3); break;
        case 0xdd: set(&gb->l, 3); break;
        case 0xde:  break;
        case 0xdf: set(&gb->a, 3); break;


        case 0xe0: set(&gb->b, 4); break;
        case 0xe1: set(&gb->c, 4); break;
        case 0xe2: set(&gb->d, 4); break;
        case 0xe3: set(&gb->e, 4); break;
        case 0xe4: set(&gb->h, 4); break;
        case 0xe5: set(&gb->l, 4); break;
        case 0xe6:  break;
        case 0xe7: set(&gb->a, 4); break;
        case 0xe8: set(&gb->b, 5); break;
        case 0xe9: set(&gb->c, 5); break;
        case 0xea: set(&gb->d, 5); break;
        case 0xeb: set(&gb->e, 5); break;
        case 0xec: set(&gb->h, 5); break;
        case 0xed: set(&gb->l, 5); break;
        case 0xee:  break;
        case 0xef: set(&gb->a, 5); break;


        case 0xf0: set(&gb->b, 6); break;
        case 0xf1: set(&gb->c, 6); break;
        case 0xf2: set(&gb->d, 6); break;
        case 0xf3: set(&gb->e, 6); break;
        case 0xf4: set(&gb->h, 6); break;
        case 0xf5: set(&gb->l, 6); break;
        case 0xf6:  break;
        case 0xf7: set(&gb->a, 6); break;
        case 0xf8: set(&gb->b, 7); break;
        case 0xf9: set(&gb->c, 7); break;
        case 0xfa: set(&gb->d, 7); break;
        case 0xfb: set(&gb->e, 7); break;
        case 0xfc: set(&gb->h, 7); break;
        case 0xfd: set(&gb->l, 7); break;
        case 0xfe:  break;
        case 0xff: set(&gb->a, 7); break;
    }
}

void daa(Cpu *gb)
{
    uint8_t correction = 0; 

    if (gb->cc.h || (!gb->cc.n && (gb->a & 0xf) > 9))
    {
        correction |= 0x6;
    }   
    if (gb->cc.c || (!gb->cc.n && gb->a > 0x99)) {
        correction |= 0x60;
        gb->cc.c = 1;
    }   
    gb->a += gb->cc.n ? -correction : correction;  
    gb->a &= 0xff;  

    if (gb->a == 0)
        gb->cc.z = 1;
    else
        gb->cc.z = 0;
    gb->cc.h = 0;
}