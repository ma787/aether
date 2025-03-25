#include <stdio.h>
#include <sys/time.h>
#include "constants.h"

int get_rank(int pos) { return (pos >> 4) - 4; }

int get_file(int pos) { return (pos & 0x0F) - 4; }

int to_index(int pos) { return 8 * get_rank(pos) + get_file(pos); }

int flip_square(int pos) { return (~pos & 0xF0) | (pos & 0x0F); }

int square_diff(int start, int dest) { return 0x77 + dest - start; }

int is_attacking(int piece, int start, int dest) {
    if ((piece & PAWN) && ((piece & COLOUR_MASK) == BLACK)) {
        start = flip_square(start);
        dest = flip_square(dest);
    }

    int res = NO_CHECK;
    int diff = square_diff(start, dest);
    if (MOVE_TABLE[diff] & piece) {
        res = (UNIT_VEC[diff] == dest - start) ? CONTACT_CHECK : DISTANT_CHECK;
    }
    return res;
}

int get_step(int start, int dest) { return UNIT_VEC[square_diff(start, dest)]; }

int string_to_coord(char *sqr_str) {
    return ((sqr_str[1] - '1') << 4) + (sqr_str[0] - 'a') + A1;
}

char* coord_to_string(int pos) { return COORDS[to_index(pos)]; }

int get_time(void) {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (t.tv_sec * 1000) + (t.tv_usec / 1000);
}
