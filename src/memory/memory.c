#include "memory.h"

/* fills a block of memory with a specified value */
void *memset(void *ptr, int c, size_t size)
{
    char *c_ptr = (char *)ptr;
    for (int i = 0; i < size; i++)
    {
        c_ptr[i] = (char)c;
    }
    return ptr;
}

/* compare two block of memory */
int memcmp(void *s1, void *s2, int count)
{
    char *c1 = s1;
    char *c2 = s2;

    for (int i = 0; i < count; i++)
    {
        if (c1[i] != c2[i])
        {
            return (c1[i] - c1[i]);
        }
    }

    return 0;
}