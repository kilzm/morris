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

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t  i8;

#define true 1
#define false 0

#endif /* DEFS_H */
