#include <stdio.h>
#include <stdlib.h>
#include "aether.h"

uint64_t perft(int depth) {
    if (depth == 0) {
        return 1;
    }
    uint64_t total = 0UL;

    MOVE_LIST *moves = malloc(sizeof(MOVE_LIST));
    moves->index = 0;
    all_moves(moves);

    for (int i = 0; i < moves->index; i++) {
        if (make_move(moves->moves[i].move) == 0) {
            total += perft(depth - 1);
        }
        unmake_move();
    }

    free(moves);
    return total;
}

void divide(int depth) {
    if (depth < 1) {
        return;
    }

    uint64_t total = 0;
    MOVE_LIST *moves = malloc(sizeof(MOVE_LIST));
    moves->index = 0;
    all_moves(moves);

    for (int i = 0; i < moves->index; i++) {
        int mv = moves->moves[i].move;
        char mstr[6];
        move_to_string(mv, mstr);
        if (make_move(mv) == 0) {
            uint64_t n = perft(depth - 1);
            total += n;
            printf("%s %lu\n", mstr, n);
        }
        unmake_move();
    }

    free(moves);
    printf("\n%lu\n", total);
}