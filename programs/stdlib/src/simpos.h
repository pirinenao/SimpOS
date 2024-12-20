#ifndef SIMPOS_H
#define SIMPOS_H
#include <stddef.h>
#include <stdbool.h>

struct command_argument
{
    char argument[512];
    struct command_argument* next;
};

struct process_arguments
{
    int argc;
    char** argv;
};

void print(const char *message);
int simpos_getkey();
void *simpos_malloc(size_t size);
void *simpos_free(void *ptr);
void simpos_putchar(char c);
int simpos_getkeyblock();
void simpos_terminal_readline(char *out, int max, bool output_while_typing);
void simpos_process_load_start(const char *filename);
struct command_argument* simpos_parse_command(const char* command, int max);
void simpos_process_get_arguments(struct process_arguments* arguments);
int simpos_system(struct command_argument* arguments);
int simpos_system_run(const char* command);
void simpos_exit();

#endif
