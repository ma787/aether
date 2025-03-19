extern unsigned int board[256];
extern unsigned int *w_pieces;
extern unsigned int *b_pieces;
extern unsigned int side;
extern unsigned int c_rights;
extern unsigned int ep_square;
extern unsigned int h_clk;
extern unsigned int check_info;

int set_position(char *fen_str);

/* Changes the side to move. */
void switch_side();

/* Saves the irreversible state of a position. */
void save_state();

/* Restores the irreversible state of a position. */
void restore_state();

/* Vertically flips the board array and inverts position state incl. piece lists. */
void flip_position();

/* Determines whether a square is attacked by black.

Input:
    - int pos: The index of the square to check in the board array.

Returns:
    - int: 1 if the square is attacked, otherwise 0.
*/
int is_square_attacked(int pos);

/*
Converts a position to a fen string.

Input:
    - char *fen_str: A string pointer to store the result in.
*/
void to_fen(char *fen_str);

/* 
Validates a fen string.

Input:
    - char *fen_str: A pointer to the fen string.*/
int fen_match(char *fen_str);