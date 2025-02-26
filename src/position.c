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

    if (pstn->side == BLACK) {
        flip_position(pstn);
    }

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
                break;
            case BLACK:
                off = sq & KING ? 0 : b_off++;
                pstn->b_pieces[off] = i;
                break;
        }
        pstn->arr[i] |= (i << 8);
        i++;
    }

    return pstn;
}

void flip_position(info *pstn) {
    pstn->side = ~(pstn->side) & 3;
    pstn->c_rights = ((pstn->c_rights & 12) >> 2) | ((pstn->c_rights & 3) << 2);

    unsigned int *tmp = pstn->w_pieces;
    pstn->w_pieces = pstn->b_pieces;
    pstn->b_pieces = tmp;

    for (int i = 0x44; i < 0x84; i += 0x10) {
        for (int j = 0; j < 8; j++) {
            int k = (~i & 0xF0) ^ (i & 0x0F);
            int sq1 = pstn->arr[i + j];
            int sq2 = pstn->arr[k + j];
            pstn->arr[i] = sq2 ^ (sq2 & 3);
            pstn->arr[k] = sq1 ^ (sq1 & 3);
        }       
    };
}

void clear_position(info *pstn) {
    for (int i = 0; i < 0x80; i += 0x10) {
        memset(pstn->arr + (0x44 + i), 0, 8 * sizeof(int));
    }
    memset(pstn->w_pieces, 0, 16 * sizeof(int));
    memset(pstn->b_pieces, 0, 16 * sizeof(int));
    free(pstn);
}
