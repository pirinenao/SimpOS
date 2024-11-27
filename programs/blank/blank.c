#include "simpos.h"
#include "stdlib.h"
#include "stdio.h"

int main(int argc, char **argv)
{
    int num = 1337;

    printf("Hello %d", num);

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