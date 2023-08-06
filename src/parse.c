#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "defs.h"
#include "shm.h"
#include "parse.h"

void parse_gamekind(char *buf, clientinfo_t *ci) 
{
    char gamekind[30];
    sscanf(buf, "+ PLAYING %s\n", gamekind);
    if (strcmp(gamekind, "NMMorris") == 0) {
        ci->gamekind = NMMORRIS;
        ci->num_pieces = 18;
    }
}

void parse_gamename(char *buf, clientinfo_t *ci)
{
    sscanf(buf, "+ %s", ci->gamename);
}

void parse_client_player(char *buf, playerinfo_t *pi)
{
    int16_t player_num;
    sscanf(buf, "+ YOU %hd %[^\t\n]\n", &player_num, pi->name);
    pi->ready = 1;
    switch (player_num) {
        case 0: pi->color = WHITE; break;
        case 1: pi->color = BLACK; break;
        default: assert(false);
    }
}

void parse_num_players(char *buf, clientinfo_t *ci)
{
    sscanf(buf, "+ TOTAL %hu\n", &ci->num_players);
}

void parse_other_player(char *buf, playerinfo_t *pi)
{
    int16_t player_num;
    char name_flag[NAME_LEN + 2];
    sscanf(buf, "+ %hd %[^\t\n]\n", &player_num, name_flag);
    size_t name_len = strlen(name_flag) - 2;

    switch (player_num) {
        case 0: pi->color = WHITE; break;
        case 1: pi->color = BLACK; break;
        default: assert(false);
    }

    pi->ready = name_flag[name_len + 1] - '0';
    memcpy(pi->name, name_flag, name_len);
}

void parse_server_phase(char *buf, clientinfo_t *ci)
{
    char phase[16];
    sscanf(buf, "+ %s", phase);
    //printf("%s\n", phase);
    if (strcmp(phase, "WAIT") == 0) {
        ci->serverphase = WAIT;
    } else if (strcmp(phase, "MOVE") == 0) {
        ci->serverphase = MOVE;
    } else if (strcmp(phase, "GAMEOVER") == 0) {
        ci->serverphase = GAMEOVER;
    } else {
        ci->serverphase = INVALID_PHASE;
    }
}

void parse_move_time(char *buf, clientinfo_t *ci)
{
    sscanf(buf, "+ MOVE %ld\n", &ci->time_to_move);
}

void parse_capture(char *buf, clientinfo_t *ci)
{
    sscanf(buf, "+ CAPTURE %hd\n", &ci->num_caps);
}

void parse_piece(char *buf, piece_t *pc)
{
    int16_t player_num;
    sscanf(buf, "+ PIECE%hd.%hd %s", &player_num, &pc->piece_num, pc->position);
    switch (player_num) {
        case 0: pc->color = WHITE; break;
        case 1: pc->color = BLACK; break;
        default: assert(false);
    }
}

void parse_win(char *buf, clientinfo_t *ci, playerinfo_t *pi)
{
    int16_t player_num;
    char winstr[3];
    sscanf(buf, "+ PLAYER%hdWON %s\n", &player_num, winstr);
    bool won = (strncmp(winstr, "Yes", 3) == 0);
    if (ci->client.color == (playercolor_t) player_num) {
        ci->client.winner = won;
    } else {
        for (int32_t i = 0; i < ci->num_players - 1; ++i) {
            if (pi[i].color == (playercolor_t) player_num) {
                pi[i].winner = won;
            }
        }
    }
}

void print_player(playerinfo_t *pi) 
{
    printf("\n");
    if (pi->ready) {
        printf("Player %d: %s is ready\n", pi->color + 1, pi->name);
    } else {
        printf("Player %d: %s is not ready\n", pi->color + 1, pi->name);
    }
    printf("\n");
}

void print_winner(playerinfo_t *pi, clientinfo_t *ci)
{
    bool draw = ci->client.winner;
    for (int32_t i = 0; i < ci->num_players - 1; ++i) {
        draw &= pi[i].winner;
    }
    if (draw) {
        printf("It's a draw!\n");
    } else if (ci->client.winner) {
        printf("Congrats! %s won.\n", ci->client.name);
    } else {
        for (int32_t i = 0; i < ci->num_players - 1; ++i) {
            if (pi[i].winner) {
                printf("Better luck next time! %s won.\n", pi[i].name);
            }
        }
    }
}