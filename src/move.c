#include <string.h>
#include "aether.h"

move_t get_move(int start, int dest, int flags) {
    int captured_piece = 0;

    if (flags & CAPTURE_FLAG) {
        int cap_pos = (flags == EP_FLAG) ? dest + S : dest;
        captured_piece = board[cap_pos];
    }

    move_t mv = {
        .start = start,
        .dest = dest,
        .flags = flags,
        .captured_piece = captured_piece,
        .side = side,
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
        | ((side | (mv.captured_piece & 0xFFC)) << 20)
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

void move_piece(unsigned int start, unsigned int dest) {
    unsigned int piece = board[start];
    board[start] = 0;
    board[dest] = piece;
    w_pieces[piece >> 8] = dest;
}

void capture_piece(unsigned int pos) {
    unsigned int piece = board[pos];
    board[pos] = 0;
    b_pieces[piece >> 8] = 0;
}

int is_square_attacked(int pos) {
    for (int i = 0; i < 16; i++) {
        int vec = SUPERPIECE[i];
        int current = pos + vec;
        int contact = 1;
        int sq;

        for (;;) {
            sq = board[current];

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

void update_check(int start, int dest) {
    check_info = 0;
    int k_pos = b_pieces[0];
    int step_to_king = get_step(dest, k_pos);

    // check if the moved piece checks the king
    switch(is_attacking(board[dest], dest, k_pos)) {
        case CONTACT_CHECK:
            check_info = (CONTACT_CHECK | (dest << 2));
            break;
        case DISTANT_CHECK:
            int current = dest + step_to_king;
            while (current != k_pos) {
                if (board[current]) {
                    break;
                }
                current += step_to_king;
            }
            if (current == k_pos) {
                check_info = (DISTANT_CHECK | (dest << 2));
            }
            break;
    }

    // search for a discovered check
    if (is_attacking(QUEEN, start, k_pos)) {
        int discovered_vec = get_step(k_pos, start);
        if (discovered_vec == -step_to_king) {
            return; // this ray has already been checked
        }

        int current = k_pos;
        int sq;

        for (;;) {
            current += discovered_vec;
            sq = board[current];
            
            if (
                (sq & COLOUR_MASK) == WHITE 
                && is_attacking(sq, current, k_pos) == DISTANT_CHECK
            ) {
                if (check_info) {
                    check_info |= (DOUBLE_CHECK | (current << 10));
                } else {
                    check_info |= (DISTANT_CHECK | (current << 2));
                }
                break;
            } else if (sq) {
                break;
            }
        }
    }
}

int make_move(move_t mv) {
    int legal = 0;
    int kp_square = 0;
    save_state();
    ply++;
    move_history[ply] = mv;
    repetition_table[board_hash & 0x00003FFF] += 1;
    update_hash(mv);

    if (mv.flags == EP_FLAG) { goto en_passant; }

    ep_square = 0;
    int piece = board[mv.start];

    if (piece & PAWN || mv.flags & CAPTURE_FLAG) {
        h_clk = 0;
    } else {
        h_clk++;
    }

    if (mv.flags & CAPTURE_FLAG) {
        capture_piece(mv.dest);
    }

    move_piece(mv.start, mv.dest);

    switch(mv.flags) {
        case DPP_FLAG:
            ep_square = mv.dest + S;
            break;
        case K_CASTLE_FLAG:
            kp_square = F1;
            move_piece(H1, F1);
            break;
        case Q_CASTLE_FLAG:
            kp_square = D1;
            move_piece(A1, D1);
    }

    if (mv.flags & PROMO_FLAG) {
        unsigned int p_code = board[mv.dest] & 0xF00;
        board[mv.dest] = p_code | PROMOTIONS[mv.flags & 3] | WHITE;
    }

    if (piece & KING) {
        if (is_square_attacked(mv.dest)) {
            legal = -1; // king left in check
        } else if (kp_square && is_square_attacked(kp_square)) {
            legal = -1; // king castles through check
        } else {
            c_rights &= (((mv.dest != A8) << 2) | ((mv.dest != H8) << 3));
            update_check(mv.start, mv.dest);
        }

        // search for check from castling rook
        if (kp_square) {
            int r_dest = (mv.flags == K_CASTLE_FLAG) ? F1 : D1;
            int current = r_dest;

            for (;;) {
                current += N;
                int sq = board[current];

                if (sq & KING) {
                    check_info = (DISTANT_CHECK | (r_dest << 2));
                } else if (sq) {
                    break;
                }
            }
        }

    } else {
        c_rights &= (
            (mv.start != A1)
            | ((mv.start != H1) << 1)
            | ((mv.dest != A8) << 2)
            | ((mv.dest != H8) << 3)
        );
        update_check(mv.start, mv.dest);
    }

    switch_side();
    flip_position();
    return legal;

    en_passant:
        int k_pos = w_pieces[0];
        int b_pawn_pos = mv.dest + S;
        int ep_exposed_checker = 0;

        // check for an undiscovered pin on king on ep rank
        if (get_rank(k_pos) == get_rank(mv.start)) {
            int vec = get_step(k_pos, mv.start);
            int current = k_pos;

            for (;;) {
                current += vec;
                if (current == mv.start || current == b_pawn_pos) {
                    continue;
                }

                int sq = board[current];

                if (
                    (sq & COLOUR_MASK) == BLACK
                    && is_attacking(sq, current, k_pos)
                ) {
                    legal = -1;
                    break;
                } else if (sq) {
                    break;
                }
            }
        }

        int b_king_pos = b_pieces[0];
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

                int sq = board[current];

                if (
                    (sq & COLOUR_MASK) == WHITE
                    && passed_pawn
                    && is_attacking(sq, current, b_king_pos)
                ) {
                    ep_exposed_checker = current;
                }

                if (sq) {
                    break;
                }
            }
        }

        move_piece(mv.start, mv.dest);
        capture_piece(b_pawn_pos);
        h_clk = 0;
        ep_square = 0;
        update_check(mv.start, mv.dest);

        if (ep_exposed_checker) {
            if (check_info) {
                check_info |= (DOUBLE_CHECK | (ep_exposed_checker << 10));
            } else {
                check_info = (DISTANT_CHECK | (ep_exposed_checker << 2));
            }
        }

        switch_side();
        flip_position();
        return legal;
}

void unmake_move(move_t mv) {
    flip_position();
    switch_side();
    move_piece(mv.dest, mv.start);

    if (mv.flags & PROMO_FLAG) {
        unsigned int p_code = board[mv.start] & 0xF00;
        board[mv.start] = p_code | PAWN | WHITE;
    }

    if (mv.flags & CAPTURE_FLAG) {
        int cap_pos = mv.dest;
        if (mv.flags == EP_FLAG) {
            cap_pos += S;
        }

        board[cap_pos] = mv.captured_piece;
        b_pieces[mv.captured_piece >> 8] = cap_pos;
    }

    switch(mv.flags) {
        case K_CASTLE_FLAG:
            move_piece(F1, H1);
            break;
        case Q_CASTLE_FLAG:
            move_piece(D1, A1);
            break;
    }

    ply--;
    restore_state();
    repetition_table[board_hash & 0x00003FFF] -= 1;
}
