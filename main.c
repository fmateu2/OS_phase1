#include "maester.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const char *CITADEL_ID = "VALAR-COMPILIS";

static void init_envoys(app_state_t *state)
{
    int n = state->config.num_envoys;
    if (n <= 0) n = 1;

    state->envoys = malloc(sizeof(envoy_t) * n);
    if (!state->envoys) {
        wprintln("ERROR: Cannot allocate envoys.");
        return;
    }
    for (int i = 0; i < n; i++) {
        state->envoys[i].id     = i + 1;
        state->envoys[i].status = ENVOY_FREE;
        state->envoys[i].mission[0] = '\0';
    }
}

/* Populate alliance table from route list (all start PENDING for display) */
static void init_alliances(app_state_t *state)
{
    int n = state->config.num_routes;
    if (n <= 0) { state->alliances = NULL; state->alliance_count = 0; return; }

    state->alliances = malloc(sizeof(alliance_t) * n);
    if (!state->alliances) {
        wprintln("ERROR: Cannot allocate alliance table.");
        return;
    }
    for (int i = 0; i < n; i++) {
        strncpy(state->alliances[i].realm, state->config.routes[i].name,
                sizeof(state->alliances[0].realm) - 1);
        state->alliances[i].realm[sizeof(state->alliances[0].realm)-1] = '\0';
        state->alliances[i].status = ALLIANCE_PENDING;
    }
    state->alliance_count = n;
}

static void free_state(app_state_t *state)
{
    free_config(&state->config);
    free_inventory(state->inventory);
    if (state->alliances) { free(state->alliances); state->alliances = NULL; }
    if (state->envoys)    { free(state->envoys);    state->envoys    = NULL; }
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        wprintln("Usage: maester <config.dat> <stock.db>");
        return 1;
    }

    setup_signals();

    app_state_t state;
    memset(&state, 0, sizeof(state));

    /* Load config */
    if (load_config(argv[1], &state.config) < 0) {
        wprintln("ERROR: Failed to load configuration.");
        return 1;
    }

    /* Build stock path */
    strncpy(state.stock_path, argv[2], sizeof(state.stock_path) - 1);

    /* Load inventory */
    if (load_inventory(state.stock_path, &state.inventory, &state.inv_count) < 0) {
        wprintln("WARNING: Inventory could not be loaded. Starting empty.");
        state.inventory  = NULL;
        state.inv_count  = 0;
    }

    /* Init envoys and alliances */
    init_envoys(&state);
    init_alliances(&state);

    /* Greeting */
    wprint("Maester of ");
    wprint(state.config.realm);
    wprintln(" initialized. The board is set.");

    /* Command loop */
    run_command_loop(&state);

    /* Cleanup */
    free_state(&state);
    return 0;
}
