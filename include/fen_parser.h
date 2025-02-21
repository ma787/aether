#ifndef FEN_PARSER_H
#define FEN_PARSER_H 1

int piece_from_sym(char symbol);

int string_to_coord(char *sqr_string);

void coord_to_string(int pos, char *res);

int parse_board_string(int *arr, char *str);

int validate_fen_string(char *fen_str);

info* parse_fen_string(char *fen_str, int *arr);

void print_board(int *arr);

#endif