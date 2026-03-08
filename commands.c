#include "maester.h"

void dispatch(Maester *maester, char *line) {
    char *upper = NULL;
    char *ucopy = NULL;
    char *orig  = NULL;
    char *token = NULL;
    char *arg1  = NULL;
    char *arg2  = NULL;
    char *buffer = NULL;
    char *realm  = NULL;
    int i = 0;

    upper = malloc(strlen(line) + 1);
    ucopy = malloc(strlen(line) + 1);
    orig  = malloc(strlen(line) + 1);

    if (!upper || !ucopy || !orig) {
        free(upper);
        free(ucopy);
        free(orig);
        return;
    }

    str_toupper_copy(upper, line, strlen(line) + 1);
    strcpy(ucopy, upper);
    strcpy(orig, line);

    token = strtok(ucopy, " ");

    if (!token) {
        printF("Unknown command\n");
        free(upper);
        free(ucopy);
        free(orig);
        return;
    }


    if (strcmp(token, "EXIT") == 0) {
        if (strtok(NULL, " ") != NULL) {
            printF("Unknown command\n");
            free(upper);
            free(ucopy);
            free(orig);
            return;
        }
        asprintf(&buffer, "The Maester of %s signs off. The ravens rest.\n", maester->config.realm);
        if (buffer) {
            printF(buffer);
            free(buffer);
        }
        keep_running = 0;
        free(upper);
        free(ucopy);
        free(orig);
        return;
    }

   

    if (strcmp(token, "LIST") == 0) {

        arg1 = strtok(NULL, " ");

        if (!arg1) {
            printF("Missing arguments. Did you mean LIST REALMS or LIST PRODUCTS?\n");
            free(upper);
            free(ucopy);
            free(orig);
            return;
        }

        if (strcmp(arg1, "REALMS") == 0) {
            if (strtok(NULL, " ") != NULL) {
                printF("Unknown command\n");
                free(upper);
                free(ucopy);
                free(orig);
                return;
            }
            for (i = 0; i < maester->config.num_routes; i++) {
                asprintf(&buffer, "- %s\n", maester->config.routes[i].name);
                if (buffer) {
                    printF(buffer);
                    free(buffer);
                    buffer = NULL;
                }
            }
            free(upper);
            free(ucopy);
            free(orig);
            return;
        }

        if (strcmp(arg1, "PRODUCTS") == 0) {
            arg2 = strtok(NULL, " ");
            if (!arg2) {
                display_inventory(maester->inventory);
                free(upper);
                free(ucopy);
                free(orig);
                return;
            }
            if (strtok(NULL, " ") != NULL) {
                printF("Unknown command\n");
                free(upper);
                free(ucopy);
                free(orig);
                return;
            }
            printF("Command OK\n");
            free(upper);
            free(ucopy);
            free(orig);
            return;
        }

        printF("Unknown command\n");
        free(upper);
        free(ucopy);
        free(orig);
        return;
    }



    if (strcmp(token, "PLEDGE") == 0) {

        arg1 = strtok(NULL, " ");

        if (!arg1) {
            printF("Did you mean to send a pledge? Please review syntax.\n");
            free(upper);
            free(ucopy);
            free(orig);
            return;
        }

        if (strcmp(arg1, "STATUS") == 0) {
            if (strtok(NULL, " ") != NULL) {
                printF("Unknown command\n");
                free(upper);
                free(ucopy);
                free(orig);
                return;
            }
            printF("Command OK\n");
            free(upper);
            free(ucopy);
            free(orig);
            return;
        }

        if (strcmp(arg1, "RESPOND") == 0) {
            char *r = strtok(NULL, " ");
            char *decision = strtok(NULL, " ");

            if (!r || !decision) {
                printF("Did you mean to respond to a pledge? Please review syntax.\n");
                free(upper);
                free(ucopy);
                free(orig);
                return;
            }
            if (strtok(NULL, " ") != NULL) {
                printF("Unknown command\n");
                free(upper);
                free(ucopy);
                free(orig);
                return;
            }
            printF("Command OK\n");
            free(upper);
            free(ucopy);
            free(orig);
            return;
        }

        arg2 = strtok(NULL, " ");

        if (!arg2) {
            printF("Did you mean to send a pledge? Please review syntax.\n");
            free(upper);
            free(ucopy);
            free(orig);
            return;
        }
        if (strtok(NULL, " ") != NULL) {
            printF("Unknown command\n");
            free(upper);
            free(ucopy);
            free(orig);
            return;
        }
        printF("Command OK\n");
        free(upper);
        free(ucopy);
        free(orig);
        return;
    }

   

    if (strcmp(token, "START") == 0) {

        arg1 = strtok(NULL, " ");

        if (!arg1 || strcmp(arg1, "TRADE") != 0) {
            printF("Unknown command\n");
            free(upper);
            free(ucopy);
            free(orig);
            return;
        }

        arg2 = strtok(NULL, " ");

        if (!arg2) {
            printF("Missing arguments, can't start a trade. Please review the syntax.\n");
            free(upper);
            free(ucopy);
            free(orig);
            return;
        }
        if (strtok(NULL, " ") != NULL) {
            printF("Unknown command\n");
            free(upper);
            free(ucopy);
            free(orig);
            return;
        }

        strtok(orig, " ");
        strtok(NULL, " ");
        realm = strtok(NULL, " ");
        start_trade_session(maester, realm);
        free(upper);
        free(ucopy);
        free(orig);
        return;
    }

   
    if (strcmp(token, "ENVOY") == 0) {
        arg1 = strtok(NULL, " ");
        if (!arg1 || strcmp(arg1, "STATUS") != 0) {
            printF("Missing arguments. Did you mean ENVOY STATUS?\n");
            free(upper);
            free(ucopy);
            free(orig);
            return;
        }
        if (strtok(NULL, " ") != NULL) {
            printF("Unknown command\n");
            free(upper);
            free(ucopy);
            free(orig);
            return;
        }
        printF("Command OK\n");
        free(upper);
        free(ucopy);
        free(orig);
        return;
    }


    printF("Unknown command\n");
    free(upper);
    free(ucopy);
    free(orig);
}
