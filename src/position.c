#include <regex.h>
#include <string.h>
#include "position.h"
#include "constants.h"
#include "utils.h"

#define STACK_SIZE 50

struct state {
    unsigned int c_rights: 4;
    unsigned int ep_square : 8;
    unsigned int h_clk: 6;
    unsigned int check_info: 18;
};
typedef struct state state_t;

state_t prev_state[STACK_SIZE];
int top = -1;

char *reg_str = (
    "((([pnbrqkPNBRQK]|[1-8]){1,})[/]){7}([pnbrqkPNBRQK]|[1-8]){1,}[ ]"
    "[bw][ ](([K]?[Q]?[k]?[q]?)|-)[ ](([a-h][36])|-)([ ][0-9]+){2}"
);

unsigned int board[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, G, G, G, G, G, G, G, G, G, G, G, G, 0, 0,
    0, 0, G, G, G, G, G, G, G, G, G, G, G, G, 0, 0,
    0, 0, G, G, 0, 0, 0, 0, 0, 0, 0, 0, G, G, 0, 0,
    0, 0, G, G, 0, 0, 0, 0, 0, 0, 0, 0, G, G, 0, 0,
    0, 0, G, G, 0, 0, 0, 0, 0, 0, 0, 0, G, G, 0, 0,
    0, 0, G, G, 0, 0, 0, 0, 0, 0, 0, 0, G, G, 0, 0,
    0, 0, G, G, 0, 0, 0, 0, 0, 0, 0, 0, G, G, 0, 0,
    0, 0, G, G, 0, 0, 0, 0, 0, 0, 0, 0, G, G, 0, 0,
    0, 0, G, G, 0, 0, 0, 0, 0, 0, 0, 0, G, G, 0, 0,
    0, 0, G, G, 0, 0, 0, 0, 0, 0, 0, 0, G, G, 0, 0,
    0, 0, G, G, G, G, G, G, G, G, G, G, G, G, 0, 0,
    0, 0, G, G, G, G, G, G, G, G, G, G, G, G, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

unsigned int white_pieces[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned int black_pieces[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

unsigned int *w_pieces = white_pieces;
unsigned int *b_pieces = black_pieces;

unsigned int side = WHITE;
unsigned int c_rights = 0xF;
unsigned int ep_square = 0;
unsigned int h_clk = 0;
unsigned int check_info = 0;


int fen_match(char *fen_str) {
    regex_t preg;
    int res;

    regcomp(&preg, reg_str, REG_EXTENDED);
    res = regexec(&preg, fen_str, (size_t) 0, NULL, 0);
    regfree(&preg);

    if (res != 0) {
        return -1;
    }

    return 0;
}

int parse_board_string(char *fen_str) {
    if (fen_match(fen_str) != 0) {
        return -1;
    }

    int i = A8;
    int j = 0;
    int count = 0;

    for (;;) {
        char val = fen_str[j++];
        switch(val) {
            case ' ':
                if (count != 8) {
                    return -1;
                }
                return j;
            case '/':
                if (count != 8) {
                    return -1;
                }
                i -= 0x18;
                count = 0;
                break;
            default:
                int piece = PIECES[(int) val];
                if (piece) {
                    board[i++] = piece;
                    count++;
                } else {
                    int inc = val - '0';
                    i += inc;
                    count += inc;
                }
        }
    }
}

int is_square_attacked(int pos) {
    for (int i = 0; i < 16; i++) {
        int vec = SUPERPIECE[i];
        int current = pos + vec;
        int contact = 1;
        int sq;

        for (;;) {
            sq = board[current];

            if ((sq & COLOUR_MASK) == BLACK) {
                switch(is_attacking(sq, current, pos)) {
                    case CONTACT_CHECK:
                        if (contact) { return 1; }
                        break;
                    case DISTANT_CHECK:
                        return 1;
                }
                break;
            } else if (sq) {
                break;
            }

            current += vec;
            contact = 0;
        }
    }

    return 0;
}

void flip_position() {
    c_rights = ((c_rights & 12) >> 2) | ((c_rights & 3) << 2);

    // swap pointers for white and black piece lists
    unsigned int **pwp = &w_pieces;
    unsigned int **pbp = &b_pieces;
    unsigned int *tmp = w_pieces;
    *pwp = *pbp;
    *pbp = tmp;

    for (int i = 0; i < 16; i++) {
        if (w_pieces[i]) {
            w_pieces[i] = flip_square(w_pieces[i]);
        }
        if (b_pieces[i]) {
            b_pieces[i] = flip_square(b_pieces[i]);
        }
    }

    if (ep_square) {
        ep_square = flip_square(ep_square);
    }

    if (check_info) {
        int check = check_info & 3;
        int first_checker = flip_square((check_info >> 2) & 0xFF);
        if (check == DOUBLE_CHECK) {
            check |= (flip_square((check_info >> 10) & 0xFF) << 10);
        }
        check_info = (check | (first_checker << 2));
    }
    
    for (int i = A1; i < A5; i += 0x10) {
        for (int j = 0; j < 8; j++) {
            int k = flip_square(i);
            int sq1 = board[i + j];
            int sq2 = board[k + j];
            board[i + j] = sq2 ? (sq2 & 0xFFC) | (~sq2 & 3) : 0;
            board[k + j] = sq1 ? (sq1 & 0xFFC) | (~sq1 & 3) : 0;
        }       
    };
}

void set_check() {
    int flipped = 0;
    if (side == WHITE) {
        flipped = 1;
        flip_position();
    }

    check_info = 0;
    int k_pos = b_pieces[0];

    for (int i = 0; i < 16; i++) {
        int vec = SUPERPIECE[i];
        int current = k_pos + vec;
        int sq;

        for (;;) {
            sq = board[current];
            int colour = sq & COLOUR_MASK;

            if (colour == G || colour == BLACK) {
                goto exit_loop;
            } else if (colour == WHITE) {
                int attack = is_attacking(sq, current, k_pos);

                if (attack) {
                    check_info |= attack;
                    if ((check_info >> 2) & 0xFF) {
                        check_info |= (current << 10);
                        check_info |= DOUBLE_CHECK;
                    } else {
                        check_info |= (current << 2);
                    }
                }
                goto exit_loop;
            } else if (i > 8) {  // cannot move >1 step in knight direction
                goto exit_loop;
            } 
              else {
                current += vec;
            }
        }
        exit_loop:
            if (check_info == DOUBLE_CHECK) {
                if (flipped) {
                    flip_position();
                }
                return;
            }
    }

    if (flipped) {
        flip_position();
    }
}

int set_position(char *fen_str) {
    // clear previous board position
    for (int i = A1; i <= A8; i += 0x10) {
        memset(board + i, 0, 8 * sizeof(int));
    }
    memset(white_pieces, 0, 16 * sizeof(int));
    memset(black_pieces, 0, 16 * sizeof(int));

    int idx = parse_board_string(fen_str);

    if (idx == -1) {
        return -1;
    }

    side = fen_str[idx++] == 'w' ? WHITE : BLACK;
    c_rights = 0;

    if (fen_str[++idx] == '-') {    
        idx += 2;
    } else {
        char val;
        do {
            val = fen_str[idx++];
            c_rights |= CASTLING_RIGHTS[(int) val];
        } while (val != ' ');
    }

    if (fen_str[idx] == '-') {
        ep_square = 0;
        idx += 2;
    } else {
        ep_square = string_to_coord(fen_str + idx);
        idx += 3;
    }

    h_clk = fen_str[idx] - '0';

    int i = A1;
    int w_off = 1;
    int b_off = 1;

    while (i <= H8) {
        int sq = board[i];
        int off;

        switch (sq & COLOUR_MASK) {
            case G:
                i += 8;
                continue;
            case WHITE:
                off = sq & KING ? 0 : w_off++;
                w_pieces[off] = i;
                board[i] |= (off << 8);
                break;
            case BLACK:
                off = sq & KING ? 0 : b_off++;
                b_pieces[off] = i;
                board[i] |= (off << 8);
                break;
        }
        i++;
    }

    set_check();
    if (side == BLACK) {
        flip_position();
    }

    return 0;
}

void switch_side() { side = ~side & 3; }

void save_state() {
    state_t state = {
        .c_rights = c_rights,
        .ep_square = ep_square,
        .h_clk = h_clk,
        .check_info = check_info
    };

    prev_state[++top] = state;
}

void restore_state() {
    state_t state = prev_state[top--];

    c_rights = state.c_rights;
    ep_square = state.ep_square;
    h_clk = state.h_clk;
    check_info = state.check_info;
}

void to_fen(char *fen_str) {
    if (side != WHITE) {
        flip_position();
    }

    int i = A8;
    int j = 0;
    int sq;

    while (i != 0x4C) {
        sq = board[i];

        if (sq == G) {
            fen_str[j++] = '/';
            i -= 0x18;
        } else if (sq == 0) {
            int count = 0;
            while (sq == 0) {
                count++;
                sq = board[++i];
            }
            fen_str[j++] = '0' + count;
        } else {
            fen_str[j++] = SYMBOLS[sq & 0xFF];
            i++;
        }
    }

    fen_str[j++] = ' ';
    fen_str[j++] = side == WHITE ? 'w' : 'b';
    fen_str[j++] = ' ';

    if (c_rights) {
        if (c_rights & WHITE_KINGSIDE) {
            fen_str[j++] = 'K';
        }
        if (c_rights & WHITE_QUEENSIDE) {
            fen_str[j++] = 'Q';
        }
        if (c_rights & BLACK_KINGSIDE) {
            fen_str[j++] = 'k';
        }
        if (c_rights & BLACK_QUEENSIDE) {
            fen_str[j++] = 'q';
        }
    } else {
        fen_str[j++] = '-';
    }
    fen_str[j++] = ' ';

    if (ep_square) {
        strcpy(fen_str + j, coord_to_string(ep_square));
        j += 2;
    } else {
        fen_str[j++] = '-';
    }
    fen_str[j++] = ' ';
    fen_str[j++] = '0' + h_clk;
    fen_str[j++] = ' ';
    fen_str[j++] = '1'; // fullmove number not implemented
    fen_str[j] = '\0';
}
