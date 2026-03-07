#ifndef MAESTER_H
#define MAESTER_H

#include "config.h"
#include "inventory.h"
#include "envoy.h"
#include "alliance.h"

typedef struct {
    Config config;
    Inventory inventory;
    Envoy *envoys;
    int num_envoys;
    Alliance *alliances;
    int num_alliances;
} Maester;

Maester load_maester(const char *config_file, const char *inventory_file);
void free_maester(Maester *maester);


#endif
