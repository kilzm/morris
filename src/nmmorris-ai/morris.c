#include "morris.h"

const char *boardpos_str[NUM_POS] = {
    "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7",
    "B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7",
    "C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7",
};

/* adjacent positions to a position */
const boardpos_t adjacent[NUM_POS][4] = {
    // outer square
    { A7, A1, IV, IV }, { A0, A2, B1, IV }, { A1, A3, IV, IV }, { A2, A4, B3, IV },
    { A3, A5, IV, IV }, { A4, A6, B5, IV }, { A5, A7, IV, IV }, { A6, A0, B7, IV },
    // middle square
    { B7, B1, IV, IV }, { B0, B2, A1, C1 }, { B1, B3, IV, IV }, { B2, B4, A3, C3 },
    { B3, B5, IV, IV }, { B4, B6, A5, C5 }, { B5, B7, IV, IV }, { B6, B0, A7, C7 },
    // inner square
    { C7, C1, IV, IV }, { C0, C2, B1, IV }, { C1, C3, IV, IV }, { C2, C4, B3, IV },
    { C3, C5, IV, IV }, { C4, C6, B5, IV }, { C5, C7, IV, IV }, { C6, C0, B7, IV }
};

/* adjacent positions to a position as bitboards */
const bitboard_t adjacentb[NUM_POS] = {
    // outer square
    M(A7) | M(A1), M(A0) | M(A2) | M(B1), M(A1) | M(A3), M(A2) | M(A4) | M(B3),
    M(A3) | M(A5), M(A4) | M(A6) | M(B5), M(A5) | M(A7), M(A6) | M(A0) | M(B7),
    // middle square
    M(B7) | M(B1), M(B0) | M(B2) | M(A1) | M(C1), M(B1) | M(B3), M(B2) | M(B4) | M(A3) | M(C3),
    M(B3) | M(B5), M(B4) | M(B6) | M(A5) | M(C5), M(B5) | M(B7), M(B6) | M(B0) | M(A7) | M(C7),
    // inner square
    M(C7) | M(C1), M(C0) | M(C2) | M(B1), M(C1) | M(C3), M(C2) | M(C4) | M(B3),
    M(C3) | M(C5), M(C4) | M(C6) | M(B5), M(C5) | M(C7), M(C6) | M(C0) | M(B7)
};

/* all possible mill configurations as bitboards */
const bitboard_t mills[16] = {
    // outer square
    M(A0) | M(A1) | M(A2), M(A2) | M(A3) | M(A4), M(A4) | M(A5) | M(A6), M(A6) | M(A7) | M(A0),
    // middle square
    M(B0) | M(B1) | M(B2), M(B2) | M(B3) | M(B4), M(B4) | M(B5) | M(B6), M(B6) | M(B7) | M(B0),
    // inner square
    M(C0) | M(C1) | M(C2), M(C2) | M(C3) | M(C4), M(C4) | M(C5) | M(C6), M(C6) | M(C7) | M(C0),
    // perpendicular
    M(A1) | M(B1) | M(C1), M(A3) | M(B3) | M(C3), M(A5) | M(B5) | M(C5), M(A7) | M(B7) | M(C7)
};

/* corresponding mill indices to a position on the board */
const int32_t piece_mills[24][2] = {
    [A0] = {3, 0}, [A1] = {0, 12}, [A2] = {0, 1}, [A3] = {1, 13},
    [A4] = {1, 2}, [A5] = {2, 14}, [A6] = {2, 3}, [A7] = {3, 15},
    [B0] = {7, 4}, [B1] = {4, 12}, [B2] = {4, 5}, [B3] = {5, 13},
    [B4] = {5, 6}, [B5] = {6, 14}, [B6] = {6, 7}, [B7] = {7, 15},
    [C0] = {11, 8}, [C1] = {8, 12}, [C2] = {8, 9}, [C3] = {9, 13},
    [C4] = {9, 10}, [C5] = {10, 14}, [C6] = {10, 11}, [C7] = {11, 15}, 
};