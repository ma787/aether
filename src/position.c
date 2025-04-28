#include <stdlib.h>
#include <string.h>
#include "aether.h"

void swap_piece_lists(int **w_pieces, int **b_pieces) {
    int *tmp = *w_pieces;
    *w_pieces = *b_pieces;
    *b_pieces = tmp;
}

void swap_pawn_bitboards(uint64_t *w_pawns, uint64_t *b_pawns) {
    uint64_t tmp = *w_pawns;
    *w_pawns = *b_pawns;
    *b_pawns = tmp;
}

void flip_position(POSITION *pstn) {
    pstn->c_rights = ( 
    ((pstn->c_rights & 12) >> 2) 
    | ((pstn->c_rights & 3) << 2)
    );

    swap_piece_lists(&(pstn->w_pieces), &(pstn->b_pieces));
    swap_pawn_bitboards(&(pstn->w_pawns), &(pstn->b_pawns));

    for (int i = 0; i < 32; i++) {
        if (pstn->piece_list[i]) {
            pstn->piece_list[i] = flip_square(pstn->piece_list[i]);
        }
    }

    flip_bits(&pstn->w_pawns);
    flip_bits(&pstn->b_pawns);

    if (pstn->ep_sq) {
        pstn->ep_sq = flip_square(pstn->ep_sq);
    }

    if (pstn->check) {
        pstn->fst_checker = flip_square(pstn->fst_checker);
        if (pstn->check == DOUBLE_CHECK) {
            pstn->snd_checker = flip_square(pstn->snd_checker);
        }
    }
    
    for (int i = A1; i < A5; i += 0x10) {
        for (int j = 0; j < 8; j++) {
            int k = flip_square(i);
            int sq1 = pstn->board[i + j];
            int sq2 = pstn->board[k + j];
            pstn->board[i + j] = sq2 ? change_piece_colour(sq2, (~sq2 & 3)) : 0;
            pstn->board[k + j] = sq1 ? change_piece_colour(sq1, (~sq1 & 3)) : 0;
        }       
    };
}

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

void set_check(POSITION *pstn) {
    if (pstn->side == WHITE) {
        flip_position(pstn);
    }

    pstn->check = 0;
    pstn->fst_checker = 0;
    pstn->snd_checker = 0;

    int k_pos = pstn->b_pieces[0];
    int alignment, piece, w_pos, current, step;

    for (int i = 1; i < 16; i++) {
        w_pos = pstn->w_pieces[i];
        piece = pstn->board[w_pos];
        alignment = get_alignment(w_pos, k_pos);

        if (alignment & piece) {
            add_checker(pstn, w_pos, CONTACT_CHECK);
        } else if ((alignment >> 8) & piece) {
            current = w_pos;
            step = get_step(w_pos, k_pos);

            for (;;) {
                current += step;
                if (current == k_pos) {
                    add_checker(pstn, w_pos, DISTANT_CHECK);
                    break;
                } 
                if (pstn->board[current]) {
                    break;
                }
            }
        }
    }

    piece = pstn->board[k_pos + S + E];
    if ((piece & WHITE) && (piece & PAWN)) {
        add_checker(pstn, k_pos + S + E, CONTACT_CHECK);
    } else {
        piece = pstn->board[k_pos + S + W];
        if ((piece & WHITE) && (piece & PAWN)) {
            add_checker(pstn, k_pos + S + W, CONTACT_CHECK);
        }
    }

    if (pstn->side == WHITE) {
        flip_position(pstn);
    }
}

void set_piece_list(POSITION *pstn) {
    int i = A1, sq, colour;
    int king_offs[2] = {0, 16};
    int list_offs[2] = {1, 17};

    while (i <= H8) {
        sq = pstn->board[i];
        colour = sq & COLOUR_MASK;

        if (!sq) {
            i++;
        } else if (colour == G) {
            i += 8;
        } else if (sq & KING) {
            pstn->piece_list[king_offs[colour - 1]] = i++;
        } else {
            pstn->piece_list[list_offs[colour - 1]] = i;
            pstn->board[i++] |= (list_offs[colour - 1]++ << 8);
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

void clear_tables(POSITION *pstn) {
    // clear board
    for (int i = A1; i <= A8; i += 0x10) {
        memset(pstn->board + i, 0, 8 * sizeof(int));
    }

    // reset piece lists
    memset(pstn->piece_list, 0, 32 * sizeof(int));
    pstn->w_pieces = &(pstn->piece_list[0]);
    pstn->b_pieces = &(pstn->piece_list[16]);

    // clear bitboards
    pstn->w_pawns = 0ULL;
    pstn->b_pawns = 0ULL;

    // initialise repetition table
    memset(pstn->rep_table, 0, REPETITION_TABLE_SIZE);

    // initialise killer table
    for (int i = 0; i < HISTORY_TABLE_SIZE; i++) {
        pstn->search_killers[0][i] = NULL_MOVE;
        pstn->search_killers[1][i] = NULL_MOVE;
    }
}

POSITION* new_position(void) {
    // allocate struct and set up initial position
    POSITION *pstn = malloc(sizeof(POSITION));
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
    pstn->side = WHITE;
    pstn->c_rights = 0b1111;
    pstn->ep_sq = 0;
    pstn->h_clk = 0;
    pstn->check = 0;
    pstn->fst_checker = 0;
    pstn->snd_checker = 0;

    fen_to_board_array(pstn, START_POS);
    set_piece_list(pstn);
    set_hash(pstn);

    // allocate memory for pv table
    (pstn->pv_table)->n_entries = PV_TABLE_SIZE / sizeof(TABLE_ENTRY);
    (pstn->pv_table)->n_entries -= 2; // ensures that memory is not overrun
    (pstn->pv_table)->table = (TABLE_ENTRY *) malloc((pstn->pv_table)->n_entries * sizeof(TABLE_ENTRY));
    clear_pv_table(pstn);

    // allocate and zero initialise memory for history table
    pstn->search_history[PAWN] = calloc(S_HIS_TABLE_SIZE, sizeof(int));
    pstn->search_history[KNIGHT] = calloc(S_HIS_TABLE_SIZE, sizeof(int));
    pstn->search_history[BISHOP] = calloc(S_HIS_TABLE_SIZE, sizeof(int));
    pstn->search_history[ROOK] = calloc(S_HIS_TABLE_SIZE, sizeof(int));
    pstn->search_history[QUEEN] = calloc(S_HIS_TABLE_SIZE, sizeof(int));
    pstn->search_history[KING] = calloc(S_HIS_TABLE_SIZE, sizeof(int));

    return pstn;
}

void free_position(POSITION *pstn) {
    free((pstn->pv_table)->table);
    free(pstn->search_history[PAWN]);
    free(pstn->search_history[KNIGHT]);
    free(pstn->search_history[BISHOP]);
    free(pstn->search_history[ROOK]);
    free(pstn->search_history[QUEEN]);
    free(pstn->search_history[KING]);
    free(pstn);
}

int update_position(POSITION *pstn, char *fen_str) {
    clear_tables(pstn);

    pstn->ply = 0;
    pstn->c_rights = 0;
    pstn->ep_sq = 0;

    int idx;

    if ((idx = fen_to_board_array(pstn, fen_str)) == -1) {
        return 0;
    }

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
        pstn->ep_sq = string_to_coord(fen_str + idx++);
    }
    idx += 2;

    pstn->h_clk = fen_str[idx] - '0';

    set_piece_list(pstn);
    set_check(pstn);
    set_hash(pstn);

    if (pstn->side == BLACK) {
        flip_position(pstn);
    }

    return idx + 2;
}

bool is_repetition(POSITION *pstn) {
    if (!(pstn->rep_table[pstn->key & 0x00003FFF])) {
        return false;
    }

    for (int i = pstn->ply - pstn->h_clk; i < pstn->ply; i++) {
        if (pstn->history[i].key == pstn->key) {
            return true;
        }
    }

    return false;
}