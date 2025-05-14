#include <string.h>
#include "aether.h"

move_t get_move(POSITION *pstn, int start, int dest, int flags) {
    int captured_piece = 0;

    if (flags & CAPTURE_FLAG) {
        int cap_pos = (flags == EP_FLAG) ? dest + PAWN_STEP[opp_side(pstn->side)] : dest;
        captured_piece = pstn->board[cap_pos];
    }

    move_t mv = {
        .start = start,
        .dest = dest,
        .flags = flags,
        .captured_piece = captured_piece,
        .score = 0
    };

    return mv;
}

bool moves_equal(move_t mv1, move_t mv2) {
    return (
        mv1.start == mv2.start
        && mv1.dest == mv2.dest
        && mv1.captured_piece == mv2.captured_piece
    );
}

bool is_null_move(move_t mv) {
    return mv.start == 0;
}

int move_to_int(move_t mv) {
    return (
        mv.start 
        | (mv.dest << 8) 
        | (mv.flags << 16) 
        | (mv.captured_piece << 20)
    );
}

move_t move_of_int(int m_int) {
    move_t mv = {
        .start = m_int & 0xFF,
        .dest = (m_int >> 8) & 0xFF,
        .flags = (m_int >> 16) & 0xF,
        .captured_piece = (m_int >> 20),
        .score = 0
    };
    return mv;
}

void move_piece(POSITION *pstn, int start, int dest) {
    int piece = pstn->board[start];
    pstn->board[start] = 0;
    pstn->board[dest] = piece;
    PLIST(pstn)[get_piece_list_index(piece)] = dest;
    int p_type = get_piece_type(piece);
    pstn->pcsq_sum[pstn->side] -= EVAL_TABLES[p_type][start];
    pstn->pcsq_sum[pstn->side] += EVAL_TABLES[p_type][dest];
}

void capture_piece(POSITION *pstn, move_t mv) {
    pstn->h_clk = 0;
    int enemy_side = opp_side(pstn->side);
    int cap_pos = ENEMY_PLIST(pstn)[get_piece_list_index(mv.captured_piece)];

    if (!(mv.captured_piece & PAWN)) {
        pstn->big_pieces[enemy_side]--;
    }

    pstn->board[cap_pos] = 0;
    ENEMY_PLIST(pstn)[get_piece_list_index(mv.captured_piece)] = 0;
    int p_type = get_piece_type(mv.captured_piece);
    pstn->material[enemy_side] -= PIECE_VALS[p_type];
    pstn->pcsq_sum[enemy_side] -= EVAL_TABLES[p_type][flip_square(cap_pos)];
}

void restore_piece(POSITION *pstn, move_t mv) {
    int cap_pos = mv.dest;
    int enemy_side = opp_side(pstn->side);
    if (mv.flags == EP_FLAG) {
        cap_pos += PAWN_STEP[enemy_side];
    }

    if (!(mv.captured_piece & PAWN)) {
        pstn->big_pieces[enemy_side]++;
    }

    pstn->board[cap_pos] = mv.captured_piece;
    ENEMY_PLIST(pstn)[get_piece_list_index(mv.captured_piece)] = cap_pos;
    int p_type = get_piece_type(mv.captured_piece);
    pstn->material[enemy_side] += PIECE_VALS[p_type];
    pstn->pcsq_sum[enemy_side] += EVAL_TABLES[p_type][flip_square(cap_pos)];
}

void promote_piece(POSITION *pstn, move_t mv, int piece) {
    int pr_type = PROMOTIONS[mv.flags & 3];
    pstn->board[mv.dest] = change_piece_type(piece, pr_type);
    pstn->material[pstn->side] += (PIECE_VALS[pr_type] - PIECE_VALS[PAWN]);
    pstn->pcsq_sum[pstn->side] += (EVAL_TABLES[pr_type][mv.dest] - EVAL_TABLES[PAWN][mv.dest]);
    pstn->big_pieces[pstn->side]++;
}

void demote_piece(POSITION *pstn, move_t mv, int piece) {
    int pr_type = PROMOTIONS[mv.flags & 3];
    pstn->board[mv.dest] = change_piece_type(piece, PAWN);
    pstn->material[WHITE] += (PIECE_VALS[PAWN] - PIECE_VALS[pr_type]);
    pstn->pcsq_sum[WHITE] += (EVAL_TABLES[PAWN][mv.dest] - EVAL_TABLES[pr_type][mv.dest]);
    pstn->big_pieces[pstn->side]--;
}

int is_square_attacked(POSITION *pstn, int pos, int side) {
    for (int i = 0; i < 16; i++) {
        int enemy_pos = SIDE_PLIST(pstn, side)[i];

        int piece = pstn->board[enemy_pos];
        int alignment = get_alignment(enemy_pos, pos);

        if (alignment & piece) { // contact check
            return enemy_pos;
        } else if ((alignment >> 8) & piece) { // distant check
            int step = get_step(enemy_pos, pos);
            int current = enemy_pos;

            for (;;) {
                current += step;
                if (current == pos) {
                    return enemy_pos;
                }
                if (pstn->board[current]) {
                    break;
                }
            }
        }
    }

    int pawn_off = PAWN_STEP[opp_side(side)];

    int e_pawn = pstn->board[pos + pawn_off + E];
    if ((e_pawn & side) && (e_pawn & PAWN)) {
        return pos + pawn_off + E;
    }

    int w_pawn = pstn->board[pos + pawn_off + W];
    if ((w_pawn & side) && (w_pawn & PAWN)) {
        return pos + pawn_off + W;
    }

    return 0;
}

void update_check(POSITION *pstn, move_t mv) {
    pstn->check = 0;
    pstn->fst_checker = 0;
    pstn->snd_checker = 0;

    int k_pos = ENEMY_PLIST(pstn)[0];
    int piece = pstn->board[mv.dest];

    // check if the moved piece checks the king
    if (piece & PAWN) {
        int pawn_off = PAWN_STEP[opp_side(pstn->side)];
        int diff = mv.dest - k_pos;
        if (diff == (pawn_off + E) || diff == (pawn_off + W)) {
            pstn->check = CONTACT_CHECK;
            pstn->fst_checker = mv.dest;
        }
    } else {
        int dest_alignment = get_alignment(mv.dest, k_pos);

        if (dest_alignment & piece) {
            pstn->check = CONTACT_CHECK;
            pstn->fst_checker = mv.dest;
        } else if ((dest_alignment >> 8) & piece) {
            int step = get_step(mv.dest, k_pos);
            int current = mv.dest + step;

            while (current != k_pos) {
                if (pstn->board[current]) {
                    break;
                }
                current += step;
            }
            if (current == k_pos) {
                pstn->check = DISTANT_CHECK;
                pstn->fst_checker = mv.dest;
            }
        }
    }

    // check if this move discovered a check
    int start_alignment = get_alignment(mv.start, k_pos);

    if ((start_alignment & QUEEN) || ((start_alignment >> 8) & QUEEN)) {
        int step = get_step(k_pos, mv.start);
        int current = k_pos;

        for (;;) {
            current += step;
            int sq = pstn->board[current];

            if (
                ((sq & COLOUR_MASK) == pstn->side) && 
                ((get_alignment(current, k_pos) >> 8) & sq)
            ) {
                add_checker(pstn, current, DISTANT_CHECK);
            }

            if (sq) {
                break;
            }
        }
    }
}

bool confirm_king_move(POSITION *pstn, move_t mv) {
    int kp_square = 0;

    if (mv.flags == K_CASTLE_FLAG) {
        kp_square = (pstn->side == WHITE) ? F1 : F8;
    } else if (mv.flags == Q_CASTLE_FLAG) {
        kp_square = (pstn->side == WHITE) ? D1 : D8;
    }

    if (is_square_attacked(pstn, mv.dest, opp_side(pstn->side))) {
        return false;
    }

    update_check(pstn, mv);

    if (kp_square) {
        if (is_square_attacked(pstn, kp_square, opp_side(pstn->side))) {
            return false;
        }

        // search for check from castling rook
        int r_dest = kp_square;
        int current = r_dest;

        for (;;) {
            current += PAWN_STEP[pstn->side];
            int sq = pstn->board[current];

            if (sq & KING) {
                pstn->check = DISTANT_CHECK;
                pstn->fst_checker = r_dest;
            } else if (sq) {
                break;
            }
        }
    }

    return true;
}

bool confirm_ep_move(POSITION *pstn, move_t mv) {
    // check if king is attacked
    if (is_square_attacked(pstn, PLIST(pstn)[0], opp_side(pstn->side))) {
        return false;
    }

    update_check(pstn, mv);

    // search for a check discovered by disappearance of captured pawn
    int e_king_pos = ENEMY_PLIST(pstn)[0];
    int e_pawn_pos = mv.dest + S;
    int step = get_step(e_king_pos, e_pawn_pos);

    int current = e_king_pos;
    bool passed_pawn = false;

    for (;;) {
        current += step;
        if (current == e_pawn_pos) {
            passed_pawn = true;
            continue;
        }

        int sq = pstn->board[current];

        if (
            (sq & COLOUR_MASK) == pstn->side
            && passed_pawn
            && ((get_alignment(current, e_king_pos) >> 8) & sq)
        ) {
            add_checker(pstn, current, DISTANT_CHECK);
            break;
        }

        if (sq) {
            break;
        }
    }

    return true;
}

bool confirm_legal(POSITION *pstn, move_t mv) {
    if ((pstn->board[mv.dest] & KING))  {
        return confirm_king_move(pstn, mv);
    } else if (mv.flags == EP_FLAG) {
        return confirm_ep_move(pstn, mv);
    }

    update_check(pstn, mv);
    return true;
}

void make_pseudo_legal_move(POSITION *pstn, move_t mv) {
    save_state(pstn);
    pstn->rep_table[pstn->key & 0x00003FFF] += 1;
    pstn->ply++;
    pstn->s_ply++;
    pstn->move_history[pstn->ply] = mv;
    pstn->ep_sq = 0;

    int piece = pstn->board[mv.start];
    piece & PAWN ? pstn->h_clk = 0 : pstn->h_clk++;
    pstn->c_rights &= (CASTLE_UPDATES[mv.start] & CASTLE_UPDATES[mv.dest]);

    if (mv.flags & CAPTURE_FLAG) {
        capture_piece(pstn, mv);
    }

    move_piece(pstn, mv.start, mv.dest);

    if (mv.flags == DPP_FLAG) {
        pstn->ep_sq = mv.dest - PAWN_STEP[pstn->side];
    } else if (mv.flags == K_CASTLE_FLAG) {
        move_piece(pstn, K_ROOK_MOVES[pstn->side][0], K_ROOK_MOVES[pstn->side][1]);
    } else if (mv.flags == Q_CASTLE_FLAG) {
        move_piece(pstn, Q_ROOK_MOVES[pstn->side][0], Q_ROOK_MOVES[pstn->side][1]);
    } else if (mv.flags & PROMO_FLAG) {
        promote_piece(pstn, mv, piece);
    }
}

void unmake_pseudo_legal_move(POSITION *pstn, move_t mv) {
    if (mv.flags & PROMO_FLAG) {
        demote_piece(pstn, mv, pstn->board[mv.dest]);
    }

    move_piece(pstn, mv.dest, mv.start);

    if (mv.flags & CAPTURE_FLAG) {
        restore_piece(pstn, mv);
    } else if (mv.flags == K_CASTLE_FLAG) {
        move_piece(pstn, K_ROOK_MOVES[pstn->side][1], K_ROOK_MOVES[pstn->side][0]);
    } else if (mv.flags == Q_CASTLE_FLAG) {
        move_piece(pstn, Q_ROOK_MOVES[pstn->side][1], Q_ROOK_MOVES[pstn->side][0]);
    }

    pstn->ply--;
    pstn->s_ply--;
    restore_state(pstn);
    pstn->rep_table[pstn->key & 0x00003FFF] -= 1;
}

bool make_move(POSITION *pstn, move_t mv) {
    make_pseudo_legal_move(pstn, mv);

    if (confirm_legal(pstn, mv)) {
        update_hash(pstn, mv);
        switch_side(pstn);
        return true;
    } else {
        unmake_pseudo_legal_move(pstn, mv);
        return false;
    }
}

void unmake_move(POSITION *pstn, move_t mv) {
    switch_side(pstn);
    unmake_pseudo_legal_move(pstn, mv);
}

void make_null_move(POSITION *pstn) {
    save_state(pstn);
    pstn->rep_table[pstn->key & 0x00003FFF] += 1;
    pstn->ply++;
    pstn->s_ply++;
    pstn->h_clk++;
    pstn->move_history[pstn->ply] = NULL_MOVE;

    if (pstn->ep_sq) {
        pstn->key ^= HASH_VALUES[EP_OFF + get_file(pstn->ep_sq)];
        pstn->ep_sq = 0;
    }
    
    pstn->key ^= HASH_VALUES[SIDE_OFF];
    switch_side(pstn);
}

void unmake_null_move(POSITION *pstn) {
    switch_side(pstn);
    pstn->ply--;
    pstn->s_ply--;
    restore_state(pstn);
    pstn->rep_table[pstn->key & 0x00003FFF] -= 1;
}