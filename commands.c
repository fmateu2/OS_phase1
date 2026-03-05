#define _POSIX_C_SOURCE 200809L
#include "maester.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>


/* ── helpers ── */

static int next_free_envoy(app_state_t *state)
{
    for (int i = 0; i < state->config.num_envoys; i++)
        if (state->envoys[i].status == ENVOY_FREE) return i;
    return -1;
}

static const char *alliance_status_str(alliance_status_t s)
{
    switch (s) {
        case ALLIANCE_PENDING:    return "PENDING";
        case ALLIANCE_ON_PENDING: return "ON_PENDING";
        case ALLIANCE_ALLIED:     return "ALLIED";
        case ALLIANCE_FAILED:     return "FAILED";
        case ALLIANCE_REJECTED:   return "REJECTED";
        default:                  return "UNKNOWN";
    }
}

/* ── command handlers ── */

static void cmd_list_realms(app_state_t *state)
{
    wprintln("Known realms:");
    for (int i = 0; i < state->config.num_routes; i++) {
        wprint("  - ");
        wprintln(state->config.routes[i].name);
    }
}

static void cmd_list_products_own(app_state_t *state)
{
    if (state->inv_count == 0) {
        wprintln("Your inventory is empty.");
        return;
    }
    display_inventory(state->inventory, state->inv_count);
}

static void cmd_pledge_status(app_state_t *state)
{
    if (state->alliance_count == 0) {
        wprintln("No alliances recorded.");
        return;
    }
    for (int i = 0; i < state->alliance_count; i++) {
        wprint("- ");
        wprint(state->alliances[i].realm);
        wprint(": ");
        wprintln(alliance_status_str(state->alliances[i].status));
    }
}

static void cmd_envoy_status(app_state_t *state)
{
    for (int i = 0; i < state->config.num_envoys; i++) {
        wprint("- Envoy ");
        wprint_int(state->envoys[i].id);
        wprint(": ");
        if (state->envoys[i].status == ENVOY_FREE) {
            wprintln("FREE");
        } else {
            wprint("ON_MISSION (");
            wprint(state->envoys[i].mission);
            wprintln(")");
        }
    }
}

/* Stub pledge: mark envoy busy, record alliance PENDING */
static void cmd_pledge(app_state_t *state, const char *realm, const char *sigil)
{
    (void)sigil;

    /* check already allied */
    for (int i = 0; i < state->alliance_count; i++) {
        if (strcasecmp(state->alliances[i].realm, realm) == 0) {
            if (state->alliances[i].status == ALLIANCE_ALLIED) {
                wprint("Already allied with ");
                wprintln(realm);
                return;
            }
        }
    }

    int envoy_idx = next_free_envoy(state);
    if (envoy_idx < 0) {
        wprintln("ERROR: No free Envoys available.");
        return;
    }

    /* record alliance */
    int found = 0;
    for (int i = 0; i < state->alliance_count; i++) {
        if (strcasecmp(state->alliances[i].realm, realm) == 0) {
            state->alliances[i].status = ALLIANCE_PENDING;
            found = 1; break;
        }
    }
    if (!found) {
        alliance_t *tmp = realloc(state->alliances,
                          sizeof(alliance_t) * (state->alliance_count + 1));
        if (!tmp) { wprintln("ERROR: Out of memory."); return; }
        state->alliances = tmp;
        strncpy(state->alliances[state->alliance_count].realm, realm,
                sizeof(state->alliances[0].realm) - 1);
        state->alliances[state->alliance_count].realm[
            sizeof(state->alliances[0].realm)-1] = '\0';
        state->alliances[state->alliance_count].status = ALLIANCE_PENDING;
        state->alliance_count++;
    }

    /* assign envoy */
    state->envoys[envoy_idx].status = ENVOY_ON_MISSION;
    snprintf(state->envoys[envoy_idx].mission,
             sizeof(state->envoys[0].mission),
             "PLEDGE to %s", realm);

    wprint("Pledge sent to ");
    wprint(realm);
    wprint(". Envoy ");
    wprint_int(state->envoys[envoy_idx].id);
    wprintln(" is on its way.");
}

static void cmd_pledge_respond(const char *realm, const char *response)
{
    /* Phase 1 stub */
    (void)realm; (void)response;
    wprintln("Command OK");
}

static void cmd_list_products_remote(app_state_t *state, const char *realm)
{
    /* check alliance */
    int allied = 0;
    for (int i = 0; i < state->alliance_count; i++) {
        if (strcasecmp(state->alliances[i].realm, realm) == 0 &&
            state->alliances[i].status == ALLIANCE_ALLIED) {
            allied = 1; break;
        }
    }
    if (!allied) {
        wprint("ERROR: You must have an alliance with ");
        wprint(realm);
        wprintln(" to trade.");
        return;
    }
    /* Phase 1 stub */
    wprintln("Command OK");
}

/* ── main parser ── */

static void dispatch(app_state_t *state, const char *raw)
{
    char upper[512];
    char buf[512];

    strncpy(buf, raw, sizeof(buf) - 1);
    buf[sizeof(buf)-1] = '\0';
    str_trim(buf);
    if (buf[0] == '\0') return;

    str_toupper(upper, buf, sizeof(upper));

    /* ── EXIT ── */
    if (strcmp(upper, "EXIT") == 0) {
        wprint("The Maester of ");
        wprint(state->config.realm);
        wprintln(" signs off. The ravens rest.");
        keep_running = 0;
        return;
    }

    /* ── LIST REALMS ── */
    if (strcmp(upper, "LIST REALMS") == 0) {
        cmd_list_realms(state);
        return;
    }

    /* ── LIST PRODUCTS (no arg) ── */
    if (strcmp(upper, "LIST PRODUCTS") == 0) {
        cmd_list_products_own(state);
        return;
    }

    /* ── LIST PRODUCTS <REALM> ── */
    if (str_startswith(upper, "LIST PRODUCTS ")) {
        const char *realm = buf + strlen("LIST PRODUCTS ");
        while (*realm == ' ') realm++;
        if (*realm == '\0') {
            wprintln("Did you mean: LIST PRODUCTS <realm>? Please review syntax.");
            return;
        }
        cmd_list_products_remote(state, realm);
        return;
    }

    /* ── LIST (incomplete) ── */
    if (strcmp(upper, "LIST") == 0) {
        wprintln("Did you mean LIST REALMS or LIST PRODUCTS? Please review syntax.");
        return;
    }

    /* ── PLEDGE STATUS ── */
    if (strcmp(upper, "PLEDGE STATUS") == 0) {
        cmd_pledge_status(state);
        return;
    }

    /* ── PLEDGE RESPOND <REALM> ACCEPT|REJECT ── */
    if (str_startswith(upper, "PLEDGE RESPOND ")) {
        /* parse: PLEDGE RESPOND <realm> ACCEPT|REJECT */
        char tmp[512];
        strncpy(tmp, buf + strlen("PLEDGE RESPOND "), sizeof(tmp)-1);
        str_trim(tmp);
        /* find last token */
        char *last = NULL;
        for (char *p = tmp; *p; p++) if (*p == ' ') last = p;
        if (!last) {
            wprintln("Did you mean: PLEDGE RESPOND <realm> ACCEPT|REJECT? Please review syntax.");
            return;
        }
        *last = '\0';
        char *realm    = tmp;
        char *response = last + 1;
        char resp_up[16];
        str_toupper(resp_up, response, sizeof(resp_up));
        if (strcmp(resp_up, "ACCEPT") != 0 && strcmp(resp_up, "REJECT") != 0) {
            wprintln("Did you mean: PLEDGE RESPOND <realm> ACCEPT|REJECT? Please review syntax.");
            return;
        }
        cmd_pledge_respond(realm, resp_up);
        return;
    }

    /* ── PLEDGE <REALM> <sigil> ── */
    if (str_startswith(upper, "PLEDGE ")) {
        /* must NOT be PLEDGE STATUS or PLEDGE RESPOND (already handled) */
        char tmp[512];
        strncpy(tmp, buf + strlen("PLEDGE "), sizeof(tmp)-1);
        str_trim(tmp);
        /* split into realm and sigil */
        char *space = strchr(tmp, ' ');
        if (!space) {
            wprintln("Did you mean to send a pledge? Please review syntax: PLEDGE <realm> <sigil.jpg>");
            return;
        }
        *space = '\0';
        char *realm = tmp;
        char *sigil = space + 1;
        while (*sigil == ' ') sigil++;
        if (*sigil == '\0') {
            wprintln("Did you mean to send a pledge? Please review syntax: PLEDGE <realm> <sigil.jpg>");
            return;
        }
        cmd_pledge(state, realm, sigil);
        return;
    }

    /* ── PLEDGE (alone) ── */
    if (strcmp(upper, "PLEDGE") == 0) {
        wprintln("Did you mean to send a pledge? Please review syntax: PLEDGE <realm> <sigil.jpg>");
        return;
    }

    /* ── START TRADE <REALM> ── */
    if (str_startswith(upper, "START TRADE ")) {
        const char *realm = buf + strlen("START TRADE ");
        while (*realm == ' ') realm++;
        if (*realm == '\0') {
            wprintln("Did you mean: START TRADE <realm>? Please review syntax.");
            return;
        }
        start_trade_session(state, realm);
        return;
    }

    /* ── START TRADE (incomplete) ── */
    if (strcmp(upper, "START TRADE") == 0 || strcmp(upper, "START") == 0) {
        wprintln("Did you mean: START TRADE <realm>? Please review syntax.");
        return;
    }

    /* ── ENVOY STATUS ── */
    if (strcmp(upper, "ENVOY STATUS") == 0) {
        cmd_envoy_status(state);
        return;
    }

    /* ── ENVOY (incomplete) ── */
    if (strcmp(upper, "ENVOY") == 0) {
        wprintln("Did you mean: ENVOY STATUS? Please review syntax.");
        return;
    }

    /* ── Unknown ── */
    wprintln("Unknown command");
}

void run_command_loop(app_state_t *state)
{
    char line[512];

    while (keep_running) {
        wprint("$ ");
        int n = readline_fd(STDIN_FILENO, line, sizeof(line));
        if (n < 0) {
            /* EOF or SIGINT already set keep_running = 0 */
            break;
        }
        dispatch(state, line);
    }
}
