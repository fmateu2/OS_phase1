#include "maester.h"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


/* Remove all '&' characters from string in-place */
void strip_ampersand(char *s)
{
    char *r = s, *w = s;
    while (*r) {
        if (*r != '&') *w++ = *r;
        r++;
    }
    *w = '\0';
}

/* Parse a single route line: "NAME ip port" or "DEFAULT ip port" */
static int parse_route_line(const char *line, route_t *r)
{
    char name[128], ip[32];
    int  port;
    int  n;

    /* try "NAME ip port" */
    n = 0;
    {
        /* manual parse: token1 token2 token3 */
        const char *p = line;
        int i = 0;
        /* name */
        while (*p && !isspace((unsigned char)*p) && i < 127) name[i++] = *p++;
        name[i] = '\0';
        while (*p && isspace((unsigned char)*p)) p++;
        /* ip */
        i = 0;
        while (*p && !isspace((unsigned char)*p) && i < 31) ip[i++] = *p++;
        ip[i] = '\0';
        while (*p && isspace((unsigned char)*p)) p++;
        /* port */
        port = 0;
        while (*p && isdigit((unsigned char)*p)) { port = port * 10 + (*p - '0'); p++; }
        n = (name[0] && ip[0]) ? 3 : 0;
    }

    if (n < 3) return -1;

    strncpy(r->name, name, sizeof(r->name) - 1);
    r->name[sizeof(r->name) - 1] = '\0';
    strip_ampersand(r->name);

    /* detect wildcard */
    if (strncmp(ip, "*.*.*.*", 7) == 0) {
        r->wildcard = 1;
        strncpy(r->ip, "*.*.*.*", sizeof(r->ip) - 1);
        r->port = 0;
    } else {
        r->wildcard = 0;
        strncpy(r->ip, ip, sizeof(r->ip) - 1);
        r->ip[sizeof(r->ip) - 1] = '\0';
        r->port = port;
    }

    return 0;
}

int load_config(const char *path, maester_t *cfg)
{
    int  fd;
    char line[512];
    int  section_routes = 0;
    int  line_num = 0;

    memset(cfg, 0, sizeof(*cfg));
    cfg->routes     = NULL;
    cfg->num_routes = 0;
    cfg->has_default = 0;

    fd = open(path, O_RDONLY);
    if (fd < 0) {
        wprintln("ERROR: Cannot open configuration file.");
        return -1;
    }

    while (readline_fd(fd, line, sizeof(line)) >= 0) {
        str_trim(line);
        if (line[0] == '\0') continue;  /* skip blank */

        if (!section_routes) {
            switch (line_num) {
                case 0:
                    strncpy(cfg->realm, line, sizeof(cfg->realm) - 1);
                    strip_ampersand(cfg->realm);
                    break;
                case 1:
                    strncpy(cfg->folder, line, sizeof(cfg->folder) - 1);
                    break;
                case 2:
                    cfg->num_envoys = atoi(line);
                    break;
                case 3:
                    strncpy(cfg->ip, line, sizeof(cfg->ip) - 1);
                    break;
                case 4:
                    cfg->port = atoi(line);
                    break;
                default:
                    if (strcmp(line, "--- ROUTES ---") == 0)
                        section_routes = 1;
                    break;
            }
            line_num++;
            /* detect routes header on line 5 */
            if (line_num == 5 && strcmp(line, "--- ROUTES ---") == 0)
                section_routes = 1;
        } else {
            /* inside ROUTES section */
            if (strcmp(line, "--- ROUTES ---") == 0) continue;

            route_t r;
            memset(&r, 0, sizeof(r));
            if (parse_route_line(line, &r) == 0) {
                if (strcmp(r.name, "DEFAULT") == 0) {
                    cfg->default_route = r;
                    cfg->has_default   = 1;
                } else {
                    route_t *tmp = realloc(cfg->routes,
                                   sizeof(route_t) * (cfg->num_routes + 1));
                    if (!tmp) {
                        wprintln("ERROR: Out of memory loading routes.");
                        close(fd);
                        return -1;
                    }
                    cfg->routes = tmp;
                    cfg->routes[cfg->num_routes++] = r;
                }
            }
        }
    }

    close(fd);
    return 0;
}

void free_config(maester_t *cfg)
{
    if (cfg->routes) {
        free(cfg->routes);
        cfg->routes = NULL;
    }
    cfg->num_routes = 0;
}
