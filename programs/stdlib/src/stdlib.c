#include "stdlib.h"
#include "simpos.h"

/* convert integer to string */
char *itoa(int i)
{
    static char text[12];
    int loc = 11;
    text[11] = 0;
    char neg = 1;

    if (i >= 0)
    {
        neg = 0;
        i = -i;
    }

    // extract digits and store them in the buffer (right to left)
    while (i)
    {
        text[--loc] = '0' - (i % 10);
        i /= 10;
    }

    // handle the case where the number is zero
    if (loc == 11)
    {
        text[--loc] = '0';
    }

    // add a minus sign if the number was negative
    if (neg)
    {
        text[--loc] = '-';
    }

    return &text[loc];
}

void *malloc(size_t size)
{
    return simpos_malloc(size);
}

void free(void *ptr)
{
    simpos_free(ptr);
}