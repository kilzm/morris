#ifndef CONFIG_H
#define CONFIG_H

#include "defs.h"

typedef struct config {
    char host[NAME_LEN];
    uint16_t port;
    char gamekind[NAME_LEN];
} config_t;

void read_config(const char *path, config_t *conf);

#endif /* CONFIG_H */