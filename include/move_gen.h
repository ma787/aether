#ifndef MOVE_GEN_H
#define MOVE_GEN_H 1

#include "move.h"

struct move_list {
    int index;
    move_t moves[256];
};
typedef struct move_list move_list;

typedef void (*MOVE_GENERATOR)(int, int, move_list*);

/* Populates a move list with a set of moves at a position.

Input:
    move_list *moves: The move list.
*/
void all_moves(move_list *moves);

#endif