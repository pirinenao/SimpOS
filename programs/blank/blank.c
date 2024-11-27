#include "simpos.h"
#include "stdlib.h"
#include "stdio.h"

int main(int argc, char **argv)
{
    print(itoa(1337));

    putchar('X');

    void *ptr = malloc(512);
    free(ptr);
    if (ptr)
    {
    }

    while (1)
    {
        if (getkey() != 0)
            print("key was pressed\n");
    }

    return 0;
}