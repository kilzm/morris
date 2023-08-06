#ifndef DEFS_H
#define DEFS_H

#include <stdint.h>
#include <stdbool.h>

#define BUF_SIZE 1024
#define ID_LEN 14
#define CONF_LEN 32
#define NAME_LEN 100

#define _XOPEN_SOURCE 700

typedef enum err {
    NO_ERROR = 0,
    INVALID_ARGUMENT = -1,
    COMMUNICATION_ERROR = -2,
    SOCK_ERROR = -3,
    SEND_ERROR = -4,
    RECV_ERROR = -5,
    SERVER_ERROR = -6,
    POLL_ERROR = -7,
} err_t;

#define err_check_return(err_code) { if (err_code < 0) { return err_code; } }

#endif /* DEFS_H */
