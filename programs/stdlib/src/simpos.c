#include "simpos.h"

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