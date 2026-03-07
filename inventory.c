#include "inventory.h"
#include "utils.h"
#include <stdlib.h>
#include <fcntl.h>

Inventory load_inventory(char *path) {
    Inventory inv = {0};
    Product *product;
    Product temp;
    int fd = open(path, O_RDONLY);
    int len;

    if (fd < 0) {
        printF("ERROR: Cannot open inventory binary file.");
        return inv;
    } else {
        len = read(fd, &temp, sizeof(Product));

        while (len > 0) {
            if (len != sizeof(Product)) {
                printF("ERROR: Corrupted inventory file.");
                free(inv.items);
                inv.items = NULL;
                inv.num_items = 0;
                close(fd);
                return inv;
            }

            product = realloc(inv.items, sizeof(Product) * (inv.num_items + 1));
            if (product == NULL) {
                printF("ERROR: Cannot allocate memory for inventory.");
                free(inv.items);
                inv.items = NULL;
                inv.num_items = 0;
                close(fd);
                return inv;
            }

            inv.items = product;
            inv.items[inv.num_items] = temp;
            inv.num_items++;

            len = read(fd, &temp, sizeof(Product));
        }
        close(fd);
    }

    return inv;
}

void display_inventory(Inventory inv) {
    printF("--- Trade Ledger ---\n");
    printF("Item | Value (Gold) | Weight (Stone)\n");
    printF("--------------------------------------------------------\n");
    for (int i = 0; i < inv.num_items; i++) {
        char *line;
        asprintf(&line, "%s | %d | %.1f\n", inv.items[i].name, inv.items[i].amount, inv.items[i].weight);
        printF(line);
        free(line);
    }
    printF("--------------------------------------------------------\n");
    asprintf(&line, "Total Entries: %d\n", inv.num_items);
    printF(line);
    free(line);
    line = NULL;
}

void save_inventory(Inventory inv) {
    int fd = open("stock.db", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        printF("ERROR: Cannot open inventory for writing.");
        return;
    } else {
        for (int i = 0; i < inv.num_items; i++) {
            write(fd, &inv.items[i], sizeof(Product));
        }
        close(fd);
    }
}

void free_inventory(Inventory *inv){
    if (!inv) {
        return;
    } else {
        free(inv->items);
        inv->items = NULL;
        inv->num_items = 0;
    }  
}
