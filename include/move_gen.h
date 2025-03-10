#ifndef MOVE_GEN_H
#define MOVE_GEN_H 1

#include "constants.h"
#include "move.h"

struct move_list {
    int index;
    move_t moves[MOVE_LIST_SIZE];
};
typedef struct move_list move_list;

typedef void (*MOVE_GENERATOR)(info*, int, int, move_list*);

/* Populates a move list with a set of moves at a position.

Input:
    info *pstn: The position to analyse.
    move_list *moves: The move list.
*/
void all_moves(info *pstn, move_list *moves);

#endif