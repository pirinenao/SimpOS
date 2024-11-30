#ifndef SIMPOS_H
#define SIMPOS_H
#include <stddef.h>
#include <stdbool.h>

void print(const char *message);
int simpos_getkey();
void *simpos_malloc(size_t size);
void *simpos_free(void *ptr);
void simpos_putchar(char c);
int simpos_getkeyblock();
void simpos_terminal_readline(char *out, int max, bool output_while_typing);
void simpos_process_load_start(const char *filename);

#endif