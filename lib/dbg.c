#include <dbg.h>
#include <bus.h>

static char dbg_message[1024] = {0};
int msg_size = 0;

void dbg_update()
{
    if (busRead(0xFF02) == 0x81)
    {
        char c = busRead(0xFF01);
        dbg_message[msg_size++] = c;
        busWrite(0xff02, 0);
    }
}

void dbg_print(void)
{
    if (dbg_message[0])
    {
        printf("DBG: %s\n", dbg_message);
    }
}

