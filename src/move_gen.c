#include <stdlib.h>
#include "aether.h"

void add_quiet_move(move_t mv, MOVE_LIST *moves) {
    if (moves_equal(search_killers[0][ply], mv)) {
        mv.score = FIRST_KILLER_VALUE;
    } else if (moves_equal(search_killers[1][ply], mv)) {
        mv.score = SECOND_KILLER_VALUE;
    } else {
        mv.score = search_history[board[mv.start] & 0xFC][mv.dest];
    }

    moves->moves[moves->index++] = mv;
}

void add_capture_move(move_t mv, MOVE_LIST *moves) {
    mv.score = MVV_LVA_SCORES[board[mv.dest] & 0xFC][board[mv.start] & 0xFC] + CAP_VALUE;
    moves->moves[moves->index++] = mv;
}

void add_ep_capture_move(move_t mv, MOVE_LIST *moves) {
    mv.score = 105 + CAP_VALUE;
    moves->moves[moves->index++] = mv;
}

void add_pawn_quiet_move(int start, int dest, MOVE_LIST *moves) {
    if (get_rank(dest) == 7) {
        add_quiet_move(get_move(start, dest, KNIGHT_PROMO), moves);
        add_quiet_move(get_move(start, dest, BISHOP_PROMO), moves);
        add_quiet_move(get_move(start, dest, ROOK_PROMO), moves);
        add_quiet_move(get_move(start, dest, QUEEN_PROMO), moves);
    } else {
        add_quiet_move(get_move(start, dest, Q_FLAG), moves);
    }
}

void add_pawn_capture_move(int start, int dest, MOVE_LIST *moves) {
    if (get_rank(dest) == 7) {
        add_capture_move(get_move(start, dest, KNIGHT_PROMO | CAPTURE_FLAG), moves);
        add_capture_move(get_move(start, dest, BISHOP_PROMO | CAPTURE_FLAG), moves);
        add_capture_move(get_move(start, dest, ROOK_PROMO | CAPTURE_FLAG), moves);
        add_capture_move(get_move(start, dest, QUEEN_PROMO | CAPTURE_FLAG), moves);
    } else {
        add_capture_move(get_move(start, dest, CAPTURE_FLAG), moves);
    }
}

void gen_pawn_move(int pos, int vec, MOVE_LIST *moves) {
    int current = pos + vec;
    int sq = board[current];

    if (vec == N) {
        if (sq) {
            return;
        }

        add_pawn_quiet_move(pos, current, moves);

        if (get_rank(current) == 2 && !(board[current + vec])) {
            add_quiet_move(get_move(pos, current + vec, DPP_FLAG), moves);
        }
    } else if ((sq & COLOUR_MASK) == BLACK) {
        add_pawn_capture_move(pos, current, moves);
    } else if (current == ep_square && !sq) {
        add_ep_capture_move(get_move(pos, current, EP_FLAG), moves);
    }
}

void gen_step(int pos, int vec, MOVE_LIST *moves) {
    int current = pos + vec;
    int sq = board[current];

    if ((sq & COLOUR_MASK) == BLACK) {
        add_capture_move(get_move(pos, current, CAPTURE_FLAG), moves);
    } else if (!sq) {
        add_quiet_move(get_move(pos, current, Q_FLAG), moves);
    }
}

void gen_slider(int pos, int vec, MOVE_LIST *moves) {
    int current = pos;

    for (;;) {
        current += vec;
        int sq = board[current];

        if (!sq) {
            add_quiet_move(get_move(pos, current, Q_FLAG), moves);
            continue;
        } else if ((sq & COLOUR_MASK) == BLACK) {
            add_capture_move(get_move(pos, current, CAPTURE_FLAG), moves);
        }

        return;
    }
}

void gen_moves_from_position(int pos, MOVE_LIST *moves) {
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

bool can_capture(int piece, int pos, int enemy_pos) {
    if (!is_attacking(piece, pos, enemy_pos)) {
        return false;
    }

    int vec = get_step(pos, enemy_pos);
    int current = pos + vec;

    while (current != enemy_pos) {
        if (board[current]) {
            return false;
            break;
        }
        current += vec;
    }

    return true;
}

void gen_capture(int pos, int enemy_pos, MOVE_LIST *moves) {
    int piece = board[pos];

    if (can_capture(piece, pos, enemy_pos)) {
        if (piece & PAWN) {
            add_pawn_capture_move(pos, enemy_pos, moves);
        } else {
            add_capture_move(get_move(pos, enemy_pos, CAPTURE_FLAG), moves);
        }
    } else if (
        piece & PAWN
        && enemy_pos == (ep_square + S) 
        && is_attacking(piece, pos, ep_square)
    ) {
        add_ep_capture_move(get_move(pos, ep_square, EP_FLAG), moves);
    }
}

void gen_moves_in_check(int pos, MOVE_LIST *moves) {
    int checker = (check_info >> 2) & 0xFF;

    // attempt to capture the checker
    gen_capture(pos, checker, moves);

    // generate moves which might block the checker
    int k_pos = w_pieces[0];
    int k_step = get_step(k_pos, checker);
    MOVE_LIST *blocking_moves = malloc(sizeof(MOVE_LIST));
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

bool find_pinned_piece(int vec, int *pinned_loc, int *pinning_piece) {
    int possible_pin = 0, current = w_pieces[0], sq;

    for (;;) {
        current += vec;
        sq = board[current];

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
                if (possible_pin && is_attacking(sq, current, w_pieces[0])) {
                    *pinning_piece = current;
                    return true;
                }
                return false;
            }
    }
}

void gen_pinned_pieces(MOVE_LIST *moves, int *temp_removed, bool captures_only) {
    for (int i = 0; i < 8; i++) {
        int vec = KING_OFFS[i];
        int pinned_loc = 0, pinning_piece = 0;

        if (find_pinned_piece(vec, &pinned_loc, &pinning_piece)) {
            int pinned_piece = board[pinned_loc];
            int p_list_index = pinned_piece >> 8;
            w_pieces[p_list_index] = 0;
            temp_removed[p_list_index] = pinned_loc;

            if (check_info) {
                continue;
            }

            if (captures_only) {
                if (is_attacking(pinned_piece, pinned_loc, pinned_loc + vec)) {
                    if (pinned_piece & PAWN) {
                        gen_pawn_move(pinned_loc, vec, moves);
                    } else {
                        add_capture_move(get_move(pinned_loc, pinning_piece, CAPTURE_FLAG), moves);
                    }
                } else if (
                        pinned_piece & PAWN
                        && is_attacking(pinned_piece, pinned_loc, pinned_loc - vec)
                    ) {
                        gen_pawn_move(pinned_loc, -vec, moves);
                    }
                continue;
            }
            
            if (pinned_piece & PAWN) {
                if (vec == N || vec == S) {
                    gen_pawn_move(pinned_loc, N, moves);
                } else if (is_attacking(pinned_piece, pinned_loc, pinned_loc + vec)) {
                    gen_pawn_move(pinned_loc, vec, moves);
                } else if (is_attacking(pinned_piece, pinned_loc, pinned_loc - vec)) {
                    gen_pawn_move(pinned_loc, -vec, moves);
                }
            } else if (pinned_piece & (BISHOP | ROOK | QUEEN)) {
                if (!(is_attacking(pinned_piece, pinned_loc, pinned_loc + vec))) {
                    continue;
                }
                gen_slider(pinned_loc, vec, moves);
                gen_slider(pinned_loc, -vec, moves);
            }
        }
    }
}

void all_moves(MOVE_LIST *moves) {
    int temp_removed[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    gen_pinned_pieces(moves, temp_removed, false);

    // generate king moves
    gen_moves_from_position(w_pieces[0], moves);

    int check = check_info & 3;

    if (check < DOUBLE_CHECK) {
        GEN_FROM_POSITION gen;

        if (check == CONTACT_CHECK || check == DISTANT_CHECK) {
            gen = gen_moves_in_check;
        } else {
            gen = gen_moves_from_position;

            if (c_rights & WHITE_KINGSIDE && !(board[F1] | board[G1])) {
                add_quiet_move(get_move(E1, G1, K_CASTLE_FLAG), moves);
            }

            if (c_rights & WHITE_QUEENSIDE && !(board[B1] | board[C1] | board[D1])) {
                add_quiet_move(get_move(E1, C1, Q_CASTLE_FLAG), moves);
            }
        }

        for (int i = 1; i < 16; i++) {
            gen(w_pieces[i], moves);
        }
    }

    for (int i = 1; i < 16; i++) {
        if (temp_removed[i]) {
            w_pieces[i] = temp_removed[i];
        }
    }
}

bool move_exists(move_t mv) {
    MOVE_LIST *moves = malloc(sizeof(MOVE_LIST));
    moves->index = 0;
    all_moves(moves);

    for (int i = 0; i < moves->index; i++) {
        move_t m = moves->moves[i];

        if (make_move(m) == 0) {
            if (moves_equal(m, mv)) {
                unmake_move(m);
                free(moves);
                return true;
            };
        }
        unmake_move(m);
    }

    free(moves);
    return false;
}