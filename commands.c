#include <string.h>
#include <stdlib.h>
#include "maester.h"

void dispatch(Maester *maester, char *line) {
    char *upper;
    char *token;
    char *buffer;
    char *arg1;
    char *arg2;

    upper = str_toupper(line);

    token = strtok(upper, " ");

    if (!token) {
        printF("Unknown command\n");
        return;
    }

    /* ================= EXIT ================= */

    if (strcmp(token, "EXIT") == 0) {
        asprintf(&buffer, "The Maester of %s signs off. The ravens rest.\n", maester->config.realm);
        printF(buffer);
        free(buffer);
        exit(0);
    }

    /* ================= LIST ================= */

    if (strcmp(token, "LIST") == 0) {

        arg1 = strtok(NULL, " ");
        arg2 = strtok(NULL, " ");

        if (!arg1) {
            printF("Unknown command\n");
            return;
        }

        if (strcmp(arg1, "REALMS") == 0) {

            if (arg2 != NULL) {
                printF("Unknown command\n");
                return;
            }

            for (int i = 0; i < maester->config.num_routes; i++) {
                asprintf(&buffer, "- %s\n", maester->config.routes[i].name);
                printF(buffer);
                free(buffer);
            }

            return;
        }

        if (strcmp(arg1, "PRODUCTS") == 0) {

            if (!arg2) {
                display_inventory(maester->inventory);
                return;
            }

            if (strtok(NULL, " ") != NULL) {
                printF("Unknown command\n");
                return;
            }

            printF("Command OK\n");
            return;
        }

        printF("Unknown command\n");
        return;
    }

    /* ================= PLEDGE ================= */

    if (strcmp(token, "PLEDGE") == 0) {

        arg1 = strtok(NULL, " ");

        if (!arg1) {
            printF("Did you mean to send a pledge? Please review syntax.\n");
            return;
        }

        if (strcmp(arg1, "RESPOND") == 0) {

            char *realm = strtok(NULL, " ");
            char *decision = strtok(NULL, " ");

            if (!realm || !decision) {
                printF("Did you mean to respond to a pledge? Please review syntax.\n");
                return;
            }

            if (strtok(NULL, " ") != NULL) {
                printF("Unknown command\n");
                return;
            }

            printF("Command OK\n");
            return;
        }

        arg2 = strtok(NULL, " ");

        if (!arg2) {
            printF("Did you mean to send a pledge? Please review syntax.\n");
            return;
        }

        if (strtok(NULL, " ") != NULL) {
            printF("Unknown command\n");
            return;
        }

        printF("Command OK\n");
        return;
    }

    /* ================= START TRADE ================= */

    if (strcmp(token, "START") == 0) {

        arg1 = strtok(NULL, " ");

        if (!arg1 || strcmp(arg1, "TRADE") != 0) {
            printF("Unknown command\n");
            return;
        }

        arg2 = strtok(NULL, " ");

        if (!arg2) {
            printF("Missing arguments, can't start a trade. Please review the syntax.\n");
            return;
        }

        if (strtok(NULL, " ") != NULL) {
            printF("Unknown command\n");
            return;
        }

        start_trade_session(maester, arg2);
        return;
    }

    /* ================= STATUS ================= */

    if (strcmp(token, "PLEDGE") == 0) {

        arg1 = strtok(NULL, " ");

        if (arg1 && strcmp(arg1, "STATUS") == 0) {
            printF("Command OK\n");
            return;
        }
    }

    if (strcmp(token, "ENVOY") == 0) {

        arg1 = strtok(NULL, " ");

        if (arg1 && strcmp(arg1, "STATUS") == 0) {
            printF("Command OK\n");
            return;
        }
    }

    /* ================= UNKNOWN ================= */

    printF("Unknown command\n");
}