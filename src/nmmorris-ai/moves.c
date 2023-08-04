#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "moves.h"
#include "board.h"
#include "../util/vec.h"

void move_invalid(move_t *move)
{
    move->from = IV;
    move->to = IV;
    move->movedi = -1;
    move->num_caps = 0;
    move->caps[0] = IV;
    move->caps[1] = IV;
    move->capsi[0] = -1;
    move->capsi[1] = -1;
}

void make_move_color(playerboard_t *self, playerboard_t *other, move_t *move)
{
    switch (move->phase)
    {
    case SET:
        self->set_pieces--;
        self->board_pieces++;
        if (self->set_pieces <= 0) {
            self->phase = MOV;
        }
        self->board = set_piece(self->board, move->to);
        self->pieces[move->movedi] = move->to;
        for (i16 i = 0; i < move->num_caps; ++i) {
            other->board = rem_piece(other->board, move->caps[i]);
            other->board_pieces--;
            other->pieces[move->capsi[i]] = CP;
        }
        break;
    case MOV:
    case JMP:
        self->board = mov_piece(self->board, move->from, move->to);
        self->pieces[move->movedi] = move->to;
        if (move->num_caps) {
            other->board = rem_piece(other->board, move->caps[0]);
            other->board_pieces--;
            if (other->board_pieces == 3) {
                other->phase = JMP;
            } else if (other->board_pieces < 3) {
                other->phase = GMO;
            }
            other->pieces[move->capsi[0]] = CP;
        }
        break;
    case GMO:
        break;
    default:
        assert(false);
        break;
    }
}

void make_move(board_t *board, move_t *move)
{
    if (board->white_to_move){
        make_move_color(&board->white, &board->black, move);
        board->white_to_move = false;
    } else {
        make_move_color(&board->black, &board->white, move);
        board->white_to_move = true;
    }
}

void unmake_move_color(playerboard_t *self, playerboard_t *other, move_t *move)
{
    switch (move->phase)
    {
        case SET:
            self->set_pieces++;
            self->board_pieces--;
            self->board = rem_piece(self->board, move->to);
            self->pieces[move->movedi] = OH;
            for (i16 i = 0; i < move->num_caps; ++i) {
                other->board = set_piece(other->board, move->caps[i]);
                other->board_pieces++;
                other->pieces[move->capsi[i]] = move->caps[i];
            }
            break;
        case MOV:
        case JMP:
            self->board = mov_piece(self->board, move->to, move->from);
            self->pieces[move->movedi] = move->from;
            if (move->num_caps > 0) {
                other->board = set_piece(other->board, move->caps[0]);
                other->board++;
                other->pieces[move->capsi[0]] = move->caps[0];
            }
            break;
        case GMO:
            break;
        default:
            assert(false);
            break;
    }
    self->phase = move->phase;
}

void unmake_move(board_t *board, move_t *move)
{
    if (board->white_to_move){
        unmake_move_color(&board->black, &board->white, move);
        board->white_to_move = false;
    } else {
        unmake_move_color(&board->white, &board->black, move);
        board->white_to_move = true;
    }
}

void set_move_str(char *movestr, move_t *move, i16 num_caps)
{
    memset(movestr, '\0', 6);
    switch (num_caps) 
    {
    case 2:
        strncpy(movestr, boardpos_str[move->caps[1]], 2); 
        break;
    case 1:
        strncpy(movestr, boardpos_str[move->caps[0]], 2); 
        break;
    case 0:
        if (move->phase == SET) {
            strcpy(movestr, boardpos_str[move->to]);
        } else if ((move->phase == MOV) || (move->phase == JMP)) {
            strncpy(movestr + 0, boardpos_str[move->from], 2);
            movestr[2] = ':';
            strncpy(movestr + 3, boardpos_str[move->to], 2);
        }
        break;
    default:
        assert(false);
        break;
    }
}

i32 get_caps_set(bitboard_t board, boardpos_t to)
{
    bitboard_t mill1, mill2;
    mill1 = mills[piece_mills[to][0]];
    mill2 = mills[piece_mills[to][1]];
    return (check_mask(set_piece(board, to), mill1))
        +  (check_mask(set_piece(board, to), mill2));
}

bool get_cap_mov_jmp(bitboard_t board, boardpos_t from, boardpos_t to)
{
    bitboard_t mill1, mill2;
    mill1 = mills[piece_mills[to][0]];
    mill2 = mills[piece_mills[to][1]];
    return (check_mask(mov_piece(board, from, to), mill1))
        || (check_mask(mov_piece(board, from, to), mill2));
}

void print_move(move_t *move)
{
    const char *phase;
    switch (move->phase) {
        case SET: phase = "SET"; break;
        case MOV: phase = "MOV"; break;
        case JMP: phase = "JMP"; break;
        case GMO: phase = "GMO"; break;
        default: break;
    }
    const char *from = move->from >= A0 ? boardpos_str[move->from] : "__";
    const char *to = move->to >= A0 ? boardpos_str[move->to] : "__";
    const char *cap = move->caps[0] >= A0 ? boardpos_str[move->caps[0]] : "__";
    printf("PHASE: %s, %d|%s -> %d|%s\"n", phase, move->from, from, move->to, to);
    if (move->num_caps) {
        printf("CAP: %d|%s\n", move->caps[0], cap);
    }
}
