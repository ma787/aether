#include <stdlib.h>
#include <string.h>
#include "engine_vals.h"
#include "utils.h"
#include "position.h"
#include "move_gen.h"

int evaluate() {
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

int negamax(int depth) {
    if (depth == 0) {
        return evaluate();
    }

    move_list *moves = malloc(sizeof(move_list));
    moves->index = 0;
    all_moves(moves);

    int score = -100000;

    for (int i = 0; i < moves->index; i++) {
        move_t mv = moves->moves[i];
        if (make_move(mv) == 0) {
            int new_score = -negamax(depth - 1);
            if (new_score > score) {
                score = new_score;
            }
        }
        unmake_move(mv);
    }

    free(moves);
    return score;
}

void search(int depth, char *best_move) {
    best_move[0] = '\0';

    if (depth == 0) {
        return;
    }

    move_list *moves = malloc(sizeof(move_list));
    moves->index = 0;
    all_moves(moves);

    int score = -100000;

    for (int i = 0; i < moves->index; i++) {
        move_t mv = moves->moves[i];
        int new_score = score;
        if (make_move(mv) == 0) {
            new_score = -negamax(depth - 1);
        }
        unmake_move(mv);
        if (new_score > score) {
            score = new_score;
            best_move[0] = '\0';
            move_to_string(mv, best_move);
        }
    }

    free(moves);
}