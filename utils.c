#include "maester.h"
#include <unistd.h>
#include <string.h>
#include <ctype.h>


/* Write a string to stdout */
void wprint(const char *msg)
{
    if (!msg) return;
    write(STDOUT_FILENO, msg, strlen(msg));
}

/* Write a string followed by newline */
void wprintln(const char *msg)
{
    wprint(msg);
    write(STDOUT_FILENO, "\n", 1);
}

/* Write an integer to stdout */
void wprint_int(int n)
{
    char buf[32];
    int  neg = 0, i = 0, len;
    if (n < 0) { neg = 1; n = -n; }
    if (n == 0) { write(STDOUT_FILENO, "0", 1); return; }
    while (n > 0) { buf[i++] = '0' + (n % 10); n /= 10; }
    if (neg) buf[i++] = '-';
    /* reverse */
    for (int a = 0, b = i - 1; a < b; a++, b--) {
        char tmp = buf[a]; buf[a] = buf[b]; buf[b] = tmp;
    }
    len = i;
    write(STDOUT_FILENO, buf, len);
}

/* Read one line from fd into buf (strips \n). Returns chars read or -1. */
int readline_fd(int fd, char *buf, int maxlen)
{
    int  total = 0;
    char c;
    while (total < maxlen - 1) {
        int n = read(fd, &c, 1);
        if (n <= 0) {
            if (total == 0) return -1;
            break;
        }
        if (c == '\n') break;
        if (c == '\r') continue;
        buf[total++] = c;
    }
    buf[total] = '\0';
    return total;
}

/* Convert string to uppercase into dst */
void str_toupper(char *dst, const char *src, int maxlen)
{
    int i;
    for (i = 0; i < maxlen - 1 && src[i]; i++)
        dst[i] = (char)toupper((unsigned char)src[i]);
    dst[i] = '\0';
}

/* Trim leading/trailing whitespace in-place */
void str_trim(char *s)
{
    int start = 0, end;
    while (s[start] && isspace((unsigned char)s[start])) start++;
    if (start > 0) memmove(s, s + start, strlen(s) - start + 1);
    end = (int)strlen(s) - 1;
    while (end >= 0 && isspace((unsigned char)s[end])) s[end--] = '\0';
}

/* Check if s starts with prefix (case-sensitive) */
int str_startswith(const char *s, const char *prefix)
{
    return strncmp(s, prefix, strlen(prefix)) == 0;
}
