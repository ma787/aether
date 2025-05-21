#ifdef _WIN64
#include <windows.h>
#include <conio.h>
#else
#include <stddef.h>
#include <unistd.h>
#include <sys/time.h>
#endif

#include "aether.h"


int change_piece_type(int piece, int p_type) { return (piece & 0xF03) | p_type; }

bool same_colour(int p1, int p2) { return !((p1 ^ p2) & COLOUR_MASK); }
bool diff_colour(int p1, int p2) { return ((p1 ^ p2) & COLOUR_MASK) == G; }

#ifdef _WIN64

uint64_t get_time(void) {
	return GetTickCount64();
}

int input_waiting(void) {
	DWORD mode;
	HANDLE stdin_h = GetStdHandle(STD_INPUT_HANDLE);
	int is_console = GetConsoleMode(stdin_h, &mode);

	if (is_console) {
		return _kbhit();  // check for recent input in console
	} else {
		if (!PeekNamedPipe(stdin_h, NULL, 0, NULL, &mode, NULL)) {
			return 1;  // failed to read pipe, assume input waiting
		}
		return mode;  // return number of bytes available in pipe
	}
}

#else

uint64_t get_time(void) {
    struct timeval tv;
	gettimeofday(&tv, NULL);
	return(tv.tv_sec * 1000LL + (tv.tv_usec / 1000));
}

int input_waiting(void) {
	fd_set readfds;  // a set of file descriptors
    struct timeval tv = {0, 0};  // tells select call to return immediately

	FD_ZERO (&readfds); // clears set
	FD_SET (STDIN_FILENO, &readfds);  // adds stdin to set
	select(16, &readfds, 0, 0, &tv);  // checks if there is any data in stdin

	// checks if stdin is still present in set, i.e., it has data to be read
	return (FD_ISSET(STDIN_FILENO, &readfds));
}

#endif