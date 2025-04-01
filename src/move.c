#include <string.h>
#include "aether.h"

int encode_move(int start, int dest, int flags) {
    int captured_piece = 0;

    if (flags & CAPTURE_FLAG) {
        int cap_pos = (flags == EP_FLAG) ? dest + S : dest;
        captured_piece = board[cap_pos];
    }

    return (start | (dest << 8) | (flags << 16) | ((side | (captured_piece & 0xFFC)) << 20));
}

int get_start(int mv) { return mv & 0xFF; }
int get_dest(int mv) { return (mv >> 8) & 0xFF; }
int get_flags(int mv) { return (mv >> 16) & 0xF; }
int get_captured_piece(int mv) { return ((mv >> 20) & 0xFFC) | BLACK; }
int get_side(int mv) { return (mv >> 20) & 3; }

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

int make_move(int mv) {
    int legal = 0;
    int kp_square = 0;
    save_state();
    update_hash(mv);

    int start = get_start(mv), dest = get_dest(mv), flags = get_flags(mv);

    if (flags == EP_FLAG) { goto en_passant; }

    ep_square = 0;
    int piece = board[start];

    if (piece & PAWN || flags & CAPTURE_FLAG) {
        h_clk = 0;
    } else {
        h_clk++;
    }

    if (flags & CAPTURE_FLAG) {
        capture_piece(dest);
    }

    move_piece(start, dest);

    switch(flags) {
        case DPP_FLAG:
            ep_square = dest + S;
            break;
        case K_CASTLE_FLAG:
            kp_square = F1;
            move_piece(H1, F1);
            break;
        case Q_CASTLE_FLAG:
            kp_square = D1;
            move_piece(A1, D1);
    }

    if (flags & PROMO_FLAG) {
        unsigned int p_code = board[dest] & 0xF00;
        board[dest] = p_code | PROMOTIONS[flags & 3] | WHITE;
    }

    if (piece & KING) {
        if (is_square_attacked(dest)) {
            legal = -1; // king left in check
        } else if (kp_square && is_square_attacked(kp_square)) {
            legal = -1; // king castles through check
        } else {
            c_rights &= (((dest != A8) << 2) | ((dest != H8) << 3));
            update_check(start, dest);
        }

        // search for check from castling rook
        if (kp_square) {
            int r_dest = (flags == K_CASTLE_FLAG) ? F1 : D1;
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
            (start != A1)
            | ((start != H1) << 1)
            | ((dest != A8) << 2)
            | ((dest != H8) << 3)
        );
        update_check(start, dest);
    }

    switch_side();
    flip_position();
    return legal;

    en_passant:
        int k_pos = w_pieces[0];
        int b_pawn_pos = dest + S;
        int ep_exposed_checker = 0;

        // check for an undiscovered pin on king on ep rank
        if (get_rank(k_pos) == get_rank(start)) {
            int vec = get_step(k_pos, start);
            int current = k_pos;

            for (;;) {
                current += vec;
                if (current == start || current == b_pawn_pos) {
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
        if (step_to_pawn != get_step(b_king_pos, dest)) {
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

        move_piece(start, dest);
        capture_piece(b_pawn_pos);
        h_clk = 0;
        ep_square = 0;
        update_check(start, dest);

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

void unmake_move(int mv) {
    int start = get_start(mv), dest = get_dest(mv), flags = get_flags(mv);

    flip_position();
    switch_side();
    move_piece(dest, start);

    if (flags & PROMO_FLAG) {
        unsigned int p_code = board[start] & 0xF00;
        board[start] = p_code | PAWN | WHITE;
    }

    if (flags & CAPTURE_FLAG) {
        int cap_pos = dest;
        if (flags == EP_FLAG) {
            cap_pos += S;
        }

        int cap_piece = get_captured_piece(mv);
        board[cap_pos] = cap_piece;
        b_pieces[cap_piece >> 8] = cap_pos;
    }

    switch(flags) {
        case K_CASTLE_FLAG:
            move_piece(F1, H1);
            break;
        case Q_CASTLE_FLAG:
            move_piece(D1, A1);
            break;
    }

    restore_state();
}
