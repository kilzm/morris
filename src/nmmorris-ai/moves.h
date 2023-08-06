#ifndef MOVES_H
#define MOVES_H

#include "../defs.h"
#include "board.h"
#include "morris.h"

#define DEFAULT_CAP 128

typedef struct move {
    boardpos_t from;
    boardpos_t to;
    boardpos_t caps[2];
    int16_t num_caps;
    int16_t movedi;
    int16_t capsi[2];
    gamephase_t phase;
} move_t;

void set_move_str(char *movestr, move_t *move, int16_t caps);

int32_t get_caps_set(bitboard_t board, boardpos_t to);
bool get_cap_mov_jmp(bitboard_t board, boardpos_t from, boardpos_t to);

void move_invalid(move_t *move);
void make_move(board_t *board, move_t *move);
void unmake_move(board_t *board, move_t *move);

void print_move(move_t *move);

#endif /* MOVES_H */
