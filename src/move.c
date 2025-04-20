#include <string.h>
#include "aether.h"

#include <stdio.h>

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
        | ((mv.side | (mv.captured_piece & 0xFFC)) << 20)
    );
}

move_t move_of_int(int m_int) {
    move_t mv = {
        .start = m_int & 0xFF,
        .dest = (m_int >> 8) & 0xFF,
        .flags = (m_int >> 16) & 0xF,
        .captured_piece = ((m_int >> 20) & 0xFFC) | BLACK,
        .side = (m_int >> 20) & 3,
        .score = 0
    };
    return mv;
}

void move_piece(POSITION *pstn, int start, int dest) {
    unsigned int piece = pstn->board[start];
    pstn->board[start] = 0;
    pstn->board[dest] = piece;
    pstn->w_pieces[piece >> 8] = dest;
}

void capture_piece(POSITION *pstn, unsigned int pos) {
    unsigned int piece = pstn->board[pos];
    pstn->board[pos] = 0;
    pstn->b_pieces[piece >> 8] = 0;
}

int is_square_attacked(POSITION *pstn, int pos) {
    for (int i = 0; i < 16; i++) {
        int vec = SUPERPIECE[i];
        int current = pos + vec;
        int contact = 1;
        int sq;

        for (;;) {
            sq = pstn->board[current];

            if ((sq & COLOUR_MASK) == BLACK) {
                switch(is_attacking(sq, current, pos)) {
                    case CONTACT_CHECK:
                        if (contact) { return 1; }
                        break;
                    case DISTANT_CHECK:
                        return 1;
                }
                break;
            } else if (sq) {
                break;
            }

            current += vec;
            contact = 0;
        }
    }

    return 0;
}

void find_direct_check(POSITION *pstn, move_t mv, int step_to_king) {
    int k_pos = pstn->b_pieces[0];

    // check if the moved piece checks the king
    switch(is_attacking(pstn->board[mv.dest], mv.dest, k_pos)) {
        case CONTACT_CHECK:
            pstn->check = CONTACT_CHECK;
            pstn->fst_checker = mv.dest;
            break;
        case DISTANT_CHECK:
            int current = mv.dest + step_to_king;
            while (current != k_pos) {
                if (pstn->board[current]) {
                    break;
                }
                current += step_to_king;
            }
            if (current == k_pos) {
                pstn->check = DISTANT_CHECK;
                pstn->fst_checker = mv.dest;
            }
            break;
    }
}

void find_discovered_check(POSITION *pstn, move_t mv, int step_to_king) {
    int k_pos = pstn->b_pieces[0];

    if (is_attacking(QUEEN, mv.start, k_pos)) {
        int discovered_vec = get_step(k_pos, mv.start);
        if (discovered_vec == -step_to_king) {
            return; // black piece moved along its pin ray
        }

        int current = k_pos;
        int sq;

        for (;;) {
            current += discovered_vec;
            sq = pstn->board[current];
            
            if (
                (sq & COLOUR_MASK) == WHITE 
                && is_attacking(sq, current, k_pos) == DISTANT_CHECK
            ) {
                if (pstn->check) {
                    pstn->check = DOUBLE_CHECK;
                    pstn->snd_checker = current;
                } else {
                    pstn->check = DISTANT_CHECK;
                    pstn->fst_checker = current;
                }
                break;
            } else if (sq) {
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
    int k_pos = pstn->w_pieces[0];
    int b_pawn_pos = mv.dest + S;

    // check for an undiscovered pin on king on ep rank
    if (get_rank(k_pos) == get_rank(mv.start)) {
        int vec = get_step(k_pos, mv.start);
        int current = k_pos;

        for (;;) {
            current += vec;
            if (current == mv.start || current == b_pawn_pos) {
                continue;
            }

            int sq = pstn->board[current];

            if (
                (sq & COLOUR_MASK) == BLACK
                && is_attacking(sq, current, k_pos)
            ) {
                return false;
            } else if (sq) {
                break;
            }
        }
    }

    int b_king_pos = pstn->b_pieces[0];
    int step_to_pawn = get_step(b_king_pos, b_pawn_pos);
    int current = b_king_pos;
    int passed_pawn = 0;

    // search for a check discovered by this capture
    if (step_to_pawn != get_step(b_king_pos, mv.dest)) {
        for (;;) {
            current += step_to_pawn;
            if (current == b_pawn_pos) {
                passed_pawn = 1;
                continue;
            }

            int sq = pstn->board[current];

            if (
                (sq & COLOUR_MASK) == WHITE
                && passed_pawn
                && is_attacking(sq, current, b_king_pos)
            ) {
                if (pstn->check) {
                    pstn->check = DOUBLE_CHECK;
                    pstn->snd_checker = current;
                } else {
                    pstn->check = DISTANT_CHECK;
                    pstn->fst_checker = current;
                }
                break;
            }

            if (sq) {
                break;
            }
        }
    }

    int step_to_king = get_step(mv.dest, k_pos);

    if (pstn->check) {
        find_discovered_check(pstn, mv, step_to_king);
    }
    find_direct_check(pstn, mv, step_to_king);
    return true;
}

bool confirm_legal(POSITION *pstn, move_t mv) {
    int piece = pstn->board[mv.dest];

    if (piece & KING) {
        if (!confirm_king_move(pstn, mv)) {
            return false;
        }
        find_discovered_check(pstn, mv, get_step(mv.dest, pstn->w_pieces[0]));
        return true;
    } else if (mv.flags == EP_FLAG) {
        return confirm_ep_move(pstn, mv);
    }

    int step_to_king = get_step(mv.dest, pstn->b_pieces[0]);
    find_direct_check(pstn, mv, step_to_king);
    find_discovered_check(pstn, mv, step_to_king);
    return true;
}

void make_pseudo_legal_move(POSITION *pstn, move_t mv) {
    save_state(pstn);
    pstn->rep_table[pstn->key & 0x00003FFF] += 1;
    update_hash(pstn, mv);
    pstn->ply++;
    pstn->move_history[pstn->ply] = mv;

    pstn->ep_sq = 0;
    pstn->check = 0;
    pstn->fst_checker = 0;
    pstn->snd_checker = 0;

    int piece = pstn->board[mv.start];

    if (piece & PAWN || (mv.flags & CAPTURE_FLAG)) {
        pstn->h_clk = 0;
    } else {
        pstn->h_clk++;
    }

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
        if (mv.flags == EP_FLAG) {
            capture_piece(pstn, mv.dest + S);
        } else {
            capture_piece(pstn, mv.dest);
        }
    }

    move_piece(pstn, mv.start, mv.dest);

    if (mv.flags == DPP_FLAG) {
        pstn->ep_sq = mv.dest + S;
    } else if (mv.flags == K_CASTLE_FLAG) {
        move_piece(pstn, H1, F1);
    } else if (mv.flags == Q_CASTLE_FLAG) {
        move_piece(pstn, A1, D1);
    } else if (mv.flags & PROMO_FLAG) {
        pstn->board[mv.dest] = (
            (pstn->board[mv.dest] & 0xF00) | PROMOTIONS[mv.flags & 3] | WHITE
        );
    }
}

void unmake_pseudo_legal_move(POSITION *pstn, move_t mv) {
    move_piece(pstn, mv.dest, mv.start);

    if (mv.flags & PROMO_FLAG) {
        pstn->board[mv.start] = (pstn->board[mv.start] & 0xF00) | PAWN | WHITE;
    }

    if (mv.flags & CAPTURE_FLAG) {
        int cap_pos = mv.dest;
        if (mv.flags == EP_FLAG) {
            cap_pos += S;
        }

        pstn->board[cap_pos] = mv.captured_piece;
        pstn->b_pieces[mv.captured_piece >> 8] = cap_pos;
    }

    switch(mv.flags) {
        case K_CASTLE_FLAG:
            move_piece(pstn, F1, H1);
            break;
        case Q_CASTLE_FLAG:
            move_piece(pstn, D1, A1);
            break;
    }

    pstn->ply--;
    restore_state(pstn);
    pstn->rep_table[pstn->key & 0x00003FFF] -= 1;
}

bool make_move(POSITION *pstn, move_t mv) {
    make_pseudo_legal_move(pstn, mv);

    if (confirm_legal(pstn, mv)) {
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