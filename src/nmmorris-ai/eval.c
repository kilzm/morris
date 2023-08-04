#include <stdio.h>

#include "eval.h"
#include "morris.h"

static inline
i32 eval_pieces(board_t *board)
{
    return (board->white.set_pieces + board->white.board_pieces) - 
           (board->black.set_pieces + board->black.board_pieces);
}

static inline
i32 count_mills(bitboard_t board)
{
    i32 mill_count = 0;
    for (i32 i = 0; i < 16; ++i) {
        if (check_mask(board, mills[i])) {
            mill_count++;
        }
    }
    return mill_count;
}

static inline
i32 eval_mills(board_t *board)
{
    return count_mills(board->white.board) - count_mills(board->black.board);
}

static inline
i32 count_moves(playerboard_t *board, bitboard_t empty)
{
    i32 move_count = 0;
    boardpos_t pos;
    for (u16 i = 0; i < 9; ++i) {
        pos = board->pieces[i];
        if (pos < A0) continue;
        bitboard_t avail_moves = empty & adjacentb[pos];
        move_count += __builtin_popcount(avail_moves);
    }
    return move_count;
}

static inline
i32 eval_moves(board_t *board)
{
    bitboard_t empty = get_empty(board);
    return count_moves(&board->white, empty) - count_moves(&board->black, empty);
}

i32 eval_board(board_t *board, playercolor_t client_colo)
{
    i32 evaluation = 0;
    i32 pcs = eval_pieces(board);
    i32 mls = eval_mills(board);
    i32 mvs = eval_moves(board);
    evaluation += 10 * pcs;
    evaluation += 8 * mls;
    evaluation += 5 * mvs;
    //printf("pcs: %d, mls: %d, mvs: %d\n", pcs, mls, mvs);
    if (!board->white_to_move != (client_colo == WHITE)) {
        evaluation *= -1;
    }
    return evaluation;

}

i32 eval_gameover(board_t *board)
{
    if (board->black.phase == GMO) {
        return 1;
    } 
    if (board->white.phase == GMO) {
        return -1;
    }
    return 0;

}
