#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    memset(pv_line, NULL_MOVE, MAX_DEPTH * sizeof(int));
    memset(search_killers, NULL_MOVE, 2 * MAX_DEPTH * sizeof(int));

    memset(search_history[PAWN], 0, H8 * sizeof(int));
    memset(search_history[KNIGHT], 0, H8 * sizeof(int));
    memset(search_history[BISHOP], 0, H8 * sizeof(int));
    memset(search_history[ROOK], 0, H8 * sizeof(int));
    memset(search_history[QUEEN], 0, H8 * sizeof(int));
    memset(search_history[KING], 0, H8 * sizeof(int));
    
    s_info->start_time = get_time();
    s_info->stopped = 0;
    s_info->nodes = 0L;

    s_info->fh = 0;
    s_info->fhf = 0;
}

int make_next_move(MOVE_LIST *moves) {
    MOVE_INFO m_info;
    int best_index = -1;

    for (int i = 0; i < moves->index; i++) {
        m_info = moves->moves[i];
        if (m_info.move != NULL_MOVE) {
            best_index = i;
            break;
        };
    }

    if (best_index == -1) {
        return NULL_MOVE;
    }

    MOVE_INFO new_info;

    for (int j = best_index + 1; j < moves->index; j++) {
        new_info = moves->moves[j];

        if (new_info.move != NULL_MOVE && new_info.score > m_info.score) {
            m_info = new_info;
            best_index = j;
        }
    }

    int best_move = m_info.move;

    if (make_move(best_move) != 0) {
        unmake_move(best_move);
        best_move = -1;
    }

    moves->moves[best_index].move = NULL_MOVE;

    return best_move;
}

int alpha_beta(int alpha, int beta, int depth, SEARCH_INFO *s_info) {
    s_info->nodes++;

    if (depth == 0 || ply > MAX_DEPTH - 1) {
        return evaluate();
    }

    MOVE_LIST *moves = malloc(sizeof(MOVE_LIST));
    moves->index = 0;
    all_moves(moves);

    int best_move = NULL_MOVE;
    int old_alpha = alpha;
    int score = -INFINITY;
    int n = 0;

    while (1) {
        int mv = make_next_move(moves);

        if (mv == NULL_MOVE) {
            break;
        } else if (mv == -1) {
            continue;
        }

        n++;
        score = -alpha_beta(-beta, -alpha, depth - 1, s_info);
        unmake_move(mv);

        if (score > alpha) {
            if (score >= beta) {
                if (n == 1) {
                    s_info->fhf++;
                }
                s_info->fh++;

                if (!(get_flags(mv) & CAPTURE_FLAG)) {
                    search_killers[1][ply] = search_killers[0][ply];
                    search_killers[0][ply] = mv;
                }
                
                free(moves);
                return beta;
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
    int best_move = NULL_MOVE;
    int score = -INFINITY;
    int current_depth = 0, pv_count = 0;
    char mstr[6];

    init_search(s_info);

    for (current_depth = 1; current_depth <= s_info->depth; current_depth++) {
        score = alpha_beta(-INFINITY, INFINITY, current_depth, s_info);
        pv_count = get_pv_line(current_depth);
        best_move = pv_line[0];

        printf("info depth %d score cp %d nodes %li pv", current_depth, score, s_info->nodes);

        for (int i = 0; i < pv_count; i++) {
            move_to_string(pv_line[i], mstr);
            printf(" %s", mstr);
        }
        printf("\n");

        if (current_depth > 1) {
            printf("fhf: %.2f, fh: %.2f, ordering: %.2f\n", s_info->fhf, s_info->fh, (s_info->fhf/s_info->fh));
        }
    }

    move_to_string(best_move, mstr);
    printf("bestmove %s\n", mstr);
}