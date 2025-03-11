#include "constants.h"

char *FILES = "abcdefgh";

char *COORDS[64] = {
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
};

unsigned int PIECES[] = {
    [1] = 0, [2] = 0, [3] = 0, [4] = 0,
    [5] = 0, [6] = 0, [7] = 0, [8] = 0,
    ['P'] = WHITE | PAWN,
    ['N'] = WHITE | KNIGHT,
    ['B'] = WHITE | BISHOP,
    ['R'] = WHITE | ROOK,
    ['Q'] = WHITE | QUEEN,
    ['K'] = WHITE | KING,
    ['p'] = BLACK | PAWN,
    ['n'] = BLACK | KNIGHT,
    ['b'] = BLACK | BISHOP,
    ['r'] = BLACK | ROOK,
    ['q'] = BLACK | QUEEN,
    ['k'] = BLACK | KING,
};

char SYMBOLS[] = {
    [WHITE | PAWN] = 'P',
    [WHITE | KNIGHT] = 'N',
    [WHITE | BISHOP] = 'B',
    [WHITE | ROOK] = 'R',
    [WHITE | QUEEN] = 'Q',
    [WHITE | KING] = 'K',
    [BLACK | PAWN] = 'p',
    [BLACK | KNIGHT] = 'n',
    [BLACK | BISHOP] = 'b',
    [BLACK | ROOK] = 'r',
    [BLACK | QUEEN] = 'q',
    [BLACK | KING] = 'k',
};

unsigned int CASTLING_RIGHTS[] = {
    ['K'] = WHITE_KINGSIDE,
    ['Q'] = WHITE_QUEENSIDE,
    ['k'] = BLACK_KINGSIDE,
    ['q'] = BLACK_QUEENSIDE
};

unsigned int PROMOTIONS[] = {KNIGHT, BISHOP, ROOK, QUEEN};

int PAWN_OFFS[3] = {N, N + E, N + W};
int KNIGHT_OFFS[8] = {
    N + N + W, N + N + E, S + S + W, S + S + E,
    E + E + N, E + E + S, W + W + N, W + W + S
};
int BISHOP_OFFS[4] = {N + E, N + W, S + E, S + W};
int ROOK_OFFS[4] = {N, E, S, W};
int QUEEN_OFFS[8] = {N, E, S, W, N + E, N + W, S + E, S + W};
int KING_OFFS[8] = {N, E, S, W, N + E, N + W, S + E, S + W};

int N_VECS[] = {
    [PAWN] = 3, [KNIGHT] = 8, [BISHOP] = 4, [ROOK] = 4, [QUEEN] = 8, [KING] = 8
};

int *MOVE_SETS[] = {
    [PAWN] = PAWN_OFFS, [KNIGHT] = KNIGHT_OFFS, [BISHOP] = BISHOP_OFFS,
    [ROOK] = ROOK_OFFS, [QUEEN] = QUEEN_OFFS, [KING] = KING_OFFS
};

int SUPERPIECE[16] = {
    N, E, S, W, N + E, N + W, S + E, S + W,
    N + N + W, N + N + E, S + S + W, S + S + E,
    E + E + N, E + E + S, W + W + N, W + W + S
};

unsigned int MOVE_TABLE[239] = {
    80, 0, 0, 0, 0, 0, 0, 96, 0, 0, 0, 0, 0, 0, 80, 0, 0, 80, 0, 0, 0, 0, 0, 96,
    0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 80, 0, 0, 0, 0, 96, 0, 0, 0, 0, 80, 0, 0, 0,
    0, 0, 0, 80, 0, 0, 0, 96, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 96,
    0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 80, 8, 96, 8, 80, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 8, 208, 224, 208, 8, 0, 0, 0, 0, 0, 0, 96, 96, 96, 96, 96, 96,
    224, 0, 224, 96, 96, 96, 96, 96, 96, 0, 0, 0, 0, 0, 0, 8, 212, 228, 212, 8, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 80, 8, 96, 8, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    80, 0, 0, 96, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 96, 0, 0, 0, 80,
    0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 96, 0, 0, 0, 0, 80, 0, 0, 0, 0, 80, 0, 0, 0,
    0, 0, 96, 0, 0, 0, 0, 0, 80, 0, 0, 80, 0, 0, 0, 0, 0, 0, 96, 0, 0, 0, 0, 0, 0, 80
};

unsigned int UNIT_VEC[239] = {
    -17, 0, 0, 0, 0, 0, 0, -16, 0, 0, 0, 0, 0, 0, -15, 0, 0, -17, 0, 0, 0, 0, 0,
    -16, 0, 0, 0, 0, 0, -15, 0, 0, 0, 0, -17, 0, 0, 0, 0, -16, 0, 0, 0, 0, -15, 0,
    0, 0, 0, 0, 0, -17, 0, 0, 0, -16, 0, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, 0, -17, 0,
    0, -16, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -17, -33, -16, -31, -15, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, -18, -17, -16, -15, -14, 0, 0, 0, 0, 0, 0, -1, -1, -1,
    -1, -1, -1, -1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 14, 15, 16, 17, 18, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 31, 16, 33, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    15, 0, 0, 16, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 0, 15, 0, 0, 0, 16, 0, 0, 0, 17, 0,
    0, 0, 0, 0, 0, 15, 0, 0, 0, 0, 16, 0, 0, 0, 0, 17, 0, 0, 0, 0, 15, 0, 0, 0, 0, 0,
    16, 0, 0, 0, 0, 0, 17, 0, 0, 15, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 17
};
