#include "simpos.h"
#include "stdlib.h"

int main(int argc, char **argv)
{
    print("Hello from the user program\n");

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