#ifndef BUS_H
#define BUS_H

#include "common.h"

uint8_t busRead(uint16_t addr);
void busWrite(uint16_t addr, uint8_t value);

#endif