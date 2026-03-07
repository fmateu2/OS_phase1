#ifndef MAESTER_H
#define MAESTER_H

#include "utils.h"


typedef enum {
    PENDING,
    ON_PENDING,
    ALLIED,
    FAILED,
} AllianceStatus;

typedef struct {
    char *realm;
    AllianceStatus alliance;
} Alliance;

typedef struct {
    Config config;
    Inventory inventory;
    Envoy *envoys;
    int num_envoys;
    Alliance *alliances;
    int num_alliances;
} Maester;








#endif
