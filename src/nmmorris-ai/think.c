#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

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

void think(char *movestr, time_t time_to_move, piece_t *pc, int16_t num_caps, playerinfo_t *client)
{

    board_t board;
    set_board(&board, pc, client);
    print_gameboard(&board);
    if (num_caps == 0) {
        pthread_t search_thread;
        searchinfo_t sinfo = { .best_move = &current_move, .board = &board };
        pthread_create(&search_thread, NULL, thread_search, (void*) &sinfo);
        sleep(1);
        // pthread_join(search_thread, NULL);
    }
    set_move_str(movestr, &current_move, num_caps); 
}