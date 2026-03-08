#include "maester.h"

int main(int argc, char *argv[]) {
    Maester maester;
    char *line;
    char *msg = NULL;
    int keep_running = 1;

    if (argc < 3) {
        printF("Usage: Maester <config.dat> <stock.db>\n");
        return 1;
    }

    setup_signals(&keep_running);

    maester = load_maester(argv[1], argv[2]);

    asprintf(&msg, "Maester of %s initialized. The board is set.\n", maester.config.realm);
    if (msg) {
        printF(msg);
        free(msg);
    }

    while (keep_running) {
        printF("$ ");
        line = readUntil(STDIN_FILENO, '\n');
        if (!line) break;
        str_trim(line);
        if (line[0] != '\0' && keep_running) {
            dispatch(&maester, line, &keep_running);
        }
        free(line);
    }

    free_maester(&maester);
    return 0;
}
