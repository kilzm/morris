#ifndef CONNECT_H
#define CONNECT_H

#define TIMEOUT_MS 3000

#include "shm.h"

err_t perform_connection(clientinfo_t *ci, int32_t pipe_fd);

#endif /* CONNECT_H */