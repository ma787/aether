#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "aether.h"

int evaluate(void) {
    int score = 0, i = 0x44;

    while (i < 0xBC) {
        int sq = board[i];
        int p_type = sq & 0xFC;

        switch(sq & COLOUR_MASK) {
            case WHITE:
                score += PIECE_VALS[p_type] + EVAL_TABLES[p_type][to_index(i++)];
                break;
            case BLACK:
                score -= (
                    PIECE_VALS[p_type] 
                    + EVAL_TABLES[p_type][to_index(flip_square(i++))]
                );
                break;
            case G:
                i += 8;
                break;
            case 0:
                i++;
        }
    }

    if (side == BLACK) {
        score *= -1;
    }

    return score;
}

void init_search(SEARCH_INFO *s_info) {
    for (int i = 0; i < MAX_DEPTH; i++) {
        pv_line[i] = NULL_MOVE;
    }

    memset(search_history[PAWN], 0, H8 * sizeof(int));
    memset(search_history[KNIGHT], 0, H8 * sizeof(int));
    memset(search_history[BISHOP], 0, H8 * sizeof(int));
    memset(search_history[ROOK], 0, H8 * sizeof(int));
    memset(search_history[QUEEN], 0, H8 * sizeof(int));
    memset(search_history[KING], 0, H8 * sizeof(int));

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

int make_next_move(MOVE_LIST *moves, move_t *move_to_return) {
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
        return 0;
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

    if (make_move(best_move) != 0) {
        return 1;
    }

    return 0;
}

int quiescence(int alpha, int beta, SEARCH_INFO *s_info) {
    if ((s_info->nodes & 2047) == 0) {
        check_status(s_info);
    }

    s_info->nodes++;

    if (ply > MAX_DEPTH - 1) {
        return evaluate();
    }

    MOVE_LIST *moves;

    if (!check_info) {
        int stand_pat = evaluate();

        if (stand_pat >= beta) {
            return beta;
        }

        if (stand_pat > alpha) {
            alpha = stand_pat;
        }

        moves = malloc(sizeof(MOVE_LIST));
        moves->index = 0;
        all_captures(moves);
    } else {
        moves = malloc(sizeof(MOVE_LIST));
        moves->index = 0;
        all_moves(moves);
    }

    move_t best_move = NULL_MOVE;
    int old_alpha = alpha;
    int score = -INFINITY;
    move_t pv_move = get_pv_move();

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
        int res = make_next_move(moves, &mv);

        if (res != 0) {
            unmake_move(mv);
            continue;
        } else if (is_null_move(mv)) {
            break;
        }

        score = -quiescence(-beta, -alpha, s_info);
        unmake_move(mv);

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
        store_move(best_move);
    }

    return alpha;
}

int alpha_beta(int alpha, int beta, int depth, SEARCH_INFO *s_info) {
    if (depth == 0) {
        return quiescence(alpha, beta, s_info);
    }

    if ((s_info->nodes & 2047) == 0) {
        check_status(s_info);
    }

    s_info->nodes++;

    // assign draw score to repetitions
    if (is_repetition() || h_clk >= 100) {
        return 0;
    }

    MOVE_LIST *moves = malloc(sizeof(MOVE_LIST));
    moves->index = 0;
    all_moves(moves);

    move_t best_move = NULL_MOVE;
    int old_alpha = alpha;
    int score = -INFINITY;
    int n = 0;
    move_t pv_move = get_pv_move();

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
        int res = make_next_move(moves, &mv);

        if (res != 0) {
            unmake_move(mv);
            continue;
        } else if (is_null_move(mv)) {
            break;
        }

        n++;
        score = -alpha_beta(-beta, -alpha, depth - 1, s_info);
        unmake_move(mv);

        if (s_info->stopped == true) {
            return 0;
        }

        if (score > alpha) {
            if (score >= beta) {
                if (!(mv.flags & CAPTURE_FLAG)) {
                    search_killers[1][ply] = search_killers[0][ply];
                    search_killers[0][ply] = mv;
                }
                
                free(moves);
                return beta;
            }

            if (!(mv.flags & CAPTURE_FLAG)) {
                search_history[board[mv.start] & 0xFC][mv.dest] += depth;
            }

            alpha = score;
            best_move = mv;
        }
    }

    free(moves);

    // end of game - check for mate
    if (n == 0) {
        if (is_square_attacked(w_pieces[0])) {
            return -MATE + ply;
        } else {
            return 0;
        }
    }

    if (alpha != old_alpha) {
        store_move(best_move);
    }

    return alpha;
}

void search(SEARCH_INFO *s_info) {
    move_t best_move = NULL_MOVE;
    int score = -INFINITY;
    int current_depth = 0, pv_count = 0;
    char mstr[6];

    init_search(s_info);

    for (current_depth = 1; current_depth <= s_info->depth; current_depth++) {
        score = alpha_beta(-INFINITY, INFINITY, current_depth, s_info);

        if (s_info->stopped == true) {
            break;
        }

        pv_count = get_pv_line(current_depth);
        best_move = pv_line[0];

        printf(
            "info depth %d score cp %d nodes %lu time %lu pv", 
            current_depth, score, s_info->nodes, (get_time() - s_info->start_time)
        );

        for (int i = 0; i < pv_count; i++) {
            move_to_string(pv_line[i], mstr);
            printf(" %s", mstr);
        }
        printf("\n");
        fflush(stdout);
        s_info->found_move = true;
    }

    move_to_string(best_move, mstr);
    printf("bestmove %s\n", mstr);
}