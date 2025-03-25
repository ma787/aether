#ifndef UTILS_H
#define UTILS_H 1

int get_rank(int pos);

int get_file(int pos);

int to_index(int pos);

int flip_square(int pos);

int square_diff(int start, int dest);

int is_attacking(int p_type, int start, int dest);

int get_step(int start, int dest);

int string_to_coord(char *sqr_str);

char* coord_to_string(int pos);

int get_time(void);

#endif