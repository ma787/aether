#include <stdio.h>
#include <stdlib.h>
#include "perft.h"
#include "position.h"
#include "move_gen.h"


int perft(int depth) {
    if (depth == 0) {
        return 1;
    }
    int total = 0;
    move_list *moves = malloc(sizeof(move_list));
    moves->index = 0;
    all_moves(moves);

    for (int i = 0; i < moves->index; i++) {
        move_t mv = moves->moves[i];
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

    int total = 0;
    move_list *moves = malloc(sizeof(move_list));
    moves->index = 0;
    all_moves(moves);

    for (int i = 0; i < moves->index; i++) {
        move_t mv = moves->moves[i];
        char mstr[6];
        move_to_string(mv, mstr);
        if (make_move(mv) == 0) {
            int n = perft(depth - 1);
            total += n;
            printf("%s %d\n", mstr, n);
        }
        unmake_move(mv);
    }

    free(moves);
    printf("\n%d\n", total);
}