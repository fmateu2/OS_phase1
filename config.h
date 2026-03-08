#define _GNU_SOURCE
#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    char *name;
    char *ip;
    int port;
    int known;
} Route;

typedef struct {
    char *realm;
    char *path;
    int num_envoys;
    char *ip;
    int port;
    Route *routes;
    int num_routes;
    Route default_route;
    int has_default;
} Config;

Config load_file(char *path);
void remove_ampersand(char *s);
void free_config(Config *cfg);


#endif

