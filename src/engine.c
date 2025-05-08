#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "aether.h"

int evaluate(POSITION *pstn) {
    int w_score = pstn->material[WHITE] + pstn->pcsq_sum[WHITE];
    int b_score = pstn->material[BLACK] + pstn->pcsq_sum[BLACK];
    int score = w_score - b_score;

    if (pstn->side == BLACK) {
        score *= -1;
    }

    return score;
}

void init_search(POSITION *pstn, SEARCH_INFO *s_info) {
    for (int i = 0; i < MAX_DEPTH; i++) {
        pstn->pv_line[i] = NULL_MOVE;
    }

    memset(pstn->search_history[PAWN], 0, H8 * sizeof(int));
    memset(pstn->search_history[KNIGHT], 0, H8 * sizeof(int));
    memset(pstn->search_history[BISHOP], 0, H8 * sizeof(int));
    memset(pstn->search_history[ROOK], 0, H8 * sizeof(int));
    memset(pstn->search_history[QUEEN], 0, H8 * sizeof(int));
    memset(pstn->search_history[KING], 0, H8 * sizeof(int));

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

    if (pstn->ply > MAX_DEPTH - 1) {
        return evaluate(pstn);
    }

    MOVE_LIST *moves;

    if (!pstn->check) {
        int stand_pat = evaluate(pstn);

        if (stand_pat >= beta) {
            return beta;
        }

        if (stand_pat > alpha) {
            alpha = stand_pat;
        }

        moves = malloc(sizeof(MOVE_LIST));
        moves->index = 0;
        all_captures(pstn, moves);
    } else {
        moves = malloc(sizeof(MOVE_LIST));
        moves->index = 0;
        all_moves(pstn, moves);
    }

    move_t best_move = NULL_MOVE;
    int old_alpha = alpha;
    int score = -INFINITY;
    move_t pv_move = get_pv_move(pstn);

    if (!is_null_move(pv_move)) {
        for (int i = 0; i < moves->index; i++) {
            if (moves_equal(moves->moves[i], pv_move)) {
                moves->moves[i].score = CAP_VALUE * 2;
                break;
            }
        }
    }

    while (1) {
        move_t mv;
        if (!make_next_move(pstn, moves, &mv)) {
            if (is_null_move(mv)) {
                break;
            }
            continue;
        }

        score = -quiescence(pstn, -beta, -alpha, s_info);
        unmake_move(pstn, mv);

        if (s_info->stopped == true) {
            return 0;
        }

        if (score > alpha) {
            if (score >= beta) {
                free(moves);
                return beta;
            }

            alpha = score;
            best_move = mv;
        }
    }

    free(moves);

    if (alpha != old_alpha) {
        store_move(pstn, best_move);
    }

    return alpha;
}

int alpha_beta(POSITION *pstn, int alpha, int beta, int depth, SEARCH_INFO *s_info) {
    if (depth == 0) {
        return quiescence(pstn, alpha, beta, s_info);
    }

    if ((s_info->nodes & 2047) == 0) {
        check_status(s_info);
    }

    s_info->nodes++;

    // assign draw score to repetitions
    if (is_repetition(pstn) || pstn->h_clk >= 100) {
        return 0;
    }

    if (pstn->check) {
        depth++;
    }

    MOVE_LIST *moves = malloc(sizeof(MOVE_LIST));
    moves->index = 0;
    all_moves(pstn, moves);

    move_t best_move = NULL_MOVE;
    int old_alpha = alpha;
    int score = -INFINITY;
    int n = 0;
    move_t pv_move = get_pv_move(pstn);

    if (!(is_null_move(pv_move))) {
        for (int i = 0; i < moves->index; i++) {
            if (moves_equal(moves->moves[i], pv_move)) {
                moves->moves[i].score = CAP_VALUE * 2;
                break;
            }
        }
    }

    while (1) {
        move_t mv;
        if (!make_next_move(pstn, moves, &mv)) {
            if (is_null_move(mv)) {
                break;
            }
            continue;
        }

        n++;
        score = -alpha_beta(pstn, -beta, -alpha, depth - 1, s_info);
        unmake_move(pstn, mv);

        if (s_info->stopped == true) {
            return 0;
        }

        if (score > alpha) {
            if (score >= beta) {
                if (!(mv.flags & CAPTURE_FLAG)) {
                    pstn->search_killers[1][pstn->ply] = pstn->search_killers[0][pstn->ply];
                    pstn->search_killers[0][pstn->ply] = mv;
                }
                
                free(moves);
                return beta;
            }

            if (!(mv.flags & CAPTURE_FLAG)) {
                pstn->search_history[get_piece_type(pstn->board[mv.start])][mv.dest] += depth;
            }

            alpha = score;
            best_move = mv;
        }
    }

    free(moves);

    // end of game - check for mate
    if (n == 0) {
        if (is_square_attacked(pstn, pstn->w_pieces[0])) {
            return -MATE + pstn->ply;
        } else {
            return 0;
        }
    }

    if (alpha != old_alpha) {
        store_move(pstn, best_move);
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
        score = alpha_beta(pstn, -INFINITY, INFINITY, current_depth, s_info);

        if (s_info->stopped == true) {
            break;
        }

        pv_count = get_pv_line(pstn, current_depth);
        best_move = pstn->pv_line[0];

        printf(
            "info depth %d score cp %d nodes %lu time %lu pv", 
            current_depth, score, s_info->nodes, (get_time() - s_info->start_time)
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