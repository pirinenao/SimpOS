#ifndef SIMPOS_STRING_H
#define SIMPOS_STRING_H

#include <stdbool.h>

/* function prototypes */
int strlen(const char *ptr);
bool isdigit(char c);
int tonumericdigit(char c);
int strnlen(const char *ptr, int max_length);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, int count);
char tolower(char s1);
int strncmp(const char *str1, const char *str2, int n);
int istrncmp(const char *s1, const char *s2, int n);
int strlen_terminator(const char *str, int max, char terminator);
char *strtok(char *str, const char *delimiters);

#endif