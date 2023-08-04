#ifndef THINK_H
#define THINK_H

#include "moves.h"
#include "../shm.h"

void think(char *movestr, time_t time_to_move, piece_t *pc, i16 num_caps, playerinfo_t *client);

#endif /* THINK_H */