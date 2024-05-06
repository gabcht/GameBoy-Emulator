#ifndef RAM_H
#define RAM_H

#include "common.h"

uint8_t read_ram(uint16_t addr);
void write_ram(uint16_t addr, uint8_t value);
uint8_t read_hram(uint16_t addr);
void write_hram(uint16_t addr, uint8_t value);

#endif