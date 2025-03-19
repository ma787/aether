#include <string.h>
#include "move.h"
#include "constants.h"
#include "position.h"
#include "utils.h"

move_t encode_move(int start, int dest, int flags) {
    move_t mv = {start, dest, flags, 0};

    if (flags & CAPTURE_FLAG) {
        int cap_pos = (flags == EP_FLAG) ? dest + S : dest;
        mv.captured_piece = board[cap_pos];
    }

    return mv;
}

move_t of_string(char *mstr) {
    int start = string_to_coord(mstr);
    int dest = string_to_coord(mstr + 2);
    int flags = Q_FLAG;

    if (side == BLACK) {
        start = flip_square(start);
        dest = flip_square(dest);
    }

    if (board[dest]) {
        flags |= CAPTURE_FLAG;
    };

    switch(board[start] & 0x0FC) {
        case KING:
            if (start == E1) {
                if (dest == C1) {
                    flags |= Q_CASTLE_FLAG;
                } else if (dest == G1) {
                    flags |= K_CASTLE_FLAG;
                }
            }
            break;
        case PAWN:
            if (get_rank(start) == 1 && get_rank(dest) == 3) {
                flags = DPP_FLAG;
            } else if (dest == ep_square) {
                flags = EP_FLAG;
            } else if (get_rank(dest) == 7) {
                flags |= PROMO_FLAG;
                switch(PIECES[(int) mstr[4]] & 0xFC) {
                    case BISHOP:
                        flags |= 1;
                        break;
                    case ROOK:
                        flags |= 2;
                        break;
                    case QUEEN:
                        flags |= 3;
                        break;
                }
            }
            break;
    }

    return encode_move(start, dest, flags);
}

void move_to_string(move_t mv, char* mstr) {
    int start = mv.start;
    int dest = mv.dest;

    if (side == BLACK) {
        start = flip_square(start);
        dest = flip_square(dest);
    }

    strcpy(mstr, coord_to_string(start));
    strcpy(mstr + 2, coord_to_string(dest));
    if (mv.flags & PROMO_FLAG) {
        mstr[4] = SYMBOLS[PROMOTIONS[mv.flags & 3] | BLACK];
        mstr[5] = '\0';
    }
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

void update_check(int piece, move_t mv) {
    check_info = 0;
    int k_pos = b_pieces[0];
    int step_to_king = get_step(mv.dest, k_pos);

    // check if the moved piece checks the king
    switch(is_attacking(piece, mv.dest, k_pos)) {
        case CONTACT_CHECK:
            check_info = (CONTACT_CHECK | (mv.dest << 2));
            break;
        case DISTANT_CHECK:
            int current = mv.dest + step_to_king;
            while (current != k_pos) {
                if (board[current]) {
                    break;
                }
                current += step_to_king;
            }
            if (current == k_pos) {
                check_info = (DISTANT_CHECK | (mv.dest << 2));
            }
            break;
    }

    // search for a discovered check
    if (is_attacking(QUEEN, mv.start, k_pos)) {
        int discovered_vec = get_step(k_pos, mv.start);
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
            kp_square = D1;
            move_piece(H1, F1);
            break;
        case Q_CASTLE_FLAG:
            kp_square = F1;
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
            c_rights &= 12;
            update_check(piece, mv);
        }

        // search for check from castling rook
        if (kp_square) {
            int r_dest = K_CASTLE_FLAG ? F1 : D1;
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
        update_check(piece, mv);
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
        update_check(WHITE | PAWN, mv);

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

    restore_state();
}
