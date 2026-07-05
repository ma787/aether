#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aether.h"

void add_checker(POSITION *pstn, int checker, int check_type) {
    if (pstn->check) {
        if (checker != pstn->fst_checker) {
            pstn->check = DOUBLE_CHECK;
            pstn->snd_checker = checker;
        }
    } else {
        pstn->check = check_type;
        pstn->fst_checker = checker;
    }
}

static void set_check(POSITION *pstn) {
    pstn->check = 0;
    pstn->fst_checker = 0;
    pstn->snd_checker = 0;

    int k_pos = PLIST(pstn)[0];
    int enemy_side = OTHER(pstn->side);

    int e_pawn_pos = k_pos + PAWN_STEP[pstn->side] + E;
    int w_pawn_pos = k_pos + PAWN_STEP[pstn->side] + W;

    int e_pawn = pstn->board[e_pawn_pos];
    if ((e_pawn & enemy_side) && (e_pawn & PAWN)) {
        add_checker(pstn, e_pawn_pos, CONTACT_CHECK);
    } else {
        int w_pawn = pstn->board[w_pawn_pos];
        if ((w_pawn & enemy_side) && (w_pawn & PAWN)) {
            add_checker(pstn, w_pawn_pos, CONTACT_CHECK);
        }
    }

    for (int i = 0; i < 8; i++) {
        int vec = VECS(KNIGHT)[i];
        int piece = pstn->board[k_pos + vec];
        if ((piece & enemy_side) && (piece & KNIGHT)) {
            add_checker(pstn, k_pos + vec, CONTACT_CHECK);
        }
    }

    for (int i = 0; i < 8; i++) {
        int vec = VECS(KING)[i];
        int current = k_pos;
        
        for (;;) {
            current += vec;
            int piece = pstn->board[current];
            if (piece) {
                if (!(SAME_COLOUR(piece, enemy_side))) {
                    break;
                }

                int alignment = ALIGNMENT(current, k_pos);
                if (alignment & piece) {
                    add_checker(pstn, current, CONTACT_CHECK);
                } else if ((alignment >> 8) & piece) {
                    add_checker(pstn, current, DISTANT_CHECK);
                }

                break;
            }
        }
    }
}

static void set_piece_list(POSITION *pstn) {
    int i = A1;
    int king_offs[3] = {0, 0, 16};
    int list_offs[3] = {0, 1, 17};

    while (i <= H8) {
        int sq = pstn->board[i];
        int colour = sq & COLOUR_MASK;

        if (!sq) {
            i++;
        } else if (colour == G) {
            i += 8;
        } else {
            int pos = i++;
            int off = (sq & KING) ? king_offs[colour] : list_offs[colour]++;

            if (!(sq & PAWN)) {
                pstn->big_pieces[colour]++;
            }

            pstn->piece_list[off] = pos;
            pstn->board[pos] |= ((off & 0xF) << 8);
        }
    }
}

void switch_side(POSITION *pstn) { pstn->side = ~(pstn->side) & 3; }

void save_state(POSITION *pstn) {
    HISTORY_ENTRY h_entry = {
        pstn->key, 
        pstn->c_rights, 
        pstn->ep_sq,
        pstn->h_clk,
        pstn->check,
        pstn->fst_checker,
        pstn->snd_checker
    };
    pstn->history[pstn->ply] = h_entry;
}

void restore_state(POSITION *pstn) {
    HISTORY_ENTRY h_entry = pstn->history[pstn->ply];

    pstn->key = h_entry.key;
    pstn->c_rights = h_entry.c_rights;
    pstn->ep_sq = h_entry.ep_sq;
    pstn->h_clk = h_entry.h_clk;
    pstn->check = h_entry.check;
    pstn->fst_checker = h_entry.fst_checker;
    pstn->snd_checker = h_entry.snd_checker;
}

static void clear_tables(POSITION *pstn) {
    // clear board
    for (int i = A1; i <= A8; i += 0x10) {
        memset(pstn->board + i, 0, 8 * sizeof(int));
    }

    // clear big piece list
    memset(pstn->big_pieces, 0, 3 * sizeof(int));

    // reset piece lists
    memset(pstn->piece_list, 0, 32 * sizeof(int));
    pstn->p_lists[WHITE] = &(pstn->piece_list[0]);
    pstn->p_lists[BLACK] = &(pstn->piece_list[16]);

    // initialise repetition table
    memset(pstn->rep_table, 0, REPETITION_TABLE_SIZE);

    // initialise killer table
    for (int i = 0; i < HISTORY_TABLE_SIZE; i++) {
        pstn->search_killers[0][i] = NULL_MOVE;
        pstn->search_killers[1][i] = NULL_MOVE;
    }
}

static int fen_to_board(POSITION *pstn, char *fen_str) {
    int i = A8, j = 0;
    for (char val; (val = fen_str[j++]) != ' '; ) {
        if (val == '/') {
            i -= 0x18;
        } else {
            int piece = PIECES[(int) val];
            if (piece) {
                pstn->board[i++] = piece;
            } else {
                i += val - '0';
            }
        }
    }
    return j;
}

POSITION* new_position(void) {
    // allocate struct and set up initial position
    POSITION *pstn = malloc(sizeof(POSITION));

    // zero-initialise board array
    memset(pstn->board, 0, 256 * sizeof(int));

    clear_tables(pstn);

    // adding guard squares to board
    for (int i = 0x20; i < 0xE0; i += 0x10) {
        int row = i >> 4;
    
        if (row == 2 || row == 3 || row == 0xC || row == 0xD) {
            for (int j = 2; j < 14; j++) {
                pstn->board[i + j] = G;
            }
        } else {
            pstn->board[i + 2] = G;
            pstn->board[i + 3] = G;
            pstn->board[i + 12] = G;
            pstn->board[i + 13] = G;
        }
    }

    pstn->ply = 0;
    pstn->s_ply = 0;
    pstn->base_ply = 0;
    pstn->side = WHITE;
    pstn->c_rights = 0b1111;
    pstn->ep_sq = 0;
    pstn->h_clk = 0;
    pstn->check = 0;
    pstn->fst_checker = 0;
    pstn->snd_checker = 0;

    fen_to_board(pstn, START_POS);
    set_piece_list(pstn);
    set_hash(pstn);

    // allocate memory for pv table
    (pstn->hash_table)->n_entries = HASH_TABLE_SIZE / sizeof(TABLE_ENTRY);
    (pstn->hash_table)->n_entries -= 2; // ensures that memory is not overrun
    int table_size = (pstn->hash_table)->n_entries * sizeof(TABLE_ENTRY);
    (pstn->hash_table)->table = (TABLE_ENTRY *) malloc(table_size);
    clear_hash_table(pstn);

    // allocate and zero initialise memory for history table
    for (int i = 0; i < 12; i++) {
        pstn->search_history[i] = calloc(S_HIS_TABLE_SIZE, sizeof(int));
    }

    return pstn;
}

void free_position(POSITION *pstn) {
    free((pstn->hash_table)->table);

    for (int i = 0; i < 12; i++) {
        free(pstn->search_history[i]);
    }
    free(pstn);
}

static bool fen_match(char *fen_str) {
    int j = 0;

    for (int rank = 0; rank < 8; rank++) {
        int count = 0;
        while (fen_str[j] != '/' && fen_str[j] != ' ' && fen_str[j] != '\0') {
            char c = fen_str[j++];
            if (c >= '1' && c <= '8') {
                count += c - '0';
            } else if (PIECES[(int) c]) {
                count++;
            } else {
                return false;
            }
        }
        if (count != 8) return false;
        if (rank < 7) {
            if (fen_str[j++] != '/') return false;
        }
    }
    if (fen_str[j++] != ' ') return false;

    if (fen_str[j] != 'w' && fen_str[j] != 'b') return false;
    j++;
    if (fen_str[j++] != ' ') return false;

    if (fen_str[j] == '-') {
        j++;
    } else {
        int start = j;
        if (fen_str[j] == 'K') j++;
        if (fen_str[j] == 'Q') j++;
        if (fen_str[j] == 'k') j++;
        if (fen_str[j] == 'q') j++;
        if (j == start) return false;
    }
    if (fen_str[j++] != ' ') return false;

    if (fen_str[j] == '-') {
        j++;
    } else {
        if (fen_str[j] < 'a' || fen_str[j] > 'h') return false;
        j++;
        if (fen_str[j] != '3' && fen_str[j] != '6') return false;
        j++;
    }
    if (fen_str[j++] != ' ') return false;

    if (fen_str[j] < '0' || fen_str[j] > '9') return false;
    while (fen_str[j] >= '0' && fen_str[j] <= '9') j++;
    if (fen_str[j++] != ' ') return false;

    if (fen_str[j] < '0' || fen_str[j] > '9') return false;
    while (fen_str[j] >= '0' && fen_str[j] <= '9') j++;

    return fen_str[j] == '\0' || fen_str[j] == '\n' || fen_str[j] == '\r'
        || fen_str[j] == ' ';
}

int update_position(POSITION *pstn, char *fen_str) {
    if (!fen_match(fen_str)) {
        return -1;
    }

    clear_tables(pstn);

    pstn->ply = 0;
    pstn->s_ply = 0;
    pstn->c_rights = 0;
    pstn->ep_sq = 0;

    int idx = fen_to_board(pstn, fen_str);
    pstn->side = (fen_str[idx++] == 'w') ? WHITE : BLACK;

    if (fen_str[++idx] == '-') {    
        idx += 2;
    } else {
        char val;
        do {
            val = fen_str[idx++];
            pstn->c_rights |= CASTLING_RIGHTS[(int) val];
        } while (val != ' ');
    }

    if (fen_str[idx] != '-') {
        pstn->ep_sq = INDEX((fen_str + idx));
        idx++;
    }
    idx += 2;

    char *end;
    pstn->h_clk = (int) strtol(fen_str + idx, &end, 10);

    int full_move = (int) strtol(end, &end, 10);
    pstn->base_ply = 2 * (full_move - 1) + (pstn->side == BLACK ? 1 : 0);
    idx = (int) (end - fen_str);

    set_piece_list(pstn);
    set_check(pstn);
    set_hash(pstn);

    return idx;
}

bool is_repetition(POSITION *pstn) {
    if (!(pstn->rep_table[pstn->key & REP_TABLE_MASK])) {
        return false;
    }

    for (int i = pstn->ply - pstn->h_clk; i < pstn->ply; i++) {
        if (pstn->history[i].key == pstn->key) {
            return true;
        }
    }

    return false;
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
    j += sprintf(fen_str + j, "%d", pstn->h_clk);
    fen_str[j++] = ' ';
    j += sprintf(fen_str + j, "%d", (pstn->base_ply + pstn->ply) / 2 + 1);
    fen_str[j] = '\0';
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
