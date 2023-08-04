#include <stdio.h>
#include <stdlib.h>

#include "../src/nmmorris-ai/board.h"
#include "../src/nmmorris-ai/moves.h"
#include "../src/nmmorris-ai/morris.h"
#include "../src/nmmorris-ai/think.h"
#include "../src/nmmorris-ai/movegen.h"
#include "../src/nmmorris-ai/search.h"
#include "../src/nmmorris-ai/eval.h"
#include "../src/util/vec.h"
#include "../src/util/math.h"

//src/connect.c src/config.c src/nmmorris-ai/moves.c src/nmmorris-ai/movegen.c src/nmmorris-ai/think.c src/nmmorris-ai/morris.c src/nmmorris-ai/board.c src/nmmorris-ai/eval.c src/nmmorris-ai/search.c src/util/vec.c src/util/math.c src/parse.c src/shm.c tests/testing.c

void set_player_board(playerboard_t *board) {
    board->board = 0b00000000000000000000000000000000;
    board->set_pieces = 9;
    board->board_pieces = 0;
    board->pieces[0] = OH;
    board->pieces[1] = OH;
    board->pieces[2] = OH;
    board->pieces[3] = OH;
    board->pieces[4] = OH;
    board->pieces[5] = OH;
    board->pieces[6] = OH;
    board->pieces[7] = OH;
    board->pieces[8] = OH;    
    board->phase = SET;
}

void set_player_board2(playerboard_t *board) {
    board->board = 0b00000000000000000000000000000000;
    board->set_pieces = 9;
    board->board_pieces = 0;
    board->pieces[0] = OH;
    board->pieces[1] = OH;
    board->pieces[2] = OH;
    board->pieces[3] = OH;
    board->pieces[4] = OH;
    board->pieces[5] = OH;
    board->pieces[6] = OH;
    board->pieces[7] = OH;
    board->pieces[8] = OH;   
    board->phase = SET;
}

u64 perft(i16 depth, board_t *board) {
    move_t *movelist;
    u64 nodes = 0;

    if (depth == 0) {
        return 1UL;
    }

    movelist = v_get_movelist(board);
    for (i32 i = 0; i < vec_get_length(movelist); ++i) {
        make_move(board, &movelist[i]);
        nodes += perft(depth - 1, board);
        unmake_move(board, &movelist[i]);
    }

    return nodes;
}

int main() {
    board_t board;
    board.white_to_move = true;
    set_player_board(&board.white);
    set_player_board2(&board.black);
    move_t *move;
    move_t played_moves[30];

    for (i32 i = 0; i < 30; ++i) {
        move_t *movelist = v_get_movelist(&board);
        move = &movelist[mod(rand(), vec_get_length(movelist))];
        memcpy(&played_moves[i], move, sizeof *move);
        make_move(&board, move);
        printf("iteration: %d\n", i);
        print_move(move);
        print_gameboard(&board);
        vec_free(movelist);
    }

    for (i32 i = 0; i < 30; ++i) {
        unmake_move(&board, &played_moves[30 - i]);
        print_move(&played_moves[30 - i]);
    }



    /*
    move_t *moves = v_get_movelist(&board);
    size_t len = vec_get_length(moves);
    make_move(&board, &moves[1]);
    i32 c = count_moves(&board.white, get_empty(&board));
    i32 d = count_moves(&board.black, get_empty(&board));
    printf("count: %d %d\n", c, d);
    
    vec_free(moves);
    */
}