#include "simpos.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int main(int argc, char **argv)
{
    struct process_arguments arguments;
    simpos_process_get_arguments(&arguments);

    printf("%d %s \n", arguments.argc, arguments.argv[0]);

    while (1)
    {
    }

    return 0;
}