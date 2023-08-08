#ifndef SEARCH_H
#define SEARCH_H

#include <time.h>
#include <pthread.h>

#include "../defs.h"
#include "board.h"
#include "moves.h"

void *thread_search(void *arg);

typedef struct searchinfo {
    move_t *best_move;
    board_t *board;
    playercolor_t client_color;
    pthread_mutex_t *mtx_quit;
} searchinfo_t;

#endif /* SEARCH_H */