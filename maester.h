#ifndef MAESTER_H
#define MAESTER_H

#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L

#include "utils.h"
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

Maester load_maester(char *config_path, char *inventory_path);
void free_maester(Maester *maester);
void setup_signals(int *keep_running);
void dispatch(Maester *maester, char *line, int *keep_running);
void start_trade_session(Maester *maester, char *realm, int *keep_running);

#endif
