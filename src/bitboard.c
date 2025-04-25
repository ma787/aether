#include "aether.h"

uint64_t get_bit(uint64_t bb, int i) {
    return bb & (1ULL << i);
}

void set_bit(uint64_t *bb, int i) {
    *bb |= (1ULL << i);
}

void clear_bit(uint64_t *bb, int i) {
    get_bit(*bb, i) ? *bb ^= (1ULL << i) : 0;
    // *bb &= ~(1ULL << i);
}

int count_bits(uint64_t bb) {
    unsigned char *p = (unsigned char *) &bb;
    
    return (
        POP_COUNT_256[p[0]]
        + POP_COUNT_256[p[1]]
        + POP_COUNT_256[p[2]]
        + POP_COUNT_256[p[3]]
        + POP_COUNT_256[p[4]]
        + POP_COUNT_256[p[5]]
        + POP_COUNT_256[p[6]]
        + POP_COUNT_256[p[7]]
    );
}

int pop_bit(uint64_t *bb) {
    int index = __builtin_ctzll(*bb);
    *bb &= (*bb - 1);
    return index;
}

void flip_bits(uint64_t *bb) {
    uint64_t x = *bb;

    *bb = (
        (x << 56)
        | ((x << 40) & 0x00ff000000000000ULL)
        | ((x << 24) & 0x0000ff0000000000ULL)
        | ((x << 8) & 0x000000ff00000000ULL)
        | ((x >>  8) & 0x00000000ff000000ULL)
        | ((x >> 24) & 0x0000000000ff0000ULL)
        | ((x >> 40) & 0x000000000000ff00ULL)
        | (x >> 56)
    );
}