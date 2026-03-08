#include "utils.h"
#include "config.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

void remove_ampersand(char *s) {
    int i;
    int j;
    int len;

    if (!s) {
        return;
    }

    len = strlen(s);
    i = 0;
    while (i < len) {
        if (s[i] == '&') {
            for (j = i; j < len; j++) {
                s[j] = s[j + 1];
            }
            len--;
        } else {
            i++;
        }
    }
}

Config load_file(char *path) {
    Config cfg = {0};
    char *line;
    int fd = open(path, O_RDONLY);
    
    if (fd < 0) {
        printF("ERROR: Cannot open configuration file.");
        return cfg;
    } else {
        line = readUntil(fd, '\n');
        remove_ampersand(line);
        cfg.realm = strdup(line);
        free(line);

        line = readUntil(fd, '\n');
        cfg.path = strdup(line);
        free(line);

        line = readUntil(fd, '\n');
        cfg.num_envoys = atoi(line);
        free(line);

        line = readUntil(fd, '\n');
        cfg.ip = strdup(line);
        free(line);

        line = readUntil(fd, '\n');
        cfg.port = atoi(line);
        free(line);

        line = readUntil(fd, '\n');
        free(line);

        line = readUntil(fd, '\n');
        while (line != NULL) {
            char *name;
            char *ip;
            int port;

            name = strtok(line, " ");
            ip = strtok(NULL, " ");
            port = atoi(strtok(NULL, " "));
            remove_ampersand(name);

            if (strcmp(name, "DEFAULT") == 0) {
                cfg.has_default = 1;
                cfg.default_route.name = strdup(name);
                cfg.default_route.ip = strdup(ip);
                cfg.default_route.port = port;

            } else {
                cfg.routes = realloc(cfg.routes, sizeof(Route) * (cfg.num_routes + 1));

                cfg.routes[cfg.num_routes].name = strdup(name);
                cfg.routes[cfg.num_routes].ip = strdup(ip);
                cfg.routes[cfg.num_routes].port = port;

                if (strcmp(ip, "*.*.*.*") == 0) {
                    cfg.routes[cfg.num_routes].known = 0;
                } else {
                    cfg.routes[cfg.num_routes].known = 1;
                }
                cfg.num_routes++;
            }
            free(line);
            line = readUntil(fd, '\n');
        }
        free(line);
        close(fd);
    }

    return cfg;
}

void free_config(Config *cfg){
    if (!cfg) {
        return;
    } else {
        free(cfg->realm);
        free(cfg->path);
        free(cfg->ip);

        if (cfg->has_default) {
            free(cfg->default_route.name);
            free(cfg->default_route.ip);
        }

        for (int i = 0; i < cfg->num_routes; i++) {
            free(cfg->routes[i].name);
            free(cfg->routes[i].ip);
        }

        free(cfg->routes);

        cfg->realm = NULL;
        cfg->path = NULL;
        cfg->ip = NULL;
        cfg->routes = NULL;
        cfg->num_routes = 0;
        cfg->has_default = 0;
    }  
}
