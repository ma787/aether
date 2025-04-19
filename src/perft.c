#include <stdio.h>
#include <stdlib.h>
#include "aether.h"

uint64_t perft(POSITION *pstn, int depth) {
    if (depth == 0) {
        return 1;
    }
    uint64_t total = 0UL;

    MOVE_LIST *moves = malloc(sizeof(MOVE_LIST));
    moves->index = 0;
    all_moves(pstn, moves);

    move_t mv;

    for (int i = 0; i < moves->index; i++) {
        if (make_move(pstn, (mv = moves->moves[i])) == 0) {
            total += perft(pstn, depth - 1);
        }
        unmake_move(pstn, mv);
    }

    free(moves);
    return total;
}

void divide(POSITION *pstn, int depth) {
    if (depth < 1) {
        return;
    }

    uint64_t total = 0;
    MOVE_LIST *moves = malloc(sizeof(MOVE_LIST));
    moves->index = 0;
    all_moves(pstn, moves);

    move_t mv;

    for (int i = 0; i < moves->index; i++) {
        mv = moves->moves[i];
        char mstr[6];
        move_to_string(mv, mstr);
        if (make_move(pstn, mv) == 0) {
            uint64_t n = perft(pstn, depth - 1);
            total += n;
            printf("%s %lu\n", mstr, n);
        }
        unmake_move(pstn, mv);
    }

    free(moves);
    printf("\n%lu\n", total);
}

uint64_t count_captures(POSITION *pstn, int depth) {
    if (depth == 0) {
        return 0;
    }
    uint64_t total = 0UL;

    MOVE_LIST *moves = malloc(sizeof(MOVE_LIST));
    moves->index = 0;
    all_moves(pstn, moves);
    move_t mv;

    if (depth == 1) {
        MOVE_LIST *captures = malloc(sizeof(MOVE_LIST));
        captures->index = 0;
        all_captures(pstn, captures);

        for (int i = 0; i < captures->index; i++) {
            if (make_move(pstn, (mv = captures->moves[i])) == 0) {
                total += 1;
            }
            unmake_move(pstn, mv);
        }

        
    } else {
        for (int i = 0; i < moves->index; i++) {
            if (make_move(pstn, (mv = moves->moves[i])) == 0) {
                total += count_captures(pstn, depth - 1);
            }
            unmake_move(pstn, mv);
        }
    }

    free(moves);
    return total;
}
