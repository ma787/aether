#ifndef POSITION_H
#define POSITION_H 1

#include <stdint.h>
#include <stdbool.h>

/* project-wide constants */

#define NAME "Aether"
#define AUTHOR "Mohamed Omar"

#define STACK_SIZE 50
#define MOVE_LIST_SIZE 256
#define HISTORY_TABLE_SIZE 1024
#define REPETITION_TABLE_SIZE 32768
#define PV_TABLE_SIZE 0x100000 * 2
#define S_HIS_TABLE_SIZE (H8 + 1) * sizeof(int)

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

#define KNIGHT_PROMO 8
#define BISHOP_PROMO 9
#define ROOK_PROMO 10
#define QUEEN_PROMO 11

#define EP_OFF 768
#define C_OFF 776
#define SIDE_OFF 780

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

extern int *EVAL_TABLES[];

extern int *MVV_LVA_SCORES[];

extern uint64_t HASH_VALUES[781];

/* structs and typedefs */

typedef struct {
    uint64_t board_hash;
    int c_rights;
    int ep_square;
    int h_clk;
    int check_info;
} HISTORY_ENTRY;

typedef struct {
    unsigned int start: 8;
    unsigned int dest: 8;
    unsigned int flags: 4;
    unsigned int captured_piece: 12;
    unsigned int side: 2;
    unsigned int score: 21;
} move_t;

extern move_t NULL_MOVE;

typedef struct {
    int index;
    move_t moves[256];
} MOVE_LIST;

typedef void (*GEN_FROM_POSITION)(int, MOVE_LIST*);
typedef void (*MOVE_GENERATOR)(int, int, MOVE_LIST*);

typedef struct {
    uint64_t key;
    move_t best_move;
} TABLE_ENTRY;

typedef struct {
    TABLE_ENTRY *table;
    int n_entries;
} HASH_TABLE;

typedef struct {
    int start_time;
    int stop_time;
    int depth;
    int depth_lim;
    int time_lim;
    int move_lim;
    int infinite;
    int quit;
    int stopped;
    long nodes;

    float fh;
    float fhf;
} SEARCH_INFO;

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

extern move_t move_history[HISTORY_TABLE_SIZE];
extern uint8_t repetition_table[REPETITION_TABLE_SIZE];

extern HASH_TABLE pv_table[1];
extern move_t pv_line[MAX_DEPTH];

extern int* search_history[];
extern move_t search_killers[2][HISTORY_TABLE_SIZE];

/* functions to change/query the current position */

void flip_position(void);
int set_position(char *fen_str);

void switch_side(void);

void save_state(void);
void restore_state(void);

bool is_repetition(void);

/* deallocating tables */

void free_tables(void);

/* move encoding/decoding, getting and comparison functions */

move_t get_move(int start, int dest, int flags);

bool moves_equal(move_t mv1, move_t mv2);
bool is_null_move(move_t mv);

int move_to_int(move_t mv);
move_t move_of_int(int m_int);

/* move-making functions */

int is_square_attacked(int pos);
int make_move(move_t mv);
void unmake_move(move_t mv);

/* move generation functions */

void all_moves(MOVE_LIST *moves);
bool move_exists(move_t mv);

/* perft and divide functions */

uint64_t perft(int depth);
void divide(int depth);

/* hashing-related functions */

void set_hash(void);
void update_hash(move_t mv);

void clear_table(void);

void store_move(move_t mv);
move_t get_pv_move(void);

int get_pv_line(int depth);

/* engine search functions */

void search(SEARCH_INFO *s_info);

/* string-related functions */

void board_to_fen(char *fen_str);
int fen_to_board_array(char *fen_str);
bool fen_match(char *fen_str);
void print_board(void);

void move_to_string(move_t mv, char* mstr);
move_t string_to_move(char *mstr);

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