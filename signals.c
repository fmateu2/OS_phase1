#include "maester.h"
#include <signal.h>

int *g_keep_running = NULL;

void signal_handler(int sig) {
    (void)sig;
    if (g_keep_running != NULL) {
        *g_keep_running = 0;
    }
}

void setup_signals(int *keep_running) {
    struct sigaction sa;
    g_keep_running = keep_running;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT,  &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}