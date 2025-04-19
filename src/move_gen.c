#include <stdlib.h>
#include "aether.h"

void add_quiet_move(POSITION *pstn, move_t mv, MOVE_LIST *moves) {
    if (moves_equal(pstn->search_killers[0][pstn->ply], mv)) {
        mv.score = FIRST_KILLER_VALUE;
    } else if (moves_equal(pstn->search_killers[1][pstn->ply], mv)) {
        mv.score = SECOND_KILLER_VALUE;
    } else {
        mv.score = pstn->search_history[pstn->board[mv.start] & 0xFC][mv.dest];
    }

    moves->moves[moves->index++] = mv;
}

void add_capture_move(POSITION *pstn, move_t mv, MOVE_LIST *moves) {
    mv.score = MVV_LVA_SCORES[pstn->board[mv.dest] & 0xFC][pstn->board[mv.start] & 0xFC] + CAP_VALUE;
    moves->moves[moves->index++] = mv;
}

void add_ep_capture_move(move_t mv, MOVE_LIST *moves) {
    mv.score = 105 + CAP_VALUE;
    moves->moves[moves->index++] = mv;
}

void add_pawn_quiet_move(POSITION *pstn, int start, int dest, MOVE_LIST *moves) {
    if (get_rank(dest) == 7) {
        add_quiet_move(pstn, get_move(pstn, start, dest, KNIGHT_PROMO), moves);
        add_quiet_move(pstn, get_move(pstn, start, dest, BISHOP_PROMO), moves);
        add_quiet_move(pstn, get_move(pstn, start, dest, ROOK_PROMO), moves);
        add_quiet_move(pstn, get_move(pstn, start, dest, QUEEN_PROMO), moves);
    } else {
        add_quiet_move(pstn, get_move(pstn, start, dest, Q_FLAG), moves);
    }
}

void add_pawn_capture_move(POSITION *pstn, int start, int dest, MOVE_LIST *moves) {
    if (get_rank(dest) == 7) {
        add_capture_move(pstn, get_move(pstn, start, dest, KNIGHT_PROMO | CAPTURE_FLAG), moves);
        add_capture_move(pstn, get_move(pstn, start, dest, BISHOP_PROMO | CAPTURE_FLAG), moves);
        add_capture_move(pstn, get_move(pstn, start, dest, ROOK_PROMO | CAPTURE_FLAG), moves);
        add_capture_move(pstn, get_move(pstn, start, dest, QUEEN_PROMO | CAPTURE_FLAG), moves);
    } else {
        add_capture_move(pstn, get_move(pstn, start, dest, CAPTURE_FLAG), moves);
    }
}

void gen_pawn_move(POSITION *pstn, int pos, int vec, MOVE_LIST *moves) {
    int current = pos + vec;
    int sq = pstn->board[current];

    if (vec == N) {
        if (sq) {
            return;
        }

        add_pawn_quiet_move(pstn, pos, current, moves);

        if (get_rank(current) == 2 && !(pstn->board[current + vec])) {
            add_quiet_move(pstn, get_move(pstn, pos, current + vec, DPP_FLAG), moves);
        }
    } else if ((sq & COLOUR_MASK) == BLACK) {
        add_pawn_capture_move(pstn, pos, current, moves);
    } else if (current == pstn->ep_sq && !sq) {
        add_ep_capture_move(get_move(pstn, pos, current, EP_FLAG), moves);
    }
}

void gen_step(POSITION *pstn, int pos, int vec, MOVE_LIST *moves) {
    int current = pos + vec;
    int sq = pstn->board[current];

    if ((sq & COLOUR_MASK) == BLACK) {
        add_capture_move(pstn, get_move(pstn, pos, current, CAPTURE_FLAG), moves);
    } else if (!sq) {
        add_quiet_move(pstn, get_move(pstn, pos, current, Q_FLAG), moves);
    }
}

void gen_slider(POSITION *pstn, int pos, int vec, MOVE_LIST *moves) {
    int current = pos;

    for (;;) {
        current += vec;
        int sq = pstn->board[current];

        if (!sq) {
            add_quiet_move(pstn, get_move(pstn, pos, current, Q_FLAG), moves);
            continue;
        } else if ((sq & COLOUR_MASK) == BLACK) {
            add_capture_move(pstn, get_move(pstn, pos, current, CAPTURE_FLAG), moves);
        }

        return;
    }
}

void gen_moves_from_position(POSITION *pstn, int pos, MOVE_LIST *moves) {
    int p_type = pstn->board[pos] & 0xFC;
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

bool can_capture(POSITION *pstn, int piece, int pos, int enemy_pos) {
    if (!is_attacking(piece, pos, enemy_pos)) {
        return false;
    }

    int vec = get_step(pos, enemy_pos);
    int current = pos + vec;

    while (current != enemy_pos) {
        if (pstn->board[current]) {
            return false;
            break;
        }
        current += vec;
    }

    return true;
}

void gen_capture(POSITION *pstn, int pos, int enemy_pos, MOVE_LIST *moves) {
    int piece = pstn->board[pos];

    if (can_capture(pstn, piece, pos, enemy_pos)) {
        if (piece & PAWN) {
            add_pawn_capture_move(pstn, pos, enemy_pos, moves);
        } else {
            add_capture_move(pstn, get_move(pstn, pos, enemy_pos, CAPTURE_FLAG), moves);
        }
    } else if (
        piece & PAWN
        && enemy_pos == (pstn->ep_sq + S) 
        && is_attacking(piece, pos, pstn->ep_sq)
    ) {
        add_ep_capture_move(get_move(pstn, pos, pstn->ep_sq, EP_FLAG), moves);
    }
}

void gen_moves_in_check(POSITION *pstn, int pos, MOVE_LIST *moves) {
    // attempt to capture the checker
    gen_capture(pstn, pos, pstn->fst_checker, moves);

    // generate moves which might block the checker
    int k_pos = pstn->w_pieces[0];
    int k_step = get_step(k_pos, pstn->fst_checker);
    MOVE_LIST *blocking_moves = malloc(sizeof(MOVE_LIST));
    blocking_moves->index = 0;
    gen_moves_from_position(pstn, pos, blocking_moves);

    // check if each move actually blocks the checker
    for (int i = 0; i < blocking_moves->index; i++) {
        move_t mv = blocking_moves->moves[i];

        if (get_step(k_pos, mv.dest) == k_step) {
            int current = k_pos + k_step;
            while (current != pstn->fst_checker) {
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

bool find_pinned_piece(POSITION *pstn, int vec, int *pinned_loc, int *pinning_piece) {
    int possible_pin = 0, current = pstn->w_pieces[0], sq;

    for (;;) {
        current += vec;
        sq = pstn->board[current];

        switch (sq & COLOUR_MASK) {
            case G:
                return false;
            case WHITE:
                if (possible_pin) {
                    return false;
                }
                possible_pin = 1;
                *pinned_loc = current;
                break;
            case BLACK:
                if (possible_pin && is_attacking(sq, current, pstn->w_pieces[0])) {
                    *pinning_piece = current;
                    return true;
                }
                return false;
            }
    }
}

void gen_pinned_pieces(POSITION *pstn, MOVE_LIST *moves, int *temp_removed, bool captures_only) {
    for (int i = 0; i < 8; i++) {
        int vec = KING_OFFS[i];
        int pinned_loc = 0, pinning_piece = 0;

        if (find_pinned_piece(pstn, vec, &pinned_loc, &pinning_piece)) {
            int pinned_piece = pstn->board[pinned_loc];
            int p_list_index = pinned_piece >> 8;
            pstn->w_pieces[p_list_index] = 0;
            temp_removed[p_list_index] = pinned_loc;

            if (pstn->check) {
                continue;
            }

            if (captures_only) {
                if (is_attacking(pinned_piece, pinned_loc, pinned_loc + vec)) {
                    if (pinned_piece & PAWN) {
                        gen_pawn_move(pstn, pinned_loc, vec, moves);
                    } else {
                        add_capture_move(pstn, get_move(pstn, pinned_loc, pinning_piece, CAPTURE_FLAG), moves);
                    }
                } else if (
                        pinned_piece & PAWN
                        && is_attacking(pinned_piece, pinned_loc, pinned_loc - vec)
                    ) {
                        gen_pawn_move(pstn, pinned_loc, -vec, moves);
                    }
                continue;
            }
            
            if (pinned_piece & PAWN) {
                if (vec == N || vec == S) {
                    gen_pawn_move(pstn, pinned_loc, N, moves);
                } else if (is_attacking(pinned_piece, pinned_loc, pinned_loc + vec)) {
                    gen_pawn_move(pstn, pinned_loc, vec, moves);
                } else if (is_attacking(pinned_piece, pinned_loc, pinned_loc - vec)) {
                    gen_pawn_move(pstn, pinned_loc, -vec, moves);
                }
            } else if (pinned_piece & (BISHOP | ROOK | QUEEN)) {
                if (!(is_attacking(pinned_piece, pinned_loc, pinned_loc + vec))) {
                    continue;
                }
                gen_slider(pstn, pinned_loc, vec, moves);
                gen_slider(pstn, pinned_loc, -vec, moves);
            }
        }
    }
}

void all_moves(POSITION *pstn, MOVE_LIST *moves) {
    int temp_removed[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    gen_pinned_pieces(pstn, moves, temp_removed, false);

    // generate king moves
    gen_moves_from_position(pstn, pstn->w_pieces[0], moves);

    if (pstn->check < DOUBLE_CHECK) {
        GEN_FROM_POSITION gen;

        if (pstn->check == CONTACT_CHECK || pstn->check == DISTANT_CHECK) {
            gen = gen_moves_in_check;
        } else {
            gen = gen_moves_from_position;

            if (pstn->c_rights & WHITE_KINGSIDE && !(pstn->board[F1] | pstn->board[G1])) {
                add_quiet_move(pstn, get_move(pstn, E1, G1, K_CASTLE_FLAG), moves);
            }

            if (pstn->c_rights & WHITE_QUEENSIDE && !(pstn->board[B1] | pstn->board[C1] | pstn->board[D1])) {
                add_quiet_move(pstn, get_move(pstn, E1, C1, Q_CASTLE_FLAG), moves);
            }
        }

        for (int i = 1; i < 16; i++) {
            gen(pstn, pstn->w_pieces[i], moves);
        }
    }

    for (int i = 1; i < 16; i++) {
        if (temp_removed[i]) {
            pstn->w_pieces[i] = temp_removed[i];
        }
    }
}

void all_captures(POSITION *pstn, MOVE_LIST *moves) {
    int temp_removed[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    gen_pinned_pieces(pstn, moves, temp_removed, true);

    // generate king captures
    for (int i = 0; i < 16; i++) {
        gen_capture(pstn, pstn->w_pieces[0], pstn->b_pieces[i], moves);
    }

    if (pstn->check < DOUBLE_CHECK) {
        if (pstn->check == CONTACT_CHECK || pstn->check == DISTANT_CHECK) {
            for (int i = 1; i < 16; i++) {
                gen_capture(pstn, pstn->w_pieces[i], pstn->fst_checker, moves);
            }
        } else {
            for (int i = 1; i < 16; i++) {
                for (int j = 1; j < 16; j++) {
                    gen_capture(pstn, pstn->w_pieces[i], pstn->b_pieces[j], moves);
                }
            }
        }
    }

    for (int i = 1; i < 16; i++) {
        if (temp_removed[i]) {
            pstn->w_pieces[i] = temp_removed[i];
        }
    }
}

bool move_exists(POSITION *pstn, move_t mv) {
    MOVE_LIST *moves = malloc(sizeof(MOVE_LIST));
    moves->index = 0;
    all_moves(pstn, moves);

    for (int i = 0; i < moves->index; i++) {
        move_t m = moves->moves[i];

        if (make_move(pstn, m) == 0) {
            if (moves_equal(m, mv)) {
                unmake_move(pstn, m);
                free(moves);
                return true;
            };
        }
        unmake_move(pstn, m);
    }

    free(moves);
    return false;
}