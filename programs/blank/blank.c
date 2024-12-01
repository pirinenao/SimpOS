#include "simpos.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int main(int argc, char **argv)
{
    int num = 1337;

    printf("Hello %d", num);

    char words[] = "Hello this is a strtok test";
    const char *token = strtok(words, " ");

    while (token)
    {
        printf("%s\n", token);
        token = strtok(NULL, " ");
    }

    while (1)
    {
    }

    return 0;
}