#ifndef SEARCH_H
#define SEARCH_H

#include <time.h>

#include "../defs.h"
#include "board.h"
#include "moves.h"

void search(move_t *best_move, board_t *board, time_t time_to_move);

#endif /* SEARCH_H */