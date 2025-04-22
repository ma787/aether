#include "aether.h"

uint64_t get_hash(uint64_t pos, uint64_t piece) {
    int sq_off = (96 * get_rank(pos)) + (12 * get_file(pos));

    if (piece & BLACK) {
        sq_off += 6;
    }

    switch(piece & 0xFC) {
        case PAWN:
            return HASH_VALUES[sq_off];
        case KNIGHT:
            return HASH_VALUES[sq_off + 1];
        case BISHOP:
            return HASH_VALUES[sq_off + 2];
        case ROOK:
            return HASH_VALUES[sq_off + 3];
        case QUEEN:
            return HASH_VALUES[sq_off + 4];
        case KING:
            return HASH_VALUES[sq_off + 5];
    }

    return 0;
}

void set_hash(POSITION *pstn) {
    pstn->key = 0UL;
    int i = 0x44;

    while (i < 0xBC) {
        int sq = pstn->board[i];
        int colour = sq & COLOUR_MASK;
        if (colour == WHITE || colour == BLACK) {
            pstn->key ^= get_hash(i++, sq);
        } else if (colour == G) {
            i += 8;
        } else {
            i++;
        }
    }

    if (pstn->side == BLACK) {
        pstn->key ^= HASH_VALUES[SIDE_OFF];
    }

    if (pstn->ep_sq) {
        pstn->key ^= HASH_VALUES[EP_OFF + get_file(pstn->ep_sq)];
    }

    for (int j = 0; j < 4; j++) {
        if (pstn->c_rights & (1 << j)) {
            pstn->key ^= HASH_VALUES[C_OFF + j];
        }
    }
}

void update_hash(POSITION *pstn, move_t mv) {
    int start = mv.start, dest = mv.dest;
    int piece = pstn->board[dest];

    HISTORY_ENTRY old_info = pstn->history[pstn->ply - 1];

    int new_c_rights = old_info.c_rights & (
        (start != A1)
        | ((start != H1) << 1)
        | ((dest != A8) << 2)
        | ((dest != H8) << 3)
    );

    if (pstn->side == BLACK) {
        start = flip_square(start);
        dest = flip_square(dest);
        piece = (piece & 0xFFC) | BLACK;
        new_c_rights = ((new_c_rights & 12) >> 2) | ((new_c_rights & 3) << 2);
    }

    pstn->key ^= get_hash(start, piece);
    pstn->key ^= get_hash(dest, piece);

    if (old_info.ep_sq) {
        pstn->key ^= HASH_VALUES[EP_OFF + get_file(old_info.ep_sq)];
    }

    pstn->key ^= HASH_VALUES[SIDE_OFF];

    if (mv.flags == K_CASTLE_FLAG || mv.flags == Q_CASTLE_FLAG) {
        int r_start = (mv.flags == K_CASTLE_FLAG) ? H1 : A1;
        int r_dest = (mv.flags == K_CASTLE_FLAG) ? F1 : D1;

        if (pstn->key == BLACK) {
            r_start = flip_square(r_start);
            r_dest = flip_square(r_dest);

            pstn->key ^= HASH_VALUES[C_OFF + 2];
            pstn->key ^= HASH_VALUES[C_OFF + 3];
        } else {
            pstn->key ^= HASH_VALUES[C_OFF];
            pstn->key ^= HASH_VALUES[C_OFF + 1];
        }
        
        pstn->key ^= get_hash(r_start, pstn->side | ROOK);
        pstn->key ^= get_hash(r_dest, pstn->side | ROOK);

        return;
    }

    if (mv.flags == DPP_FLAG) {
        pstn->key ^= HASH_VALUES[EP_OFF + get_file(dest)];
        return;
    }

    if (mv.flags & PROMO_FLAG) {
        pstn->key ^= get_hash(start, PAWN | pstn->side);
        pstn->key ^= get_hash(start, piece);
    }

    if (mv.flags & CAPTURE_FLAG) {
        int cap_pos = dest, cap_piece = mv.captured_piece, off = S;

        if (pstn->side == BLACK) {
            cap_piece = (cap_piece & 0xFC) | WHITE;
            off = N;
        }
        
        if (mv.flags == EP_FLAG) {
            cap_pos += off;
        }

        pstn->key ^= get_hash(cap_pos, cap_piece);
    }

    for (int i = 0; i < 4; i++) {
        if (old_info.c_rights & i) {
            pstn->key ^= HASH_VALUES[C_OFF + i];
        }
        if (new_c_rights & i) {
            pstn->key ^= HASH_VALUES[C_OFF + i];
        }
    }
}

void clear_pv_table(POSITION *pstn) {
    TABLE_ENTRY *t_entry = (pstn->pv_table)->table;

    while (t_entry < (pstn->pv_table)->table + (pstn->pv_table)->n_entries) {
        t_entry->key = 0UL;
        t_entry->best_move = NULL_MOVE;
        t_entry++;
    }
}

void store_move(POSITION *pstn, move_t mv) {
    int index = pstn->key % (pstn->pv_table)->n_entries;

    (pstn->pv_table)->table[index].key = pstn->key;
    (pstn->pv_table)->table[index].best_move = mv;
}

move_t get_pv_move(POSITION *pstn) {
    int index = pstn->key % (pstn->pv_table)->n_entries;

    if ((pstn->pv_table)->table[index].key == pstn->key) {
        return (pstn->pv_table)->table[index].best_move;
    }

    return NULL_MOVE;
}

int get_pv_line(POSITION *pstn, int depth) {
    move_t mv = get_pv_move(pstn);
    int count = 0;

    while (!is_null_move(mv) && count < depth) {
        if (move_exists(pstn, mv)) {
            pstn->pv_line[count++] = mv;
            make_move(pstn, mv);
        } else {
            break;
        }
        mv = get_pv_move(pstn);
    }

    for (int i = count - 1; i >= 0; i--) {
        unmake_move(pstn, pstn->pv_line[i]);
    }

    return count;
}