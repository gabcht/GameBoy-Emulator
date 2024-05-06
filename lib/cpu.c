#include <cpu.h>

void init_cpu(Cpu *cpu, int debug)
{
    if (debug == 1)
    {
        cpu->a = 0x01; 
        cpu->b = 0x00;
        cpu->c = 0x13;
        cpu->d = 0x00;
        cpu->e = 0xD8;
        cpu->h = 0x01;
        cpu->l = 0x4D;
        cpu->pc = 0x0100;
        cpu->sp = 0xFFFE;

        cpu->cc.c = 1;
        cpu->cc.h = 1;
        cpu->cc.n = 0;
        cpu->cc.z = 1;
    }
    else
    {
        memset(cpu, 0, sizeof(*cpu));
        cpu->pc = 0x0100;
    }
}