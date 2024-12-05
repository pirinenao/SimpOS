#include "simpos.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int main(int argc, char **argv)
{
    char str[] = "hello world";
    struct command_argument* root_command = simpos_parse_command(str, sizeof(str));

    printf("%s\n", root_command->argument);
    printf("%s\n", root_command->next);

    while (1)
    {
    }

    return 0;
}