#ifndef ALLIANCE_H
#define ALLIANCE_H

#include "config.h"

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

Alliance *load_aliances(Config cfg);
void free_alliances(Alliance *alliances, Config cfg);

#endif