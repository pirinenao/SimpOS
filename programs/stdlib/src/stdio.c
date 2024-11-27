#include "stdio.h"
#include "simpos.h"

int putchar(int c)
{
    simpos_putchar((char)c);
    return 0;
}