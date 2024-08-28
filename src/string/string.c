#include "string.h"

/* returns the string length */
int strlen(const char *ptr)
{

    int i = 0;
    while (*ptr != '\0')
    {
        i++;
        ptr += 1;
    }

    return i;
}

/* returns true if the input is digit */
bool isdigit(char c)
{
    return c >= 48 && c <= 57;
}

/* converts a numeric characters into its
 * corresponding integer value
 */
int tonumericdigit(char c)
{
    return c - 48;
}

/* returns the string length, with variable
 * max_length helping to avoid overflows
 */
int strnlen(const char *ptr, int max_length)
{
    int i = 0;

    for (i = 0; i < max_length; i++)
    {
        if (ptr[i] == 0)
        {
            break;
        }
    }

    return i;
}