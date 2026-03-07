#ifndef ENVOY_H
#define ENVOY_H

#include "config.h"

typedef enum {
    FREE,
    ON_MISSION
} EnvoyStatus;

typedef struct {
    int id;
    EnvoyStatus status;
} Envoy;

Envoy* load_envoys(Config cfg);
void free_envoys(Envoy *envoys);

#endif