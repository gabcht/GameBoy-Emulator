#include <io.h>

static char serial_data[2];

uint8_t read_io(uint16_t addr)
{
    if (addr == 0xFF01) {
        return serial_data[0];
    }

    if (addr == 0xFF02) {
        return serial_data[1];
    }
    if (addr == 0xFF44)
        return 0x90;
}
void write_io(uint16_t addr, uint8_t value)
{
    if (addr == 0xFF01) {
        serial_data[0] = value;
    }

    if (addr == 0xFF02) {
        serial_data[1] = value;
    }
}