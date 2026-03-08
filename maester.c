#include "maester.h"

Maester load_maester(char *config_path, char *inventory_path) {
    Maester maester = {0};
    maester.config       = load_file(config_path);
    maester.inventory    = load_inventory(inventory_path);
    maester.envoys       = load_envoys(maester.config);
    maester.num_envoys   = maester.config.num_envoys;
    maester.alliances    = init_alliances();
    maester.num_alliances = 0;
    return maester;
}

void free_maester(Maester *maester) {
    if (!maester) return;
    free_alliances(maester->alliances, maester->num_alliances);
    free_envoys(maester->envoys);
    free_inventory(&maester->inventory);
    free_config(&maester->config);
    maester->alliances    = NULL;
    maester->envoys       = NULL;
    maester->num_alliances = 0;
    maester->num_envoys   = 0;
}
