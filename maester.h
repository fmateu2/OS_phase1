#define _POSIX_C_SOURCE 200809L
#ifndef MAESTER_H
#define MAESTER_H

#include <sys/types.h>
#include <strings.h>

/* ─── Routing entry ─── */
typedef struct {
    char  name[128];
    char  ip[32];
    int   port;
    int   wildcard;   /* 1 if *.*.*.* */
} route_t;

/* ─── Maester configuration ─── */
typedef struct {
    char     realm[128];
    char     folder[256];
    int      num_envoys;
    char     ip[32];
    int      port;
    route_t *routes;
    int      num_routes;
    route_t  default_route;
    int      has_default;
} maester_t;

/* ─── Inventory product ─── */
typedef struct {
    char  name[100];
    int   amount;
    float weight;
} product_t;

/* ─── Alliance status ─── */
typedef enum {
    ALLIANCE_PENDING,
    ALLIANCE_ON_PENDING,
    ALLIANCE_ALLIED,
    ALLIANCE_FAILED,
    ALLIANCE_REJECTED
} alliance_status_t;

typedef struct {
    char             realm[128];
    alliance_status_t status;
} alliance_t;

/* ─── Envoy status ─── */
typedef enum {
    ENVOY_FREE,
    ENVOY_ON_MISSION
} envoy_status_t;

typedef struct {
    int            id;
    envoy_status_t status;
    char           mission[256];
} envoy_t;

/* ─── Global app state ─── */
typedef struct {
    maester_t   config;
    product_t  *inventory;
    int         inv_count;
    alliance_t *alliances;
    int         alliance_count;
    envoy_t    *envoys;
    char        stock_path[512];
} app_state_t;

/* ─── Function prototypes ─── */

/* config.c */
int  load_config(const char *path, maester_t *cfg);
void free_config(maester_t *cfg);
void strip_ampersand(char *s);

/* inventory.c */
int  load_inventory(const char *path, product_t **inv, int *count);
int  save_inventory(const char *path, product_t *inv, int count);
void free_inventory(product_t *inv);
void display_inventory(product_t *inv, int count);

/* commands.c */
void run_command_loop(app_state_t *state);

/* trade.c */
void start_trade_session(app_state_t *state, const char *realm);

/* utils.c */
void  wprint(const char *msg);
void  wprintln(const char *msg);
void  wprint_int(int n);
int   readline_fd(int fd, char *buf, int maxlen);
void  str_toupper(char *dst, const char *src, int maxlen);
void  str_trim(char *s);
int   str_startswith(const char *s, const char *prefix);

/* signals.c */
void setup_signals(void);
extern volatile int keep_running;

#endif /* MAESTER_H */
