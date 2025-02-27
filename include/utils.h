#ifndef UTILS_H
#define UTILS_H 1

int get_rank(int pos) { return (pos >> 4) - 4; }

int get_file(int pos) { return (pos & 0x0F) - 4; }

int to_index(int pos) { return 8 * get_rank(pos) + get_file(pos); }

int square_diff(int start, int dest) { return 0x77 + start - dest; }

int string_to_coord(char *sqr_str) {
    return ((sqr_str[1] - '1') << 4) + (sqr_str[0] - 'a') + 0x44;
}

#endif