#include "simpos.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int main(int argc, char **argv)
{
    char *ptr = malloc(20);
    strcpy(ptr, "hello world");

    print(ptr);
    free(ptr);
    /* testing if memory can be accessed after free() function */
    ptr[0] = 'B';
    print("abc");

    while (1)
    {
    }

    return 0;
}