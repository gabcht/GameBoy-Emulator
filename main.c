#include <stdio.h>
#include <cpu.h>
#include <memory.h>
#include <opcode.h>
#include <dbg.h>
#include <bus.h>

int main(void)
{
    Cpu cpu;

    init_cpu(&cpu, 1);
    load_memory("special.gb");
    int debug = 0;
    int done = 1;

    while (done == 1)
    {
        emulate(&cpu);
        dbg_update();
        dbg_print();
    }

}