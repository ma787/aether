#include "perft.h"
#include <stdio.h>
#include <stdlib.h>

int perft(info *pstn, int depth) {
    if (depth == 0) {
        return 1;
    }
    int total = 0;
    move_list *moves = malloc(sizeof(move_list));
    moves->index = 0;
    all_moves(pstn, moves);

    for (int i = 0; i < moves->index; i++) {
        move_t mv = moves->moves[i];
        if (make_move(pstn, mv) == 0) {
            total += perft(pstn, depth - 1);
        }
        unmake_move(pstn, mv);
    }

    free(moves);
    return total;
}

void divide(info *pstn, int depth) {
    if (depth < 1) {
        return;
    }

    int total = 0;
    move_list *moves = malloc(sizeof(move_list));
    moves->index = 0;
    all_moves(pstn, moves);

    for (int i = 0; i < moves->index; i++) {
        move_t mv = moves->moves[i];
        char mstr[6];
        move_to_string(pstn, mv, mstr);
        if (make_move(pstn, mv) == 0) {
            int n = perft(pstn, depth - 1);
            total += n;
            printf("%s %d\n", mstr, n);
        }
        unmake_move(pstn, mv);
    }

    free(moves);
    printf("\n%d\n", total);
}