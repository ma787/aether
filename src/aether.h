#ifndef POSITION_H
#define POSITION_H 1

#include <stdint.h>
#include <stdbool.h>

/* project-wide constants */

#define NAME "Aether"
#define AUTHOR "Mohamed Omar"

#define STACK_SIZE 50
#define MOVE_LIST_SIZE 256
#define TABLE_SIZE 0x100000 * 2

#define MAX_DEPTH 64
#define INFINITY 10000000

#define MATE 32768

#define START_POS "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define FEN_REGEX ( \
    "((([pnbrqkPNBRQK]|[1-8]){1,})[/]){7}([pnbrqkPNBRQK]|[1-8]){1,}[ ]" \
    "[bw][ ](([K]?[Q]?[k]?[q]?)|-)[ ](([a-h][36])|-)([ ][0-9]+){2}" \
)

#define MOVE_REGEX "[a-h][1-8][a-h][1-8]([pnbrqk]?)"

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

#define NULL_MOVE 0

#define EP_OFF -13
#define C_OFF -5
#define SIDE_OFF -1

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

extern int PAWN_OFFS[3];
extern int KNIGHT_OFFS[8];
extern int BISHOP_OFFS[4];
extern int ROOK_OFFS[4];
extern int QUEEN_OFFS[8];
extern int KING_OFFS[8];

extern int N_VECS[];
extern int *MOVE_SETS[];

extern unsigned int MOVE_TABLE[239];
extern unsigned int UNIT_VEC[239];

extern int PIECE_VALS[];

extern int PAWN_TABLE[];
extern int KNIGHT_TABLE[];
extern int BISHOP_TABLE[];
extern int ROOK_TABLE[];
extern int QUEEN_TABLE[];
extern int KING_TABLE[];

extern int *EVAL_TABLES[];

extern uint64_t HASH_VALUES[781];

/* structs and typedefs */

typedef struct {
    int index;
    int moves[256];
} move_list;

typedef void (*MOVE_GENERATOR)(int, int, move_list*);

typedef struct {
    uint64_t key;
    int best_move;
} TABLE_ENTRY;

typedef struct {
    TABLE_ENTRY *table;
    int n_entries;
} HASH_TABLE;

/* board and position state definitions */

extern int ply;

extern unsigned int board[256];
extern unsigned int *w_pieces;
extern unsigned int *b_pieces;
extern unsigned int side;
extern unsigned int c_rights;
extern unsigned int ep_square;
extern unsigned int h_clk;
extern unsigned int check_info;

extern uint64_t board_hash;

extern HASH_TABLE pv_table[1];
extern int pv_line[MAX_DEPTH];

/* functions to set/update/modify the current position */

void flip_position(void);
int set_position(char *fen_str);

void switch_side(void);

void save_state(void);
void restore_state(void);

/* move encoding and decoding functions */

int encode_move(int start, int dest, int flags);

int get_start(int mv);
int get_dest(int mv);
int get_flags(int mv);
int get_captured_piece(int mv);
int get_side(int mv);

/* move-making functions */

int is_square_attacked(int pos);
int make_move(int mv);
void unmake_move(int mv);

/* move generation functions */

void all_moves(move_list *moves);

/* perft and divide functions */

uint64_t perft(int depth);
void divide(int depth);

/* hashing-related functions */

void set_hash(void);
void update_hash(int mv);

void clear_table(void);
void init_table(void);

void store_move(int mv);
int get_pv_move(void);

int get_pv_line(int depth);

/* engine search functions */

void search(int depth);

/* string-related functions */

void board_to_fen(char *fen_str);
bool fen_match(char *fen_str);

void move_to_string(int mv, char* mstr);
int string_to_move(char *mstr);

int string_to_coord(char *sqr_str);
char* coord_to_string(int pos);

/* miscellaneous functions */

int get_rank(int pos);
int get_file(int pos);
int to_index(int pos);
int flip_square(int pos);

int square_diff(int start, int dest);
int is_attacking(int p_type, int start, int dest);
int get_step(int start, int dest);

int get_time(void);

#endif