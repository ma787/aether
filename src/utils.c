#include "constants.h"

int get_rank(int pos) { return (pos >> 4) - 4; }

int get_file(int pos) { return (pos & 0x0F) - 4; }

int to_index(int pos) { return 8 * get_rank(pos) + get_file(pos); }

int flip_square(int pos) { return (~pos & 0xF0) | (pos & 0x0F); }

int square_diff(int start, int dest) { return 0x77 + dest - start; }

int is_attacking(int p_type, int start, int dest) {
    int res = NO_CHECK;
    int diff = square_diff(start, dest);
    if (MOVE_TABLE[diff] & p_type) {
        res = (UNIT_VEC[diff] == dest - start) ? CONTACT_CHECK : DISTANT_CHECK;
    }
    return res;
}

int string_to_coord(char *sqr_str) {
    return ((sqr_str[1] - '1') << 4) + (sqr_str[0] - 'a') + A1;
}

char* coord_to_string(int pos) { return COORDS[to_index(pos)]; }
