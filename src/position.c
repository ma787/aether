#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "constants.h"
#include "position.h"
#include "utils.h"

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

unsigned int w_pieces[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned int b_pieces[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

state_t prev_state[STACK_SIZE];
int top = -1;

int parse_board_string(char *fen_str) {
    regex_t preg;
    int res;
    char *reg_str = (
        "((([pnbrqkPNBRQK]|[1-8]){1,})[/]){7}([pnbrqkPNBRQK]|[1-8]){1,}[ ]"
        "[bw][ ](([K]?[Q]?[k]?[q]?)|-)[ ](([a-h][36])|-)([ ][0-9]+){2}"
    );

    regcomp(&preg, reg_str, REG_EXTENDED);
    res = regexec(&preg, fen_str, (size_t) 0, NULL, 0);
    regfree(&preg);

    if (res != 0) {
        return -1;
    }

    int i = 0xB4;
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

int is_attacking(int p_type, int start, int dest) {
    int res = NO_CHECK;
    int diff = square_diff(start, dest);
    if (MOVE_TABLE[diff] & p_type) {
        res = UNIT_VEC[diff] == start - dest ? CONTACT_CHECK : DISTANT_CHECK;
    }
    return res;
}

int is_square_attacked(info *pstn, int pos) {
    if (pstn->side == BLACK) {
        pos = (~pos & 0xF0) | (pos & 0x0F);
    }

    for (int i = 0; i < 16; i++) {
        int vec = SUPERPIECE[i];
        int current = pos + vec;
        int contact = 1;
        int sq;

        for (;;) {
            sq = pstn->arr[current];

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

void set_check(info *pstn) {
    pstn->check_info = 0;
    int k_pos = pstn->w_pieces[0];

    for (int i = 0; i < 16; i++) {
        int vec = SUPERPIECE[i];
        int current = k_pos + vec;
        int sq;

        for (;;) {
            sq = pstn->arr[current];
            int colour = sq & COLOUR_MASK;

            if (colour == G || colour == WHITE) {
                goto exit_loop;
            } else if (colour == BLACK) {
                int attack = is_attacking(sq, current, k_pos);

                if (attack) {
                    pstn->check_info |= attack;
                    if ((pstn->check_info >> 2) & 0xFF) {
                        pstn->check_info |= (current << 10);
                        pstn->check_info |= DOUBLE_CHECK;
                    } else {
                        pstn->check_info |= (current << 2);
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
            if (pstn->check_info == DOUBLE_CHECK) {
                return;
            }
    }
}

info *new_position(char *fen_str) {
    info *pstn = NULL;
    int idx = parse_board_string(fen_str);

    if (idx == -1) {
        return pstn;
    }

    pstn = malloc(sizeof(info));
    pstn->arr = board;
    pstn->w_pieces = w_pieces;
    pstn->b_pieces = b_pieces;
    
    if (fen_str[idx++] == 'w') {
        pstn->side = WHITE;
    } else {
        pstn->side = BLACK;
    }

    pstn->c_rights = 0;

    if (fen_str[++idx] == '-') {    
        idx += 2;
    } else {
        char val;
        do {
            val = fen_str[idx++];
            pstn->c_rights |= CASTLING_RIGHTS[(int) val];
        } while (val != ' ');
    }

    if (fen_str[idx] == '-') {
        pstn->ep_square = 0;
        idx += 2;
    } else {
        pstn->ep_square = string_to_coord(fen_str + idx);
        idx += 3;
    }

    pstn->h_clk = fen_str[idx] - '0';

    int i = 0x44;
    int w_off = 1;
    int b_off = 1;

    while (i < 0xBC) {
        int sq = pstn->arr[i];
        int off;

        switch (sq & COLOUR_MASK) {
            case G:
                i += 8;
                continue;
            case WHITE:
                off = sq & KING ? 0 : w_off++;
                pstn->w_pieces[off] = i;
                pstn->arr[i] |= (off << 8);
                break;
            case BLACK:
                off = sq & KING ? 0 : b_off++;
                pstn->b_pieces[off] = i;
                pstn->arr[i] |= (off << 8);
                break;
        }
        i++;
    }

    if (pstn->side == BLACK) {
        flip_position(pstn);
    }
    set_check(pstn);

    return pstn;
}

void switch_side(info *pstn) { pstn->side = ~(pstn->side) & 3; }

void save_state(info *pstn) {
    state_t state = {
        .c_rights = pstn->c_rights,
        .ep_square = pstn->ep_square,
        .h_clk = pstn->h_clk,
        .check_info = pstn->check_info
    };

    prev_state[++top] = state;
}

void restore_state(info *pstn) {
    state_t state = prev_state[top--];

    pstn->c_rights = state.c_rights;
    pstn->ep_square = state.ep_square;
    pstn->h_clk = state.h_clk;
    pstn->check_info = state.check_info;
}


void flip_position(info *pstn) {
    pstn->c_rights = ((pstn->c_rights & 12) >> 2) | ((pstn->c_rights & 3) << 2);

    unsigned int *tmp = pstn->w_pieces;
    pstn->w_pieces = pstn->b_pieces;
    pstn->b_pieces = tmp;

    for (int i = 0; i < 16; i++) {
        int wp = pstn->w_pieces[i];
        pstn->w_pieces[i] = (~wp & 0xF0) | (wp & 0x0F);
        int bp = pstn->b_pieces[i];
        pstn->b_pieces[i] = (~bp & 0xF0) | (bp & 0x0F);
    }

    if (pstn->ep_square) {
        pstn->ep_square = (~pstn->ep_square & 0xF0) | (pstn->ep_square & 0x0F);
    }
    
    for (int i = 0x44; i < 0x84; i += 0x10) {
        for (int j = 0; j < 8; j++) {
            int k = (~i & 0xF0) | (i & 0x0F);
            int sq1 = pstn->arr[i + j];
            int sq2 = pstn->arr[k + j];
            pstn->arr[i + j] = sq2 ? (sq2 & 0xFFC) | (~sq2 & 3) : 0;
            pstn->arr[k + j] = sq1 ? (sq1 & 0xFFC) | (~sq1 & 3) : 0;
        }       
    };
}

void to_fen(info *pstn, char *fen_str) {
    if (pstn->side != WHITE) {
        flip_position(pstn);
    }

    int i = 0xB4;
    int j = 0;
    int sq;

    while (i != 0x4C) {
        sq = pstn->arr[i];

        if (sq == G) {
            fen_str[j++] = '/';
            i -= 0x18;
        } else if (sq == 0) {
            int count = 0;
            while (sq == 0) {
                count++;
                sq = pstn->arr[++i];
            }
            fen_str[j++] = '0' + count;
        } else {
            fen_str[j++] = SYMBOLS[sq & 0xFF];
            i++;
        }
    }

    fen_str[j++] = ' ';
    fen_str[j++] = pstn->side == WHITE ? 'w' : 'b';
    fen_str[j++] = ' ';

    if (pstn->c_rights) {
        if (pstn->c_rights & WHITE_KINGSIDE) {
            fen_str[j++] = 'K';
        }
        if (pstn->c_rights & WHITE_QUEENSIDE) {
            fen_str[j++] = 'Q';
        }
        if (pstn->c_rights & BLACK_KINGSIDE) {
            fen_str[j++] = 'k';
        }
        if (pstn->c_rights & BLACK_QUEENSIDE) {
            fen_str[j++] = 'q';
        }
    } else {
        fen_str[j++] = '-';
    }
    fen_str[j++] = ' ';

    if (pstn->ep_square) {
        strcpy(fen_str + j, COORDS[to_index(pstn->ep_square)]);
        j += 2;
    } else {
        fen_str[j++] = '-';
    }
    fen_str[j++] = ' ';
    fen_str[j++] = '0' + pstn->h_clk;
    fen_str[j++] = ' ';
    fen_str[j++] = '1'; // fullmove number not implemented
    fen_str[j] = '\0';
}

void clear_position(info *pstn) {
    for (int i = 0; i < 0x80; i += 0x10) {
        memset(pstn->arr + (0x44 + i), 0, 8 * sizeof(int));
    }
    memset(pstn->w_pieces, 0, 16 * sizeof(int));
    memset(pstn->b_pieces, 0, 16 * sizeof(int));
    free(pstn);
}
