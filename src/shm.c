#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>

#include "defs.h"
#include "shm.h"

clientinfo_t *shm_create_ci(int32_t *shmid) 
{
    clientinfo_t *ci;
    *shmid = shmget(IPC_PRIVATE, sizeof(clientinfo_t), IPC_CREAT | 0666);
    if (*shmid == -1) {
        perror("shmget()");
        return NULL;
    }

    ci = (clientinfo_t*) shmat(*shmid, NULL, 0);
    return ci;
}

playerinfo_t *shm_create_pi(int32_t *shmid, key_t key, uint16_t num_players) 
{
    playerinfo_t *pi;
    *shmid = shmget(key, sizeof(playerinfo_t) * (num_players - 1), IPC_CREAT | 0666);
    if (*shmid == -1) {
        perror("shmget()");
        return NULL;
    }

    pi = (playerinfo_t*) shmat(*shmid, NULL, 0);
    return pi;
}

piece_t *shm_create_pc(int32_t *shmid, key_t key, uint16_t num_pieces)
{
    piece_t *pc;
    *shmid = shmget(key, sizeof(piece_t) * num_pieces, IPC_CREAT | 0666);
    if (*shmid == -1) {
        perror("shmget()");
        return NULL;
    }

    pc = (piece_t*) shmat(*shmid, NULL, 0);
    return pc;
}

void shm_destroy_ci(int32_t shmid, clientinfo_t *ci)
{
    shmctl(shmid, IPC_RMID, 0);
    shmdt(ci);
}

void shm_destroy_pi(int32_t shmid, playerinfo_t *pi)
{
    shmctl(shmid, IPC_RMID, 0);
    shmdt(pi);
}

void shm_destroy_pc(int32_t shmid, piece_t *pc)
{
    shmctl(shmid, IPC_RMID, 0);
    shmdt(pc);
}