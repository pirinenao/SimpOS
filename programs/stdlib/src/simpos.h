#ifndef SIMPOS_H
#define SIMPOS_H
#include <stddef.h>

void print(const char *message);
int getkey();
void *simpos_malloc(size_t size);
void *simpos_free(void *ptr);

#endif