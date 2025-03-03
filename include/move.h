#ifndef MOVE_H
#define MOVE_H 1

#include "position.h"

struct move {
    unsigned int start : 8;
    unsigned int dest: 8;
    unsigned int flags: 4;
    unsigned int captured_piece: 12;
};
typedef struct move move_t;

/* Parses a move string and returns a move struct.

Input:
    - info *pstn: The current board position.
    - char *mstr: The move string.
    
Returns:
    - move_t: A move struct containing the move information. 
*/
move_t of_string(info *pstn, char *mstr);

#endif