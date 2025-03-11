#ifndef PERFT_H
#define PERFT_H 1

#include "move_gen.h"

int perft(info *pstn, int depth);

void divide(info *pstn, int depth);

#endif