#ifndef STRING_H
#define STRING_H
#include <stdbool.h>

/* function prototypes */
int strlen(const char *ptr);
bool isdigit(char c);
int tonumericdigit(char c);
int strnlen(const char *ptr, int max_length);

#endif