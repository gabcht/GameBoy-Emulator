#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct ConditionsCode
{
    uint8_t z; // Zero flag
    uint8_t n; // Subtraction flag (BCD)
    uint8_t h; // Half Carry flag (BCD)
    uint8_t c; // Carry flags
}ConditionsCode;

typedef struct Cpu
{
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    uint8_t f;
    uint16_t pc; // Program Counter
    uint16_t sp; // Stack Pointer
    ConditionsCode cc;
}Cpu;


#endif