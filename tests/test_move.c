#include <stdio.h>
#include <string.h>
#include "move.h"
#include "constants.h"

#define N_TESTS 8
#define N_RUN 8

int test_of_string(char *fen_str, char *mstr, move_t expected_mv) {
    printf("Testing parse of move string %s\n", mstr);

    info *pstn = new_position(fen_str);

    int match = 0;
    move_t test_mv = of_string(pstn, mstr);

    char* names[] = {"Start", "Destination", "Flags"};
    int attrs_1[] = {test_mv.start, test_mv.dest, test_mv.flags};
    int attrs_2[] = {expected_mv.start, expected_mv.dest, expected_mv.flags};

    for (int i = 0; i < 3; i++) {
        if (attrs_1[i] != attrs_2[i]) {
            printf(
                "Attribute %s - Expected: %d, Actual: %d\n",
                names[i], attrs_2[i], attrs_1[i]
            );
            match = -1;
        }
    }

    if ((test_mv.captured_piece & 0xFF) != expected_mv.captured_piece) {
        printf(
            "Attribute Captured Piece - Expected: %d, Actual: %d\n",
            expected_mv.captured_piece, test_mv.captured_piece & 0xFF
        );
        match = -1;
    }

    clear_position(pstn);
    return match;
}

int main(void) {
    char* move_strings[N_TESTS] = {
        "f2f3", "e2e4", "e1g1", "e8c8", "f1a6", "d5e6", "b7b8q", "b2a1n"
    };

    char* fen_strings[N_TESTS] = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "rnbqkbnr/3ppppp/ppp5/8/8/3BP2N/PPPP1PPP/RNBQK2R w KQkq - 0 1",
        "r3kbnr/pppqpppp/n2p4/5b2/8/PPPP3P/4PPP1/RNBQKBNR b KQkq - 0 1",
        "rnbqkbnr/1ppppppp/p7/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1",
        "rnbqkbnr/1ppp1ppp/p7/3Pp3/8/8/PPP1PPPP/RNBQKBNR w KQkq e6 0 1",
        "r1bqkbnr/pPpppp2/p1n5/6pp/8/4P3/P1PP1PPP/RNBQK1NR w KQkq - 0 1",
        "rnbqkbnr/pp1ppppp/8/7P/8/2N5/PpPPPPP1/R1BQKBNR b KQkq - 0 1"
    };

    move_t expected_moves[N_TESTS] = {
     {F2, F3, 0, 0},
     {E2, E4, DPP_FLAG, 0},
     {E1, G1, K_CASTLE_FLAG, 0},
     {E1, C1, Q_CASTLE_FLAG, 0},
     {F1, A6, CAPTURE_FLAG, BLACK | PAWN},
     {D5, E6, EP_FLAG, BLACK | PAWN},
     {B7, B8, PROMO_FLAG | 3, 0},
     {B7, A8, CAPTURE_FLAG | PROMO_FLAG, BLACK | ROOK}
    };

    int passed = 0;

    for (int i = 0; i < N_RUN; i++) {
        if (test_of_string(fen_strings[i], move_strings[i], expected_moves[i]) == 0) {
            printf("Passed Test %d\n\n", i+1);
            passed++;
        } else {
            printf("Failed Test %d\n\n", i+1);
        }
    }

    printf("Passed %d/%d tests.\n", passed, N_RUN);

    return 0;
}
