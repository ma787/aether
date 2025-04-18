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

void set_hash(void) {
    board_hash = 0UL;
    int i = 0x44;

    while (i < 0xBC) {
        int sq = board[i];
        int colour = sq & COLOUR_MASK;
        if (colour == WHITE || colour == BLACK) {
            board_hash ^= get_hash(i++, sq);
        } else if (colour == G) {
            i += 8;
        } else {
            i++;
        }
    }

    if (side == BLACK) {
        board_hash ^= HASH_VALUES[SIDE_OFF];
    }

    if (ep_square) {
        board_hash ^= HASH_VALUES[EP_OFF + get_file(ep_square)];
    }

    for (int j = 0; j < 4; j++) {
        if (c_rights & (1 << j)) {
            board_hash ^= HASH_VALUES[C_OFF + j];
        }
    }
}

void update_hash(move_t mv) {
    int start = mv.start, dest = mv.dest;
    int piece = board[start];

    int new_c_rights = c_rights & (
        (start != A1)
        | ((start != H1) << 1)
        | ((dest != A8) << 2)
        | ((dest != H8) << 3)
    );

    if (side == BLACK) {
        start = flip_square(start);
        dest = flip_square(dest);
        piece = (piece & 0xFFC) | BLACK;
        new_c_rights = ((new_c_rights & 12) >> 2) | ((new_c_rights & 3) << 2);
    }

    board_hash ^= get_hash(start, piece);
    board_hash ^= get_hash(dest, piece);

    if (ep_square) {
        board_hash ^= HASH_VALUES[EP_OFF + get_file(ep_square)];
    }

    board_hash ^= HASH_VALUES[SIDE_OFF];

    if (mv.flags == K_CASTLE_FLAG || mv.flags == Q_CASTLE_FLAG) {
        int r_start = (mv.flags == K_CASTLE_FLAG) ? H1 : A1;
        int r_dest = (mv.flags == K_CASTLE_FLAG) ? F1 : D1;

        if (side == BLACK) {
            r_start = flip_square(r_start);
            r_dest = flip_square(r_dest);

            board_hash ^= HASH_VALUES[C_OFF + 2];
            board_hash ^= HASH_VALUES[C_OFF + 3];
        } else {
            board_hash ^= HASH_VALUES[C_OFF];
            board_hash ^= HASH_VALUES[C_OFF + 1];
        }
        
        board_hash ^= get_hash(r_start, side | ROOK);
        board_hash ^= get_hash(r_dest, side | ROOK);

        return;
    }

    if (mv.flags == DPP_FLAG) {
        board_hash ^= HASH_VALUES[EP_OFF + get_file(dest)];
        return;
    }

    if (mv.flags & PROMO_FLAG) {
        board_hash ^= get_hash(dest, piece);
        board_hash ^= get_hash(dest, side | PROMOTIONS[mv.flags & 3]);
    }

    if (mv.flags & CAPTURE_FLAG) {
        int cap_pos = dest, cap_piece = mv.captured_piece, off = S;

        if (side == BLACK) {
            cap_piece = (cap_piece & 0xFC) | WHITE;
            off = N;
        }
        
        if (mv.flags == EP_FLAG) {
            cap_pos += off;
        }

        board_hash ^= get_hash(cap_pos, cap_piece);
    }

    for (int i = 0; i < 4; i++) {
        if (c_rights & i) {
            board_hash ^= HASH_VALUES[C_OFF + i];
        }
        if (new_c_rights & i) {
            board_hash ^= HASH_VALUES[C_OFF + i];
        }
    }
}

void clear_pv_table(void) {
    TABLE_ENTRY *t_entry = pv_table->table;

    while (t_entry < pv_table->table + pv_table->n_entries) {
        t_entry->key = 0UL;
        t_entry->best_move = NULL_MOVE;
        t_entry++;
    }
}

void store_move(move_t mv) {
    int index = board_hash % pv_table->n_entries;

    pv_table->table[index].key = board_hash;
    pv_table->table[index].best_move = mv;
}

move_t get_pv_move(void) {
    int index = board_hash % pv_table->n_entries;

    if (pv_table->table[index].key == board_hash) {
        return pv_table->table[index].best_move;
    }

    return NULL_MOVE;
}

int get_pv_line(int depth) {
    move_t mv = get_pv_move();
    int count = 0;

    while (!is_null_move(mv) && count < depth) {
        if (move_exists(mv)) {
            pv_line[count++] = mv;
            make_move(mv);
        } else {
            break;
        }
        mv = get_pv_move();
    }

    for (int i = count - 1; i >= 0; i--) {
        unmake_move(pv_line[i]);
    }

    return count;
}