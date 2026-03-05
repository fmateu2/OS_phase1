#include "maester.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


/* ── helpers ── */
static void write_float(float f)
{
    /* render float with 1 decimal using only write() */
    int   neg  = (f < 0);
    float absf = neg ? -f : f;
    int   intpart  = (int)absf;
    int   fracpart = (int)((absf - intpart) * 10 + 0.5f);
    if (fracpart >= 10) { intpart++; fracpart = 0; }
    if (neg) write(STDOUT_FILENO, "-", 1);
    wprint_int(intpart);
    write(STDOUT_FILENO, ".", 1);
    wprint_int(fracpart);
}

static void write_padded(const char *s, int width)
{
    int len = (int)strlen(s);
    write(STDOUT_FILENO, s, len < width ? len : width);
    for (int i = len; i < width; i++) write(STDOUT_FILENO, " ", 1);
}

static void write_int_padded(int n, int width)
{
    char buf[32];
    int  i = 0, neg = 0;
    if (n < 0) { neg = 1; n = -n; }
    if (n == 0) buf[i++] = '0';
    while (n > 0) { buf[i++] = '0' + n % 10; n /= 10; }
    if (neg) buf[i++] = '-';
    for (int a = 0, b = i-1; a < b; a++, b--) { char t=buf[a]; buf[a]=buf[b]; buf[b]=t; }
    int pad = width - i;
    for (int j = 0; j < pad; j++) write(STDOUT_FILENO, " ", 1);
    write(STDOUT_FILENO, buf, i);
}

/* ── public API ── */

int load_inventory(const char *path, product_t **inv, int *count)
{
    int       fd;
    product_t p;
    int       n = 0;
    product_t *arr = NULL;

    *inv   = NULL;
    *count = 0;

    fd = open(path, O_RDONLY);
    if (fd < 0) return 0;   /* no stock yet — not an error */

    while (1) {
        ssize_t r = read(fd, &p, sizeof(product_t));
        if (r == 0) break;
        if (r != sizeof(product_t)) {
            wprintln("WARNING: Corrupt inventory entry skipped.");
            break;
        }
        product_t *tmp = realloc(arr, sizeof(product_t) * (n + 1));
        if (!tmp) {
            wprintln("ERROR: Out of memory reading inventory.");
            free(arr);
            close(fd);
            return -1;
        }
        arr = tmp;
        arr[n++] = p;
    }

    close(fd);
    *inv   = arr;
    *count = n;
    return 0;
}

int save_inventory(const char *path, product_t *inv, int count)
{
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        wprintln("ERROR: Cannot open inventory for writing.");
        return -1;
    }
    for (int i = 0; i < count; i++) {
        if (write(fd, &inv[i], sizeof(product_t)) != sizeof(product_t)) {
            wprintln("ERROR: Failed to write inventory entry.");
            close(fd);
            return -1;
        }
    }
    close(fd);
    return 0;
}

void free_inventory(product_t *inv)
{
    if (inv) free(inv);
}

/* Display inventory as the Trade Ledger table */
void display_inventory(product_t *inv, int count)
{
    const char *sep =
        "--------------------------------------------------------\n";
    wprintln("--- Trade Ledger ---");
    wprint("Item                    | Value (Gold) | Weight (Stone)\n");
    wprint(sep);

    for (int i = 0; i < count; i++) {
        write_padded(inv[i].name, 23);
        wprint(" | ");
        write_int_padded(inv[i].amount, 12);
        wprint(" | ");
        write_float(inv[i].weight);
        write(STDOUT_FILENO, "\n", 1);
    }

    wprint(sep);
    wprint("Total Entries: ");
    wprint_int(count);
    write(STDOUT_FILENO, "\n", 1);
}
