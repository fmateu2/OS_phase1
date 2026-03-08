#include "maester.h"
#include <fcntl.h>

typedef struct {
    char *name;
    int qty;
} OrderItem;

void start_trade_session(Maester *maester, char *realm, int *keep_running) {    
    char *msg = NULL;
    char *line = NULL;
    char *upper = NULL;
    char *args = NULL;
    char *last_space = NULL;
    char *p = NULL;
    char *prod = NULL;
    char *qty_str = NULL;
    char *out = NULL;
    char *err = NULL;
    char *path = NULL;
    char *entry = NULL;
    char *ok = NULL;
    OrderItem *items = NULL;
    OrderItem *tmp = NULL;
    int item_count = 0;
    int qty = 0;
    int valid = 1;
    int found = 0;
    int fd = -1;
    int i = 0;

    asprintf(&msg, "Entering trade mode with %s.\n", realm);
    if (msg) {
        printF(msg);
        free(msg);
    }

    while (keep_running) {
        printF("(trade)> ");
        line = readUntil(STDIN_FILENO, '\n');
        if (line == NULL) {
            break;
        }
        str_trim(line);

        if (line[0] == '\0') {
            free(line);
            continue;
        }

        upper = malloc(strlen(line) + 1);
        if (!upper) {
            free(line);
            break;
        }
        str_toupper_copy(upper, line, strlen(line) + 1);

     
        if (strcmp(upper, "SEND") == 0) {
            if (item_count == 0) {
                printF("Shopping list is empty. Nothing to send.\n");
            } else {
                asprintf(&path, "%s/trade_%s.txt", maester->config.path, realm);
                if (path != NULL) {
                    fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    free(path);
                    path = NULL;
                    if (fd < 0) {
                        printF("ERROR: Cannot write trade file.\n");
                    } else {
                        for (i = 0; i < item_count; i++) {
                            asprintf(&entry, "%s %d\n", items[i].name, items[i].qty);
                            if (entry) {
                                write(fd, entry, strlen(entry));
                                free(entry);
                                entry = NULL;
                            }
                        }
                        close(fd);
                        asprintf(&ok, "Trade list sent to %s.\n", realm);
                        if (ok) {
                            printF(ok);
                            free(ok);
                            ok = NULL;
                        }
                    }
                }
            }
            free(upper);
            free(line);
            break;

       
        } else if (strncmp(upper, "ADD ", strlen("ADD ")) == 0) {
            args = line + 4;
            last_space = NULL;
            str_trim(args);

            for (p = args; *p != '\0'; p++) {
                if (*p == ' '){
                    last_space = p;
                } 
            }

            if (last_space == NULL) {
                printF("Usage: add <product> <amount>\n");
                free(upper);
                free(line);
                continue;
            }

            *last_space = '\0';
            prod = args;
            qty_str = last_space + 1;

            qty = 0;
            valid = 1;
            for (i = 0; qty_str[i] != '\0'; i++) {
                if (!isdigit((int)qty_str[i])){ 
                    valid = 0; break; 
                }
                qty = qty * 10 + (qty_str[i] - '0');
            }

            if (!valid || qty <= 0) {
                printF("ERROR: Amount must be a positive number.\n");
                free(upper);
                free(line);
                continue;
            }

            found = 0;
            for (i = 0; i < item_count; i++) {
                if (strcasecmp(items[i].name, prod) == 0) {
                    items[i].qty += qty;
                    found = 1;
                    break;
                }
            }

            if (!found) {
                tmp = realloc(items, sizeof(OrderItem) * (item_count + 1));
                if (tmp == NULL) {
                    printF("ERROR: Out of memory.\n");
                    free(upper);
                    free(line);
                    break;
                }
                items = tmp;
                items[item_count].name = strdup(prod);
                items[item_count].qty  = qty;
                item_count++;
            }

            asprintf(&out, "Added %d x %s\n", qty, prod);
            if (out) {
                printF(out);
                free(out);
                out = NULL;
            }

        } else if (strncmp(upper, "REMOVE ", strlen("REMOVE ")) == 0) {
            args = line + 7;
            last_space = NULL;
            str_trim(args);

            for (p = args; *p != '\0'; p++) {
                if (*p == ' ') last_space = p;
            }

            if (last_space == NULL) {
                printF("Usage: remove <product> <amount>\n");
                free(upper);
                free(line);
                continue;
            }

            *last_space = '\0';
            prod = args;
            qty_str = last_space + 1;

            qty = 0;
            valid = 1;
            
            for (i = 0; qty_str[i] != '\0'; i++) {
                if (!isdigit((int)qty_str[i])){ 
                    valid = 0; break; 
                }
                qty = qty * 10 + (qty_str[i] - '0');
            }

            if (!valid || qty <= 0) {
                printF("ERROR: Amount must be a positive number.\n");
                free(upper);
                free(line);
                continue;
            }

            found = 0;
            for (i = 0; i < item_count; i++) {
                if (strcasecmp(items[i].name, prod) == 0) {
                    items[i].qty -= qty;
                    found = 1;
                    if (items[i].qty <= 0) {
                        free(items[i].name);
                        for (int j = i; j < item_count - 1; j++) {
                            items[j] = items[j + 1];
                        }
                        item_count--;
                    }
                    break;
                }
            }

            if (!found) {
                asprintf(&err, "ERROR: '%s' not in shopping list.\n", prod);
                if (err) {
                    printF(err);
                    free(err);
                    err = NULL;
                }
            } else {
                asprintf(&out, "Removed %d x %s\n", qty, prod);
                if (out) {
                    printF(out);
                    free(out);
                    out = NULL;
                }
            }

        } else if (strcmp(upper, "CANCEL") == 0) {
            printF("Trade session cancelled.\n");
            free(upper);
            free(line);
            break;

        } else {
            printF("Unknown trade command. Use: add | remove | send | cancel\n");
        }

        free(upper);
        free(line);
    }

    if (items != NULL) {
        for (i = 0; i < item_count; i++) {
            free(items[i].name);
        }
        free(items);
    }
}
