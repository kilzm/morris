#include <pthread.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>

#include "search.h"
#include "movegen.h"
#include "eval.h"
#include "../util/vec.h"
#include "../util/math.h"

i32 negamax(move_t *best_move, board_t *board, u32 depth, u32 ply_from_root, i32 alpha, i32 beta)
{   
    playercolor_t client_colo;
    if (ply_from_root == 0) {
        // change this back when search function is back to normal
        client_colo = !board->white_to_move ? WHITE : BLACK;
    }
    if (depth == 0) {
        return eval_board(board, client_colo);
    }
    i32 eval = INT32_MIN;
    move_t *movelist = v_get_movelist(board);
    for (size_t i = 0; i < vec_get_length(movelist); ++i) {
        make_move(board, &movelist[i]);
        eval = -negamax(best_move, board, depth - 1, ply_from_root + 1, INT32_MIN, INT32_MAX);
        unmake_move(board, &movelist[i]);
        if (eval > alpha) {
            alpha = eval;
            if (ply_from_root == 0) {
                memcpy(best_move, &movelist[i], sizeof *best_move);
            }
        }
        if (eval >= beta) {
            return beta;
        }
    }
    vec_free(movelist);
    return eval;
}

//volatile bool abort_search;
/*
void search(move_t *best_move, board_t *board, time_t time_to_move)
{
    negamax(best_move, board, 4, 0, INT32_MIN, INT32_MAX);
}
*/

void search(move_t *best_move, board_t *board, time_t time_to_move)
{
    move_t garbage;
    move_t *movelist = v_get_movelist(board);
    i32 eval, best_eval = INT32_MIN;
    for (i32 i = 0; i < vec_get_length(movelist); ++i) {
        make_move(board, &movelist[i]);
        eval = negamax(&garbage, board, 4, 0, INT32_MIN, INT32_MAX);
        print_move(&movelist[i]);
        printf(": %d\n", eval);
        if (eval > best_eval) {
            best_eval = eval;
            memcpy(best_move, &movelist[i], sizeof *best_move);
        }
        unmake_move(board, &movelist[i]);
    }
    vec_free(movelist);
}
