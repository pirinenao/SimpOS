#include "simpos.h"

int main(int argc, char **argv)
{
    print("Hello from the user program\n");

    while (1)
    {
        if (getkey() != 0)
            print("key was pressed\n");
    }

    return 0;
}