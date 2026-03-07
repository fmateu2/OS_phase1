#ifndef INVENTORY_H
#define INVENTORY_H

typedef struct {
    char name[100];
    int amount;
    float weight;
} Product;

typedef struct {
    Product *items;
    int num_items;
} Inventory;

Inventory load_inventory(char *path);
void display_inventory(Inventory inv);
void save_inventory(Inventory inv);
void free_inventory(Inventory *inv);


#endif
