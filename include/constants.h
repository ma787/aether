#ifndef CONTSTANTS_H
#define CONSTANTS_H 1

#define STACK_SIZE 50

#define WHITE 1U
#define BLACK (WHITE << 1)
#define PAWN (WHITE << 2)
#define KNIGHT (WHITE << 3)
#define BISHOP (WHITE << 4)
#define ROOK (WHITE << 5)
#define QUEEN (WHITE << 6)
#define KING (WHITE << 7)

#define G (WHITE | BLACK)
#define COLOUR_MASK 3

#define NO_CHECK 0U
#define CONTACT_CHECK 1U
#define DISTANT_CHECK 2U
#define DOUBLE_CHECK 3U

#define WHITE_QUEENSIDE 1U
#define WHITE_KINGSIDE (WHITE_QUEENSIDE << 1)
#define BLACK_QUEENSIDE (WHITE_QUEENSIDE << 2)
#define BLACK_KINGSIDE (WHITE_QUEENSIDE << 3)

#define N 16
#define E 1
#define S -16
#define W -1

#define Q_FLAG 0
#define DPP_FLAG 1
#define Q_CASTLE_FLAG 2
#define K_CASTLE_FLAG 3
#define CAPTURE_FLAG 4
#define EP_FLAG 5
#define PROMO_FLAG 8

enum SQUARES {
    A1 = 0x44, B1, C1, D1, E1, F1, G1, H1,
    A2 = 0x54, B2, C2, D2, E2, F2, G2, H2,
    A3 = 0x64, B3, C3, D3, E3, F3, G3, H3,
    A4 = 0x74, B4, C4, D4, E4, F4, G4, H4,
    A5 = 0x84, B5, C5, D5, E5, F5, G5, H5,
    A6 = 0x94, B6, C6, D6, E6, F6, G6, H6,
    A7 = 0xA4, B7, C7, D7, E7, F7, G7, H7,
    A8 = 0xB4, B8, C8, D8, E8, F8, G8, H8,
};

extern char *FILES;
extern char *COORDS[64];
extern unsigned int PIECES[];
extern char SYMBOLS[];
extern unsigned int PROMOTIONS[];
extern unsigned int CASTLING_RIGHTS[];
extern int SUPERPIECE[16];

extern int PAWN_OFFS[4];
extern int KNIGHT_OFFS[8];
extern int BISHOP_OFFS[4];
extern int ROOK_OFFS[4];
extern int QUEEN_OFFS[8];
extern int *KING_OFFS;

extern unsigned int MOVE_TABLE[239];
extern unsigned int UNIT_VEC[239];

#endif