#include <memory.h>

static uint8_t *memory;

void load_memory(char *filename)
{

    FILE *f= fopen(filename, "rb");
	if (f==NULL)
	{
		printf("error: Couldn't open %s\n",filename);

		exit(1); 
	}
	fseek(f, 0L, SEEK_END);
	int ROM_SIZE = ftell(f);
	fseek(f, 0L, SEEK_SET);
	
    memory = malloc(ROM_SIZE); //malloc(gb->header.ROMsize);

	fread(memory, ROM_SIZE, 1, f);
	fclose(f);
}
uint8_t read_memory(uint16_t pc)
{
    return memory[pc];
}

void write_memory(uint16_t pc, uint8_t value)
{
	memory[pc] = value;
}