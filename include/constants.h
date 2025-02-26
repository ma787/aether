#ifndef CONTSTANTS_H
#define CONSTANTS_H 1

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

extern char *FILES;
extern char *COORDS[64];
extern unsigned int PIECES[];
extern unsigned int CASTLING_RIGHTS[];
extern int SUPERPIECE[16];

extern int PAWN_OFFS[4];
extern int KNIGHT_OFFS[8];
extern int BISHOP_OFFS[4];
extern int ROOK_OFFS[4];
extern int QUEEN_OFFS[8];
extern int *KING_OFFS;

#endif