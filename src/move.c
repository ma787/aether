#include "constants.h"
#include "move.h"

move_t of_string(info *pstn, char *mstr) {
    move_t mv = {
        .start = string_to_coord(mstr),
        .dest = string_to_coord(mstr + 2),
        .flags = 0,
        .captured_piece = 0
    };

    if (pstn->side == BLACK) {
        mv.start = (~mv.start & 0xF0) | (mv.start & 0x0F);
        mv.dest = (~mv.dest & 0xF0) | (mv.dest & 0x0F);
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

int make_move(info *pstn, move_t mv) {
    if (
        mv.flags == EP_FLAG 
        || mv.flags == K_CASTLE_FLAG
        || mv.flags == Q_CASTLE_FLAG
    ) { return -1; }

    save_state(pstn);
    pstn->ep_square = 0;

    if (pstn->arr[mv.start] & PAWN || mv.flags & CAPTURE_FLAG) {
        pstn->h_clk = 0;
    } else {
        pstn->h_clk++;
    }

    if (mv.flags & CAPTURE_FLAG) {
        capture_piece(pstn, mv.dest);
    }

    move_piece(pstn, mv.start, mv.dest);

    if (mv.flags == DPP_FLAG) {
        pstn->ep_square = mv.dest + S;
    }

    if (mv.flags & PROMO_FLAG) {
        unsigned int p_code = pstn->arr[mv.dest] & 0xF00;
        pstn->arr[mv.dest] = p_code | PROMOTIONS[mv.flags & 3] | WHITE;
    }

    switch_side(pstn);
    flip_position(pstn);
    return 0;
}

void unmake_move(info *pstn, move_t mv) {
    if (
        mv.flags == EP_FLAG 
        || mv.flags == K_CASTLE_FLAG
        || mv.flags == Q_CASTLE_FLAG
    ) { return; }

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

    restore_state(pstn);
}
