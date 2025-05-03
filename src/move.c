#include <string.h>
#include "aether.h"

move_t get_move(POSITION *pstn, int start, int dest, int flags) {
    int captured_piece = 0;

    if (flags & CAPTURE_FLAG) {
        int cap_pos = (flags == EP_FLAG) ? dest + S : dest;
        captured_piece = pstn->board[cap_pos];
    }

    move_t mv = {
        .start = start,
        .dest = dest,
        .flags = flags,
        .captured_piece = captured_piece,
        .side = pstn->side,
        .score = 0
    };

    return mv;
}

bool moves_equal(move_t mv1, move_t mv2) {
    return (
        mv1.start == mv2.start
        && mv1.dest == mv2.dest
        && mv1.captured_piece == mv2.captured_piece
        && mv1.side == mv2.side
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
        | ((change_piece_colour(mv.captured_piece, mv.side)) << 20)
    );
}

move_t move_of_int(int m_int) {
    move_t mv = {
        .start = m_int & 0xFF,
        .dest = (m_int >> 8) & 0xFF,
        .flags = (m_int >> 16) & 0xF,
        .captured_piece = change_piece_colour(m_int >> 20, BLACK),
        .side = (m_int >> 20) & 3,
        .score = 0
    };
    return mv;
}

void move_piece(POSITION *pstn, int start, int dest) {
    unsigned int piece = pstn->board[start];
    pstn->board[start] = 0;
    pstn->board[dest] = piece;
    pstn->w_pieces[get_piece_list_index(piece)] = dest;
}

void capture_piece(POSITION *pstn, unsigned int pos) {
    unsigned int piece = pstn->board[pos];
    pstn->board[pos] = 0;
    pstn->b_pieces[get_piece_list_index(piece)] = 0;
}

int is_square_attacked(POSITION *pstn, int pos) {
    for (int i = 0; i < 16; i++) {
        int b_pos = pstn->b_pieces[i];
        int piece = pstn->board[b_pos];
        int alignment = get_alignment(b_pos, pos);

        if (alignment & piece) { // contact check
            return b_pos;
        } else if ((alignment >> 8) & piece) { // distant check
            int step = get_step(b_pos, pos);
            int current = b_pos;

            for (;;) {
                current += step;
                if (current == pos) {
                    return b_pos;
                }
                if (pstn->board[current]) {
                    break;
                }
            }
        }
    }

    int ne_piece = pstn->board[pos + N + E];
    if ((ne_piece & BLACK) && (ne_piece & PAWN)) {
        return pos + N + E;
    }

    int nw_piece = pstn->board[pos + N + W];
    if ((nw_piece & BLACK) && (nw_piece & PAWN)) {
        return pos + N + W;
    }

    return 0;
}

void update_check(POSITION *pstn, move_t mv) {
    pstn->check = 0;
    pstn->fst_checker = 0;
    pstn->snd_checker = 0;

    int k_pos = pstn->b_pieces[0];
    int piece = pstn->board[mv.dest];

    // check if the moved piece checks the king
    if (piece & PAWN) {
        int diff = mv.dest - k_pos;
        if (diff == (S + E) || diff == (S + W)) {
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
                ((sq & COLOUR_MASK) == WHITE) && 
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
        kp_square = F1;
    } else if (mv.flags == Q_CASTLE_FLAG) {
        kp_square = D1;
    }

    if (is_square_attacked(pstn, mv.dest)) {
        return false;
    }

    update_check(pstn, mv);

    if (kp_square) {
        if (is_square_attacked(pstn, kp_square)) {
            return false;
        }

        // search for check from castling rook
        int r_dest = kp_square;
        int current = r_dest;

        for (;;) {
            current += N;
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
    if (is_square_attacked(pstn, pstn->w_pieces[0])) {
        return false;
    }

    update_check(pstn, mv);

    // search for a check discovered by disappearance of captured pawn
    int b_king_pos = pstn->b_pieces[0];
    int b_pawn_pos = mv.dest + S;
    int step = get_step(b_king_pos, b_pawn_pos);

    int current = b_king_pos;
    bool passed_pawn = false;

    for (;;) {
        current += step;
        if (current == b_pawn_pos) {
            passed_pawn = true;
            continue;
        }

        int sq = pstn->board[current];

        if (
            (sq & COLOUR_MASK) == WHITE
            && passed_pawn
            && ((get_alignment(current, b_king_pos) >> 8) & sq)
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
    pstn->move_history[pstn->ply] = mv;
    pstn->ep_sq = 0;

    int mtrl_delta = 0, pcsq_delta = 0;
    int piece = pstn->board[mv.start];

    piece & PAWN ? pstn->h_clk = 0 : pstn->h_clk++;

    if (piece & KING) {
        pstn->c_rights &= (((mv.dest != A8) << 2) | ((mv.dest != H8) << 3));
    } else {
        pstn->c_rights &= (
            (mv.start != A1)
            | ((mv.start != H1) << 1)
            | ((mv.dest != A8) << 2)
            | ((mv.dest != H8) << 3)
        );
    }

    if (mv.flags & CAPTURE_FLAG) {
        pstn->h_clk = 0;
        int cap_pos = mv.dest;

        if (mv.flags == EP_FLAG) {
            cap_pos += S;
        }
        capture_piece(pstn, cap_pos);

        int cap_type = get_piece_type(mv.captured_piece);
        mtrl_delta += PIECE_VALS[cap_type];
        pcsq_delta += EVAL_TABLES[cap_type][cap_pos];
    }

    move_piece(pstn, mv.start, mv.dest);

    int p_type = get_piece_type(piece);
    pcsq_delta += (EVAL_TABLES[p_type][mv.dest] - EVAL_TABLES[p_type][mv.start]);

    if (mv.flags == DPP_FLAG) {
        pstn->ep_sq = mv.dest + S;
    } else if (mv.flags == K_CASTLE_FLAG) {
        move_piece(pstn, H1, F1);
        pcsq_delta -= EVAL_TABLES[ROOK][pstn->side == WHITE ? F1 : F8];
        pcsq_delta += EVAL_TABLES[ROOK][pstn->side == WHITE ? H1 : H8];
    } else if (mv.flags == Q_CASTLE_FLAG) {
        move_piece(pstn, A1, D1);
        pcsq_delta -= EVAL_TABLES[ROOK][pstn->side == WHITE ? A1 : A8];
        pcsq_delta += EVAL_TABLES[ROOK][pstn->side == WHITE ? A1 : A8];
    } else if (mv.flags & PROMO_FLAG) {
        int pr_type = PROMOTIONS[mv.flags & 3];
        pstn->board[mv.dest] = change_piece_type(piece, pr_type);

        mtrl_delta += (PIECE_VALS[pr_type] - PIECE_VALS[PAWN]);
        pcsq_delta += (EVAL_TABLES[pr_type][mv.dest] - EVAL_TABLES[PAWN][mv.dest]);
    }

    if (pstn->side == WHITE) {
        pstn->material += mtrl_delta;
        pstn->pcsq_sum += pcsq_delta;
    } else {
        pstn->material -= mtrl_delta;
        pstn->pcsq_sum -= mtrl_delta;
    }
}

void unmake_pseudo_legal_move(POSITION *pstn, move_t mv) {
    move_piece(pstn, mv.dest, mv.start);

    if (mv.flags & PROMO_FLAG) {
        pstn->board[mv.start] = change_piece_type(pstn->board[mv.start], PAWN);
    }

    if (mv.flags & CAPTURE_FLAG) {
        int cap_pos = mv.dest;
        if (mv.flags == EP_FLAG) {
            cap_pos += S;
        }
        pstn->board[cap_pos] = mv.captured_piece;
        pstn->b_pieces[get_piece_list_index(mv.captured_piece)] = cap_pos;
    } else if (mv.flags == K_CASTLE_FLAG) {
        move_piece(pstn, F1, H1);
    } else if (mv.flags == Q_CASTLE_FLAG) {
        move_piece(pstn, D1, A1);
    }

    pstn->ply--;
    restore_state(pstn);
}

bool make_move(POSITION *pstn, move_t mv) {
    make_pseudo_legal_move(pstn, mv);

    if (confirm_legal(pstn, mv)) {
        update_hash(pstn, mv);
        switch_side(pstn);
        flip_position(pstn);
        return true;
    } else {
        unmake_pseudo_legal_move(pstn, mv);
        return false;
    }
}

void unmake_move(POSITION *pstn, move_t mv) {
    flip_position(pstn);
    switch_side(pstn);
    unmake_pseudo_legal_move(pstn, mv);
}