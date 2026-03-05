/*
 * The Citadel System - Phase 1
 * Curs 2025-26
 *
 * @author: Jan Francesc Porcar
 */

#include "maester.h"

/**
 * @brief Reads characters from fd until separator is found.
 * @return Dynamically allocated string, or NULL on EOF/error
 */
char *readUntil(int fd, char separator) {
    char ch;
    int i = 0;
    int capacity = 16;

    char *buffer = malloc(capacity);
    if (buffer == NULL) return NULL;

    while (read(fd, &ch, 1) == 1) {
        if (ch == separator) {
            buffer[i] = '\0';
            return buffer;
        }

        if (i >= capacity - 1) {
            capacity *= 2;
            char *new_buf = realloc(buffer, capacity);
            if (new_buf == NULL) {
                free(buffer);
                return NULL;
            }
            buffer = new_buf;
        }

        buffer[i++] = ch;
    }

    if (i == 0) {
        free(buffer);
        return NULL;
    }

    buffer[i] = '\0';
    return buffer;
}

/**
 * @brief Copies src into dst converting all chars to uppercase.
 */
void str_toupper_copy(char *dst, const char *src, int maxlen) {
    int i;
    for (i = 0; i < maxlen - 1 && src[i] != '\0'; i++)
        dst[i] = (char)toupper((unsigned char)src[i]);
    dst[i] = '\0';
}

/**
 * @brief Trims leading and trailing whitespace in place.
 */
void str_trim(char *s) {
    int start = 0, end;
    while (s[start] != '\0' && isspace((unsigned char)s[start])) start++;
    if (start > 0) memmove(s, s + start, strlen(s) - start + 1);
    end = (int)strlen(s) - 1;
    while (end >= 0 && isspace((unsigned char)s[end])) s[end--] = '\0';
}
