#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>
#include "constants.h"
#include "utils.h"

int piece_from_sym(char symbol) {
    char *res = strchr(LETTERS, symbol);
    if (res == NULL) {
        return -1;
    }
    else {
        return res - LETTERS;
    }
}

int string_to_coord(char *sqr_string) {
    int rank = (sqr_string[1] - '1') << 4;
    int file = strchr(FILES, sqr_string[0]) - FILES;
    return rank + file + 0x44;
}

void coord_to_string(int pos, char *res) {
    char rank = FILES[get_file(pos)];
    char file = '1' + get_rank(pos);
    sprintf(res, "%c%c", rank, file);
}

int parse_board_string(int *arr, char *str) {
    int i = 0xB4;
    int j = 0;
    char val;
    int piece;

    for (;;) {
        char val = str[j++];

        if (val == ' ') {
            break;
        }
        else if (val == '/') {
            i -= 0x18;
            continue;
        }

        piece = piece_from_sym(val);
        if (piece == -1) {
            i += (val - '0');
        } else {
            arr[i++] = piece;
        }
    }

    return j;
}

int validate_fen_string(char *fen_str) {
    regex_t regex;
    char *reg_str = (
        "((([pnbrqkPNBRQK]|[1-8]){1,})[/]){7}(([pnbrqkPNBRQK]|[1-8]){1,})[ ]"
        "[bw][ ](([K]?[Q]?[k]?[q]?)|-)[ ](([a-h][36])|-)[ ]([0-9]+)[ ]([0-9]+)"
    );
    int res;
    char buffer[100];

    regcomp(&regex, reg_str, REG_EXTENDED);
    res = regexec(&regex, fen_str, (size_t) 0, NULL, 0);
    regfree(&regex);

    if (res != 0) {
        return -1;
    }

    int i = 0;
    int count = 0;
    char val;

    do {
        val = fen_str[i++];

        if (val == '/') {
            if (count != 8) {
                return -1;
            } else {
                count = 0;
            }
        } else if (isalpha(val)) {
            count++;
        } else {
            count += (val - '0');
        }
    } while (val != ' '); 

    return 0;
}

info* parse_fen_string(char *fen_str, int *arr) {
    info *pstn = NULL;
    
    if (validate_fen_string(fen_str) == 0) {
        int idx = 0;
        idx = parse_board_string(arr, fen_str);

        pstn = malloc(sizeof(info));
        
        if (fen_str[idx++] == 'w') {
            pstn->side = WHITE;
        } else {
            pstn->side = BLACK;
        }

        if (fen_str[++idx] == '-') {
            pstn->c_rights = 0;
            idx += 2;
        } else {
            char val;
            int c_rights = 0;
            do {
                val = fen_str[idx++];
                switch (val)
                {
                case 'K':
                    c_rights |= (1 << 3);
                    break;
                case 'Q':
                    c_rights |= (1 << 2);
                    break;
                case 'k':
                    c_rights |= (1 << 1);
                    break;
                case 'q':
                    c_rights |= 1;
                    break;
                };
            } while (val != ' ');
            pstn->c_rights = c_rights;
        }

        if (fen_str[idx] == '-') {
            pstn->ep_square = -1;
        } else {
            pstn->ep_square = string_to_coord(fen_str + idx++);
        }
        idx += 2;

        pstn->h_clk = fen_str[idx] - '0';
    }

    pstn->check_info = 0;

    return pstn;
}
