#ifndef MOVE_H
#define MOVE_H 1

struct move {
    unsigned int start : 8;
    unsigned int dest: 8;
    unsigned int flags: 4;
    unsigned int captured_piece: 12;
};
typedef struct move move_t;

/* Encodes information about a move into a bitfield struct.

Input:
    - int start: The coordinates of the starting square.
    - int dest: The coordinates of the destination square.
    - int flags: Any flags associated with the move.

Returns:
    - move_t: A move struct containing the move information.
*/
move_t encode_move(int start, int dest, int flags);

/* Parses a move string and returns a move struct.

Input:
    - char *mstr: The move string.
    
Returns:
    - move_t: A move struct containing the move information. 
*/
move_t of_string(char *mstr);

/* Returns the string representation of a move from a struct. */
void move_to_string(move_t mv, char *mstr);

/* Updates a position by making a move.

Input:
    - move_t mv: The move struct containing the move info.

Returns:
    - int: 0 if the resulting position is legal, -1 otherwise.
*/
int make_move(move_t mv);

/* Unmakes a move and restores a position to its previous state.

Input:
    - move_t mv: The move to unmake.
*/
void unmake_move(move_t mv);

#endif