#include <stdio.h>
#include <stdlib.h>
#include "aether.h"

uint64_t perft(int depth) {
    if (depth == 0) {
        return 1;
    }
    uint64_t total = 0UL;

    move_list *moves = malloc(sizeof(move_list));
    moves->index = 0;
    all_moves(moves);

    for (int i = 0; i < moves->index; i++) {
        int mv = moves->moves[i];
        if (make_move(mv) == 0) {
            total += perft(depth - 1);
        }
        unmake_move(mv);
    }

    free(moves);
    return total;
}

void divide(int depth) {
    if (depth < 1) {
        return;
    }

    uint64_t total = 0;
    move_list *moves = malloc(sizeof(move_list));
    moves->index = 0;
    all_moves(moves);

    for (int i = 0; i < moves->index; i++) {
        int mv = moves->moves[i];
        char mstr[6];
        move_to_string(mv, mstr);
        if (make_move(mv) == 0) {
            uint64_t n = perft(depth - 1);
            total += n;
            printf("%s %lu\n", mstr, n);
        }
        unmake_move(mv);
    }

    free(moves);
    printf("\n%lu\n", total);
}