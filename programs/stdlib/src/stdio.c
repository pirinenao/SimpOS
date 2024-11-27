#include "stdio.h"
#include "simpos.h"
#include "stdlib.h"
#include <stdarg.h>

int putchar(int c)
{
    simpos_putchar((char)c);
    return 0;
}

int printf(const char *format, ...)
{
    /* list of variable arguments */
    va_list ap;
    const char *p;
    char *sval;
    int ival;

    va_start(ap, format);

    /* loop through the given string */
    for (p = format; *p; p++)
    {
        /* if char is not format specifier, print the char and continue */
        if (*p != '%')
        {
            putchar(*p);
            continue;
        }

        /* if format specifier was encountered */
        switch (*++p)
        {

        /* if specifier was integer */
        case 'd':
            ival = va_arg(ap, int);
            print(itoa(ival));
            break;
        /* if specifier was string */
        case 's':
            sval = va_arg(ap, char *);
            print(sval);
            break;

        default:
            putchar(*p);
            break;
        }
    }
    /* cleanup */
    va_end(ap);

    return 0;
}