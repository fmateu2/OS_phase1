#include "alliance.h"
#include "utils.h"

Alliance *init_alliances(void) {
    return NULL;
}

void free_alliances(Alliance *alliances, int count) {
    if (!alliances) return;
    for (int i = 0; i < count; i++) {
        free(alliances[i].realm);
    }
    free(alliances);
}
