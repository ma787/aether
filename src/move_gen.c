#include <stdlib.h>
#include "aether.h"

void add_quiet_move(POSITION *pstn, move_t mv, MOVE_LIST *moves) {
    if (moves_equal(pstn->search_killers[0][pstn->s_ply], mv)) {
        mv.score = FIRST_KILLER_VALUE;
    } else if (moves_equal(pstn->search_killers[1][pstn->s_ply], mv)) {
        mv.score = SECOND_KILLER_VALUE;
    } else {
        mv.score = pstn->search_history[get_piece_type(pstn->board[mv.start])][mv.dest];
    }

    moves->moves[moves->index++] = mv;
}

void add_capture_move(POSITION *pstn, move_t mv, MOVE_LIST *moves) {
    mv.score = MVV_LVA_SCORES[get_piece_type(pstn->board[mv.dest])][get_piece_type(pstn->board[mv.start])] + CAP_VALUE;
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

void gen_pawn_step(POSITION *pstn, int pos, MOVE_LIST *moves) {
    int current = pos + N;

    if (!pstn->board[current]) {
        add_pawn_quiet_move(pstn, pos, current, moves);

        if (get_rank(current) == 2 && !(pstn->board[(current += N)])) {
            add_quiet_move(pstn, get_move(pstn, pos, current, DPP_FLAG), moves);
        }
    }
}

void gen_pawn_capture(POSITION *pstn, int pos, int vec, MOVE_LIST *moves) {
    int current = pos + vec;
    if ((pstn->board[current] & COLOUR_MASK) == BLACK) {
        add_pawn_capture_move(pstn, pos, current, moves);
    }
}

void gen_ep_capture(POSITION *pstn, MOVE_LIST *moves) {
    int current = pstn->ep_sq + S + E, sq = pstn->board[current];

    if ((sq & WHITE) && (sq & PAWN)) {
        add_ep_capture_move(get_move(pstn, current, pstn->ep_sq, EP_FLAG), moves);
    }
    current = pstn->ep_sq + S + W;
    sq = pstn->board[current];

    if ((sq & WHITE) && (sq & PAWN)) {
        add_ep_capture_move(get_move(pstn, current, pstn->ep_sq, EP_FLAG), moves);
    }
}

void gen_step(POSITION *pstn, int pos, int vec, MOVE_LIST *moves) {
    int current = pos + vec, sq = pstn->board[current];

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
        int colour = pstn->board[current] & COLOUR_MASK;

        if (colour & WHITE) {
            break;
        } else if (colour & BLACK) {
            add_capture_move(pstn, get_move(pstn, pos, current, CAPTURE_FLAG), moves);
            break;
        } else {
            add_quiet_move(pstn, get_move(pstn, pos, current, Q_FLAG), moves);
        }
    }
}

void gen_moves_from_position(POSITION *pstn, int pos, MOVE_LIST *moves) {
    int p_type = get_piece_type(pstn->board[pos]);
    if (!p_type) {
        return;
    }
    MOVE_GENERATOR gen;

    if (p_type & (BISHOP | ROOK | QUEEN)) {
        gen = gen_slider;
    } else if (p_type & (KING | KNIGHT)) {
        gen = gen_step;
    } else {
        gen_pawn_step(pstn, pos, moves);
        gen_pawn_capture(pstn, pos, N + E, moves);
        gen_pawn_capture(pstn, pos, N + W, moves);
        return;
    }

    for (int i = 0; i < N_VECS[p_type]; i++) {
        gen(pstn, pos, MOVE_SETS[p_type][i], moves);
    }
}

void gen_capture(POSITION *pstn, int pos, int enemy_pos, MOVE_LIST *moves) {
    int piece = pstn->board[pos];

    if (!piece || !pstn->board[enemy_pos]) {
        return;
    }

    if (piece & PAWN) {
        if (enemy_pos == (pos + N + E) || enemy_pos == (pos + N + W)) {
            add_pawn_capture_move(pstn, pos, enemy_pos, moves);
        }
        return;
    }

    int alignment = get_alignment(pos, enemy_pos);

    if (alignment & piece) {
        add_capture_move(pstn, get_move(pstn, pos, enemy_pos, CAPTURE_FLAG), moves);
    } else if ((alignment >> 8) & piece) {
        int step = get_step(pos, enemy_pos);
        int current = pos + step;

        while (current != enemy_pos) {
            if (pstn->board[current]) {
                break;
            }
            current += step;
        }
        if (current == enemy_pos) {
            add_capture_move(pstn, get_move(pstn, pos, enemy_pos, CAPTURE_FLAG), moves);
        }
    }
}

void gen_moves_in_check(POSITION *pstn, int pos, MOVE_LIST *moves) {
    int k_pos = pstn->w_pieces[0];

    // attempt to capture the checker
    gen_capture(pstn, pos, pstn->fst_checker, moves);

    // generate moves which might block the checker
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

bool find_pinned_piece(POSITION *pstn, PIN_INFO *p_info) {
    bool possible_pin = false;
    int current = pstn->w_pieces[0];

    for (;;) {
        current += p_info->pin_vector;
        int sq = pstn->board[current];

        switch (sq & COLOUR_MASK) {
            case G:
                return false;
            case WHITE:
                if (possible_pin) {
                    return false;
                }
                possible_pin = true;
                p_info->pinned_loc = current;
                break;
            case BLACK:
                if (possible_pin && ((get_alignment(current, pstn->w_pieces[0]) >> 8) & sq)) {
                    p_info->pinning_loc = current;
                    return true;
                }
                return false;
            }
    }
}

void gen_pinned_moves(POSITION *pstn, PIN_INFO *p_info, MOVE_LIST *moves) {
    if (p_info->pinned_piece & PAWN) {
        if (p_info->pin_vector == N || p_info->pin_vector == S) {
            gen_pawn_step(pstn, p_info->pinned_loc, moves);
        } else if (p_info->pin_vector == (N + E) || p_info->pin_vector == (N + W)) {
            gen_pawn_capture(pstn, p_info->pinned_loc, p_info->pin_vector, moves);
        } else if (p_info->pin_vector == (S + E) || p_info->pin_vector == (S + W)) {
            gen_pawn_capture(pstn, p_info->pinned_loc, -p_info->pin_vector, moves);
        }
    } else if (
        p_info->pinned_piece & (BISHOP | ROOK | QUEEN)
        && (get_alignment(p_info->pinned_loc, p_info->pinned_loc + p_info->pin_vector) & p_info->pinned_piece)
    ) {
        gen_slider(pstn, p_info->pinned_loc, p_info->pin_vector, moves);
        gen_slider(pstn, p_info->pinned_loc, -p_info->pin_vector, moves);
    }
}

void gen_pinned_captures(POSITION *pstn, PIN_INFO *p_info, MOVE_LIST *moves) {
    if (p_info->pinned_piece & PAWN) {
        if (p_info->pin_vector == (N + E) || p_info->pin_vector == (N + W)) {
            gen_pawn_capture(pstn, p_info->pinned_loc, p_info->pin_vector, moves);
        } else if (p_info->pin_vector == (S + E) || p_info->pin_vector == (S + W)) {
            gen_pawn_capture(pstn, p_info->pinned_loc, -p_info->pin_vector, moves);
        }
        return;
    }
    
    int alignment = get_alignment(p_info->pinned_loc, p_info->pinning_loc);
    if (
        alignment & p_info->pinned_piece
        || ((alignment >> 8) & p_info->pinned_piece)
    ) {
        add_capture_move(pstn, get_move(pstn, p_info->pinned_loc, p_info->pinning_loc, CAPTURE_FLAG), moves);
    }
}

void gen_pinned(POSITION *pstn, MOVE_LIST *moves, GEN_PINNED gen, int *temp_removed) {
    PIN_INFO p_info;

    for (int i = 0; i < 8; i++) {
        p_info.pin_vector = KING_OFFS[i]; 

        if (find_pinned_piece(pstn, &p_info)) {
            p_info.pinned_piece = pstn->board[p_info.pinned_loc];

            // temporarily remove pinned piece from piece list
            int p_list_index = get_piece_list_index(p_info.pinned_piece);
            pstn->w_pieces[p_list_index] = 0;
            temp_removed[p_list_index] = p_info.pinned_loc;

            if (pstn->check) {
                continue;
            }

            gen(pstn, &p_info, moves);
        }
    }
}

void all_moves(POSITION *pstn, MOVE_LIST *moves) {
    int temp_removed[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    gen_pinned(pstn, moves, gen_pinned_moves, temp_removed);

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

        if (pstn->ep_sq) {
            gen_ep_capture(pstn, moves);
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
    gen_pinned(pstn, moves, gen_pinned_captures, temp_removed);

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

        if (pstn->ep_sq) {
            gen_ep_capture(pstn, moves);
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

        if (make_move(pstn, m)) {
            unmake_move(pstn, m);
            if (moves_equal(m, mv)) {
                free(moves);
                return true;
            };
        }
    }

    free(moves);
    return false;
}