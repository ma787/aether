#ifndef AETHER_H
#define AETHER_H 1

#include <stdint.h>
#include <stdbool.h>

/* project-wide constants */

#define NAME "Aether"
#define AUTHOR "Mohamed Omar"

#define INPUT_BUFFER_SIZE 2400
#define MOVE_LIST_SIZE 256
#define HISTORY_TABLE_SIZE 1024
#define REPETITION_TABLE_SIZE 32768
#define HASH_TABLE_SIZE 0x100000 * 16
#define S_HIS_TABLE_SIZE 256 * sizeof(int)

#define MAX_DEPTH 64
#define INFINITY 10000000

#define MATE 32768

#define CAP_VALUE 1000000
#define FIRST_KILLER_VALUE 900000
#define SECOND_KILLER_VALUE 800000

#define START_POS "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define FEN_REGEX ( \
    "((([pnbrqkPNBRQK]|[1-8]){1,})[/]){7}([pnbrqkPNBRQK]|[1-8]){1,}[ ]" \
    "[bw][ ](([K]?[Q]?[k]?[q]?)|-)[ ](([a-h][36])|-)([ ][0-9]+){2}" \
)

#define MOVE_REGEX "[a-h][1-8][a-h][1-8]([pnbrqk]?)"

#define WHITE 1
#define BLACK 2
#define PAWN 4
#define KNIGHT 8
#define BISHOP 16
#define ROOK 32
#define QUEEN 64
#define KING 128

#define G (WHITE | BLACK)
#define COLOUR_MASK (WHITE | BLACK)

#define WHITE_KINGSIDE 8
#define WHITE_QUEENSIDE 4
#define BLACK_KINGSIDE 2
#define BLACK_QUEENSIDE 1

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

#define KNIGHT_PROMO 8
#define BISHOP_PROMO 9
#define ROOK_PROMO 10
#define QUEEN_PROMO 11

#define EP_OFF 768
#define C_OFF 776
#define SIDE_OFF 780

#define START_PHASE 24

#define REP_TABLE_MASK 0x00003FFF

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

enum RANKS {
    RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8
};

enum FILES {
    FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H
};

enum CHECK_TYPES {
    NO_CHECK, CONTACT_CHECK, DISTANT_CHECK, DOUBLE_CHECK
};

enum N_TYPES {
    NONE, ALL, CUT, PV
};

extern int FIRST_RANK[3];
extern int SECOND_RANK[3];
extern int FINAL_RANK[3];
extern int DPP_RANK[3];

extern int PAWN_STEP[3];

extern int K_ROOK_MOVES[3][2];
extern int Q_ROOK_MOVES[3][2];
extern int K_KING_MOVES[3][2];
extern int Q_KING_MOVES[3][2];
extern int KINGSIDE_RIGHTS[3];
extern int QUEENSIDE_RIGHTS[3];

extern char *FILES;
extern char *COORDS[256];
extern char *SIDES[3];

extern int PIECES[];
extern int P_CODE[];
extern int P_COLOUR_CODE[];
extern char SYMBOLS[];

extern int PROMOTIONS[];
extern int CASTLING_RIGHTS[];
extern char *CASTLE_STRINGS[16];

extern int N_VECS[6];
extern int *MOVE_SETS[6];

extern int CASTLE_UPDATES[256];

extern int MOVE_TABLE[239];
extern int UNIT_VEC[239];

extern int *PST_START[6];
extern int *PST_END[6];
extern int PHASES[12];
extern int MVV_LVA_SCORES[6][6];

extern uint64_t HASH_VALUES[781];

/* structs and typedefs */

typedef struct {
    uint64_t key;
    int c_rights;
    int ep_sq;
    int h_clk;
    int check;
    int fst_checker;
    int snd_checker;
    int phase;
} HISTORY_ENTRY;

typedef struct {
    unsigned int start: 8;
    unsigned int dest: 8;
    unsigned int flags: 4;
    unsigned int captured_piece: 12;
    unsigned int score: 21;
} move_t;

extern move_t NULL_MOVE;

typedef struct {
    int index;
    move_t moves[MOVE_LIST_SIZE];
} MOVE_LIST;

typedef struct {
    int piece;
    int pinning;
    int pinned;
    int vec;
} PIN_INFO;

typedef struct {
    uint64_t key;
    move_t best_move;
    int score;
    int depth;
    int n_type;
} TABLE_ENTRY;

typedef struct {
    TABLE_ENTRY *table;
    int n_entries;
    int new_writes;
    int over_writes;
    int hit;
    int cut;
} HASH_TABLE;

typedef struct {
    uint64_t start_time;
    uint64_t stop_time;
    int depth;
    bool time_set;
    bool quit;
    bool stopped;
    bool found_move;
    uint64_t nodes;
} SEARCH_INFO;

typedef struct {
    int board[256];
    int piece_list[32];
    int* p_lists[3];

    int ply;
    int s_ply;
    int side;
    int c_rights;
    int ep_sq;
    int h_clk;
    int check;
    int fst_checker;
    int snd_checker;

    int big_pieces[3];

    uint64_t key;

    HISTORY_ENTRY history[HISTORY_TABLE_SIZE];
    move_t move_history[HISTORY_TABLE_SIZE];
    uint8_t rep_table[REPETITION_TABLE_SIZE];

    HASH_TABLE hash_table[1];
    move_t pv_line[MAX_DEPTH];

    int *search_history[12];
    move_t search_killers[2][HISTORY_TABLE_SIZE];

} POSITION;

typedef void (*GEN_FROM_POSITION)(POSITION*, int, MOVE_LIST*);
typedef void (*GEN_PINNED)(POSITION*, PIN_INFO*, MOVE_LIST*);
typedef void (*MOVE_GENERATOR)(POSITION*, int, int, MOVE_LIST*);

/* functions to create/update/query position structs */

POSITION* new_position(void);
void free_position(POSITION *pstn);

int update_position(POSITION *pstn, char *fen_str);

void switch_side(POSITION *pstn);
void add_checker(POSITION *pstn, int check_type, int checker);

void save_state(POSITION *pstn);
void restore_state(POSITION *pstn);

bool is_repetition(POSITION *pstn);

/* move encoding/decoding, getting and comparison functions */

move_t get_move(POSITION *pstn, int start, int dest, int flags);

bool moves_equal(move_t mv1, move_t mv2);
bool is_null_move(move_t mv);

/* move-making functions */

bool is_square_attacked(POSITION *pstn, int pos, int side);

bool make_move(POSITION *pstn, move_t mv);
void unmake_move(POSITION *pstn, move_t mv);

void make_null_move(POSITION *pstn);
void unmake_null_move(POSITION *pstn);

/* move generation functions */

void all_moves(POSITION *pstn, MOVE_LIST *moves);
void all_captures(POSITION *pstn, MOVE_LIST *moves);
bool move_exists(POSITION *pstn, move_t mv);

/* perft and divide functions */

uint64_t perft(POSITION *pstn, int depth);
uint64_t count_captures(POSITION *pstn, int depth);
void divide(POSITION *pstn, int depth);

/* hashing-related functions */

void set_hash(POSITION *pstn);
void update_hash(POSITION *pstn, move_t mv);

void clear_hash_table(POSITION *pstn);

void store_entry(POSITION *pstn, move_t mv, int score, int depth, int n_type);
bool get_entry_info(POSITION *pstn, move_t *mv, int *score, int alpha, int beta, int depth);

int get_pv_line(POSITION *pstn, int depth);

/* engine search functions */

void search(POSITION *pstn, SEARCH_INFO *s_info);

/* string-related functions */

void board_to_fen(POSITION *pstn, char *fen_str);
int fen_to_board_array(POSITION *pstn, char *fen_str);
void print_board(POSITION *pstn);

bool fen_match(char *fen_str);
bool move_match(char *mstr);

void move_to_string(move_t mv, char* mstr);
move_t string_to_move(POSITION *pstn, char *mstr);

/* miscellaneous functions */

uint64_t get_time(void);
int input_waiting(void);

/* macros */

#define OTHER(side) (3 - side)
#define SAME_COLOUR(p1, p2) !((p1 ^ p2) & COLOUR_MASK)
#define DIFF_COLOUR(p1, p2) (((p1 ^ p2) & COLOUR_MASK) == G)

#define CHANGE_TYPE(piece, p_type) ((piece & 0xF03) | p_type)

#define PTYPE(piece) (piece & 0xFC)
#define PIECE(piece) (piece & 0xFF)
#define PLIST_INDEX(piece) (piece >> 8)

#define PINDEX(piece) P_CODE[PTYPE(piece)]
#define PCINDEX(piece) P_COLOUR_CODE[PIECE(piece)]

#define PLIST(pstn) pstn->p_lists[pstn->side]
#define OTHER_PLIST(pstn) pstn->p_lists[OTHER(pstn->side)]
#define SIDE_PLIST(pstn, side) pstn->p_lists[side]

#define RANK(pos) ((pos >> 4) - 4)
#define FILE(pos) ((pos & 0x0F) - 4)

#define SQ64(pos) (8 * RANK(pos) + FILE(pos))
#define SQ256(pos) (0x44 + index + (index & ~7))
#define FLIP64(pos) (pos ^ 56)
#define FLIP256(pos) (~pos & 0xFF)

#define INDEX(sqr_str) (((sqr_str[1] - '1') << 4) + (sqr_str[0] - 'a') + A1)
#define COORD(pos) COORDS[pos]
#define LETTER(piece) SYMBOLS[piece & 0xFF]

#define SQDIFF(start, dest) (0x77 + dest - start)
#define ALIGNMENT(start, dest) MOVE_TABLE[SQDIFF(start, dest)]
#define STEP(start, dest) UNIT_VEC[SQDIFF(start, dest)]

#define VECS(piece) MOVE_SETS[PINDEX(piece)]

#define HISTORY(pstn, start, dest) pstn->search_history[PCINDEX(pstn->board[start])][PCINDEX(pstn->board[dest])]
#define MVV_LVA(pstn, start, dest) MVV_LVA_SCORES[PINDEX(pstn->board[dest])][PINDEX(pstn->board[start])]

#endif