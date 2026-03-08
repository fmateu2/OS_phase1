#ifndef ALLIANCE_H
#define ALLIANCE_H

typedef enum {
    PENDING,
    ON_PENDING,
    ALLIED,
    FAILED,
    REJECTED
} AllianceStatus;

typedef struct {
    char *realm;
    AllianceStatus status;
} Alliance;

Alliance *init_alliances(void);
void free_alliances(Alliance *alliances, int count);

#endif
