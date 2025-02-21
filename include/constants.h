#ifndef CONTSTANTS_H
#define CONSTANTS_H 1

typedef enum {WHITE, BLACK} COLOURS;
typedef enum {NULL_PIECE, WP, BP, N, B, R, Q, K} P_TYPES;
typedef enum { NO_CHECK, CONTACT_CHECK, DOUBLE_CHECK} CHECK;

typedef enum {
    EMPTY, WPAWN, GUARD, WKNIGHT, WBISHOP, WROOK, WQUEEN, WKING, BVAL,
    BPAWN = BP | BVAL, BKNIGHT = WKNIGHT | BVAL, BBISHOP = WBISHOP | BVAL,
    BROOK = WROOK | BVAL, BQUEEN = WQUEEN | BVAL, BKING = WKING | BVAL 
} PIECES;

extern const char *FILES;
extern const char *LETTERS;

struct position_info {
    unsigned int side;
    unsigned int c_rights;
    int ep_square;
    int h_clk;
    int check_info;
};
typedef struct position_info info;

#endif
