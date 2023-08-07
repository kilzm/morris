#include <pthread.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>

#include "search.h"
#include "movegen.h"
#include "eval.h"
#include "../util/vec.h"
#include "../util/math.h"

int32_t negamax(move_t *best_move, board_t *board, uint32_t depth, uint32_t ply, 
                int32_t alpha, int32_t beta, playercolor_t color)
{
    if (depth == 0) {
        int32_t value = color * eval_board(board);
        return value;
    }
    
    int32_t eval;
    int32_t max_eval = INT32_MIN;
    move_t *movelist = v_get_movelist(board);
    for (size_t i = 0; i < vec_get_length(movelist); ++i) {
        move_t move = movelist[i];
        make_move(board, &move);
        eval = -negamax(best_move, board, depth - 1, ply + 1, -beta, -alpha, -color);
        unmake_move(board, &move);

        if (ply == 0) {
            print_move(&move);
            printf("%d\n", eval);
        }

        max_eval = max(eval, max_eval);
        if (max_eval > alpha) {
            alpha = max_eval;
            if (ply == 0) {
                memcpy(best_move, &move, sizeof *best_move);
            }
        }

        if (alpha >= beta) {
            break;
        }

    }
    vec_free(movelist);
    return max_eval;
}

void search(move_t *best_move, board_t *board)
{
    playercolor_t client_color = board->white_to_move ? WHITE : BLACK;
    negamax(best_move, board, 3, 0, -100000, 100000, client_color);
}

void *thread_search(void *arg) {
    searchinfo_t *sinfo = arg;
    search(sinfo->best_move, sinfo->board);
}

// void search(move_t *best_move, board_t *board, time_t time_to_move)
// {
//     move_t garbage;
//     move_t *movelist = v_get_movelist(board);
//     int32_t eval, best_eval = INT32_MIN;
//     playercolor_t client_color = board->white_to_move ? WHITE : BLACK;
//     for (size_t i = 0; i < vec_get_length(movelist); ++i) {
//         make_move(board, &movelist[i]);
//         eval = negamax(&garbage, board, 2, 0, INT32_MIN, INT32_MAX, client_color);
//         unmake_move(board, &movelist[i]);
//         print_move(&movelist[i]);
//         printf(": %d\n", eval);
//         if (eval > best_eval) {
//             best_eval = eval;
//             memcpy(best_move, &movelist[i], sizeof *best_move);
//         }
//     }
//     vec_free(movelist);
// }
