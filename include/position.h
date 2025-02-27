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
    - info *pstn: A pointer to a new position struct, or NULL if the parse failed.
*/
info *new_position(char *fen_str);

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