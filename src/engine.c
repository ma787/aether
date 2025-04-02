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
    
    s_info->start_time = get_time();
    s_info->stopped = 0;
    s_info->nodes = 0L;
}

int alpha_beta(int alpha, int beta, int depth, SEARCH_INFO *s_info) {
    s_info->nodes++;

    if (depth == 0 || ply > MAX_DEPTH - 1) {
        return evaluate();
    }

    move_list *moves = malloc(sizeof(move_list));
    moves->index = 0;
    all_moves(moves);

    int best_move = NULL_MOVE;
    int old_alpha = alpha;
    int score = -INFINITY;
    int n = 0;

    for (int i = 0; i < moves->index; i++) {
        int mv = moves->moves[i];
        if (make_move(mv) == 0) {
            n++;
            score = -alpha_beta(-beta, -alpha, depth - 1, s_info);

            if (score > alpha) {
                if (score >= beta) {
                    unmake_move(mv);
                    free(moves);
                    return beta;
                }

                alpha = score;
                best_move = mv;
            }
        }
        unmake_move(mv);
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

    return score;
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
    }

    move_to_string(best_move, mstr);
    printf("bestmove %s\n", mstr);
}