#include "alliance.h"
#include "utils.h"

Alliance *load_aliances(Config cfg) {
    Alliance *alliances = malloc(sizeof(Alliance) * cfg.num_routes);
    if (!alliances) {
        printF("ERROR: Cannot allocate alliances.");
        return NULL;
    } else {
        for (int i = 0; i < cfg.num_routes; i++) {
            alliances[i].realm = strdup(cfg.routes[i].name);
            alliances[i].alliance = PENDING;
        }
    }
    return alliances;    
}

void free_alliances(Alliance *alliances, Config cfg){
    if (!alliances) {
        return;
    } else {
        for (int i = 0; i < cfg.num_routes; i++) {
            free(alliances[i].realm);
        }
        free(alliances);
    }
}