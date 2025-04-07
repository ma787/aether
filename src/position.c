#include <string.h>
#include "aether.h"

int prev_state[STACK_SIZE];
uint64_t prev_hashes[STACK_SIZE];
int ply = 0;

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

uint64_t board_hash;

HASH_TABLE pv_table[1];
int pv_line[MAX_DEPTH];

void flip_position(void) {
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

void set_check(void) {
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

int parse_board_string(char *fen_str) {
    if (!fen_match(fen_str)) {
        return -1;
    }

    int i = A8, j = 0, count = 0;

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

int set_position(char *fen_str) {
    // clear previous board position
    for (int i = A1; i <= A8; i += 0x10) {
        memset(board + i, 0, 8 * sizeof(int));
    }
    memset(white_pieces, 0, 16 * sizeof(int));
    memset(black_pieces, 0, 16 * sizeof(int));
    ply = 0;

    int idx = parse_board_string(fen_str);

    if (idx == -1) {
        return -1;
    }

    side = (fen_str[idx++] == 'w') ? WHITE : BLACK;
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

    int i = A1, w_off = 1, b_off = 1;

    while (i <= H8) {
        int sq = board[i];
        int colour = sq & COLOUR_MASK;

        if (!sq) {
            i++;
            continue;
        }

        if (sq & KING) {
            if (colour == WHITE) {
                w_pieces[0] = i++;
            } else {
                b_pieces[0] = i++;
            }
            continue;
        }

        switch (colour) {
            case G:
                i += 8;
                continue;
            case WHITE:
                w_pieces[w_off] = i;
                board[i++] |= (w_off++ << 8);
                break;
            case BLACK:
                b_pieces[b_off] = i;
                board[i++] |= (b_off++ << 8);
                break;
        }
    }

    set_hash();
    set_check();

    if (side == BLACK) {
        flip_position();
    }

    return 0;
}

void switch_side(void) { side = ~side & 3; }

void save_state(void) {
    prev_state[ply] = (
        c_rights | (ep_square << 4) | (h_clk << 12) | (check_info << 18)
    );
    prev_hashes[ply++] = board_hash;
}

void restore_state(void) {
    board_hash = prev_hashes[--ply];
    int state = prev_state[ply];

    c_rights = state & 0xF;
    ep_square = (state >> 4) & 0xFF;
    h_clk = (state >> 12) & 0x3F;
    check_info = (state >> 18) & 0x3FFFF;
}
