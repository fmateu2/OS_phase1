#ifndef UTILS_H
#define UTILS_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#define printF(x) write(STDOUT_FILENO, (x), strlen(x));

char *readUntil(int fd, char separator);
void str_toupper_copy(char *dst, const char *src, int maxlen);
void str_trim(char *s);

#endif