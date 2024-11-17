#include "stdlib.h"
#include "simpos.h"

void *malloc(size_t size)
{
    return simpos_malloc(size);
}
