#include "move_gen.h"
#include "utils.h"

void add_move(info *pstn, int start, int dest, int flags, move_list *moves) {
    if (flags & PROMO_FLAG) {
        for (int i = 0; i < 4; i++) {
            moves->moves[moves->index++] = encode_move(pstn, start, dest, flags | i);
        }
        
    } else {
        moves->moves[moves->index++] = encode_move(pstn, start, dest, flags);
    }
}

void gen_pawn_move(info *pstn, int pos, int vec, move_list *moves) {
    int flags = Q_FLAG;
    int current = pos + vec;
    int sq = pstn->arr[current];

    if (get_rank(current) == 7) {
        flags |= PROMO_FLAG;
    }

    if (vec == N) {
        if (sq) {
            return;
        }

        if (get_rank(current) == 2 && !(pstn->arr[current + vec])) {
            add_move(pstn, pos, current + vec, DPP_FLAG, moves);
        }
    } else {
        flags |= CAPTURE_FLAG;
        if ((sq & COLOUR_MASK) != BLACK) {
            if (current == pstn->ep_square && !sq) {
                flags |= EP_FLAG;
            } else {
                return;
            }
        }
    }

    add_move(pstn, pos, current, flags, moves);
}

void gen_step(info *pstn, int pos, int vec, move_list *moves) {
    int current = pos + vec;
    int sq = pstn->arr[current];

    if ((sq & COLOUR_MASK) == BLACK) {
        add_move(pstn, pos, current, CAPTURE_FLAG, moves);
    } else if (!sq) {
        add_move(pstn, pos, current, Q_FLAG, moves);
    }
}

void gen_slider(info *pstn, int pos, int vec, move_list *moves) {
    int current = pos;

    for (;;) {
        current += vec;
        int sq = pstn->arr[current];

        if (!sq) {
            add_move(pstn, pos, current, Q_FLAG, moves);
            continue;
        } else if ((sq & COLOUR_MASK) == BLACK) {
            add_move(pstn, pos, current, CAPTURE_FLAG, moves);
        }

        return;
    }
}

void gen_moves(info *pstn, int pos, MOVE_GENERATOR gen, int n_vecs, int *move_set, move_list *moves) {
    for (int i = 0; i < n_vecs; i++) {
        gen(pstn, pos, move_set[i], moves);
    }
}

void gen_moves_from_position(info *pstn, int pos, move_list *moves) {
    int p_type = pstn->arr[pos] & 0xFC;
    int *move_set;
    int n_vecs;
    MOVE_GENERATOR gen;

    switch (p_type) {
    case PAWN:
        move_set = PAWN_OFFS;
        gen = gen_pawn_move;
        n_vecs = 3;
        break;
    case KNIGHT:
        move_set = KNIGHT_OFFS;
        gen = gen_step;
        n_vecs = 8;
        break;
    case BISHOP:
        move_set = BISHOP_OFFS;
        gen = gen_slider;
        n_vecs = 4;
        break;
    case ROOK:
        move_set = ROOK_OFFS;
        gen = gen_slider;
        n_vecs = 4;
        break;
    case QUEEN:
        move_set = QUEEN_OFFS;
        gen = gen_slider;
        n_vecs = 8;
        break;
    case KING:
        move_set = KING_OFFS;
        gen = gen_step;
        n_vecs = 8;
    }

    gen_moves(pstn, pos, gen, n_vecs, move_set, moves);
}

void all_moves(info *pstn, move_list *moves) {
    for (int i = 0; i < 16; i++) {
        int pos = pstn->w_pieces[i];
        if (!pos) {
            continue;
        }
        gen_moves_from_position(pstn, pos, moves);
    }
}