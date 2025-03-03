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
