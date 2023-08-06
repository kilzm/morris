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
    WHITE = 1,
    BLACK = -1,
} playercolor_t;

typedef struct playerinfo {
    playercolor_t color;
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
    int16_t player_num_wish;
    playerinfo_t client;
    uint16_t num_players;
    uint16_t num_pieces;

    key_t keys_shm[2];
    pid_t pid_thinker;
    pid_t pid_connector;
    bool thinker_attach;

    serverphase_t serverphase;
    time_t time_to_move;
    int16_t num_caps;
} clientinfo_t;

typedef struct piece {
    playercolor_t color;
    int16_t piece_num;
    char position[2];
} piece_t;

clientinfo_t *shm_create_ci(int32_t *shmid);
playerinfo_t *shm_create_pi(int32_t *shmid, key_t key, uint16_t num_players);
piece_t *shm_create_pc(int32_t *shmid, key_t key, uint16_t num_pieces);

void shm_destroy_ci(int32_t shmid, clientinfo_t *ci);
void shm_destroy_pi(int32_t shmid, playerinfo_t *pi);
void shm_destroy_pc(int32_t shmid, piece_t *pc);

#endif /* SHM_H */