#include <string.h>
#include "aether.h"

void add_quiet_move(POSITION *pstn, MOVE_LIST *moves, int s, int d, int f) {
    move_t mv = get_move(pstn, s, d, f);

    if (moves_equal(pstn->search_killers[0][pstn->s_ply], mv)) {
        mv.score = FIRST_KILLER_VALUE;
    } else if (moves_equal(pstn->search_killers[1][pstn->s_ply], mv)) {
        mv.score = SECOND_KILLER_VALUE;
    } else {
        mv.score = HISTORY(pstn, mv.start, mv.dest);
    }

    moves->moves[moves->index++] = mv;
}

void add_capture_move(POSITION *pstn, MOVE_LIST *moves, int s, int d, int f) {
    move_t mv = get_move(pstn, s, d, f | CAPTURE_FLAG);
    mv.score = MVV_LVA(pstn, mv.start, mv.dest) + CAP_VALUE;
    moves->moves[moves->index++] = mv;
}

void add_ep_capture_move(POSITION *pstn, MOVE_LIST *moves, int start) {
    move_t mv = get_move(pstn, start, pstn->ep_sq, EP_FLAG);
    mv.score = 105 + CAP_VALUE;
    moves->moves[moves->index++] = mv;
}

void add_pawn_quiet_move(POSITION *pstn, int s, int d, MOVE_LIST *moves) {
    if (RANK(d) == FINAL_RANK[pstn->side]) {
        add_quiet_move(pstn, moves, s, d, KNIGHT_PROMO);
        add_quiet_move(pstn, moves, s, d, BISHOP_PROMO);
        add_quiet_move(pstn, moves, s, d, ROOK_PROMO);
        add_quiet_move(pstn, moves, s, d, QUEEN_PROMO);
    } else {
        add_quiet_move(pstn, moves, s, d, Q_FLAG);
    }
}

void add_pawn_capture_move(POSITION *pstn, int s, int d, MOVE_LIST *moves) {
    if (RANK(d) == FINAL_RANK[pstn->side]) {
        add_capture_move(pstn, moves, s, d, KNIGHT_PROMO);
        add_capture_move(pstn, moves, s, d, BISHOP_PROMO);
        add_capture_move(pstn, moves, s, d, ROOK_PROMO);
        add_capture_move(pstn, moves, s, d, QUEEN_PROMO);
    } else {
        add_capture_move(pstn, moves, s, d, 0);
    }
}

void gen_pawn_step(POSITION *pstn, int pos, MOVE_LIST *moves) {
    int current = pos + PAWN_STEP[pstn->side];

    if (!pstn->board[current]) {
        add_pawn_quiet_move(pstn, pos, current, moves);

        if (
            RANK(pos) == SECOND_RANK[pstn->side] 
            && !(pstn->board[(current += PAWN_STEP[pstn->side])])
        ) {
            add_quiet_move(pstn, moves, pos, current, DPP_FLAG);
        }
    }
}

void gen_pawn_capture(POSITION *pstn, int pos, int vec, MOVE_LIST *moves) {
    int current = pos + vec;
    if ((pstn->board[current] & COLOUR_MASK) == OTHER(pstn->side)) {
        add_pawn_capture_move(pstn, pos, current, moves);
    }
}

void gen_ep_capture(POSITION *pstn, MOVE_LIST *moves) {
    int e_pawn_pos = pstn->ep_sq + PAWN_STEP[OTHER(pstn->side)] + E;
    int e_pawn = pstn->board[e_pawn_pos];

    if ((e_pawn & pstn->side) && (e_pawn & PAWN)) {
        add_ep_capture_move(pstn, moves, e_pawn_pos);
    }
    
    int w_pawn_pos = pstn->ep_sq + PAWN_STEP[OTHER(pstn->side)] + W;
    int w_pawn = pstn->board[w_pawn_pos];

    if ((w_pawn & pstn->side) && (w_pawn & PAWN)) {
        add_ep_capture_move(pstn, moves, w_pawn_pos);
    }
}

void gen_step(POSITION *pstn, int pos, int vec, MOVE_LIST *moves) {
    int current = pos + vec, sq = pstn->board[current];

    if (DIFF_COLOUR(sq & COLOUR_MASK, pstn->side)) {
        add_capture_move(pstn, moves, pos, current, 0);
    } else if (!sq) {
        add_quiet_move(pstn, moves, pos, current, Q_FLAG);
    }
}

void gen_slider(POSITION *pstn, int pos, int vec, MOVE_LIST *moves) {
    int current = pos;

    for (;;) {
        current += vec;
        int colour = pstn->board[current] & COLOUR_MASK;

        if (colour & pstn->side) {
            break;
        } else if (DIFF_COLOUR(colour, pstn->side)) {
            add_capture_move(pstn, moves, pos, current, 0);
            break;
        } else {
            add_quiet_move(pstn, moves, pos, current, Q_FLAG);
        }
    }
}

void gen_moves_from_position(POSITION *pstn, int pos, MOVE_LIST *moves) {
    int p_type = PTYPE(pstn->board[pos]);
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
        gen_pawn_capture(pstn, pos, PAWN_STEP[pstn->side] + E, moves);
        gen_pawn_capture(pstn, pos, PAWN_STEP[pstn->side] + W, moves);
        return;
    }

    for (int i = 0; i < N_VECS[PINDEX(p_type)]; i++) {
        gen(pstn, pos, VECS(p_type)[i], moves);
    }
}

void gen_capture(POSITION *pstn, int pos, int enemy_pos, MOVE_LIST *moves) {
    int piece = pstn->board[pos];
    if (!piece || !pstn->board[enemy_pos]) {
        return;
    }

    if (piece & PAWN) {
        if (
            enemy_pos == (pos + PAWN_STEP[pstn->side] + E) 
            || enemy_pos == (pos + PAWN_STEP[pstn->side] + W)
        ) {
            add_pawn_capture_move(pstn, pos, enemy_pos, moves);
        }
        return;
    }

    int alignment = ALIGNMENT(pos, enemy_pos);
    if (alignment & piece) {
        add_capture_move(pstn, moves, pos, enemy_pos, 0);
    } else if ((alignment >> 8) & piece) {
        int step = STEP(pos, enemy_pos);
        int current = pos + step;

        while (current != enemy_pos) {
            if (pstn->board[current]) {
                break;
            }
            current += step;
        }
        if (current == enemy_pos) {
            add_capture_move(pstn, moves, pos, enemy_pos, 0);
        }
    }
}

void gen_moves_in_check(POSITION *pstn, int pos, MOVE_LIST *moves) {
    int k_pos = PLIST(pstn)[0];

    // attempt to capture the checker
    gen_capture(pstn, pos, pstn->fst_checker, moves);

    // generate moves which might block the checker
    int k_step = STEP(k_pos, pstn->fst_checker);
    MOVE_LIST blocking_moves;
    blocking_moves.index = 0;
    memset(blocking_moves.moves, 0, MOVE_LIST_SIZE * sizeof(int));
    gen_moves_from_position(pstn, pos, &blocking_moves);

    // check if each move actually blocks the checker
    for (int i = 0; i < blocking_moves.index; i++) {
        move_t mv = blocking_moves.moves[i];

        if (STEP(k_pos, mv.dest) == k_step) {
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
}

bool find_pinned_piece(POSITION *pstn, PIN_INFO *p_info) {
    bool possible_pin = false;
    int k_pos = PLIST(pstn)[0];
    int current = k_pos;

    for (;;) {
        current += p_info->vec;
        int sq = pstn->board[current];
        int colour = sq & COLOUR_MASK;

        if (colour == G) {
            return false;
        } else if (colour & pstn->side) {
            if (possible_pin) {
                return false;
            }
            possible_pin = true;
            p_info->pinned = current;
        } else if (colour & OTHER(pstn->side)) {
            if (possible_pin && ((ALIGNMENT(current, k_pos) >> 8) & sq)) {
                p_info->pinning = current;
                return true;
            }
            return false;
        }
    }
}

void gen_pinned_moves(POSITION *pstn, PIN_INFO *p_info, MOVE_LIST *moves) {
    if (p_info->piece & PAWN) {
        if (p_info->vec == N || p_info->vec == S) {
            gen_pawn_step(pstn, p_info->pinned, moves);
        } else if (
            p_info->vec == (PAWN_STEP[pstn->side] + E) 
            || p_info->vec == (PAWN_STEP[pstn->side] + W)
        ) {
            gen_pawn_capture(
                pstn, p_info->pinned, p_info->vec, moves
            );
        } else if (
            p_info->vec == (PAWN_STEP[OTHER(pstn->side)] + E) 
            || p_info->vec == (PAWN_STEP[OTHER(pstn->side)] + W)
        ) {
            gen_pawn_capture(pstn, p_info->pinned, -p_info->vec, moves);
        }
    } else if (
        p_info->piece & (BISHOP | ROOK | QUEEN)
        && (
            ALIGNMENT(p_info->pinned, p_info->pinned + p_info->vec) 
            & p_info->piece
        )
    ) {
        gen_slider(pstn, p_info->pinned, p_info->vec, moves);
        gen_slider(pstn, p_info->pinned, -p_info->vec, moves);
    }
}

void gen_pinned_captures(POSITION *pstn, PIN_INFO *p_info, MOVE_LIST *moves) {
    if (p_info->pinned & PAWN) {
        if (
            p_info->vec == (PAWN_STEP[pstn->side] + E) 
            || p_info->vec == (PAWN_STEP[pstn->side] + W)
        ) {
            gen_pawn_capture(pstn, p_info->pinned, p_info->vec, moves);
        } else if (
            p_info->vec == (PAWN_STEP[OTHER(pstn->side)] + E) 
            || p_info->vec == (PAWN_STEP[OTHER(pstn->side)] + W)
        ) {
            gen_pawn_capture(pstn, p_info->pinned, -p_info->vec, moves);
        }
        return;
    }
    
    int alignment = ALIGNMENT(p_info->pinned, p_info->pinning);
    if (alignment & p_info->pinned || ((alignment >> 8) & p_info->pinned)) {
        add_capture_move(pstn, moves, p_info->pinned, p_info->pinning, 0);
    }
}

void gen_pinned(
    POSITION *pstn, MOVE_LIST *moves, GEN_PINNED gen, int *tmp_removed
) {
    PIN_INFO p_info;

    for (int i = 0; i < 8; i++) {
        p_info.vec = VECS(KING)[i]; 

        if (find_pinned_piece(pstn, &p_info)) {
            p_info.piece = pstn->board[p_info.pinned];

            // temporarily remove pinned piece from piece list
            int p_list_index = PLIST_INDEX(p_info.piece);
            PLIST(pstn)[p_list_index] = 0;
            tmp_removed[p_list_index] = p_info.pinned;

            if (pstn->check) {
                continue;
            }

            gen(pstn, &p_info, moves);
        }
    }
}

void all_moves(POSITION *pstn, MOVE_LIST *moves) {
    moves->index = 0;
    memset(moves->moves, 0, MOVE_LIST_SIZE * sizeof(int));

    int temp_removed[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    gen_pinned(pstn, moves, gen_pinned_moves, temp_removed);

    // generate king moves
    gen_moves_from_position(pstn, PLIST(pstn)[0], moves);

    if (pstn->check < DOUBLE_CHECK) {
        GEN_FROM_POSITION gen;

        if (pstn->check == CONTACT_CHECK || pstn->check == DISTANT_CHECK) {
            gen = gen_moves_in_check;
        } else {
            gen = gen_moves_from_position;

            if (pstn->c_rights & KINGSIDE_RIGHTS[pstn->side]) {
                int start = K_KING_MOVES[pstn->side][0];
                int dest = K_KING_MOVES[pstn->side][1];
                if (!(pstn->board[start + E] | pstn->board[start + E + E])) {
                    add_quiet_move(pstn, moves, start, dest, K_CASTLE_FLAG);
                }
            } 
            
            if (pstn->c_rights & QUEENSIDE_RIGHTS[pstn->side]) {
                int start = Q_KING_MOVES[pstn->side][0];
                int dest = Q_KING_MOVES[pstn->side][1];
                if (
                    !(pstn->board[start + W]
                    | pstn->board[start + W + W]
                    | pstn->board[start + W + W + W])
                ) {
                    add_quiet_move(pstn, moves, start, dest, Q_CASTLE_FLAG);
                }
            }
        }

        for (int i = 1; i < 16; i++) {
            gen(pstn, PLIST(pstn)[i], moves);
        }

        if (pstn->ep_sq) {
            gen_ep_capture(pstn, moves);
        }
    }

    for (int i = 1; i < 16; i++) {
        if (temp_removed[i]) {
            PLIST(pstn)[i] = temp_removed[i];
        }
    }
}

void all_captures(POSITION *pstn, MOVE_LIST *moves) {
    moves->index = 0;
    memset(moves->moves, 0, MOVE_LIST_SIZE * sizeof(int));

    int temp_removed[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    gen_pinned(pstn, moves, gen_pinned_captures, temp_removed);

    // generate king captures
    for (int i = 0; i < 16; i++) {
        gen_capture(pstn, PLIST(pstn)[0], OTHER_PLIST(pstn)[i], moves);
    }

    if (pstn->check < DOUBLE_CHECK) {
        if (pstn->check == CONTACT_CHECK || pstn->check == DISTANT_CHECK) {
            for (int i = 1; i < 16; i++) {
                gen_capture(pstn, PLIST(pstn)[i], pstn->fst_checker, moves);
            }
        } else {
            for (int i = 1; i < 16; i++) {
                for (int j = 1; j < 16; j++) {
                    gen_capture(
                        pstn, PLIST(pstn)[i], OTHER_PLIST(pstn)[j], moves
                    );
                }
            }
        }

        if (pstn->ep_sq) {
            gen_ep_capture(pstn, moves);
        }
    }

    for (int i = 1; i < 16; i++) {
        if (temp_removed[i]) {
            PLIST(pstn)[i] = temp_removed[i];
        }
    }
}

bool move_exists(POSITION *pstn, move_t mv) {
    MOVE_LIST moves;
    all_moves(pstn, &moves);

    for (int i = 0; i < moves.index; i++) {
        move_t m = moves.moves[i];

        if (make_move(pstn, m)) {
            unmake_move(pstn, m);
            if (moves_equal(m, mv)) {
                return true;
            };
        }
    }

    return false;
}