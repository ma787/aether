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

int alpha_beta(int alpha, int beta, int depth) {
    if (depth == 0) {
        return evaluate();
    }

    move_list *moves = malloc(sizeof(move_list));
    moves->index = 0;
    all_moves(moves);

    int score = -INFINITY;

    for (int i = 0; i < moves->index; i++) {
        int mv = moves->moves[i];
        if (make_move(mv) == 0) {
            int new_score = -alpha_beta(-beta, -alpha, depth - 1);
            if (new_score > score) {
                score = new_score;
            }

            if (score >= beta) {
                unmake_move(mv);
                free(moves);
                return beta;
            }

            if (score > alpha) {
                alpha = score;
            }
        }
        unmake_move(mv);
    }

    free(moves);
    return score;
}

void search(int depth) {
    int best_move = NULL_MOVE;
    int score = -INFINITY;
    char mstr[6];

    move_list *moves = malloc(sizeof(move_list));
    moves->index = 0;
    all_moves(moves);

    for (int i = 0; i < moves->index; i++) {
        int mv = moves->moves[i];
        int new_score = score;
        if (make_move(mv) == 0) {
            new_score = -alpha_beta(-100000, 100000, depth - 1);
        }
        unmake_move(mv);
        if (new_score > score) {
            score = new_score;
            best_move = mv;
        }
    }

    move_to_string(best_move, mstr);
    printf("bestmove %s\n", mstr);

    free(moves);
}