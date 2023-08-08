#include <pthread.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <assert.h>

#include "search.h"
#include "movegen.h"
#include "eval.h"
#include "../util/vec.h"
#include "../util/math.h"

#define COPY_BOARD true

static inline
bool quit_mutex_locked(pthread_mutex_t *mtx) {
    if(pthread_mutex_trylock(mtx) == 0) {
        pthread_mutex_unlock(mtx);
        return false;
    }
    return true;
}

int32_t negamax(move_t *best_move, board_t *board, uint32_t depth, uint32_t ply, 
                int32_t alpha, int32_t beta, playercolor_t color, pthread_mutex_t *mtx)
{
    int32_t gameover = eval_gameover(board);
    if (gameover) {
        return 10000 * color * gameover;
    }

    if (depth == 0) {
        int32_t value = color * eval_board(board);
        return value;
    }
    
    int32_t eval;
    int32_t max_eval = INT32_MIN;
    move_t *movelist = v_get_movelist(board);
    assert(movelist != NULL);
    for (size_t i = 0; i < vec_get_length(movelist); ++i) {
        if (!quit_mutex_locked(mtx)) break;

        move_t move = movelist[i];

    #if COPY_BOARD
        board_t copy;
        memcpy(&copy, board, sizeof *board);
    #endif

        make_move(board, &move);
        eval = -negamax(best_move, board, depth - 1, ply + 1, -beta, -alpha, -color, mtx);

    #if COPY_BOARD
        memcpy(board, &copy, sizeof *board);
    #else
        unmake_move(board, &move);
    #endif

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

void *thread_search(void *arg) {
    searchinfo_t *sinfo = arg;

    move_t *best_move = sinfo->best_move;
    board_t *board = sinfo->board;
    playercolor_t client_color = sinfo->client_color;
    pthread_mutex_t *mtx_quit = sinfo->mtx_quit;

    uint32_t cur_depth = 1;
    move_t cur_best_move;
    move_invalid(&cur_best_move);

    /* Iterative deepening loop */
    while (true) {
        negamax(&cur_best_move, board, cur_depth, 0, -INT32_MAX, INT32_MAX, client_color, mtx_quit);

        if (!quit_mutex_locked(mtx_quit)) {
            return NULL;
        }

        printf("depth: %d | best move: ", cur_depth); print_move(&cur_best_move);

        memcpy(best_move, &cur_best_move, sizeof *best_move);

        cur_depth++;
    }
}
