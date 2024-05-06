#ifndef IO_H
#define IO_H

#include "common.h"

uint8_t read_io(uint16_t addr);
void write_io(uint16_t addr, uint8_t value);

#endif