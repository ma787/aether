#include "constants.h"
#include "move.h"
#include "utils.h"

move_t of_string(info *pstn, char *mstr) {
    move_t mv = {
        .start = string_to_coord(mstr),
        .dest = string_to_coord(mstr + 2),
        .flags = 0,
        .captured_piece = 0
    };

    if (pstn->side == BLACK) {
        mv.start = flip_square(mv.start);
        mv.dest = flip_square(mv.dest);
    }

    if (pstn->arr[mv.dest]) {
        mv.flags |= CAPTURE_FLAG;
        mv.captured_piece = pstn->arr[mv.dest];
    };

    switch(pstn->arr[mv.start] & 0x0FC) {
        case KING:
            if (mv.start == E1) {
                if (mv.dest == C1) {
                    mv.flags |= Q_CASTLE_FLAG;
                } else if (mv.dest == G1) {
                    mv.flags |= K_CASTLE_FLAG;
                }
            }
            break;
        case PAWN:
            if (get_rank(mv.start) == 1 && get_rank(mv.dest) == 3) {
                mv.flags = DPP_FLAG;
            } else if (mv.dest == pstn->ep_square) {
                mv.flags = EP_FLAG;
                mv.captured_piece = pstn->arr[mv.dest + S];
            } else if (get_rank(mv.dest) == 7) {
                mv.flags |= PROMO_FLAG;
                switch(PIECES[(int) mstr[4]] & 0xFC) {
                    case BISHOP:
                        mv.flags |= 1;
                        break;
                    case ROOK:
                        mv.flags |= 2;
                        break;
                    case QUEEN:
                        mv.flags |= 3;
                        break;
                }
            }
            break;
    }

    return mv;
}

void move_piece(info *pstn, unsigned int start, unsigned int dest) {
    unsigned int piece = pstn->arr[start];
    pstn->arr[start] = 0;
    pstn->arr[dest] = piece;
    pstn->w_pieces[piece >> 8] = dest;
}

void capture_piece(info *pstn, unsigned int pos) {
    unsigned int piece = pstn->arr[pos];
    pstn->arr[pos] = 0;
    pstn->b_pieces[piece >> 8] = 0;
}

void update_check(info *pstn, int piece, move_t mv) {
    pstn->check_info = 0;
    int k_pos = pstn->b_pieces[0];
    int step_to_king = UNIT_VEC[square_diff(mv.dest, k_pos)];

    // check if the moved piece checks the king
    switch(is_attacking(piece, mv.dest, k_pos)) {
        case CONTACT_CHECK:
            pstn->check_info = (CONTACT_CHECK | (mv.dest << 2));
            break;
        case DISTANT_CHECK:
            int current = mv.dest + step_to_king;
            while (current != k_pos) {
                if (pstn->arr[current]) {
                    break;
                }
                current += step_to_king;
            }
            if (current == k_pos) {
                pstn->check_info = (DISTANT_CHECK | (mv.dest << 2));
            }
            break;
    }

    // search for a discovered check
    if (is_attacking(QUEEN, mv.start, k_pos)) {
        int discovered_vec = UNIT_VEC[square_diff(k_pos, mv.start)];
        if (discovered_vec == -step_to_king) {
            return; // this ray has already been checked
        }

        int current = k_pos;
        int sq;

        for (;;) {
            current += discovered_vec;
            sq = pstn->arr[current];
            
            if (
                (sq & COLOUR_MASK) == WHITE 
                && is_attacking(sq, current, k_pos) == DISTANT_CHECK
            ) {
                if (pstn->check_info) {
                    pstn->check_info |= (DOUBLE_CHECK | (current << 10));
                } else {
                    pstn->check_info |= (DISTANT_CHECK | (current << 2));
                }
                break;
            } else if (sq) {
                break;
            }
        }
    }
}

int make_move(info *pstn, move_t mv) {
    if (mv.flags == EP_FLAG) { return -1; }

    int legal = 0;
    int kp_square = 0;

    save_state(pstn);
    pstn->ep_square = 0;
    int piece = pstn->arr[mv.start];

    if (piece & PAWN || mv.flags & CAPTURE_FLAG) {
        pstn->h_clk = 0;
    } else {
        pstn->h_clk++;
    }

    if (mv.flags & CAPTURE_FLAG) {
        capture_piece(pstn, mv.dest);
    }

    move_piece(pstn, mv.start, mv.dest);

    switch(mv.flags) {
        case DPP_FLAG:
            pstn->ep_square = mv.dest + S;
            break;
        case K_CASTLE_FLAG:
            kp_square = D1;
            move_piece(pstn, H1, F1);
            break;
        case Q_CASTLE_FLAG:
            kp_square = F1;
            move_piece(pstn, A1, D1);
    }

    if (mv.flags & PROMO_FLAG) {
        unsigned int p_code = pstn->arr[mv.dest] & 0xF00;
        pstn->arr[mv.dest] = p_code | PROMOTIONS[mv.flags & 3] | WHITE;
    }

    if (piece & KING) {
        if (is_square_attacked(pstn, mv.dest)) {
            legal = -1; // king left in check
        } else if (kp_square && is_square_attacked(pstn, kp_square)) {
            legal = -1; // king castles through check
        } else {
            pstn->c_rights &= 12;
            update_check(pstn, piece, mv);
        }
    } else {
        pstn->c_rights &= (
            (mv.start != A1)
            | ((mv.start != H1) << 1)
            | ((mv.dest != A8) << 2)
            | ((mv.dest != H8) << 3)
        );
        update_check(pstn, piece, mv);
    }

    switch_side(pstn);
    flip_position(pstn);
    return legal;
}

void unmake_move(info *pstn, move_t mv) {
    if (mv.flags == EP_FLAG) { return; }

    flip_position(pstn);
    switch_side(pstn);
    move_piece(pstn, mv.dest, mv.start);

    if (mv.flags & PROMO_FLAG) {
        unsigned int p_code = pstn->arr[mv.start] & 0xF00;
        pstn->arr[mv.start] = p_code | PAWN | WHITE;
    }

    if (mv.flags & CAPTURE_FLAG) {
        pstn->arr[mv.dest] = mv.captured_piece;
        pstn->b_pieces[mv.captured_piece >> 8] = mv.dest;
    }

    switch(mv.flags) {
        case K_CASTLE_FLAG:
            move_piece(pstn, F1, H1);
            break;
        case Q_CASTLE_FLAG:
            move_piece(pstn, D1, A1);
            break;
    }

    restore_state(pstn);
}
