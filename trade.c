#define _POSIX_C_SOURCE 200809L
#include "maester.h"
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>


/* ── Shopping list entry ── */
typedef struct {
    char name[100];
    int  qty;
} order_item_t;

static void write_trade_file(const char *folder, const char *realm,
                             order_item_t *items, int count)
{
    char path[512];
    int  fd, i;

    /* build path: folder/trade_REALM.txt */
    snprintf(path, sizeof(path), "%s/trade_%s.txt", folder, realm);

    fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        wprintln("ERROR: Cannot write trade file.");
        return;
    }

    for (i = 0; i < count; i++) {
        write(fd, items[i].name, strlen(items[i].name));
        write(fd, " ", 1);
        char nbuf[16];
        int n = items[i].qty, idx = 0;
        if (n == 0) { nbuf[idx++] = '0'; }
        while (n > 0) { nbuf[idx++] = '0' + n % 10; n /= 10; }
        /* reverse */
        for (int a = 0, b = idx-1; a < b; a++, b--) {
            char t = nbuf[a]; nbuf[a] = nbuf[b]; nbuf[b] = t;
        }
        write(fd, nbuf, idx);
        write(fd, "\n", 1);
    }

    close(fd);

    wprint("Trade list sent to ");
    wprintln(realm);
}

/* Check alliance with realm */
static int is_allied(app_state_t *state, const char *realm)
{
    for (int i = 0; i < state->alliance_count; i++) {
        if (strcasecmp(state->alliances[i].realm, realm) == 0)
            return state->alliances[i].status == ALLIANCE_ALLIED;
    }
    return 0;
}

void start_trade_session(app_state_t *state, const char *realm)
{
    char line[512];
    char upper[512];
    order_item_t *items = NULL;
    int           item_count = 0;

    if (!is_allied(state, realm)) {
        wprint("ERROR: You must have an alliance with ");
        wprint(realm);
        wprintln(" to trade.");
        return;
    }

    if (state->inv_count == 0) {
        wprintln("ERROR: No products in your inventory.");
        return;
    }

    wprint("Entering trade mode with ");
    wprintln(realm);
    wprint("Available products: ");
    for (int i = 0; i < state->inv_count; i++) {
        if (i > 0) wprint(", ");
        wprint(state->inventory[i].name);
    }
    write(STDOUT_FILENO, "\n", 1);

    /* interactive sub-loop */
    while (keep_running) {
        wprint("(trade)> ");
        int n = readline_fd(STDIN_FILENO, line, sizeof(line));
        if (n < 0) break;
        str_trim(line);
        if (line[0] == '\0') continue;

        str_toupper(upper, line, sizeof(upper));

        if (strcmp(upper, "SEND") == 0) {
            if (item_count == 0) {
                wprintln("Shopping list is empty. Nothing to send.");
            } else {
                write_trade_file(state->config.folder, realm, items, item_count);
            }
            break;

        } else if (str_startswith(upper, "ADD ")) {
            /* ADD <product name> <qty> */
            /* find last token as quantity, rest is product name */
            char *last_space = NULL;
            char *p = line + 4;  /* skip "ADD " */
            str_trim(p);

            /* find last space */
            for (char *q = p; *q; q++)
                if (*q == ' ') last_space = q;

            if (!last_space) {
                wprintln("Usage: add <product name> <quantity>");
                continue;
            }

            *last_space = '\0';
            char *prod_name = p;
            char *qty_str   = last_space + 1;
            int   qty = 0;
            for (int i = 0; qty_str[i]; i++) {
                if (qty_str[i] < '0' || qty_str[i] > '9') { qty = -1; break; }
                qty = qty * 10 + (qty_str[i] - '0');
            }

            if (qty <= 0) {
                wprintln("ERROR: Quantity must be a positive number.");
                continue;
            }

            /* check product exists */
            int found = 0;
            for (int i = 0; i < state->inv_count; i++) {
                if (strcasecmp(state->inventory[i].name, prod_name) == 0) {
                    found = 1; break;
                }
            }
            if (!found) {
                wprint("ERROR: Product '");
                wprint(prod_name);
                wprintln("' not found in inventory.");
                continue;
            }

            /* add or update order */
            int updated = 0;
            for (int i = 0; i < item_count; i++) {
                if (strcasecmp(items[i].name, prod_name) == 0) {
                    items[i].qty += qty;
                    updated = 1; break;
                }
            }
            if (!updated) {
                order_item_t *tmp = realloc(items,
                                   sizeof(order_item_t) * (item_count + 1));
                if (!tmp) { wprintln("ERROR: Out of memory."); break; }
                items = tmp;
                strncpy(items[item_count].name, prod_name,
                        sizeof(items[0].name) - 1);
                items[item_count].name[sizeof(items[0].name)-1] = '\0';
                items[item_count].qty = qty;
                item_count++;
            }

            wprint("Added ");
            wprint_int(qty);
            wprint(" x ");
            wprintln(prod_name);

        } else if (strcmp(upper, "LIST") == 0) {
            if (item_count == 0) {
                wprintln("Shopping list is empty.");
            } else {
                wprintln("Current shopping list:");
                for (int i = 0; i < item_count; i++) {
                    wprint("  - ");
                    wprint(items[i].name);
                    wprint(": ");
                    wprint_int(items[i].qty);
                    write(STDOUT_FILENO, "\n", 1);
                }
            }

        } else if (strcmp(upper, "CANCEL") == 0) {
            wprintln("Trade session cancelled.");
            break;

        } else {
            wprintln("Commands: add <product> <qty> | list | send | cancel");
        }
    }

    if (items) free(items);
}
