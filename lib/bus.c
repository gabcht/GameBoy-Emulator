#include <bus.h>
#include <memory.h>
#include <ram.h>
#include <io.h>

uint8_t busRead(uint16_t addr)
{
    if (addr < 0x8000)              // ROM BANK (2 of 16Ko)
    {
        return read_memory(addr);    
    }
    else if (addr < 0xA000)         // VRAM 8 Ko TO DO	 
    {
        return 1;
    }
    else if (addr < 0xC000)         // 8 KiB External RAM (in CARTDRIGE) 
    {
        return read_memory(addr);
    }
    else if (addr < 0xE000)         // WRAM (2 of 4 KiB) 
    {
        return read_ram(addr);
    }
    else if (addr < 0xFE00)         // UNUSABLE
    {
        return 0;
    }
    else if (addr < 0xFEA0)         // OAM  TO DO
    {
        return 1;
    }
    else if (addr < 0xFF00)         // UNUSABLE
    {
        return 0;
    }
    else if (addr < 0xFF80)         // I/O 
    {
        return read_io(addr);
    }
    else if (addr < 0xFFFF)         // High RAM (HRAM) 
    {
        return read_hram(addr);
    }
    else if (addr == 0xFFFF)         // Interrupt Enable register (IE) TO DO
    {
        return 1;
    }

}

void busWrite(uint16_t addr, uint8_t value)
{
    if (addr < 0x8000)              // ROM BANK (2 of 16Ko)
    {
        write_memory(addr, value);   
    }
    else if (addr < 0xA000)         // VRAM 8 Ko TO DO	 
    {
        printf("Invalid port: Vram");
        //exit(1);
        return;
    }
    else if (addr < 0xC000)         // 8 KiB External RAM (in CARTDRIGE) 
    {
        write_memory(addr, value);
    }
    else if (addr < 0xE000)         // WRAM (2 of 4 KiB) TO DO
    {
        write_ram(addr, value);  
    }
    else if (addr < 0xFE00)         // UNUSABLE
    {
        exit(1);
        return;
    }
    else if (addr < 0xFEA0)         // OAM  TO DO
    {
        printf("Invalid port: OAM");
        exit(1);
        return;
    }
    else if (addr < 0xFF00)         // UNUSABLE
    {
        printf("Invalid port: Unusable");
        exit(1);
        return;
    }
    else if (addr < 0xFF80)         // I/O
    {
        write_io(addr,value);    
    }
    else if (addr < 0xFFFF)         // High RAM (HRAM)
    {
        write_hram(addr, value);
    }
    else if (addr == 0xFFFF)         // Interrupt Enable register (IE) TO DO
    {
        printf("Invalid port");
        //exit(1);
        return;
    }

}