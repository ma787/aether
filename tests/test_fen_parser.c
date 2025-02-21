#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#include "board.h"
#include "fen_parser.h"

#define N_TESTS 5

int test_fen_parser(char *fen_str, int *b_arr, int *attrs, char *b_str){
    printf("Testing parse of fen string '%s'\n", fen_str);
    info *pstn = parse_fen_string(fen_str, b_arr);

    if (pstn == NULL) {
        printf("Error while parsing fen string.\n");
        return -1;
    }

    int pstn_attrs[5] = {pstn->side, pstn->c_rights, pstn->ep_square, pstn->h_clk, pstn->check_info};
    char names[5][20] = {"side", "castling rights", "ep square", "halfmove clock", "check info"};

    // Testing position struct attributes
    for (int i = 0; i < 5; i++) {
        if (pstn_attrs[i] != attrs[i]) {
            printf("Attribute '%s' - Expected: %d, Actual: %d\n", names[i], attrs[i], pstn_attrs[i]);
            return -1;
        }
    }

    // Testing board layout
    char test_b_str[72];
    board_to_string(b_arr, test_b_str);

    if (strcmp(b_str, test_b_str) != 0) {
        printf("Expected board layout:\n%s\nActual:\n%s\n", b_str, test_b_str);
        return -1;
    }

    clear_board(b_arr);
    free(pstn);
    return 0;
}

int main(void) {
    int b_arr[256];
    init_board(b_arr);

    char fen_strings[N_TESTS][92] = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "rnbqkbnr/1p1ppppp/8/p1pP4/8/8/PPP1PPPP/RNBQKBNR w KQkq c6 0 3",
        "r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3 2",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"
    };

    int results[N_TESTS][5] = {
        {WHITE, 15, -1, 0, 0},
        {WHITE, 15, 0x96, 0, 0},
        {BLACK, 12, -1, 3, 0},
        {WHITE, 15, -1, 0, 0},
        {WHITE, 0, -1, 0, 0}
    };
    
    char b_strings[N_TESTS][72] = {
        "rnbqkbnr\npppppppp\n--------\n--------\n--------\n--------\nPPPPPPPP\nRNBQKBNR",
        "rnbqkbnr\n-p-ppppp\n--------\np-pP----\n--------\n--------\nPPP-PPPP\nRNBQKBNR",
        "r------r\n-b--k-bq\n--------\n--------\n-------B\n--------\n--------\nR---K--R",
        "r---k--r\np-ppqpb-\nbn--pnp-\n---PN---\n-p--P---\n--N--Q-p\nPPPBBPPP\nR---K--R",
        "--------\n--p-----\n---p----\nKP-----r\n-R---p-k\n--------\n----P-P-\n--------"
    };

    for (int i = 0; i < N_TESTS; i++) {
        if (test_fen_parser(fen_strings[i], b_arr, results[i], b_strings[i]) == 0) {
            printf("Passed Test %d\n", i+1);
        } else {
            printf("Failed Test %d\n", i+1);
        }
    }

    return 0;
}