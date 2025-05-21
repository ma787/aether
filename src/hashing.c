#include "aether.h"

uint64_t get_hash(uint64_t pos, uint64_t piece) {
    return HASH_VALUES[(96 * RANK(pos)) + (12 * FILE(pos)) + PCINDEX(piece)];
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
        pstn->key ^= HASH_VALUES[EP_OFF + FILE(pstn->ep_sq)];
    }

    for (int j = 0; j < 4; j++) {
        if (pstn->c_rights & (1 << j)) {
            pstn->key ^= HASH_VALUES[C_OFF + j];
        }
    }
}

void update_hash(POSITION *pstn, move_t mv) {
    int piece = pstn->board[mv.dest];
    int ep_sq = pstn->history[pstn->ply - 1].ep_sq;
    int c_rights = pstn->history[pstn->ply - 1].c_rights;

    pstn->key ^= get_hash(mv.start, piece);
    pstn->key ^= get_hash(mv.dest, piece);

    if (ep_sq) {
        pstn->key ^= HASH_VALUES[EP_OFF + FILE(ep_sq)];
    }

    pstn->key ^= HASH_VALUES[SIDE_OFF];

    if (mv.flags == K_CASTLE_FLAG || mv.flags == Q_CASTLE_FLAG) {
        int r_start, r_dest;

        if (mv.flags == K_CASTLE_FLAG) {
            r_start = K_ROOK_MOVES[pstn->side][0];
            r_dest = K_ROOK_MOVES[pstn->side][1];
        } else {
            r_start = Q_ROOK_MOVES[pstn->side][0];
            r_dest = Q_ROOK_MOVES[pstn->side][1];
        }

        if (pstn->side == BLACK) {
            pstn->key ^= HASH_VALUES[C_OFF];
            pstn->key ^= HASH_VALUES[C_OFF + 1];
        } else {
            pstn->key ^= HASH_VALUES[C_OFF + 2];
            pstn->key ^= HASH_VALUES[C_OFF + 3];
        }
        
        pstn->key ^= get_hash(r_start, pstn->side | ROOK);
        pstn->key ^= get_hash(r_dest, pstn->side | ROOK);

        return;
    }

    if (mv.flags == DPP_FLAG) {
        pstn->key ^= HASH_VALUES[EP_OFF + FILE(mv.dest)];
        return;
    }

    if (mv.flags & PROMO_FLAG) {
        pstn->key ^= get_hash(mv.start, PAWN | pstn->side);
        pstn->key ^= get_hash(mv.start, piece);
    }

    if (mv.flags & CAPTURE_FLAG) {
        int cap_pos = mv.dest;
        if (mv.flags == EP_FLAG) {
            cap_pos += PAWN_STEP[OTHER(pstn->side)];
        }

        pstn->key ^= get_hash(cap_pos, mv.captured_piece);
    }

    for (int i = 0; i < 4; i++) {
        if (c_rights & i) {
            pstn->key ^= HASH_VALUES[C_OFF + i];
        }
        if (pstn->c_rights & i) {
            pstn->key ^= HASH_VALUES[C_OFF + i];
        }
    }
}

void clear_hash_table(POSITION *pstn) {
    TABLE_ENTRY *t_entry = (pstn->hash_table)->table;

    while (t_entry < (pstn->hash_table)->table + (pstn->hash_table)->n_entries) {
        t_entry->key = 0UL;
        t_entry->best_move = NULL_MOVE;
        t_entry->score = 0;
        t_entry->depth = 0;
        t_entry->n_type = 0;
        t_entry++;
    }
    
    (pstn->hash_table)->new_writes = 0;
}

void store_entry(POSITION *pstn, move_t mv, int score, int depth, int n_type) {
    int index = pstn->key % (pstn->hash_table)->n_entries;

    if ((pstn->hash_table)->table[index].key) { (pstn->hash_table)->over_writes++; }
    else { (pstn->hash_table)->new_writes++; }

    (pstn->hash_table)->table[index].key = pstn->key;
    (pstn->hash_table)->table[index].best_move = mv;
    (pstn->hash_table)->table[index].score = score;
    (pstn->hash_table)->table[index].depth = depth;
    (pstn->hash_table)->table[index].n_type = n_type;
}

bool get_entry_info(POSITION *pstn, move_t *mv, int *score, int alpha, int beta, int depth) {
    int index = pstn->key % (pstn->hash_table)->n_entries;
    TABLE_ENTRY entry = (pstn->hash_table)->table[index];

    if (entry.key == pstn->key) {
        *mv = entry.best_move;

        if (entry.depth >= depth) {
            (pstn->hash_table)->hit++;

            *score = entry.score;
            if ((pstn->hash_table)->table[index].score > MATE) { *score -= pstn->s_ply; } 
            else if ((pstn->hash_table)->table[index].score < -MATE) { *score += pstn->s_ply; }

            switch(entry.n_type) {
                case PV:
                    return true;
                case ALL:
                    if (entry.score <= alpha) {
                        *score = alpha;
                        return true;
                    }
                    break;
                case CUT:
                    if (entry.score >= beta) {
                        *score = beta;
                        return true;
                    }
            }
        }
    }

    return false;
}

move_t get_pv_move(POSITION *pstn) {
    int index = pstn->key % (pstn->hash_table)->n_entries;
    TABLE_ENTRY entry = (pstn->hash_table)->table[index];

    if (entry.key == pstn->key && entry.n_type == PV) {
        return entry.best_move;
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