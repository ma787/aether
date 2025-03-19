#include <stdlib.h>
#include "move_gen.h"
#include "constants.h"
#include "position.h"
#include "utils.h"


void add_move(int start, int dest, int flags, move_list *moves) {
    if (flags & PROMO_FLAG) {
        for (int i = 0; i < 4; i++) {
            moves->moves[moves->index++] = encode_move(start, dest, flags | i);
        }
        
    } else {
        moves->moves[moves->index++] = encode_move(start, dest, flags);
    }
}

void gen_pawn_move(int pos, int vec, move_list *moves) {
    int flags = Q_FLAG;
    int current = pos + vec;
    int sq = board[current];

    if (get_rank(current) == 7) {
        flags |= PROMO_FLAG;
    }

    if (vec == N) {
        if (sq) {
            return;
        }

        if (get_rank(current) == 2 && !(board[current + vec])) {
            add_move(pos, current + vec, DPP_FLAG, moves);
        }
    } else {
        flags |= CAPTURE_FLAG;
        if ((sq & COLOUR_MASK) != BLACK) {
            if (current == ep_square && !sq) {
                flags |= EP_FLAG;
            } else {
                return;
            }
        }
    }

    add_move(pos, current, flags, moves);
}

void gen_step(int pos, int vec, move_list *moves) {
    int current = pos + vec;
    int sq = board[current];

    if ((sq & COLOUR_MASK) == BLACK) {
        add_move(pos, current, CAPTURE_FLAG, moves);
    } else if (!sq) {
        add_move(pos, current, Q_FLAG, moves);
    }
}

void gen_slider(int pos, int vec, move_list *moves) {
    int current = pos;

    for (;;) {
        current += vec;
        int sq = board[current];

        if (!sq) {
            add_move(pos, current, Q_FLAG, moves);
            continue;
        } else if ((sq & COLOUR_MASK) == BLACK) {
            add_move(pos, current, CAPTURE_FLAG, moves);
        }

        return;
    }
}

void gen_moves_from_position(int pos, move_list *moves) {
    int p_type = board[pos] & 0xFC;
    MOVE_GENERATOR gen;

    if (p_type & (BISHOP | ROOK | QUEEN)) {
        gen = gen_slider;
    } else if (p_type & (KING | KNIGHT)) {
        gen = gen_step;
    } else {
        gen = gen_pawn_move;
    }

    for (int i = 0; i < N_VECS[p_type]; i++) {
        gen(pos, MOVE_SETS[p_type][i], moves);
    }
}

void gen_moves_in_check(int pos, move_list *moves) {
    int piece = board[pos];
    int checker = (check_info >> 2) & 0xFF;
    int vec = get_step(pos, checker);

    // attempt to capture the checker
    if (is_attacking(piece, pos, checker)) {
        int current = pos + vec;
        int blocked = 0;
        int flags = CAPTURE_FLAG;

        while (current != checker) {
            if (board[current]) {
                blocked = 1;
                break;
            }
            current += vec;
        }
        
        if (!blocked) {
            if ((piece & PAWN) && get_rank(checker) == 7) {
                flags |= PROMO_FLAG;
            }
            add_move(pos, checker, flags, moves);
        }
    }

    if ((checker == (ep_square + S)) && (piece & PAWN) && is_attacking(piece, pos, ep_square)) {
        add_move(pos, ep_square, EP_FLAG, moves);
    }

    // generate moves which might block the checker
    int k_pos = w_pieces[0];
    int k_step = get_step(k_pos, checker);
    move_list *blocking_moves = malloc(sizeof(move_list));
    blocking_moves->index = 0;
    gen_moves_from_position(pos, blocking_moves);

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

int find_pinned_piece(int vec, int *pinned_loc) {
    int possible_pin = 0;
    int current = w_pieces[0];

    for (;;) {
        current += vec;
        int sq = board[current];

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
                if (possible_pin && is_attacking(sq, current, w_pieces[0])) {
                    return 1;
                }
                return 0;
            }
    }
}

int gen_pinned_pieces(move_list *moves, int *piece_locs) {
    int pinned_pieces[15];
    int n_pinned = 0;

    for (int i = 0; i < 8; i++) {
        int vec = KING_OFFS[i];
        int pinned_loc = 0;
        int is_pinned = find_pinned_piece(vec, &pinned_loc);

        if (is_pinned) {
            int pinned_piece = board[pinned_loc];
            pinned_pieces[n_pinned++] = pinned_loc;

            if (check_info) {
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

            gen(pinned_loc, vec, moves);
        }
    }
    
    int piece_locs_index = 0;

    for (int i = 1; i < 16; i++) {
        int pos = w_pieces[i];

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

void all_moves(move_list *moves) {
    int piece_locs[15];
    int len = gen_pinned_pieces(moves, piece_locs);

    // generate king moves
    gen_moves_from_position(w_pieces[0], moves);

    int check = check_info & 3;

    if (check == NO_CHECK) {
        for (int i = 0; i < len; i++) {
            gen_moves_from_position(piece_locs[i], moves);
        }

        if (c_rights & WHITE_KINGSIDE && !(board[F1] | board[G1])) {
            add_move(E1, G1, K_CASTLE_FLAG, moves);
        }

        if (
            c_rights & WHITE_QUEENSIDE
            && !(board[B1] | board[C1] | board[D1])
        ) {
            add_move(E1, C1, Q_CASTLE_FLAG, moves);
        }
    } else if (check == CONTACT_CHECK || check == DISTANT_CHECK) {
        for (int i = 0; i < len; i++) {
            gen_moves_in_check(piece_locs[i], moves);
        }
    }
}