#include <ram.h>

static uint8_t wram[0x2000];
static uint8_t hram[0x80];

uint8_t read_ram(uint16_t addr)
{
    addr -= 0xC000;
    if (addr >= 0x2000) 
    {
        printf("INVALID WRAM ADDR %04X\n", addr + 0xC000);
        exit(-1);
    }
    return wram[addr];
}

void write_ram(uint16_t addr, uint8_t value)
{
    wram[addr-0xC000] = value;
}

uint8_t read_hram(uint16_t addr)
{
    addr -= 0xFF80;
    if (addr >= 0x2000) 
    {
        printf("INVALID WRAM ADDR %04X\n", addr + 0xC000);
        exit(-1);
    }
    return hram[addr];
}

void write_hram(uint16_t addr, uint8_t value)
{
    hram[addr-0xFF80] = value;
}