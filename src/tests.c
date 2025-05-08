#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "aether.h"

void test_fen(POSITION *pstn, char *fen_str){
    char parsed_str[92];
    board_to_fen(pstn, parsed_str);
    assert(strcmp(fen_str, parsed_str) == 0);
}

void test_string_to_move( 
    POSITION *pstn, char *mstr, int start, int dest, int flags, 
    int cap_piece_type, int side
) {
    if (cap_piece_type) {
        cap_piece_type |= BLACK;
    }

    move_t mv = string_to_move(pstn, mstr);

    assert(
        mv.start == start 
        && mv.dest == dest 
        && mv.flags == flags
        && (mv.captured_piece & 0xFF) == cap_piece_type
        && mv.side == side
    );
}

void test_update_hash(POSITION *pstn, char *mstr) {
    move_t mv = string_to_move(pstn, mstr);
    assert(make_move(pstn, mv));
    uint64_t z_hash = pstn->key;

    bool flipped = false;

    if (pstn->side == BLACK) {
        flip_position(pstn);
        flipped = true;
    }

    set_hash(pstn);

    if (flipped) {
        flip_position(pstn);
    }

    assert(pstn->key == z_hash);
}

int main(void) {
    POSITION *pstn = new_position();

    /* update_position tests */

    assert(update_position(pstn, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
    assert(pstn->side == WHITE && pstn->c_rights == 15 && !pstn->ep_sq && !pstn->h_clk && !pstn->check);
    assert(pstn->big_pieces[WHITE] == 8 && pstn->big_pieces[BLACK] == 8);
    test_fen(pstn, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    assert(update_position(pstn, "r3k2r/p1ppqpb1/bn2pnp1/1B1PN3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R w KQkq - 0 1"));
    assert(pstn->side == WHITE && pstn->c_rights == 15 && !pstn->ep_sq && !pstn->h_clk && !pstn->check);
    assert(pstn->big_pieces[WHITE] == 8 && pstn->big_pieces[BLACK] == 8);
    test_fen(pstn, "r3k2r/p1ppqpb1/bn2pnp1/1B1PN3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R w KQkq - 0 1");
    
    assert(update_position(pstn, "rnbqkbnr/1p1ppppp/8/p1pP4/8/8/PPP1PPPP/RNBQKBNR w KQkq c6 0 1"));
    assert(pstn->side == WHITE && pstn->c_rights == 15 && pstn->ep_sq == C6 && !pstn->h_clk && !pstn->check);
    assert(pstn->big_pieces[WHITE] == 8 && pstn->big_pieces[BLACK] == 8);
    test_fen(pstn, "rnbqkbnr/1p1ppppp/8/p1pP4/8/8/PPP1PPPP/RNBQKBNR w KQkq c6 0 1");
    
    assert(update_position(pstn, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"));
    assert(pstn->side == WHITE && pstn->c_rights == 15 && !pstn->ep_sq && !pstn->h_clk && !pstn->check);
    assert(pstn->big_pieces[WHITE] == 8 && pstn->big_pieces[BLACK] == 8);
    test_fen(pstn, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

    assert(update_position(pstn, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"));
    assert(pstn->side == WHITE && !pstn->c_rights && !pstn->ep_sq && !pstn->h_clk && !pstn->check);
    assert(pstn->big_pieces[WHITE] == 2 && pstn->big_pieces[BLACK] == 2);
    test_fen(pstn, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");

    assert(update_position(pstn, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/4P3/1pN2Q1p/PPPBBPPP/R3KR2 w Qkq - 0 1"));
    assert(pstn->side == WHITE && pstn->c_rights == 13 && !pstn->ep_sq && !pstn->h_clk && !pstn->check);
    assert(pstn->big_pieces[WHITE] == 8 && pstn->big_pieces[BLACK] == 8);
    test_fen(pstn, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/4P3/1pN2Q1p/PPPBBPPP/R3KR2 w Qkq - 0 1");

    assert(update_position(pstn, "r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3 1"));
    assert(
        pstn->side == BLACK && pstn->c_rights == 12 && !pstn->ep_sq && 
        pstn->h_clk == 3 && pstn->check == DISTANT_CHECK && pstn->fst_checker == H5
    );
    test_fen(pstn, "r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3 1");

    assert(update_position(pstn, "1nbqkbnr/r2pp2p/ppp2pB1/4P2Q/3P4/2N5/PPP2PPP/R1B1K1NR b KQk - 0 1"));
    assert(
        pstn->side == BLACK && pstn->c_rights == 14 && !pstn->ep_sq
        && !pstn->h_clk && pstn->check == DISTANT_CHECK && pstn->fst_checker == G3
    );
    assert(pstn->big_pieces[WHITE] == 8 && pstn->big_pieces[BLACK] == 8);
    test_fen(pstn, "1nbqkbnr/r2pp2p/ppp2pB1/4P2Q/3P4/2N5/PPP2PPP/R1B1K1NR b KQk - 0 1");

    assert(update_position(pstn, "r2k3r/p1ppqNb1/bn2pQp1/3P4/1p2P3/2N4p/PPPBBPPP/R3K2R b KQ - 0 1"));
    assert(
        pstn->side == BLACK && pstn->c_rights == 12 && !pstn->ep_sq 
        && !pstn->h_clk && pstn->check == CONTACT_CHECK && pstn->fst_checker == F2
    );
    assert(pstn->big_pieces[WHITE] == 7 && pstn->big_pieces[BLACK] == 8);
    test_fen(pstn, "r2k3r/p1ppqNb1/bn2pQp1/3P4/1p2P3/2N4p/PPPBBPPP/R3K2R b KQ - 0 1");

    assert(update_position(pstn, "4k2r/8/8/8/8/8/8/R1r1K2R w KQk - 1 1"));
    assert(
        pstn->side == WHITE && pstn->c_rights == 11 && !pstn->ep_sq
        && pstn->h_clk == 1 && pstn->check == DISTANT_CHECK && pstn->fst_checker == C1
    );
    assert(pstn->big_pieces[WHITE] == 3 && pstn->big_pieces[BLACK] == 3);
    test_fen(pstn, "4k2r/8/8/8/8/8/8/R1r1K2R w KQk - 1 1");

    assert(update_position(pstn, "r3k2r/p1pq1Pb1/bn1p1np1/1B2N3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R b KQkq - 0 1"));
    assert(
        pstn->side == BLACK && pstn->c_rights == 15 && !pstn->ep_sq 
        && !pstn->h_clk && pstn->check == CONTACT_CHECK && pstn->fst_checker == F2
    );
    assert(pstn->big_pieces[WHITE] == 8 && pstn->big_pieces[BLACK] == 8);
    test_fen(pstn, "r3k2r/p1pq1Pb1/bn1p1np1/1B2N3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R b KQkq - 0 1");

    assert(update_position(pstn, "8/8/3p4/1Pp4r/1K3R2/6k1/4P1P1/8 w - c6 0 1"));
    assert(
        pstn->side == WHITE && !pstn->c_rights && pstn->ep_sq == C6 
        && !pstn->h_clk && pstn->check == CONTACT_CHECK && pstn->fst_checker == C5
    );
    assert(pstn->big_pieces[WHITE] == 2 && pstn->big_pieces[BLACK] == 2);
    test_fen(pstn, "8/8/3p4/1Pp4r/1K3R2/6k1/4P1P1/8 w - c6 0 1");

    assert(update_position(pstn, "rnbqkbn1/pp6/8/8/4Kr2/8/PP4b1/R3Q1BR w q - 0 1"));
    assert(pstn->side == WHITE && pstn->c_rights == 4 && !pstn->ep_sq && !pstn->h_clk);
    assert(pstn->check == DOUBLE_CHECK);
    assert(
        (pstn->fst_checker == G2 && pstn->snd_checker == F4)
        || (pstn->fst_checker == F4 && pstn->snd_checker == G2)
    );
    assert(pstn->big_pieces[WHITE] == 5 && pstn->big_pieces[BLACK] == 9);
    test_fen(pstn, "rnbqkbn1/pp6/8/8/4Kr2/8/PP4b1/R3Q1BR w q - 0 1");

    assert(update_position(pstn, "r5k1/ppp3p1/3pb3/7p/8/5r1K/PB3bPP/RN1Q3R w - - 0 1"));
    assert(pstn->side == WHITE && !pstn->c_rights && !pstn->ep_sq && !pstn->h_clk);
    assert(pstn->check == DOUBLE_CHECK);
    assert(
        (pstn->fst_checker == F3 && pstn->snd_checker == E6)
        || (pstn->fst_checker == E6 && pstn->snd_checker == F3)
    );
    assert(pstn->big_pieces[WHITE] == 6 && pstn->big_pieces[BLACK] == 5);
    test_fen(pstn, "r5k1/ppp3p1/3pb3/7p/8/5r1K/PB3bPP/RN1Q3R w - - 0 1");

    assert(update_position(pstn, "r1bq1r2/pp2n3/4N1Pk/3pPp2/1b1n2Q1/2N5/PP3PP1/R1B1K2R b KQ - 0 1"));
    assert(pstn->side == BLACK && pstn->c_rights == 12 && !pstn->ep_sq && !pstn->h_clk);
    assert(pstn->check == DOUBLE_CHECK);
    assert(
        (pstn->fst_checker == C8 && pstn->snd_checker == H8)
        || (pstn->fst_checker == H8 && pstn->snd_checker == C8)
    );
    assert(pstn->big_pieces[WHITE] == 8 && pstn->big_pieces[BLACK] == 7);
    test_fen(pstn, "r1bq1r2/pp2n3/4N1Pk/3pPp2/1b1n2Q1/2N5/PP3PP1/R1B1K2R b KQ - 0 1");

    printf("passed update_position tests\n");

    /* is_square_attacked tests */

    assert(update_position(pstn, "rnb1kbnr/1pp1pppp/p7/8/2p5/NQ1qB3/PP2PPPP/R3KBNR w KQkq - 0 1"));
    assert(is_square_attacked(pstn, D1));

    assert(update_position(pstn, "r3k2r/p1ppqpb1/bn2pnN1/3P4/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1"));
    assert(is_square_attacked(pstn, E7));

    assert(update_position(pstn, "r3k2r/Pppp1ppp/5nbN/nP6/BBP1P3/q4N2/Pp1P1bPP/R2Q2K1 w kq - 0 1"));
    assert(is_square_attacked(pstn, G1));

    assert(update_position(pstn, "r3k2r/p1ppqpb1/Bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R b KQkq - 0 1"));
    assert(!is_square_attacked(pstn, F1));

    assert(update_position(pstn, "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1"));
    assert(is_square_attacked(pstn, D1));

    printf("passed is_square_attacked tests\n");

    /* string_to_move tests */

    assert(update_position(pstn, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
    test_string_to_move(pstn, "f2f3", F2, F3, Q_FLAG, 0, WHITE);

    assert(update_position(pstn, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
    test_string_to_move(pstn, "e2e4", E2, E4, DPP_FLAG, 0, WHITE);

    assert(update_position(pstn, "rnbqkbnr/3ppppp/ppp5/8/8/3BP2N/PPPP1PPP/RNBQK2R w KQkq - 0 1"));
    test_string_to_move(pstn, "e1g1", E1, G1, K_CASTLE_FLAG, 0, WHITE);

    assert(update_position(pstn, "r3kbnr/pppqpppp/n2p4/5b2/8/PPPP3P/4PPP1/RNBQKBNR b KQkq - 0 1"));
    test_string_to_move(pstn, "e8c8", E1, C1, Q_CASTLE_FLAG, 0, BLACK);

    assert(update_position(pstn, "rnbqkbnr/1ppppppp/p7/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1"));
    test_string_to_move(pstn, "f1a6", F1, A6, CAPTURE_FLAG, PAWN, WHITE);

    assert(update_position(pstn, "rnbqkb1r/pppppppp/8/3n4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1"));
    test_string_to_move(pstn, "e4d5", E4, D5, CAPTURE_FLAG, KNIGHT, WHITE);

    assert(update_position(pstn, "rnbqkbnr/1ppp1ppp/p7/3Pp3/8/8/PPP1PPPP/RNBQKBNR w KQkq e6 0 1"));
    test_string_to_move(pstn, "d5e6", D5, E6, EP_FLAG, PAWN, WHITE);

    assert(update_position(pstn, "r1bqkbnr/pPpppp2/p1n5/6pp/8/4P3/P1PP1PPP/RNBQK1NR w KQkq - 0 1"));
    test_string_to_move(pstn, "b7b8q", B7, B8, (PROMO_FLAG + 3), 0, WHITE);

    assert(update_position(pstn, "rnbqkbnr/pp1ppppp/8/7P/8/2N5/PpPPPPP1/R1BQKBNR b KQkq - 0 1"));
    test_string_to_move(pstn, "b2a1n", B7, A8, CAPTURE_FLAG | PROMO_FLAG, ROOK, BLACK);

    printf("passed string_to_move tests\n");

    /* make_move tests */

    assert(update_position(pstn, START_POS));
    assert(make_move(pstn, string_to_move(pstn, "a2a3")));
    test_fen(pstn, "rnbqkbnr/pppppppp/8/8/8/P7/1PPPPPPP/RNBQKBNR b KQkq - 0 1");

    assert(update_position(pstn, "rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1"));
    assert(make_move(pstn, string_to_move(pstn, "e4d5")));
    test_fen(pstn, "rnbqkbnr/ppp1pppp/8/3P4/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1");

    assert(update_position(pstn, "rnbqkbnr/pppp1ppp/8/1N6/4p3/8/PPPPPPPP/R1BQKBNR w KQkq - 0 1"));
    assert(make_move(pstn, string_to_move(pstn, "f2f4")));
    test_fen(pstn, "rnbqkbnr/pppp1ppp/8/1N6/4pP2/8/PPPPP1PP/R1BQKBNR b KQkq f3 0 1");

    assert(update_position(pstn, "rnbqkbnr/1ppppppp/p7/3P4/8/8/PPP1PPPP/RNBQKBNR b KQkq - 0 1"));
    assert(make_move(pstn, string_to_move(pstn, "e7e5")));
    test_fen(pstn, "rnbqkbnr/1ppp1ppp/p7/3Pp3/8/8/PPP1PPPP/RNBQKBNR w KQkq e6 0 1");

    assert(update_position(pstn, "r1bqkbnr/pPpppp2/p1n5/6pp/8/4P3/P1PP1PPP/RNBQK1NR w KQkq - 0 1"));
    assert(make_move(pstn, string_to_move(pstn, "b7b8q")));
    test_fen(pstn, "rQbqkbnr/p1pppp2/p1n5/6pp/8/4P3/P1PP1PPP/RNBQK1NR b KQkq - 0 1");

    assert(update_position(pstn, "r1bqkbnr/pppppppp/n7/8/P7/8/1PPPPPPP/RNBQKBNR w KQkq - 1 1"));
    assert(make_move(pstn, string_to_move(pstn, "a1a2")));
    test_fen(pstn, "r1bqkbnr/pppppppp/n7/8/P7/8/RPPPPPPP/1NBQKBNR b Kkq - 2 1");

    assert(update_position(pstn, "rnbqkbnr/1ppppppp/p7/P7/8/8/1PPPPPPP/RNBQKBNR b KQkq - 0 1"));
    assert(make_move(pstn, string_to_move(pstn, "a8a7")));
    test_fen(pstn, "1nbqkbnr/rppppppp/p7/P7/8/8/1PPPPPPP/RNBQKBNR w KQk - 1 1");

    assert(update_position(pstn, "r1bqkbnr/pppppppp/n7/8/7P/8/PPPPPPP1/RNBQKBNR w KQkq - 1 1"));
    assert(make_move(pstn, string_to_move(pstn, "h1h2")));
    test_fen(pstn, "r1bqkbnr/pppppppp/n7/8/7P/8/PPPPPPPR/RNBQKBN1 b Qkq - 2 1");

    assert(update_position(pstn, "rnbqkbnr/ppppppp1/7p/7P/8/8/PPPPPPP1/RNBQKBNR b KQkq - 0 1"));
    assert(make_move(pstn, string_to_move(pstn, "h8h7")));
    test_fen(pstn, "rnbqkbn1/pppppppr/7p/7P/8/8/PPPPPPP1/RNBQKBNR w KQq - 1 1");

    assert(update_position(pstn, "r1bqkbnr/pppppppp/n7/8/8/4P3/PPPP1PPP/RNBQKBNR w KQkq - 1 1"));
    assert(make_move(pstn, string_to_move(pstn, "e1e2")));
    test_fen(pstn, "r1bqkbnr/pppppppp/n7/8/8/4P3/PPPPKPPP/RNBQ1BNR b kq - 2 1");

    assert(update_position(pstn, "rnbqkbnr/pppp1ppp/4p3/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1"));
    assert(make_move(pstn, string_to_move(pstn, "e8e7")));
    test_fen(pstn, "rnbq1bnr/ppppkppp/4p3/8/4P3/8/PPPP1PPP/RNBQKBNR w KQ - 1 1");

    assert(update_position(pstn, "rnbqkbnr/4pppp/pppp4/8/3P4/2NQB3/PPP1PPPP/R3KBNR w KQkq - 0 1"));
    assert(make_move(pstn, string_to_move(pstn, "e1c1")));
    test_fen(pstn, "rnbqkbnr/4pppp/pppp4/8/3P4/2NQB3/PPP1PPPP/2KR1BNR b kq - 1 1");

    assert(update_position(pstn, "rnbqkbnr/3ppppp/ppp5/8/8/3BP2N/PPPP1PPP/RNBQK2R w KQkq - 0 1"));
    assert(make_move(pstn, string_to_move(pstn, "e1g1")));
    test_fen(pstn, "rnbqkbnr/3ppppp/ppp5/8/8/3BP2N/PPPP1PPP/RNBQ1RK1 b kq - 1 1");

    assert(update_position(pstn, "rnbqkbnr/1ppp1ppp/p7/3Pp3/8/8/PPP1PPPP/RNBQKBNR w KQkq e6 0 1"));
    assert(make_move(pstn, string_to_move(pstn, "d5e6")));
    test_fen(pstn, "rnbqkbnr/1ppp1ppp/p3P3/8/8/8/PPP1PPPP/RNBQKBNR b KQkq - 0 1");

    printf("passed make_move tests\n");

    /* unmake_move tests */

    move_t mv;

    assert(update_position(pstn, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
    assert(make_move(pstn, mv = string_to_move(pstn, "a2a3")));
    unmake_move(pstn, mv);
    test_fen(pstn, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    assert(update_position(pstn, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1"));
    assert(make_move(pstn, mv = string_to_move(pstn, "e8d8")));
    unmake_move(pstn, mv);
    test_fen(pstn, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1");

    assert(update_position(pstn, "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1"));
    assert(make_move(pstn, mv = string_to_move(pstn, "g2g1q")));
    unmake_move(pstn, mv);
    test_fen(pstn, "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1");

    assert(update_position(pstn, "rnbqkbnr/4pppp/pppp4/8/3P4/2NQB3/PPP1PPPP/R3KBNR w KQkq - 0 1"));
    assert(make_move(pstn, mv = string_to_move(pstn, "e1c1")));
    unmake_move(pstn, mv);
    test_fen(pstn, "rnbqkbnr/4pppp/pppp4/8/3P4/2NQB3/PPP1PPPP/R3KBNR w KQkq - 0 1");

    assert(update_position(pstn, "rnbqkbnr/3ppppp/ppp5/8/8/3BP2N/PPPP1PPP/RNBQK2R w KQkq - 0 1"));
    assert(make_move(pstn, mv = string_to_move(pstn, "e1g1")));
    unmake_move(pstn, mv);
    test_fen(pstn, "rnbqkbnr/3ppppp/ppp5/8/8/3BP2N/PPPP1PPP/RNBQK2R w KQkq - 0 1");

    assert(update_position(pstn, "r1bqkbnr/pppppppp/n7/8/P7/8/1PPPPPPP/RNBQKBNR w KQkq - 1 1"));
    assert(make_move(pstn, mv = string_to_move(pstn, "a1a2")));
    unmake_move(pstn, mv);
    test_fen(pstn, "r1bqkbnr/pppppppp/n7/8/P7/8/1PPPPPPP/RNBQKBNR w KQkq - 1 1");

    assert(update_position(pstn, "rnbqkbnr/1ppppppp/p7/P7/8/8/1PPPPPPP/RNBQKBNR b KQkq - 0 1"));
    assert(make_move(pstn, mv = string_to_move(pstn, "a8a7")));
    unmake_move(pstn, mv);
    test_fen(pstn, "rnbqkbnr/1ppppppp/p7/P7/8/8/1PPPPPPP/RNBQKBNR b KQkq - 0 1");

    assert(update_position(pstn, "r1bqkbnr/pppppppp/n7/8/7P/8/PPPPPPP1/RNBQKBNR w KQkq - 1 1"));
    assert(make_move(pstn, mv = string_to_move(pstn, "h1h2")));
    unmake_move(pstn, mv);
    test_fen(pstn, "r1bqkbnr/pppppppp/n7/8/7P/8/PPPPPPP1/RNBQKBNR w KQkq - 1 1");

    assert(update_position(pstn, "rnbqkbnr/ppppppp1/7p/7P/8/8/PPPPPPP1/RNBQKBNR b KQkq - 0 1"));
    assert(make_move(pstn, mv = string_to_move(pstn, "h8h7")));
    unmake_move(pstn, mv);
    test_fen(pstn, "rnbqkbnr/ppppppp1/7p/7P/8/8/PPPPPPP1/RNBQKBNR b KQkq - 0 1");

    assert(update_position(pstn, "r1bqkbnr/pppppppp/n7/8/8/4P3/PPPP1PPP/RNBQKBNR w KQkq - 1 1"));
    assert(make_move(pstn, mv = string_to_move(pstn, "e1e2")));
    unmake_move(pstn, mv);
    test_fen(pstn, "r1bqkbnr/pppppppp/n7/8/8/4P3/PPPP1PPP/RNBQKBNR w KQkq - 1 1");

    assert(update_position(pstn, "rnbqkbnr/pppp1ppp/4p3/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1"));
    assert(make_move(pstn, mv = string_to_move(pstn, "e8e7")));
    unmake_move(pstn, mv);
    test_fen(pstn, "rnbqkbnr/pppp1ppp/4p3/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1");

    assert(update_position(pstn, "r1bqkbnr/p1pppppp/n7/1p6/P6P/8/1PPPPPP1/RNBQKBNR w Qkq - 0 1"));
    assert(make_move(pstn, mv = string_to_move(pstn, "a1a2")));
    unmake_move(pstn, mv);
    test_fen(pstn, "r1bqkbnr/p1pppppp/n7/1p6/P6P/8/1PPPPPP1/RNBQKBNR w Qkq - 0 1");

    assert(update_position(pstn, "rnbqkbnr/1ppppppp/8/8/p7/4P3/PPPP1PPP/RNBQKBNR w kq - 0 1"));
    assert(make_move(pstn, mv = string_to_move(pstn, "e1e2")));
    unmake_move(pstn, mv);
    test_fen(pstn, "rnbqkbnr/1ppppppp/8/8/p7/4P3/PPPP1PPP/RNBQKBNR w kq - 0 1");

    assert(update_position(pstn, "rn1qkbn1/ppp1ppp1/3p3r/P7/6b1/3P4/1PP1PPP1/RN1QKBNR b KQq - 0 1"));
    assert(make_move(pstn, mv = string_to_move(pstn, "h6h1")));
    unmake_move(pstn, mv);
    test_fen(pstn, "rn1qkbn1/ppp1ppp1/3p3r/P7/6b1/3P4/1PP1PPP1/RN1QKBNR b KQq - 0 1");

    assert(update_position(pstn, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"));
    assert(make_move(pstn, mv = string_to_move(pstn, "d5e6")));
    unmake_move(pstn, mv);
    test_fen(pstn, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

    printf("passed unmake_move tests\n");

    /* update_check tests */

    assert(update_position(pstn, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"));
    assert(make_move(pstn, string_to_move(pstn, "f3h5")));
    assert(pstn->check == NO_CHECK);

    assert(update_position(pstn, "rnbqkbnr/pp2pppp/2pp4/8/Q7/2P5/PP1PPPPP/RNB1KBNR w KQkq - 0 1"));
    assert(make_move(pstn, string_to_move(pstn, "a4c6")));
    assert(pstn->check == DISTANT_CHECK && pstn->fst_checker == C3);

    assert(update_position(pstn, "Q7/1PP5/2k5/8/8/8/4Kppp/8 w - - 1 1"));
    assert(make_move(pstn, string_to_move(pstn, "b7b8q")));
    assert(pstn->check == DISTANT_CHECK && pstn->fst_checker == A1);

    assert(update_position(pstn, "r3k2r/p1p1qpb1/bn1ppnp1/1B1PN3/1p2P3/2N2Q1p/PPPB1PPP/R4K1R b kq - 1 1"));
    assert(make_move(pstn, string_to_move(pstn, "a6b5")));
    assert(pstn->check == DISTANT_CHECK && pstn->fst_checker == B5);

    assert(update_position(pstn, "rnbqkbnr/pppp1ppp/8/8/4P3/5N2/PpP2PPP/R1BQKB1R w KQkq - 0 1"));
    assert(make_move(pstn, string_to_move(pstn, "d1d7")));
    assert(pstn->check == CONTACT_CHECK && pstn->fst_checker == D2);

    assert(update_position(pstn, "rnbqkbnr/pppp1ppp/8/1Q6/4P3/5N2/PpP2PPP/R1BQKB1R w KQkq - 0 1"));
    assert(make_move(pstn, string_to_move(pstn, "d1d7")));
    assert(pstn->check == CONTACT_CHECK && pstn->fst_checker == D2);

    assert(update_position(pstn, "r6r/Pp1pkppp/1P3nbN/nPp5/BB2P3/q4N2/Pp1P2PP/R2Q1RK1 w - c6 0 1"));
    assert(make_move(pstn, string_to_move(pstn, "b5c6")));
    assert(pstn->check == DISTANT_CHECK && pstn->fst_checker == B5);

    assert(update_position(pstn, "2kr3r/p2pqpb1/bn2pnp1/2pPN3/1p2P3/2Q4p/PPPBBPPP/RN2K2R w KQ c6 0 1"));
    assert(make_move(pstn, string_to_move(pstn, "d5c6")));
    assert(pstn->check == NO_CHECK);

    assert(update_position(pstn, "4k2r/8/8/8/8/8/8/5K2 b k - 1 1"));
    assert(make_move(pstn, string_to_move(pstn, "e8g8")));
    assert(pstn->check == DISTANT_CHECK && pstn->fst_checker == F8);

    assert(update_position(pstn, "k7/B7/1B6/1B6/8/8/8/K6b w - - 0 1"));
    assert(make_move(pstn, string_to_move(pstn, "b5c6")));
    assert(pstn->check == DISTANT_CHECK && pstn->fst_checker == C3);

    assert(update_position(pstn, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R4K1R b kq - 1 1"));
    assert(make_move(pstn, string_to_move(pstn, "h3g2")));
    assert(pstn->check == CONTACT_CHECK && pstn->fst_checker == G2);

    printf("passed update_check tests\n");

    /* update_hash tests */

    assert(update_position(pstn, START_POS));
    test_update_hash(pstn, "a2a3");

    assert(update_position(pstn, "rnbqkbnr/pppppppp/8/8/8/P7/1PPPPPPP/RNBQKBNR b KQkq - 0 1"));
    test_update_hash(pstn, "e7e6");

    assert(update_position(pstn, START_POS));
    test_update_hash(pstn, "b1c3");

    assert(update_position(pstn, START_POS));
    test_update_hash(pstn, "b1a3");

    assert(update_position(pstn, "rnbqkbnr/p1pppppp/8/1p6/8/2N5/PPPPPPPP/R1BQKBNR w KQkq b6 0 1"));
    test_update_hash(pstn, "c3b5");

    assert(update_position(pstn, "rnbqkbnr/ppppppp1/7p/2P5/8/8/PP1PPPPP/RNBQKBNR b KQkq - 0 1"));
    test_update_hash(pstn, "b7b5");

    assert(update_position(pstn, "rnbqkbnr/1ppp1ppp/p7/3Pp3/8/8/PPP1PPPP/RNBQKBNR w KQkq e6 0 1"));
    test_update_hash(pstn, "d5e6");

    assert(update_position(pstn, "rnbqkbnr/p1pppppp/8/8/Pp6/6PP/1PPPPP2/RNBQKBNR b KQkq a3 0 1"));
    test_update_hash(pstn, "b4a3");

    assert(update_position(pstn, "rnbqkbnr/4pppp/pppp4/8/3P4/2NQB3/PPP1PPPP/R3KBNR w KQkq - 0 1"));
    test_update_hash(pstn, "e1c1");

    assert(update_position(pstn, "rnbqkbnr/3ppppp/ppp5/8/8/3BP2N/PPPP1PPP/RNBQK2R w KQkq - 0 1"));
    test_update_hash(pstn, "e1g1");

    assert(update_position(pstn, "r1bqkbnr/pPpppp2/p1n5/6pp/8/4P3/P1PP1PPP/RNBQK1NR w KQkq - 0 1"));
    test_update_hash(pstn, "b7b8q");

    printf("passed update_hash tests\n");

    // move_exists tests

    assert(update_position(pstn, START_POS));
    assert(move_exists(pstn, string_to_move(pstn, "b1a3")));

    printf("passed move_exists tests\n");

    // gen_captures tests

    assert(update_position(pstn, START_POS));
    assert(count_captures(pstn, 1) == 0ULL);
    assert(count_captures(pstn, 2) == 0ULL);
    assert(count_captures(pstn, 3) == 34ULL);
    assert(count_captures(pstn, 4) == 1576ULL);
    assert(count_captures(pstn, 5) == 82719ULL);

    assert(update_position(pstn, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"));
    assert(count_captures(pstn, 1) == 8ULL);
    assert(count_captures(pstn, 2) == 351ULL);
    assert(count_captures(pstn, 3) == 17102ULL);

    assert(update_position(pstn, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"));
    assert(count_captures(pstn, 1) == 1ULL);
    assert(count_captures(pstn, 2) == 14ULL);
    assert(count_captures(pstn, 3) == 209ULL);
    assert(count_captures(pstn, 4) == 3348ULL);
    assert(count_captures(pstn, 5) == 52051ULL);

    assert(update_position(pstn, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"));
    assert(count_captures(pstn, 1) == 0ULL);
    assert(count_captures(pstn, 2) == 87ULL);
    assert(count_captures(pstn, 3) == 1021ULL);
    assert(count_captures(pstn, 4) == 131393ULL);

    printf("passed gen_captures tests\n");

    free_position(pstn);
    return 0;
}