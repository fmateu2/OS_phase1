#include "maester.h"

int main(int argc, char *argv[]) {
    Config cfg;
    Inventory inv;
    int valid;

     if (argc < 3) {
        printf("Usage: maester <config.dat> <stock.db>\n");
        return 1;
    }

    cfg = load_config(argv[1]);
    inv = load_inventory(argv[2]);

    //init_envoys;
    //init alliances;

    a

    
}