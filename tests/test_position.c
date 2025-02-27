#include <stdio.h>
#include "constants.h"
#include "position.h"

#define N_TESTS 5
#define N_RUN 5

int test_position(char *fen_str, int *attrs){
    printf("Testing parse of fen string '%s'\n", fen_str);
    info *pstn = new_position(fen_str);

    if (pstn == NULL) {
        printf("Error while parsing fen string.\n");
        return -1;
    }

    char parsed_str[92];
    to_fen(pstn, parsed_str);
    printf("Parsed fen string: %s\n", parsed_str);

    int pstn_attrs[5] = {pstn->side, pstn->c_rights, pstn->ep_square, pstn->h_clk, pstn->check_info};
    char names[5][20] = {"side", "castling rights", "ep square", "halfmove clock", "check info"};

    // Testing position struct attributes
    int match = 0;
    for (int i = 0; i < 5; i++) {
        if (pstn_attrs[i] != attrs[i]) {
            printf("Attribute '%s' - Expected: %u, Actual: %u\n", names[i], attrs[i], pstn_attrs[i]);
            match = -1;
        }
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
        "r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3 1"
    };

    int results[N_TESTS][5] = {
        {WHITE, 15, 0, 0, 0},
        {WHITE, 15, 0x96, 0, 0},
        {WHITE, 15, 0, 0, 0},
        {WHITE, 0, 0, 0, 0},
        {WHITE, 12, 0, 3, DISTANT_CHECK | (0x8B << 2)}
    };

    for (int i = 0; i < N_RUN; i++) {
        if (test_position(fen_strings[i], results[i]) == 0) {
            printf("Passed Test %d\n", i+1);
        } else {
            printf("Failed Test %d\n", i+1);
        }
    }

    return 0;
}