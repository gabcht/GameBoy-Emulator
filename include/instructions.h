#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "common.h"


void ld(uint8_t *dest, uint8_t src);
void ld16(uint8_t *destHo,uint8_t *destLo, uint8_t srcHo, uint8_t srcLo);

void inc(Cpu *gb,uint8_t *reg);
void inc16(uint8_t *regHi, uint8_t *regLo);
void add16(Cpu *gb, uint8_t *regDestHi, uint8_t *regDestLo, uint16_t regSrc);
void dec(Cpu *gb,uint8_t *reg);
void dec16(uint8_t *regHi, uint8_t *regLo);

void add(Cpu *gb, uint8_t reg);
void sub(Cpu *gb, uint8_t reg);
void adc(Cpu *gb, uint8_t reg);
void sbc(Cpu *gb, uint8_t reg);
void xor(Cpu *gb, uint8_t reg);
void and(Cpu *gb, uint8_t reg);
void or(Cpu *gb, uint8_t reg);
void cp(Cpu *gb, uint8_t reg);

void jr(Cpu *gb, uint8_t value);
void jrC(Cpu *gb, uint8_t value);
void jrNC(Cpu *gb, uint8_t value);
void jrZ(Cpu *gb, uint8_t value);
void jrNZ(Cpu *gb, uint8_t value);

void ret(Cpu *gb);
void retZ(Cpu *gb);
void retNZ(Cpu *gb);
void retC(Cpu *gb);
void retNC(Cpu *gb);

void jp(Cpu *gb, uint16_t value);
void jpZ(Cpu *gb, uint16_t value);
void jpNZ(Cpu *gb, uint16_t value);
void jpC(Cpu *gb, uint16_t value);
void jpNC(Cpu *gb, uint16_t value);

void call(Cpu *gb, uint16_t addr);
void callZ(Cpu *gb, uint16_t addr);
void callNZ(Cpu *gb, uint16_t addr);
void callC(Cpu *gb, uint16_t addr);
void callNC(Cpu *gb, uint16_t addr);

void push(Cpu *gb, uint8_t reg1, uint8_t reg2);
void pop(Cpu *gb, uint8_t *reg1, uint8_t *reg2);
void pop_af(Cpu *gb, uint8_t *reg1, uint8_t *reg2);

void bit(Cpu *gb, uint8_t reg, uint8_t b);
void set(uint8_t *reg, uint8_t b);
void res(uint8_t *reg, uint8_t b);

void rlc(Cpu *gb, uint8_t *reg);
void rl(Cpu *gb, uint8_t *reg);
void rrc(Cpu *gb, uint8_t *reg);
void rr(Cpu *gb, uint8_t *reg);
void sla(Cpu *gb, uint8_t *reg);
void sra(Cpu *gb, uint8_t *reg);
void srl(Cpu *gb, uint8_t *reg);
void swap(Cpu *gb, uint8_t *reg);

void prefix_cb(Cpu *gb, uint8_t opcode1);

void daa(Cpu *gb);

#endif