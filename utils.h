#ifndef UTILS_H
#define UTILS_H

#include <string.h>
#include <unistd.h>

#define printF(x) write(STDOUT_FILENO, (x), strlen(x));

char *readUntil(int fd, char separator);
void str_toupper_copy(char *dst, const char *src, int maxlen);
void str_trim(char *s);

#endif