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

unsigned int CASTLING_RIGHTS[] = {
    ['K'] = WHITE_KINGSIDE,
    ['Q'] = WHITE_QUEENSIDE,
    ['k'] = BLACK_KINGSIDE,
    ['q'] = BLACK_QUEENSIDE
};

int PAWN_OFFS[4] = {N, N + N, N + E, N + W};
int KNIGHT_OFFS[8] = {
    N + N + W, N + N + E, S + S + W, S + S + E,
    E + E + N, E + E + S, W + W + N, W + W + S
};
int BISHOP_OFFS[4] = {N + E, N + W, S + E, S + W};
int ROOK_OFFS[4] = {N, E, S, W};
int QUEEN_OFFS[8] = {N, E, S, W, N + E, N + W, S + E, S + W};
int *KING_OFFS = QUEEN_OFFS;

int SUPERPIECE[16] = {
    N, E, S, W, N + E, N + W, S + E, S + W,
    N + N + W, N + N + E, S + S + W, S + S + E,
    E + E + N, E + E + S, W + W + N, W + W + S
};
