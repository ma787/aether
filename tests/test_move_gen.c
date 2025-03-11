#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "move_gen.h"

#define TOTAL_TESTS 16

int test_move_gen(char *fen_str, char *expected_moves[], int n_moves) {
    printf("Testing all_moves, FEN: %s\n", fen_str);

    info *pstn = new_position(fen_str);

    if (pstn == NULL) {
        printf("Error - fen string parse failed.\n");
        return -1;
    }

    move_list *moves = malloc(sizeof(move_list));
    moves->index = 0;
    all_moves(pstn, moves);

    int passed = 0;
    
    int found_index = 0;
    char found_moves[moves->index][6];
    
    int extra_index = 0;
    char extra_moves[moves->index][6];

    int missing_index = 0;
    char missing_moves[n_moves][6];

    for (int i = 0; i < moves->index; i++) {
        move_t mv = moves->moves[i];
        char mstr[6];
        move_to_string(pstn, mv, mstr);
        int found = 0;

        for (int j = 0; j < n_moves; j++) {
            if (strcmp(mstr, expected_moves[j]) == 0) {
                found = 1;
                strcpy(found_moves[found_index++], mstr);
                break;
            }
        }

        if (!found) {
            if (make_move(pstn, mv) == 0) { // discard illegal moves
                strcpy(extra_moves[extra_index++], mstr);
            }
            unmake_move(pstn, mv);
        }
    }

    for (int i = 0; i < n_moves; i++) {
        int found = 0;
        char *expected_move = expected_moves[i];

        for (int j = 0; j < found_index; j++) {
            if (strcmp(expected_move, found_moves[j]) == 0) {
                found = 1;
                break;
            }
        }

        if (!found) {
            strcpy(missing_moves[missing_index++], expected_move);
        }
    }

    if (extra_index) {
        passed = -1;
        printf("Extra moves generated:\n");
        for (int i = 0; i < extra_index; i++) {
            printf("%s, ", extra_moves[i]);
            fflush(stdout);
        }
        printf("\n");
    }

    if (missing_index) {
        passed = -1;
        printf("The following moves were not generated:\n");
        for (int i = 0; i < missing_index; i++) {
            printf("%s, ", missing_moves[i]);
            fflush(stdout);
        }
        printf("\n");
    }

    clear_position(pstn);
    free(moves);
    return passed;
}

int main(void) {
    char *fen_strings[TOTAL_TESTS] = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
        "r3k2r/p1ppqpb1/Bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R b KQkq - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/P1N2Q1p/1PPBBPPP/R3K2R b KQkq - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/4P3/1pN2Q1p/PPPBBPPP/R3KR2 w Qkq - 0 1",
        "rnbqkbnr/pppp1ppp/8/8/4P3/5N2/PpP1QPPP/R1B1KB1R b KQkq - 1 1",
        "r3k2r/8/8/8/8/8/8/1R2K2R b Kkq - 0 1",
        "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1",
        "n1n5/PPPk4/8/8/8/8/4Kp1p/5N1b w - - 0 1",
        "n1n5/PPPk4/8/8/8/8/4Kp1p/5N1q w - - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/1B1PN3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R b KQkq - 1 1",
        "rnbqkbnr/1ppppppp/4Q3/p7/8/2P5/PP1PPPPP/RNB1KBNR b KQkq - 1 1",
        "rnbqkbnr/1ppppppp/8/p7/4Q3/2P5/PP1PPPPP/RNB1KBNR b KQkq - 1 1",
        "8/2p5/3p4/KP3R1r/5p2/8/4PkP1/8 b - - 5 1",
        "8/2p5/3p4/KP5r/1R2Pp1k/8/6P1/8 b - e3 0 1",
        "8/8/8/8/k2Pp2Q/8/8/3K4 b - d3 0 1"
    };

    int n_moves[TOTAL_TESTS] = {
        20, 48, 36, 44, 46, 41, 26, 24, 21, 21, 39, 21, 21, 20, 16, 6
    };

    char *test_1_moves[] = {
        "a2a3", "b2b3", "c2c3", "d2d3", "e2e3", "f2f3", "g2g3", "h2h3",
        "a2a4", "b2b4", "c2c4", "d2d4", "e2e4", "f2f4", "g2g4", "h2h4",
        "b1a3", "b1c3", "g1f3", "g1h3"
    };

    char *test_2_moves[] = {
        "a2a3", "b2b3", "g2g3", "d5d6", "a2a4", "g2g4", "g2h3", "d5e6",
        "c3b1", "c3d1", "c3a4", "c3b5", "e5d3", "e5c4", "e5g4", "e5c6",
        "e5g6", "e5d7", "e5f7", "d2c1", "d2e3", "d2f4", "d2g5", "d2h6",
        "e2d1", "e2f1", "e2d3", "e2c4", "e2b5", "e2a6", "a1b1", "a1c1",
        "a1d1", "h1f1", "h1g1", "f3d3", "f3e3", "f3g3", "f3h3", "f3f4",
        "f3g4", "f3f5", "f3h5", "f3f6", "e1d1", "e1f1", "e1g1", "e1c1"
    };

    char *test_3_moves[] = {
        "b4b3", "g6g5", "c7c6", "d7d6", "c7c5", "h3g2", "e6d5", "b4c3",
        "b6a4", "b6c4", "b6d5", "b6c8", "f6e4", "f6g4", "f6d5", "f6h5",
        "f6h7", "f6g8", "g7h6", "g7f8", "a8b8", "a8c8", "a8d8", "h8h4",
        "h8h5", "h8h6", "h8h7", "h8f8", "h8g8", "e7c5", "e7d6", "e7d8",
        "e7f8", "e8d8", "e8f8", "e8g8"
    };

    char *test_4_moves[] = {
        "b4b3", "g6g5", "c7c6", "d7d6", "c7c5", "h3g2", "b4a3", "e6d5",
        "b4c3", "b6a4", "b6c4", "b6d5", "b6c8", "f6e4", "f6g4", "f6d5",
        "f6h5", "f6h7", "f6g8", "a6e2", "a6d3", "a6c4", "a6b5", "a6b7",
        "a6c8", "g7h6", "g7f8", "a8b8", "a8c8", "a8d8", "h8h4", "h8h5",
        "h8h6", "h8h7", "h8f8", "h8g8", "e7c5", "e7d6", "e7d8", "e7f8",
        "e8d8", "e8f8", "e8g8", "e8c8"
    };

    char *test_5_moves[] = {
        "a2a3", "g2g3", "d5d6", "a2a4", "g2g4", "a2b3", "g2h3", "d5e6",
        "c2b3", "c3b1", "c3d1", "c3a4", "c3b5", "e5d3", "e5c4", "e5g4",
        "e5c6", "e5g6", "e5d7", "e5f7", "d2c1", "d2e3", "d2f4", "d2g5",
        "d2h6", "e2d1", "e2d3", "e2c4", "e2b5", "e2a6", "a1b1", "a1c1",
        "a1d1", "f1g1", "f1h1", "f3d3", "f3e3", "f3g3", "f3h3", "f3f4",
        "f3g4", "f3f5", "f3h5", "f3f6", "e1d1", "e1c1"
    };

    char *test_6_moves[] = {
        "a7a6", "b7b6", "c7c6", "d7d6", "f7f6", "g7g6", "h7h6", "a7a5",
        "b7b5", "c7c5", "d7d5", "f7f5", "g7g5", "h7h5", "b2a1q", "b2a1r",
        "b2a1b", "b2a1n", "b2c1q", "b2c1r", "b2c1b", "b2c1n", "b2b1q", "b2b1r",
        "b2b1b", "b2b1n", "b8a6", "b8c6", "g8f6", "g8h6", "g8e7", "f8a3",
        "f8b4", "f8c5", "f8d6", "f8e7", "d8h4", "d8g5", "d8f6", "d8e7",
        "e8e7"
    };

    char *test_7_moves[] = {
        "a8a1", "a8a2", "a8a3", "a8a4", "a8a5", "a8a6", "a8a7", "a8b8",
        "a8c8", "a8d8", "h8h1", "h8h2", "h8h3", "h8h4", "h8h5", "h8h6",
        "h8h7", "h8f8", "h8g8", "e8d7", "e8e7", "e8f7", "e8d8", "e8f8",
        "e8g8", "e8c8"
    };

    char *test_8_moves[] = {
        "g2f1q", "g2f1r", "g2f1b", "g2f1n", "g2h1q", "g2h1r", "g2h1b", "g2h1n",
        "g2g1q", "g2g1r", "g2g1b", "g2g1n", "a8b6", "a8c7", "c8b6", "c8d6",
        "c8a7", "c8e7", "d7c6", "d7d6", "d7e6", "d7c7", "d7e7", "d7e8"
    };

    char *test_9_moves[] = {
        "b7c8q", "b7c8r", "b7c8b", "b7c8n", "b7a8q", "b7a8r", "b7a8b", "b7a8n",
        "b7b8q", "b7b8r", "b7b8b", "b7b8n", "f1d2", "f1h2", "f1e3", "f1g3",
        "e2d1", "e2e3", "e2d2", "e2f2", "e2d3"
    };

    char *test_10_moves[] = {
        "b7c8q", "b7c8r", "b7c8b", "b7c8n", "b7a8q", "b7a8r", "b7a8b", "b7a8n",
        "b7b8q", "b7b8r", "b7b8b", "b7b8n", "f1d2", "f1h2", "f1e3", "f1g3",
        "e2d1", "e2e3", "e2d2", "e2f2", "e2d3"
    };

    char *test_11_moves[] = {
        "b4b3", "g6g5", "c7c6", "e8c8", "c7c5", "h3g2", "e6d5", "b4c3",
        "b6a4", "b6c4", "b6d5", "b6c8", "f6e4", "f6g4", "f6d5", "f6h5",
        "f6h7", "f6g8", "a6b5", "a6b7", "a6c8", "g7h6", "g7f8", "a8b8",
        "a8c8", "a8d8", "h8h4", "h8h5", "h8h6", "h8h7", "h8f8", "h8g8",
        "e7c5", "e7d6", "e7d8", "e7f8", "e8d8", "e8f8", "e8g8"
    };

    char *test_12_moves[] = {
        "a5a4", "b7b6", "c7c6", "d7d6", "f7f6", "g7g6", "h7h6", "b7b5",
        "c7c5", "d7d5", "f7f5", "g7g5", "h7h5", "f7e6", "d7e6", "b8a6",
        "b8c6", "g8f6", "g8h6", "a8a6", "a8a7"
    };

    char *test_13_moves[] = {
        "a5a4", "b7b6", "c7c6", "d7d6", "e7e6", "f7f6", "g7g6", "h7h6",
        "b7b5", "c7c5", "d7d5", "e7e5", "f7f5", "g7g5", "h7h5", "b8a6",
        "b8c6", "g8f6", "g8h6", "a8a6", "a8a7"
    };

    char *test_14_moves[] = {
        "f4f3", "d6d5", "c7c6", "c7c5", "h5h1", "h5h2", "h5h3", "h5h4",
        "h5f5", "h5g5", "h5h6", "h5h7", "h5h8", "f2e1", "f2f1", "f2g1",
        "f2e2", "f2g2", "f2e3", "f2g3"
    };

    char *test_15_moves[] = {
        "f4f3", "d6d5", "c7c6", "c7c5", "h4g5", "h5b5", "h5c5", "h5d5", 
        "h5e5", "h5f5", "h5g5", "h5h6", "h5h7", "h5h8", "h4g3", "h4g4"
    };

    char *test_16_moves[] = {"e4e3", "a4a3", "a4b3", "a4b4", "a4a5", "a4b5"};

    char **expected_moves[TOTAL_TESTS] = {
        test_1_moves, test_2_moves, test_3_moves, test_4_moves, test_5_moves,
        test_6_moves, test_7_moves, test_8_moves, test_9_moves, test_10_moves,
        test_11_moves, test_12_moves, test_13_moves, test_14_moves, test_15_moves,
        test_16_moves
    };

    int passed = 0;

    for (int i = 0; i < TOTAL_TESTS; i++) {
        if (test_move_gen(fen_strings[i], expected_moves[i], n_moves[i]) == 0) {
            printf("Passed Test %d\n\n", i+1);
            passed++;
        } else {
            printf("Failed Test %d\n\n", i+1);
        }
    }

    printf("Passed %d/%d tests.\n", passed, TOTAL_TESTS);
    
    return 0;
}