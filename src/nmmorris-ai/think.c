#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#include "think.h"
#include "moves.h"
#include "movegen.h"
#include "search.h"
#include "../util/vec.h"
#include "../util/math.h"

move_t current_move;

void rthink(char *movestr, piece_t *pc, i16 num_caps, playerinfo_t *client) 
{
    board_t board;
    set_board(&board, pc, client);
    print_gameboard(&board);
    if (num_caps == 0) {
        move_t *movelist = v_get_movelist(&board);
        srand((u32) time(NULL));
        memcpy(&current_move, &movelist[mod(rand(), (i32) vec_get_length(movelist))], sizeof(move_t));
        vec_free(movelist);
    }
    set_move_str(movestr, &current_move, num_caps);
}

void think(char *movestr, time_t time_to_move, piece_t *pc, i16 num_caps, playerinfo_t *client)
{
    board_t board;
    set_board(&board, pc, client);
    print_gameboard(&board);
    if (num_caps == 0) {
        search(&current_move, &board, time_to_move);
    }
    set_move_str(movestr, &current_move, num_caps); 
}