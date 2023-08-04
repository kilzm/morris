#ifndef MORRIS_H
#define MORRIS_H

#include "../defs.h"

typedef u32 bitboard_t;

typedef enum boardpos {
    CP = -3, OH = -2, IV = -1,
    A0, A1, A2, A3, A4, A5, A6, A7,
    B0, B1, B2, B3, B4, B5, B6, B7,
    C0, C1, C2, C3, C4, C5, C6, C7,
    NUM_POS
}  __attribute__ ((__packed__)) boardpos_t;

extern const char *boardpos_str[NUM_POS];
extern const boardpos_t adjacent[24][4];
extern const bitboard_t adjacentb[24];
extern const bitboard_t mills[16];
extern const i32 piece_mills[24][2];

// return bit mask with bit at index enabled
#define M(index) (1U << (index))

#endif /* MORRIS_H */