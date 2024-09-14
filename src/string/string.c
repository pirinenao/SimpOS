#include "string.h"

/* returns character in lowercase */
char tolower(char s1)
{
    if (s1 >= 65 && s1 <= 90)
    {
        s1 += 32;
    }

    return s1;
}

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

/* copies source string to the destination string */
char *strcpy(char *dest, const char *src)
{
    char *res = dest;
    while (*src != 0)
    {
        *dest = *src;
        src += 1;
        dest += 1;
    }

    *dest = 0x00;

    return res;
}

/* compares n amount of characters of 2 strings
 * returns 0 if characters are the same
 * returns negative value if str1 is less than str2
 * returns positive value if str1 is greater than str2
 */
int strncmp(const char *str1, const char *str2, int n)
{
    unsigned char u1, u2;

    while (n-- > 0)
    {
        u1 = (unsigned char)*str1++;
        u2 = (unsigned char)*str2++;

        if (u1 != u2)
        {
            return u1 - u2;
        }

        if (u1 == '\0')
        {
            return 0;
        }
    }

    return 0;
}

/* strncmp function with no case sensitivity */
int istrncmp(const char *s1, const char *s2, int n)
{
    unsigned char u1, u2;
    while (n-- > 0)
    {
        u1 = (unsigned char)*s1++;
        u2 = (unsigned char)*s2++;

        if (u1 != u2 && tolower(u1) != tolower(u2))
        {
            return u1 - u2;
        }

        if (u1 == '\0')
        {
            return 0;
        }
    }

    return 0;
}

/* returns the index of terminator */
int strlen_terminator(const char *str, int max, char terminator)
{
    int i = 0;

    for (i = 0; i < max; i++)
    {
        if (str[i] == '\0' || str[i] == terminator)
        {
            break;
        }
    }

    return i;
}