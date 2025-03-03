#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "position.h"

#define N_TESTS 15
#define N_RUN 15

int compare_positions(info *pstn_1, info *pstn_2) {
    char names[][20] = {"side", "castling rights", "ep square", "halfmove clock", "check type"};
    unsigned int attrs_1[] = {
        pstn_1->side, pstn_1->c_rights, pstn_1->ep_square, pstn_1->h_clk, pstn_1->check_info & 3
    };
    unsigned int attrs_2[] = {
        pstn_2->side, pstn_2->c_rights, pstn_2->ep_square, pstn_2->h_clk, pstn_2->check_info & 3
    };
    int match = 0;

    for (int i = 0; i < 5; i ++) {
        if (attrs_1[i] != attrs_2[i]) {
            printf("Attribute '%s' - Expected: %u, Actual: %u\n", names[i], attrs_2[i], attrs_1[i]);
            match = -1;
        }
    }

    unsigned int check_type = attrs_2[4];
    int check_match = 0;

    if (check_type) {
        unsigned int pstn_1_first_checker = (pstn_1->check_info >> 2) & 0xFF;
        unsigned int pstn_1_second_checker = (pstn_1->check_info >> 10) & 0xFF;

        unsigned int pstn_2_first_checker = (pstn_2->check_info >> 2) & 0xFF;
        unsigned int pstn_2_second_checker = (pstn_2->check_info >> 10) & 0xFF;

        int first_checker_match = pstn_1_first_checker == pstn_2_first_checker;

        if (check_type < DOUBLE_CHECK) {
            if (!first_checker_match) {
                printf("Checkers do not match. Expected checker: %u, Actual: %u\n", pstn_2_first_checker, pstn_1_first_checker);
                return -1;
            }
        } else if (first_checker_match) {
            if (pstn_1_second_checker != pstn_2_second_checker) {
                check_match = -1;
            }
        } else if (pstn_1_first_checker == pstn_2_second_checker) {
            if (pstn_1_second_checker != pstn_2_first_checker) {
                check_match = -1;
            }
        } else {
            check_match = -1;
        }

        if (check_match != 0) {
            printf(
                "Checkers do not match. Expected checkers: %u, %u, Actual: %u, %u\n",
                pstn_2_first_checker, pstn_2_second_checker, pstn_1_first_checker, pstn_1_second_checker
            );
            match = -1;
        }
    }
    
    return match;
}

int test_position(char *fen_str, info expected_pstn){
    printf("Testing parse of fen string '%s'\n", fen_str);
    info *pstn = new_position(fen_str);

    if (pstn == NULL) {
        printf("Error while parsing fen string.\n");
        return -1;
    }

    int match = 0;

    printf("Testing position attributes\n");
    match = compare_positions(pstn, &expected_pstn);

    if (match == 0) {
        printf("Attributes test successful.\n");
    }

    printf("Comparing outputted FEN string to original\n");
    char parsed_str[92];
    to_fen(pstn, parsed_str);
    if (strcmp(fen_str, parsed_str) == 0) {
        printf("FEN strings comparison successful.\n");
    } else {
        printf("FEN strings do not match.\n\nExpected: %s\nActual:   %s\n", fen_str, parsed_str);
        match = -1;
    }

    clear_position(pstn);
    return match;
}

int main(void) {
    char fen_strings[N_TESTS][92] = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "rnbqkbnr/1p1ppppp/8/p1pP4/8/8/PPP1PPPP/RNBQKBNR w KQkq c6 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/4P3/1pN2Q1p/PPPBBPPP/R3KR2 w Qkq - 0 1",
        "r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3 1",
        "1nbqkbnr/r2pp2p/ppp2pB1/4P2Q/3P4/2N5/PPP2PPP/R1B1K1NR b KQk - 0 1",
        "r2k3r/p1ppqNb1/bn2pQp1/3P4/1p2P3/2N4p/PPPBBPPP/R3K2R b KQ - 0 1",
        "4k2r/8/8/8/8/8/8/R1r1K2R w KQk - 1 1",
        "r3k2r/p1pq1Pb1/bn1p1np1/1B2N3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R b KQkq - 0 1",
        "8/8/3p4/1Pp4r/1K3R2/6k1/4P1P1/8 w - c6 0 1",
        "rnbqkbn1/pp6/8/8/4Kr2/8/PP4b1/R3Q1BR w q - 0 1",
        "r5k1/ppp3p1/3pb3/7p/8/5r1K/PB3bPP/RN1Q3R w - - 0 1",
        "r1bq1r2/pp2n3/4N1Pk/3pPp2/1b1n2Q1/2N5/PP3PP1/R1B1K2R b KQ - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/1B1PN3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R w KQkq - 0 1"
    };

    info results[N_TESTS] = {
        {.side = WHITE, .c_rights = 15, .ep_square = 0, .h_clk = 0, .check_info = 0},
        {.side = WHITE, .c_rights = 15, .ep_square = 0x96, .h_clk = 0, .check_info = 0},
        {.side = WHITE, .c_rights = 15, .ep_square = 0, .h_clk = 0, .check_info = 0},
        {.side = WHITE, .c_rights = 0, .ep_square = 0, .h_clk = 0, .check_info = 0},
        {.side = WHITE, .c_rights = 13, .ep_square = 0, .h_clk = 0, .check_info = 0},
        {.side = BLACK, .c_rights = 12, .ep_square = 0, .h_clk = 3, .check_info = DISTANT_CHECK | (0x8B << 2)},
        {.side = BLACK, .c_rights = 14, .ep_square = 0, .h_clk = 0, .check_info = DISTANT_CHECK | (0x6A << 2)},
        {.side = BLACK, .c_rights = 12, .ep_square = 0, .h_clk = 0, .check_info = CONTACT_CHECK | (0x59 << 2)},
        {.side = WHITE, .c_rights = 11, .ep_square = 0, .h_clk = 1, .check_info = DISTANT_CHECK | (0x46 << 2)},
        {.side = BLACK, .c_rights = 15, .ep_square = 0, .h_clk = 0, .check_info = CONTACT_CHECK | (0x59 << 2)},
        {.side = WHITE, .c_rights = 0, .ep_square = 0x96, .h_clk = 0, .check_info = CONTACT_CHECK | (0x86 << 2)},
        {.side = WHITE, .c_rights = 4, .ep_square = 0, .h_clk = 0, .check_info = DOUBLE_CHECK | (0x5A << 2) | (0x79 << 10)},
        {.side = WHITE, .c_rights = 0, .ep_square = 0, .h_clk = 0, .check_info = DOUBLE_CHECK | (0x69 << 2) | (0x98 << 10)},
        {.side = BLACK, .c_rights = 12, .ep_square = 0, .h_clk = 0, .check_info = DOUBLE_CHECK | (0xB6 << 2) | (0xBB << 10)},
        {.side = WHITE, .c_rights = 15, .ep_square = 0, .h_clk = 0, .check_info = 0}
    };

    int passed = 0;

    for (int i = 0; i < N_RUN; i++) {
        if (test_position(fen_strings[i], results[i]) == 0) {
            printf("Passed Test %d\n\n", i+1);
            passed++;
        } else {
            printf("Failed Test %d\n\n", i+1);
        }
    }

    printf("Passed %d/%d tests.\n", passed, N_RUN);

    return 0;
}