#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>

#include "think.h"
#include "moves.h"
#include "movegen.h"
#include "search.h"
#include "../util/vec.h"
#include "../util/math.h"

move_t current_move;

void rthink(char *movestr, piece_t *pc, int16_t num_caps, playerinfo_t *client) 
{
    board_t board;
    set_board(&board, pc, client);
    print_gameboard(&board);
    if (num_caps == 0) {
        move_t *movelist = v_get_movelist(&board);
        srand((uint32_t) time(NULL));
        memcpy(&current_move, &movelist[mod(rand(), (int32_t) vec_get_length(movelist))], sizeof(move_t));
        vec_free(movelist);
    }
    set_move_str(movestr, &current_move, num_caps);
}

void think(char *movestr, uint32_t time_to_move, piece_t *pc, int16_t num_caps, playerinfo_t *client)
{

    board_t board;
    set_board(&board, pc, client);
    print_gameboard(&board);

    if (num_caps > 0) {
        set_move_str(movestr, &current_move, num_caps); 
        return;
    }

    pthread_t search_thread;
    pthread_mutex_t mtx_quit;

    pthread_mutex_init(&mtx_quit, NULL);
    pthread_mutex_lock(&mtx_quit);

    searchinfo_t sinfo = { 
        .best_move = &current_move, 
        .board = &board,
        .client_color = client->color,
        .mtx_quit = &mtx_quit,
    };

    pthread_create(&search_thread, NULL, thread_search, (void*) &sinfo);

    uint32_t sleep_micros = 1000 * (time_to_move - 200);
    usleep(sleep_micros);

    pthread_mutex_unlock(&mtx_quit);
    set_move_str(movestr, &current_move, num_caps); 
}