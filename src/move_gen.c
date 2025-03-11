#include "move_gen.h"
#include "utils.h"
#include <stdlib.h>

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

void gen_moves_from_position(info *pstn, int pos, move_list *moves) {
    int p_type = pstn->arr[pos] & 0xFC;
    MOVE_GENERATOR gen;

    if (p_type & (BISHOP | ROOK | QUEEN)) {
        gen = gen_slider;
    } else if (p_type & (KING | KNIGHT)) {
        gen = gen_step;
    } else {
        gen = gen_pawn_move;
    }

    for (int i = 0; i < N_VECS[p_type]; i++) {
        gen(pstn, pos, MOVE_SETS[p_type][i], moves);
    }
}

void gen_moves_in_check(info *pstn, int pos, move_list *moves) {
    int piece = pstn->arr[pos];
    int checker = (pstn->check_info >> 2) & 0xFF;
    int vec = get_step(pos, checker);

    // attempt to capture the checker
    if (is_attacking(piece, pos, checker)) {
        int current = pos + vec;
        int blocked = 0;
        int flags = CAPTURE_FLAG;

        while (current != checker) {
            if (pstn->arr[current]) {
                blocked = 1;
                break;
            }
            current += vec;
        }
        
        if (!blocked) {
            if ((piece & PAWN) && get_rank(checker) == 7) {
                flags |= PROMO_FLAG;
            }
            add_move(pstn, pos, checker, flags, moves);
        }
    }

    if ((checker == (pstn->ep_square + S)) && (piece & PAWN) && is_attacking(piece, pos, pstn->ep_square)) {
        add_move(pstn, pos, pstn->ep_square, EP_FLAG, moves);
    }

    // generate moves which might block the checker
    int k_pos = pstn->w_pieces[0];
    int k_step = get_step(k_pos, checker);
    move_list *blocking_moves = malloc(sizeof(move_list));
    blocking_moves->index = 0;
    gen_moves_from_position(pstn, pos, blocking_moves);

    // check if each move actually blocks the checker
    for (int i = 0; i < blocking_moves->index; i++) {
        move_t mv = blocking_moves->moves[i];
        if (get_step(k_pos, mv.dest) == k_step) {
            int current = k_pos + k_step;
            while (current != checker) {
                if (current == mv.dest) {
                    moves->moves[moves->index++] = mv;
                    break;
                }
                current += k_step;
            }
        }
    }

    free(blocking_moves);
}

int find_pinned_piece(info *pstn, int vec, int *pinned_loc) {
    int possible_pin = 0;
    int current = pstn->w_pieces[0];

    for (;;) {
        current += vec;
        int sq = pstn->arr[current];

        switch (sq & COLOUR_MASK) {
            case G:
                return 0;
            case WHITE:
                if (possible_pin) {
                    return 0;
                }
                possible_pin = 1;
                *pinned_loc = current;
                break;
            case BLACK:
                if (possible_pin && is_attacking(sq, current, pstn->w_pieces[0])) {
                    return 1;
                }
                return 0;
            }
    }
}

int gen_pinned_pieces(info *pstn, move_list *moves, int *piece_locs) {
    int pinned_pieces[15];
    int n_pinned = 0;

    for (int i = 0; i < 8; i++) {
        int vec = KING_OFFS[i];
        int pinned_loc = 0;
        int is_pinned = find_pinned_piece(pstn, vec, &pinned_loc);

        if (is_pinned) {
            int pinned_piece = pstn->arr[pinned_loc];
            pinned_pieces[n_pinned++] = pinned_loc;

            if (pstn->check_info) {
                continue;
            }

            if (!(is_attacking(pinned_piece, pinned_loc, pinned_loc + vec))) {
                if (
                    pinned_piece & PAWN
                    && (vec == S || is_attacking(WHITE | PAWN, pinned_loc, pinned_loc - vec))
                ) {
                    vec *= -1;
                } else {
                    continue;
                }
            }

            int p_type = pinned_piece & 0xFC;
            MOVE_GENERATOR gen;

            if (p_type & (BISHOP | ROOK | QUEEN)) {
                gen = gen_slider;
            } else if (p_type & KNIGHT) {
                gen = gen_step;
            } else {
                gen = gen_pawn_move;
            }

            gen(pstn, pinned_loc, vec, moves);
        }
    }
    
    int piece_locs_index = 0;

    for (int i = 1; i < 16; i++) {
        int pos = pstn->w_pieces[i];

        if (!pos) {
            continue;
        }

        int found = 0;

        for (int j = 0; j < n_pinned; j++) {
            if (pos == pinned_pieces[j]) {
                found = 1;
                break;
            }
        }

        if (!found) {
            piece_locs[piece_locs_index++] = pos;
        }
    }

    return piece_locs_index;
}

void all_moves(info *pstn, move_list *moves) {
    int piece_locs[15];
    int len = gen_pinned_pieces(pstn, moves, piece_locs);

    // generate king moves
    gen_moves_from_position(pstn, pstn->w_pieces[0], moves);

    int check = pstn->check_info & 3;

    if (check == NO_CHECK) {
        for (int i = 0; i < len; i++) {
            gen_moves_from_position(pstn, piece_locs[i], moves);
        }

        if (pstn->c_rights & WHITE_KINGSIDE && !(pstn->arr[F1] | pstn->arr[G1])) {
            add_move(pstn, E1, G1, K_CASTLE_FLAG, moves);
        }

        if (
            pstn->c_rights & WHITE_QUEENSIDE
            && !(pstn->arr[B1] | pstn->arr[C1] | pstn->arr[D1])
        ) {
            add_move(pstn, E1, C1, Q_CASTLE_FLAG, moves);
        }
    } else if (check == CONTACT_CHECK || check == DISTANT_CHECK) {
        for (int i = 0; i < len; i++) {
            gen_moves_in_check(pstn, piece_locs[i], moves);
        }
    }
}