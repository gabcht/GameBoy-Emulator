#ifndef MEMORY_H
#define MEMORY_H

#include "common.h"

void load_memory(char *filename);
uint8_t read_memory(uint16_t pc);
void write_memory(uint16_t pc, uint8_t value);

#endif