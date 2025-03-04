#include <stdio.h>
#include <string.h>
#include "move.h"
#include "constants.h"

#define OF_STRING_TESTS 8
#define MAKE_MOVE_TESTS 5
#define UNMAKE_MOVE_TESTS 3
#define TOTAL_TESTS (OF_STRING_TESTS + MAKE_MOVE_TESTS + UNMAKE_MOVE_TESTS)


int test_of_string(char *fen_str, char *mstr, move_t expected_mv) {
    printf("Testing parse of move string %s\n", mstr);

    info *pstn = new_position(fen_str);
    if (pstn == NULL) {
        printf("Error - fen string parse failed.\n");
        return -1;
    }

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

int test_make_move(char *fen_str, char *mstr, char *expected_str) {
    printf("Attempting to make move %s\n", mstr);
    printf("Position: %s\n", fen_str);
    
    info *pstn = new_position(fen_str);
    if (pstn == NULL) {
        printf("Error - fen string parse failed.\n");
        return -1;
    }

    move_t mv = of_string(pstn, mstr);
    int res = 0;

    res = make_move(pstn, mv);
    if (res != 0) {
        clear_position(pstn);
        return res;
    }

    char parsed_str[92];
    to_fen(pstn, parsed_str);

    if (strcmp(expected_str, parsed_str) != 0) {
        printf("FEN strings do not match.\n\nExpected: %s\nActual:   %s\n", expected_str, parsed_str);
        res = -1;
    }

    clear_position(pstn);
    return res;
}

int test_unmake_move(char *fen_str, char *mstr) {
    printf("Attempting to make and unmake move %s\n", mstr);
    printf("Position: %s\n", fen_str);

    info *pstn = new_position(fen_str);
    if (pstn == NULL) {
        printf("Error - fen string parse failed.\n");
        return -1;
    }

    move_t mv = of_string(pstn, mstr);
    
    int res = make_move(pstn, mv);
    if (res != 0) {
        printf("Error - make move resulted in illegal position.\n");
        clear_position(pstn);
        return res;
    }

    unmake_move(pstn, mv);

    char parsed_str[92];
    to_fen(pstn, parsed_str);

    if (strcmp(fen_str, parsed_str) != 0) {
        printf("FEN strings do not match.\n\nExpected: %s\nActual:   %s\n", fen_str, parsed_str);
        res = -1;
    }

    clear_position(pstn);
    return res;
}

int run_of_string_tests(int passed) {
    char *move_strings[OF_STRING_TESTS] = {
        "f2f3", "e2e4", "e1g1", "e8c8", "f1a6", "d5e6", "b7b8q", "b2a1n"
    };

    char *fen_strings[OF_STRING_TESTS] = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "rnbqkbnr/3ppppp/ppp5/8/8/3BP2N/PPPP1PPP/RNBQK2R w KQkq - 0 1",
        "r3kbnr/pppqpppp/n2p4/5b2/8/PPPP3P/4PPP1/RNBQKBNR b KQkq - 0 1",
        "rnbqkbnr/1ppppppp/p7/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1",
        "rnbqkbnr/1ppp1ppp/p7/3Pp3/8/8/PPP1PPPP/RNBQKBNR w KQkq e6 0 1",
        "r1bqkbnr/pPpppp2/p1n5/6pp/8/4P3/P1PP1PPP/RNBQK1NR w KQkq - 0 1",
        "rnbqkbnr/pp1ppppp/8/7P/8/2N5/PpPPPPP1/R1BQKBNR b KQkq - 0 1"
    };

    move_t expected_moves[OF_STRING_TESTS] = {
     {F2, F3, 0, 0},
     {E2, E4, DPP_FLAG, 0},
     {E1, G1, K_CASTLE_FLAG, 0},
     {E1, C1, Q_CASTLE_FLAG, 0},
     {F1, A6, CAPTURE_FLAG, BLACK | PAWN},
     {D5, E6, EP_FLAG, BLACK | PAWN},
     {B7, B8, PROMO_FLAG | 3, 0},
     {B7, A8, CAPTURE_FLAG | PROMO_FLAG, BLACK | ROOK}
    };

    printf("Testing the function of_string\n\n");

    for (int i = 0; i < OF_STRING_TESTS; i++) {
        if (test_of_string(fen_strings[i], move_strings[i], expected_moves[i]) == 0) {
            printf("Passed Test %d\n\n", i+1);
            passed++;
        } else {
            printf("Failed Test %d\n\n", i+1);
        }
    }

    return passed;
}

int run_make_move_tests(int passed) {
    char *move_strings[MAKE_MOVE_TESTS] = {
        "a2a3", "e4d5", "f2f4", "e7e5", "b7b8q"
    };

    char *fen_strings[MAKE_MOVE_TESTS] = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1",
        "rnbqkbnr/pppp1ppp/8/1N6/4p3/8/PPPPPPPP/R1BQKBNR w KQkq - 0 1",
        "rnbqkbnr/1ppppppp/p7/3P4/8/8/PPP1PPPP/RNBQKBNR b KQkq - 0 1",
        "r1bqkbnr/pPpppp2/p1n5/6pp/8/4P3/P1PP1PPP/RNBQK1NR w KQkq - 0 1"
    };

    char *expected_strings[MAKE_MOVE_TESTS] = {
        "rnbqkbnr/pppppppp/8/8/8/P7/1PPPPPPP/RNBQKBNR b KQkq - 0 1",
        "rnbqkbnr/ppp1pppp/8/3P4/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1",
        "rnbqkbnr/pppp1ppp/8/1N6/4pP2/8/PPPPP1PP/R1BQKBNR b KQkq f3 0 1",
        "rnbqkbnr/1ppp1ppp/p7/3Pp3/8/8/PPP1PPPP/RNBQKBNR w KQkq e6 0 1",
        "rQbqkbnr/p1pppp2/p1n5/6pp/8/4P3/P1PP1PPP/RNBQK1NR b KQkq - 0 1"
    };

    printf("Testing the function make_move\n\n");

    for (int i = 0; i < MAKE_MOVE_TESTS; i++) {
        if (test_make_move(fen_strings[i], move_strings[i], expected_strings[i]) == 0) {
            printf("Passed Test %d\n\n", i+1);
            passed++;
        } else {
            printf("Failed Test %d\n\n", i+1);
        }
    }

    return passed;
}

int run_unmake_move_tests(int passed) {
    char *move_strings[UNMAKE_MOVE_TESTS] = {"a2a3", "e8d8","g2g1q"};
    
    char *fen_strings[UNMAKE_MOVE_TESTS] = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1",
        "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1"
    };

    for (int i = 0; i < UNMAKE_MOVE_TESTS; i++) {
        if (test_unmake_move(fen_strings[i], move_strings[i]) == 0) {
            printf("Passed Test %d\n\n", i+1);
            passed++;
        } else {
            printf("Failed Test %d\n\n", i+1);
        }
    }

    return passed;
}

int main(void) {
    int passed = run_of_string_tests(0);
    passed = run_make_move_tests(passed);
    passed = run_unmake_move_tests(passed);

    printf("Passed %d/%d tests.\n", passed, TOTAL_TESTS);

    return 0;
}
