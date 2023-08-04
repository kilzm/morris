#ifndef SHM_H
#define SHM_H

#include <sys/ipc.h>
#include <sys/types.h>

#include "config.h"

typedef enum gamekind {
    NMMORRIS
    // extendible to support more games
} __attribute__ ((__packed__)) gamekind_t;

typedef enum serverphase {
    INVALID_PHASE = -1,
    WAIT,
    MOVE,
    GAMEOVER,
} __attribute__ ((__packed__)) serverphase_t;

typedef enum playercolor {
    WHITE, BLACK
} playercolor_t;

typedef struct playerinfo {
    playercolor_t colo;
    char name[NAME_LEN];
    bool ready;
    bool winner;
} playerinfo_t;

typedef struct clientinfo {
    char conf_name[CONF_LEN];
    config_t config;
    char game_id[ID_LEN];
    char gamename[NAME_LEN];
    gamekind_t gamekind;
    i16 player_num_wish;
    playerinfo_t client;
    u16 num_players;
    u16 num_pieces;

    key_t keys_shm[2];
    pid_t pid_thinker;
    pid_t pid_connector;
    bool thinker_attach;

    serverphase_t serverphase;
    time_t time_to_move;
    i16 num_caps;
} clientinfo_t;

typedef struct piece {
    playercolor_t color;
    i16 piece_num;
    char position[2];
} piece_t;

clientinfo_t *shm_create_ci(i32 *shmid);
playerinfo_t *shm_create_pi(i32 *shmid, key_t key, u16 num_players);
piece_t *shm_create_pc(i32 *shmid, key_t key, u16 num_pieces);

void shm_destroy_ci(i32 shmid, clientinfo_t *ci);
void shm_destroy_pi(i32 shmid, playerinfo_t *pi);
void shm_destroy_pc(i32 shmid, piece_t *pc);

#endif /* SHM_H */