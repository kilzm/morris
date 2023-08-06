#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "board.h"
#include "morris.h"

boardpos_t get_board_pos(char pos[2])
{
    if (strncmp(pos, "A", 2) == 0) 
        return OH;
    if (strncmp(pos, "C", 2) == 0)
        return CP;
    return (pos[0] - 'A') * 8 + pos[1] - '0';
}

void set_playerboard(playerboard_t *ps, boardpos_t pos, int16_t pc_num)
{
    ps->pieces[pc_num % 9] = pos;
    if (pos == OH) {
        ps->set_pieces++;
    } else if (pos != CP) {
        assert(A0 <= pos && pos < NUM_POS);
        ps->board |= M(pos);
        ps->board_pieces++;
    }
    
    if (ps->set_pieces > 0) {
        ps->phase = SET;
    } else if (ps->board_pieces > 3) {
        ps->phase = MOV;
    } else if (ps->board_pieces == 3) {
        ps->phase = JMP;
    } else {
        ps->phase = GMO;
    }
}

void set_board(board_t *board, piece_t *pc, playerinfo_t *client) 
{
    memset(board, 0, sizeof *board);
    board->white_to_move = (client->color == WHITE);
    boardpos_t pos;
    for (int16_t pc_num = 0; pc_num < 18; ++pc_num) {
        pos = get_board_pos(pc[pc_num].position);
        switch (pc[pc_num].color) 
        {
        case WHITE:
            set_playerboard(&board->white, pos, pc_num); break;
        case BLACK:
            set_playerboard(&board->black, pos, pc_num); break;
        default:
            assert(false);
        }
    }
}

bitboard_t get_empty(board_t *board)
{
    return ~(board->white.board | board->black.board);
}

bitboard_t get_occupied(board_t *board)
{
    return (board->white.board | board->black.board);
}

bool boards_equal(board_t board1, board_t board2) {
    bool result = true;
    result &= board1.black.board == board2.black.board;
    result &= board1.black.board_pieces == board2.black.board_pieces;

}

typedef enum boardedge {
    EDG_L, EDG_R, EDG_M, PCE_W, PCE_B,
} boardedge_t;

void print_gameboard(board_t *board)
{
    boardedge_t b[NUM_POS] = {
        EDG_L, EDG_M, EDG_R, EDG_R, EDG_R, EDG_M, EDG_L, EDG_L,
        EDG_L, EDG_M, EDG_R, EDG_M, EDG_R, EDG_M, EDG_L, EDG_M,
        EDG_L, EDG_M, EDG_R, EDG_L, EDG_R, EDG_M, EDG_L, EDG_R,
    };

    const char *s[] = {
        [EDG_L] = " +-", [EDG_R] = "-+ ", [EDG_M] = "-+-",
        [PCE_W] = "(*)", [PCE_B] = "(#)",
    };

    for (boardpos_t pos = A0; pos < NUM_POS; ++pos) {
        if (check_bit(board->white.board, pos)) {
            b[pos] = PCE_W;
        }
        else if (check_bit(board->black.board, pos)) {
            b[pos] = PCE_B;
        }
    }

    printf("%s---------%s---------%s\n", s[b[A0]], s[b[A1]], s[b[A2]]);
    printf(" |           |           | \n");
    printf(" |  %s-----%s-----%s  | \n", s[b[B0]], s[b[B1]], s[b[B2]]);
    printf(" |   |       |       |   | \n");
    printf(" |   |  %s-%s-%s  |   | \n", s[b[C0]], s[b[C1]], s[b[C2]]);
    printf(" |   |   |       |   |   | \n");
    printf("%s-%s-%s     %s-%s-%s\n", s[b[A7]], s[b[B7]], s[b[C7]], s[b[C3]], s[b[B3]],s[b[A3]]);
    printf(" |   |   |       |   |   |\n");
    printf(" |   |  %s-%s-%s  |   | \n", s[b[C6]], s[b[C5]], s[b[C4]]);
    printf(" |   |       |       |   | \n");
    printf(" |  %s-----%s-----%s  | \n", s[b[B6]], s[b[B5]], s[b[B4]]);
    printf(" |           |           | \n");
    printf("%s---------%s---------%s\n\n", s[b[A6]], s[b[A5]], s[b[A4]]);
}

void print_bitboard(int const size, void * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    
    for (int i = size-1; i >= 0; i--) {
        for (int j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
    puts("");
}