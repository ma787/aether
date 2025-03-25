#ifndef HASHING_H
#define HASHING_H 1

#include <stdint.h>
#include "move.h"

uint64_t zobrist_hash(void);

uint64_t update_hash(uint64_t z_hash, move_t mv);

#endif