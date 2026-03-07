#include "envoy.h"
#include "utils.h"

Envoy* load_envoys(Config cfg) {
    Envoy *envoys = malloc(sizeof(Envoy) * cfg.num_envoys);
    if (!envoys) {
        printF("ERROR: Cannot allocate envoys.");
        return NULL;
    } else {
        for (int i = 0; i < cfg.num_envoys; i++) {
        envoys[i].id = i + 1;
        envoys[i].status = FREE;
        }
    }
    return envoys;
}

void free_envoys(Envoy *envoys){
    if (!envoys) {
        return;
    } else {
        free(envoys);
    }
}