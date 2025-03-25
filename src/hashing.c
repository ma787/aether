#include "hashing.h"
#include "constants.h"
#include "position.h"
#include "utils.h"

int EN_PASSANT = -13;
int CASTLING = -5;
int SIDE = -1;

uint64_t get_hash(uint64_t pos, uint64_t piece) {
    int sq_off = (6 * get_rank(pos)) + (12 * get_file(pos));

    switch(piece & 0xFC) {
        case PAWN:
            return HASH_VALUES[sq_off];
        case KNIGHT:
            return HASH_VALUES[sq_off + 1];
        case BISHOP:
            return HASH_VALUES[sq_off + 2];
        case ROOK:
            return HASH_VALUES[sq_off + 3];
        case QUEEN:
            return HASH_VALUES[sq_off + 4];
        case KING:
            return HASH_VALUES[sq_off + 5];
    }

    return 0;
}

uint64_t zobrist_hash(void) {
    uint64_t z_hash = 0;
    int i = 0x44;
    int flipped = 0;
    
    if (side == BLACK) {
        flip_position();
        flipped = 1;
    }

    while (i < 0xBC) {
        int sq = board[i];
        int colour = sq & COLOUR_MASK;
        if (colour == WHITE || colour == BLACK) {
            z_hash ^= get_hash(i++, sq);
        } else if (colour == G) {
            i += 8;
        } else {
            i++;
        }
    }

    if (side == BLACK) {
        z_hash ^= HASH_VALUES[SIDE];
    }

    if (ep_square) {
        z_hash ^= HASH_VALUES[EN_PASSANT + get_file(ep_square)];
    }

    for (int j = 0; j < 4; j++) {
        if (c_rights & (1 << j)) {
            z_hash ^= HASH_VALUES[CASTLING + j];
        }
    }
    
    if (flipped) {
        flip_position();
    }
    return z_hash;
}

uint64_t update_hash(uint64_t z_hash, move_t mv) {
    int start = mv.start, dest = mv.dest, piece = board[mv.start];

    int new_c_rights = c_rights & (
        (start != A1)
        | ((start != H1) << 1)
        | ((dest != A8) << 2)
        | ((dest != H8) << 3)
    );

    if (side == BLACK) {
        start = flip_square(start);
        dest = flip_square(dest);
        piece = (piece & 0xFFC) | BLACK;
        new_c_rights = ((new_c_rights & 12) >> 2) | ((new_c_rights & 3) << 2);
    }

    z_hash ^= get_hash(start, piece);
    z_hash ^= get_hash(dest, piece);

    if (ep_square) {
        z_hash ^= HASH_VALUES[EN_PASSANT + get_file(ep_square)];
    }

    z_hash ^= HASH_VALUES[SIDE];

    if (mv.flags == K_CASTLE_FLAG || mv.flags == Q_CASTLE_FLAG) {
        int r_start = (mv.flags == K_CASTLE_FLAG) ? H1 : A1;
        int r_dest = (mv.flags == K_CASTLE_FLAG) ? F1 : D1;

        if (side == BLACK) {
            r_start = flip_square(r_start);
            r_dest = flip_square(r_dest);

            z_hash ^= HASH_VALUES[CASTLING + 2];
            z_hash ^= HASH_VALUES[CASTLING + 3];
        } else {
            z_hash ^= HASH_VALUES[CASTLING];
            z_hash ^= HASH_VALUES[CASTLING + 1];
        }
        
        z_hash ^= get_hash(r_start, side | ROOK);
        z_hash ^= get_hash(r_dest, side | ROOK);

        return z_hash;
    }

    if (mv.flags == DPP_FLAG) {
        return z_hash ^ HASH_VALUES[EN_PASSANT + get_file(dest)];
    }

    if (mv.flags & PROMO_FLAG) {
        z_hash ^= get_hash(dest, piece);
        z_hash ^= get_hash(dest, side | PROMOTIONS[mv.flags & 3]);
    }

    if (mv.flags & CAPTURE_FLAG) {
        int cap_pos = dest, cap_piece = mv.captured_piece, off = S;

        if (side == BLACK) {
            cap_piece = (cap_piece & 0xFC) | BLACK;
            off = N;
        }
        
        if (mv.flags == EP_FLAG) {
            cap_pos += off;
        }

        z_hash ^= get_hash(cap_pos, cap_piece);
    }

    for (int i = 0; i < 4; i++) {
        if (c_rights & i) {
            z_hash ^= HASH_VALUES[CASTLING + i];
        }
        if (new_c_rights & i) {
            z_hash ^= HASH_VALUES[CASTLING + i];
        }
    }

    return z_hash;
}