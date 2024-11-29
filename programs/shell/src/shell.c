#include "shell.h"
#include "stdio.h"
#include "stdlib.h"
#include "simpos.h"

int main(int argc, char **argv)
{
    print("Simpos version 1.0.0\n");
    while (1)
    {
        print(">");
        char buf[1024];
        simpos_terminal_readline(buf, sizeof(buf), true);
        print("\n");
    }

    return 0;
}