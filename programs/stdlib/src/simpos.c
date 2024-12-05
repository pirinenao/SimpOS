#include "simpos.h"
#include "string.h"

/* parses the process arguments */
struct command_argument* simpos_parse_command(const char* command, int max)
{
    struct command_argument* root_command = 0;
    char scommand[1024];

    if(max >= (int) sizeof(scommand))
    {
        return 0;
    }

    /* copy the command so strtok() won't modify the original */
    strncpy(scommand, command, sizeof(scommand));
    /* tokenize the first token by using space as delimiter */
    char* token = strtok(scommand, " ");

    if(!token)
    {
        return 0;
    }

    /* allocates memory for the root_command */
    root_command = simpos_malloc(sizeof(struct command_argument));

    if(!root_command)
    {
        return 0;
    }

    /* copies the parsed argument into root_command struct */
    strncpy(root_command->argument, token, sizeof(root_command->argument));
    root_command->next = 0;

    struct command_argument* current = root_command;
    token = strtok(NULL, " ");  // null as strtok argument keeps on parsin the scommand

    /* check if more arguments. if so, parse them and add them to the linked list */
    while(token != 0)
    {
        struct command_argument* new_command = simpos_malloc(sizeof(struct command_argument));

        if(!new_command)
        {
            break;
        }

        strncpy(new_command->argument, token, sizeof(new_command->argument));
        new_command->next = 0x00;
        current->next = new_command;
        current = new_command;
        token = strtok(NULL, " ");
    }

    return root_command;
}

int simpos_getkeyblock()
{
    int val = 0;

    do
    {
        val = simpos_getkey();
    } while (val == 0);

    return val;
}

void simpos_terminal_readline(char *out, int max, bool output_while_typing)
{
    int i = 0;
    for (i = 0; i < max - 1; i++)
    {

        char key = simpos_getkeyblock();

        // if carriage return (enter)
        if (key == 13)
        {
            break;
        }

        // if caller wants to output the keystrokes
        if (output_while_typing)
        {
            simpos_putchar(key);
        }

        // backspace
        if (key == 0x08 && i >= 1)
        {
            out[i - 1] = 0x00;
            i -= 2;
            continue;
        }

        out[i] = key;
    }
    // add null
    out[i] = 0x00;
}