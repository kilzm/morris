#ifndef STATE_H
#define STATE_H

#include "../defs.h"
#include "../shm.h"
#include "morris.h"

typedef enum gamephase {
    SET, MOV, JMP, GMO 
} gamephase_t;

typedef struct playerboard {
    gamephase_t phase;
    bitboard_t board;
    boardpos_t pieces[9];
    i16 set_pieces;
    i16 board_pieces;
} playerboard_t;

typedef struct board {
    playerboard_t white;
    playerboard_t black;
    bool white_to_move;
} board_t;

#define check_bit(board, pos) (board & (1U << pos))
#define check_mask(board, mask) ((board & mask) == mask)
#define set_piece(board, to) (board | (1U << to))
#define rem_piece(board, pos) (board & ~(1U << pos))
#define mov_piece(board, from, to) ((board & ~(1U << from)) | (1U << to))

void set_board(board_t *board, piece_t *pc, playerinfo_t *client);
bitboard_t get_empty(board_t *board);
bitboard_t get_occupied(board_t *board);


void print_gameboard(board_t *board);
void print_bitboard(size_t const size, void const * const ptr);

#endif /* STATE_H */