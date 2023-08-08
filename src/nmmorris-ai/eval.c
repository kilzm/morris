#include <stdio.h>

#include "eval.h"
#include "morris.h"

static inline
int32_t eval_pieces(board_t *board)
{
    return (board->white.set_pieces + board->white.board_pieces) - 
           (board->black.set_pieces + board->black.board_pieces);
}

static inline
int32_t count_mills(bitboard_t board)
{
    int32_t mill_count = 0;
    for (int32_t i = 0; i < 16; ++i) {
        if (check_mask(board, mills[i])) {
            mill_count++;
        }
    }
    return mill_count;
}

static inline
int32_t eval_mills(board_t *board)
{
    return count_mills(board->white.board) - count_mills(board->black.board);
}

static inline
int32_t count_moves(playerboard_t *board, bitboard_t empty)
{
    int32_t move_count = 0;
    boardpos_t pos;
    for (uint16_t i = 0; i < 9; ++i) {
        pos = board->pieces[i];
        if (pos < A0) continue;
        bitboard_t avail_moves = empty & adjacentb[pos];
        move_count += __builtin_popcount(avail_moves);
    }
    return move_count;
}

static inline
int32_t eval_moves(board_t *board)
{
    bitboard_t empty = get_empty(board);
    return count_moves(&board->white, empty) - count_moves(&board->black, empty);
}

int32_t eval_gameover(board_t *board)
{
    if (board->black.phase == GMO) {
        return 1;
    } 
    if (board->white.phase == GMO) {
        return -1;
    }
    return 0;
}

int32_t eval_board(board_t *board)
{
    int32_t evaluation = 0;
    evaluation += 20 * eval_pieces(board);
    evaluation += 10 * eval_mills(board);
    evaluation += 3 * eval_moves(board);
    return evaluation;
}
