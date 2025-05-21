#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "aether.h"

int evaluate(POSITION *pstn) {
    int phase;
    int start_scores[3] = {0, 0, 0};
    int end_scores[3] = {0, 0, 0};

    // evaluate white pieces
    for (int i = 0; i < 16; i++) {
        int pos = pstn->piece_list[i];
        if (pos) {
            int p_index = PLIST_INDEX(pstn->board[pos]);
            start_scores[WHITE] += START_TABLES[p_index][SQ64(FLIP256(pos))];
            end_scores[WHITE] += END_TABLES[p_index][SQ64(FLIP256(pos))];
            phase += PHASES[PINDEX(pstn->board[pos])];
        }
    }

    // evaluate black pieces
    for (int i = 16; i < 32; i++) {
        int pos = pstn->piece_list[i];
        if (pos) {
            int p_index = PLIST_INDEX(pstn->board[pos]);
            start_scores[BLACK] += START_TABLES[p_index][SQ64(pos)];
            end_scores[BLACK] += END_TABLES[p_index][SQ64(pos)];
            phase += PHASES[PINDEX(pstn->board[pos])];
        }
    }

    int start_score = start_scores[pstn->side] - start_scores[OTHER(pstn->side)];
    int end_score = end_scores[pstn->side] - end_scores[OTHER(pstn->side)];

    int start_phase = phase;
    if (start_phase > 24) {
        start_phase = 24;
    }
    int end_phase = 24 - start_phase;

    return (start_score * start_phase + end_score * end_phase) / 24;
}

void init_search(POSITION *pstn, SEARCH_INFO *s_info) {
    pstn->s_ply = 0;

    for (int i = 0; i < MAX_DEPTH; i++) {
        pstn->pv_line[i] = NULL_MOVE;
    }

    (pstn->hash_table)->hit = 0;
    (pstn->hash_table)->cut = 0;
    (pstn->hash_table)->over_writes = 0;

    for (int i = 0; i < 12; i++) {
        memset(pstn->search_history[i], 0, 256 * sizeof(int));
    }

    s_info->stopped = false;
    s_info->found_move = false;
    s_info->nodes = 0UL;
}

void read_stdin(SEARCH_INFO *s_info) {
    int bytes;
    char buf[256];
    char *end;

    if (input_waiting()) {
        s_info->stopped = true;

        do {
            bytes = read(STDIN_FILENO, buf, 256);
        } while (bytes < 0);

        end = strchr(buf, '\n');
        if (end != NULL) {
            *end = 0;
        }

        if (strlen(buf) > 0 && strcmp(buf, "quit") == 0) {
            s_info->quit = true;
        }
    }
}

void check_status(SEARCH_INFO *s_info) {
    if ((s_info->time_set == true) && (get_time() > s_info->stop_time) && s_info->found_move) {
        s_info->stopped = true;
    }

    read_stdin(s_info);
}

bool make_next_move(POSITION *pstn, MOVE_LIST *moves, move_t *move_to_return) {
    move_t best_move;
    int m_index = 0;

    while (m_index < moves->index) {
        best_move = moves->moves[m_index];
        if (!is_null_move(best_move)) {
            break;
        }
        m_index++;
    }

    if (m_index == moves->index) {
        *move_to_return = NULL_MOVE;
        return false;
    }

    move_t current_move;
    int best_move_index = m_index;

    while (m_index < moves->index) {
        current_move = moves->moves[m_index];

        if (!(is_null_move(current_move)) && current_move.score > best_move.score) {
            best_move = current_move;
            best_move_index = m_index;
        }
        m_index++;
    }

    *move_to_return = best_move;
    moves->moves[best_move_index] = NULL_MOVE;

    return make_move(pstn, best_move);
}

int quiescence(POSITION *pstn, int alpha, int beta, SEARCH_INFO *s_info) {
    if ((s_info->nodes & 2047) == 0) {
        check_status(s_info);
    }

    s_info->nodes++;

    if (pstn->s_ply > MAX_DEPTH - 1) {
        return evaluate(pstn);
    }

    MOVE_LIST moves;
    int best_score;

    if (pstn->check) {
        all_moves(pstn, &moves);
        best_score = alpha;
    } else {
        best_score = evaluate(pstn); // score from 'standing pat'
        
        if (best_score >= beta) {
            return beta;
        }
        if (best_score > alpha) {
            alpha = best_score;
        }

        all_captures(pstn, &moves);
    }

    move_t best_move = NULL_MOVE;
    int score = -INFINITY;
    int old_alpha = alpha;
    int n = 0;

    while (1) {
        move_t mv;
        if (!make_next_move(pstn, &moves, &mv)) {
            if (is_null_move(mv)) {
                break;
            }
            continue;
        }

        n++;
        score = -quiescence(pstn, -beta, -alpha, s_info);
        unmake_move(pstn, mv);

        if (s_info->stopped == true) {
            return 0;
        }

        if (score > alpha) {
            if (score >= beta) {
                return beta;
            }

            alpha = score;
            best_move = mv;
        }
        if (score > best_score) {
            best_score = score;
        }
    }

    if (!n) {
        if (pstn->check) {
            return -MATE + pstn->s_ply; // checkmate
        } else {
            return best_score; // no captures available at this position
        }
    }

    return best_score;
}

int alpha_beta(POSITION *pstn, int alpha, int beta, int depth, SEARCH_INFO *s_info, bool make_null) {
    if (depth == 0) {
        return quiescence(pstn, alpha, beta, s_info);
    }

    if ((s_info->nodes & 2047) == 0) {
        check_status(s_info);
    }

    s_info->nodes++;

    // assign draw score to repetitions encountered during search
    if ((is_repetition(pstn) || pstn->h_clk >= 100) && pstn->s_ply) {
        return 0;
    }

    if (pstn->check) {
        depth++;
    } else if (
        make_null
        && pstn->s_ply
        && depth >= 4
        && pstn->big_pieces[WHITE] > 1  // probably not zugzwang
    ) {
        make_null_move(pstn);
        int score = -alpha_beta(pstn, -beta, -beta + 1, depth - 4, s_info, false);
        unmake_null_move(pstn);

        if (s_info->stopped == true) {
            return 0;
        }

        if (score >= beta) {
            return beta;
        }
    }

    MOVE_LIST moves;
    all_moves(pstn, &moves);

    move_t best_move = NULL_MOVE;
    int best_score = -INFINITY;
    move_t pv_move = NULL_MOVE;

    if (get_entry_info(pstn, &pv_move, &best_score, alpha, beta, depth)) {
        (pstn->hash_table)->cut++;
        return best_score;
    }

    if (!(is_null_move(pv_move))) {
        for (int i = 0; i < moves.index; i++) {
            if (moves_equal(moves.moves[i], pv_move)) {
                moves.moves[i].score = CAP_VALUE * 2;
                break;
            }
        }
    }

    int old_alpha = alpha;
    int n = 0;
    best_score = -INFINITY;

    while (1) {
        move_t mv;
        if (!make_next_move(pstn, &moves, &mv)) {
            if (is_null_move(mv)) {
                break;
            }
            continue;
        }

        n++;
        int score = -alpha_beta(pstn, -beta, -alpha, depth - 1, s_info, true);
        unmake_move(pstn, mv);

        if (s_info->stopped == true) {
            return 0;
        }

        if (score > best_score) {
            best_score = score;
            best_move = mv;

            if (score > alpha) {
                if (score >= beta) {
                    if (!(mv.flags & CAPTURE_FLAG)) {
                        pstn->search_killers[1][pstn->s_ply] = pstn->search_killers[0][pstn->s_ply];
                        pstn->search_killers[0][pstn->s_ply] = mv;
                    }

                    store_entry(pstn, best_move, score, depth, CUT);
                    return beta;
                }

                if (!(mv.flags & CAPTURE_FLAG)) {
                    HISTORY(pstn, mv.start, mv.dest) += depth;
                }

                alpha = score;
            }
        }
    }

    // end of game - check for mate
    if (!n) {
        if (pstn->check) {
            return -MATE + pstn->s_ply;
        } else {
            return 0;
        }
    }

    if (alpha != old_alpha) {
        store_entry(pstn, best_move, best_score, depth, PV);
    } else {
        store_entry(pstn, best_move, old_alpha, depth, ALL);
    }

    return alpha;
}

void search(POSITION *pstn, SEARCH_INFO *s_info) {
    move_t best_move = NULL_MOVE;
    int score = -INFINITY;
    int current_depth = 0, pv_count = 0;
    char mstr[6];

    init_search(pstn, s_info);

    for (current_depth = 1; current_depth <= s_info->depth; current_depth++) {
        score = alpha_beta(pstn, -INFINITY, INFINITY, current_depth, s_info, true);

        if (s_info->stopped == true) {
            break;
        }

        pv_count = get_pv_line(pstn, current_depth);
        best_move = pstn->pv_line[0];

        printf(
            "info depth %d score cp %d nodes %lu tbhits %d time %lu pv", 
            current_depth, score, s_info->nodes, (pstn->hash_table)->hit, (get_time() - s_info->start_time)
        );

        for (int i = 0; i < pv_count; i++) {
            move_to_string(pstn->pv_line[i], mstr);
            printf(" %s", mstr);
        }
        printf("\n");
        fflush(stdout);
        s_info->found_move = true;
    }

    move_to_string(best_move, mstr);
    printf("bestmove %s\n", mstr);
}