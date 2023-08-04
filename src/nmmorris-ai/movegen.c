#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "movegen.h"
#include "../util/vec.h"
#include "moves.h"

static inline 
void add_move(move_t *move, move_t *movelist, gamephase_t phase, boardpos_t from, boardpos_t to,
              boardpos_t cap1, boardpos_t cap2, i16 num_caps, i16 movedi, i16 cap1i, i16 cap2i)
{
    move = vec_pushbacks(&movelist);
    move->phase = phase;
    move->from = from;
    move->to = to;
    move->num_caps = num_caps;
    move->caps[0] = cap1;
    move->caps[1] = cap2;
    move->movedi = movedi;
    move->capsi[0] = cap1i;
    move->capsi[1] = cap2i;
    move = NULL;
}

static inline
move_t *v_get_movelist_set(playerboard_t *self, playerboard_t *other, bitboard_t occupied)
{
    move_t *curmove, *movelist;
    curmove = NULL;
    movelist =  vec_with_cap(DEFAULT_CAP, sizeof *movelist);
    boardpos_t to, cap1, cap2;
    i16 movedi = (i16) 9 - self->set_pieces;
    for (to = A0; to < NUM_POS; ++to){
        if (check_bit(occupied, to)) continue;
        switch (get_caps_set(self->board, to)) 
        {
        case 0:
            add_move(curmove, movelist, SET, IV, to, IV, IV, 0, movedi, -1, -1);
            break;
        case 1:
            for (i16 c = 0; c < 9; ++c) {
                cap1 = other->pieces[c];
                if (cap1 < A0) continue;
                add_move(curmove, movelist, SET, IV, to, cap1, IV, 1, movedi, c, -1);
            }
            break;
        case 2:
            for (i16 c1 = 0; c1 < 9; ++c1) {
                cap1 = other->pieces[c1];
                if (cap1 < A0) continue;
                for (i16 c2 = 0; c2 < 9; ++c2) {
                    cap2 = other->pieces[c2];
                    if (cap2 < A0 || c1 == c2) continue;
                    add_move(curmove, movelist, SET, IV, to, cap1, cap2, 2, movedi, c1, c2);
                }
            }
            break;  
        default:
            assert(false);
        }
    }
    return movelist;
}

static inline
move_t *v_get_movelist_mov(playerboard_t *self, playerboard_t *other, bitboard_t occupied)
{
    move_t *curmove, *movelist;
    curmove = NULL;
    movelist =  vec_with_cap(DEFAULT_CAP, sizeof *movelist);
    boardpos_t from, to, cap;
    for (i16 f = 0; f < 9; ++f) {
        from = self->pieces[f];
        if (from < A0) continue;
        for (i16 t = 0; t < 4; ++t) {
            to = adjacent[from][t];
            if (to < A0) break;
            if (check_bit(occupied, to)) continue;
            if (!get_cap_mov_jmp(self->board, from, to)) {
                add_move(curmove, movelist, MOV, from, to, IV, IV, 0, f, -1, -1);
                continue;
            } 
            for (i16 c = 0; c < 9; ++c) {
                cap = other->pieces[c];
                if (cap < A0) continue;
                add_move(curmove, movelist, MOV, from, to, cap, IV, 1, f, c, -1);
            }
        }
    }
    return movelist;
}

static inline
move_t *v_get_movelist_jmp(playerboard_t *self, playerboard_t *other, bitboard_t occupied)
{
    move_t *curmove, *movelist;
    curmove = NULL;
    movelist =  vec_with_cap(DEFAULT_CAP, sizeof *movelist);
    boardpos_t from, to, cap;
    for (i16 f = 0; f < 9; ++f) {
        from = self->pieces[f];
        if (from < A0) continue;
        for (to = A0; to < NUM_POS; ++to) {
            if (check_bit(occupied, to)) continue;
            if (!get_cap_mov_jmp(self->board, from, to)) {
                add_move(curmove, movelist, JMP, from, to, IV, IV, 0, f, -1, -1);
                continue;
            }
            for (i16 c = 0; c < 9; ++c) {
                cap = other->pieces[c];
                if (cap < A0) continue;
                add_move(curmove, movelist, JMP, from, to, cap, IV, 1, f, c, -1);
            }
        }
    }
    return movelist;
}

static inline
move_t *v_get_movelist_color(playerboard_t *self, playerboard_t *other, bitboard_t occupied)
{
    move_t *movelist = NULL;
    switch (self->phase) 
    {
    case SET:
        movelist = v_get_movelist_set(self, other, occupied);
        break;
    case MOV:
        movelist = v_get_movelist_mov(self, other, occupied);
        break;
    case JMP:
        movelist = v_get_movelist_jmp(self, other, occupied);
        break;
    case GMO:
        break;
    default:
        assert(false);
        break;
    }
    return movelist;
}

move_t *v_get_movelist(board_t *board)
{   
    move_t *movelist;
    bitboard_t occupied = get_occupied(board);
    if (board->white_to_move) {
        movelist = v_get_movelist_color(&board->white, &board->black, occupied);
    } else {
        movelist = v_get_movelist_color(&board->black, &board->white, occupied);
    }
    return movelist;
}
