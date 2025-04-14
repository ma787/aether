#include <stddef.h>
#include <unistd.h>
#include <sys/time.h>
#include "aether.h"

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

uint64_t get_time(void) {
    struct timeval tv;
	gettimeofday (&tv, NULL);
	return(tv.tv_sec * 1000LL + (tv.tv_usec / 1000));
}

int input_waiting(void) {
	fd_set readfds;
    struct timeval tv;

	FD_ZERO (&readfds);
	FD_SET (STDIN_FILENO, &readfds);
	tv.tv_sec=0; tv.tv_usec=0;
	select(16, &readfds, 0, 0, &tv);

	return (FD_ISSET(STDIN_FILENO, &readfds));
}
