#ifndef SEARCH_H
#define SEARCH_H

#include <time.h>

#include "../defs.h"
#include "board.h"
#include "moves.h"

void search(move_t *best_move, board_t *board);
void *thread_search(void *arg);

typedef struct searchinfo {
    move_t *best_move;
    board_t *board;
} searchinfo_t;

#endif /* SEARCH_H */