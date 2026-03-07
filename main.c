#include "maester.h"
#include <unistd.h>

int main(int argc, char *argv[]) {
    Maester maester;
    int len;
    char *line;

    //falta inicialitzar senyals

     if (argc < 3) {
        printF("Usage: maester <config.dat> <stock.db>\n");
        return 1;
    } else {
        maester = load_maester(argv[1], argv[2]);
        while (1) {
            wprint("$ ");
            line = readUntil(STDIN_FILENO, '\n');
            if (!line) {
                break;
            }
            str_trim(line);
            dispatch(&maester, line);
            free(line);
        }
        free_maester(&maester);
    }
    return 0;
}