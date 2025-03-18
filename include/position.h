struct position_info {
    unsigned int *arr;
    unsigned int *w_pieces;
    unsigned int *b_pieces;
    unsigned int side;
    unsigned int c_rights;
    unsigned int ep_square;
    unsigned int h_clk;
    unsigned int check_info;
};
typedef struct position_info info;

struct state {
    unsigned int c_rights: 4;
    unsigned int ep_square : 8;
    unsigned int h_clk: 6;
    unsigned int check_info: 18;
};
typedef struct state state_t;

/*
Resets the arrays pointed to by a position struct and frees it.

Input:
    - info *pstn: The pointer to the position struct.
*/
void clear_position(info *pstn);

/*
Creates a valid position struct from a FEN string.

Input:
    - char *fen_str: The FEN string.

Returns:
    - info*: A pointer to a new position struct, or NULL if the parse failed.
*/
info *new_position(char *fen_str);


/* Changes the side to move in a position struct. */
void switch_side(info *pstn);

/* Saves the irreversible state of a position. */
void save_state(info *pstn);

/* Restores the irreversible state of a position. */
void restore_state(info *pstn);


/* Determines whether a square is attacked by black.

Input:
    - info *pstn: The pointer to the position struct.
    - int pos: The index of the square to check in the board array.

Returns:
    - int: 1 if the square is attacked, otherwise 0.
*/
int is_square_attacked(info *pstn, int pos);

/*
Vertically flips the board array and inverts position state incl. piece lists.

Input:
    - info *pstn: The pointer to the position struct.
*/
void flip_position(info *pstn);

/*
Converts a position to a fen string.

Input:
    - info *pstn: The pointer to the position struct.
    - char *fen_str: A string pointer to store the result in.
*/
void to_fen(info *pstn, char *fen_str);

/* 
Validates a fen string.

Input:
    - char *fen_str: A pointer to the fen string.*/
int fen_match(char *fen_str);