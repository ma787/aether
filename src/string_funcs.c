#include <regex.h>
#include <stdio.h>
#include <string.h>
#include "aether.h"

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
            fen_str[j++] = LETTER(sq);
            i++;
        }
    }

    fen_str[j++] = ' ';
    fen_str[j++] = pstn->side == WHITE ? 'w' : 'b';
    fen_str[j++] = ' ';

    strcpy(fen_str + j, CASTLE_STRINGS[pstn->c_rights]);
    j += strlen(CASTLE_STRINGS[pstn->c_rights]);
    fen_str[j++] = ' ';

    if (pstn->ep_sq) {
        strcpy(fen_str + j, COORD(pstn->ep_sq));
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
                    pstn->board[i++] = piece;
                    int p_type = PTYPE(piece);
                    int colour = piece & COLOUR_MASK;
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
            b_str[j++] = LETTER(sq);
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

    strcpy(mstr, COORD(mv.start));
    strcpy(mstr + 2, COORD(mv.dest));
    if (mv.flags & PROMO_FLAG) {
        mstr[4] = LETTER(PROMOTIONS[mv.flags & 3] | BLACK);
        mstr[5] = '\0';
    }
}

move_t string_to_move(POSITION *pstn, char *mstr) {
    if (move_match(mstr) == false) {
        return NULL_MOVE;
    }

    int start = INDEX(mstr);
    int dest = INDEX((mstr + 2));
    int flags = Q_FLAG;

    if (pstn->board[dest]) {
        flags |= CAPTURE_FLAG;
    };

    int c_start = (pstn->side == WHITE) ? E1 : E8;

    switch(PTYPE(pstn->board[start])) {
        case KING:
            if (start == c_start) {
                if (FILE(dest) == FILE_C) {
                    flags |= Q_CASTLE_FLAG;
                } else if (FILE(dest) == FILE_G) {
                    flags |= K_CASTLE_FLAG;
                }
            }
            break;
        case PAWN:
            if (
                RANK(start) == SECOND_RANK[pstn->side] 
                && RANK(dest) == DPP_RANK[pstn->side]
            ) {
                flags = DPP_FLAG;
            } else if (dest == pstn->ep_sq) {
                flags = EP_FLAG;
            } else if (RANK(dest) == FINAL_RANK[pstn->side]) {
                flags |= PROMO_FLAG;
                switch(PTYPE(PIECES[(int) mstr[4]])) {
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
