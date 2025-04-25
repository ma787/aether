#include <regex.h>
#include <stdio.h>
#include <string.h>
#include "aether.h"

int string_to_coord(char *sqr_str) {
    return ((sqr_str[1] - '1') << 4) + (sqr_str[0] - 'a') + A1;
}

char* coord_to_string(int pos) { return COORDS[pos]; }

bool reg_match(char *input_str, char *reg_str) {
    regex_t preg;
    int res;

    regcomp(&preg, reg_str, REG_EXTENDED);
    res = regexec(&preg, input_str, (size_t) 0, NULL, 0);
    regfree(&preg);

    if (res != 0) {
        return false;
    }

    return true;
}

bool fen_match(char *fen_str) {
    return reg_match(fen_str, FEN_REGEX);
}

bool move_match(char *mstr) {
    return reg_match(mstr, MOVE_REGEX);
}

void board_to_fen(POSITION *pstn, char *fen_str) {
    if (pstn->side != WHITE) {
        flip_position(pstn);
    }

    int i = A8, j = 0, sq;

    while (i != 0x4C) {
        sq = pstn->board[i];

        if (sq == G) {
            fen_str[j++] = '/';
            i -= 0x18;
        } else if (sq == 0) {
            int count = 0;
            while (sq == 0) {
                count++;
                sq = pstn->board[++i];
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

    if (pstn->ep_sq) {
        strcpy(fen_str + j, coord_to_string(pstn->ep_sq));
        j += 2;
    } else {
        fen_str[j++] = '-';
    }
    fen_str[j++] = ' ';
    fen_str[j++] = '0' + pstn->h_clk;
    fen_str[j++] = ' ';
    fen_str[j++] = '1'; // fullmove number not implemented
    fen_str[j] = '\0';

    if (pstn->side != WHITE) {
        flip_position(pstn);
    }
}

int fen_to_board_array(POSITION *pstn, char *fen_str) {
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
                    if (piece & PAWN) {
                        if (piece & WHITE) {
                            set_bit(&pstn->w_pawns, coord_to_index(i));
                        } else {
                            set_bit(&pstn->b_pawns, coord_to_index(i));
                        }
                    }

                    pstn->board[i++] = piece;
                    count++;
                } else {
                    int inc = val - '0';
                    i += inc;
                    count += inc;
                }
        }
    }
}

void print_board(POSITION *pstn) {
    char b_str[72];
    int i = 0xB4, j = 0, sq;

    while (i != 0x4C) {
        sq = pstn->board[i];

        if (sq == G) {
            b_str[j++] = '\n';
            i -= 0x18;
        } else {
            b_str[j++] = SYMBOLS[sq & 0xFF];
            i++;
        }
    }
    
    b_str[j] = '\0';
    printf("%s\n", b_str);
}

void move_to_string(move_t mv, char* mstr) {
    if (is_null_move(mv)) {
        strcpy(mstr, "none");
        return;
    }

    int start = mv.start, dest = mv.dest;

    if (mv.side == BLACK) {
        start = flip_square(start);
        dest = flip_square(dest);
    }

    strcpy(mstr, coord_to_string(start));
    strcpy(mstr + 2, coord_to_string(dest));
    if (mv.flags & PROMO_FLAG) {
        mstr[4] = SYMBOLS[PROMOTIONS[mv.flags & 3] | BLACK];
        mstr[5] = '\0';
    }
}

move_t string_to_move(POSITION *pstn, char *mstr) {
    if (move_match(mstr) == false) {
        return NULL_MOVE;
    }

    int start = string_to_coord(mstr);
    int dest = string_to_coord(mstr + 2);
    int flags = Q_FLAG;

    if (pstn->side == BLACK) {
        start = flip_square(start);
        dest = flip_square(dest);
    }

    if (pstn->board[dest]) {
        flags |= CAPTURE_FLAG;
    };

    switch(pstn->board[start] & 0x0FC) {
        case KING:
            if (start == E1) {
                if (dest == C1) {
                    flags |= Q_CASTLE_FLAG;
                } else if (dest == G1) {
                    flags |= K_CASTLE_FLAG;
                }
            }
            break;
        case PAWN:
            if (get_rank(start) == 1 && get_rank(dest) == 3) {
                flags = DPP_FLAG;
            } else if (dest == pstn->ep_sq) {
                flags = EP_FLAG;
            } else if (get_rank(dest) == 7) {
                flags |= PROMO_FLAG;
                switch(PIECES[(int) mstr[4]] & 0xFC) {
                    case BISHOP:
                        flags++;
                        break;
                    case ROOK:
                        flags += 2;
                        break;
                    case QUEEN:
                        flags += 3;
                        break;
                }
            }
            break;
    }

    return get_move(pstn, start, dest, flags);
}
