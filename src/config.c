#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

void read_config(const char *path, config_t *conf)
{
    char buf[BUF_SIZE];
    FILE *conf_file = fopen(path, "r");
    char *key, *val;
    while (fgets(buf, BUF_SIZE, conf_file)) {
        if (buf[0] == '#') continue;
        key = strtok(buf, " =\t\n");
        val = strtok(NULL, " =\t\n");
        if (!key || !val) continue;

        if (strncmp(key, "HOST", NAME_LEN) == 0) {
            strncpy(conf->host, val, NAME_LEN - 1);
        } else if (strncmp(key, "PORT", NAME_LEN) == 0) {
            conf->port = (u16) atoi(val);
        } else if (strncmp(key, "GAME", NAME_LEN) == 0) {
            strncpy(conf->gamekind, val, NAME_LEN - 1);
        }
    }
}