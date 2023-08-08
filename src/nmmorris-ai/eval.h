#ifndef EVAL_H
#define EVAL_H

#include "../defs.h"
#include "board.h"

int32_t eval_board(board_t *board);
int32_t eval_gameover(board_t *board);

#endif /* EVAL_H */
