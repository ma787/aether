#include "aether.h"
#include "unity.h"

static void assert_fen(POSITION *pstn, const char *fen_str) {
    char parsed_str[92];
    board_to_fen(pstn, parsed_str);
    TEST_ASSERT_EQUAL_STRING(fen_str, parsed_str);
}

void test_update_position_case_01(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
    TEST_ASSERT_TRUE(pstn->side == WHITE && pstn->c_rights == 15 && !pstn->ep_sq && !pstn->h_clk && !pstn->check);
    TEST_ASSERT_TRUE(pstn->big_pieces[WHITE] == 8 && pstn->big_pieces[BLACK] == 8);
    assert_fen(pstn, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    free_position(pstn);
}

void test_update_position_case_02(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r3k2r/p1ppqpb1/bn2pnp1/1B1PN3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R w KQkq - 0 1"));
    TEST_ASSERT_TRUE(pstn->side == WHITE && pstn->c_rights == 15 && !pstn->ep_sq && !pstn->h_clk && !pstn->check);
    TEST_ASSERT_TRUE(pstn->big_pieces[WHITE] == 8 && pstn->big_pieces[BLACK] == 8);
    assert_fen(pstn, "r3k2r/p1ppqpb1/bn2pnp1/1B1PN3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R w KQkq - 0 1");
    free_position(pstn);
}

void test_update_position_case_03(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/1p1ppppp/8/p1pP4/8/8/PPP1PPPP/RNBQKBNR w KQkq c6 0 1"));
    TEST_ASSERT_TRUE(pstn->side == WHITE && pstn->c_rights == 15 && pstn->ep_sq == C6 && !pstn->h_clk && !pstn->check);
    TEST_ASSERT_TRUE(pstn->big_pieces[WHITE] == 8 && pstn->big_pieces[BLACK] == 8);
    assert_fen(pstn, "rnbqkbnr/1p1ppppp/8/p1pP4/8/8/PPP1PPPP/RNBQKBNR w KQkq c6 0 1");
    free_position(pstn);
}

void test_update_position_case_04(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"));
    TEST_ASSERT_TRUE(pstn->side == WHITE && pstn->c_rights == 15 && !pstn->ep_sq && !pstn->h_clk && !pstn->check);
    TEST_ASSERT_TRUE(pstn->big_pieces[WHITE] == 8 && pstn->big_pieces[BLACK] == 8);
    assert_fen(pstn, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    free_position(pstn);
}

void test_update_position_case_05(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"));
    TEST_ASSERT_TRUE(pstn->side == WHITE && !pstn->c_rights && !pstn->ep_sq && !pstn->h_clk && !pstn->check);
    TEST_ASSERT_TRUE(pstn->big_pieces[WHITE] == 2 && pstn->big_pieces[BLACK] == 2);
    assert_fen(pstn, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
    free_position(pstn);
}

void test_update_position_case_06(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/4P3/1pN2Q1p/PPPBBPPP/R3KR2 w Qkq - 0 1"));
    TEST_ASSERT_TRUE(pstn->side == WHITE && pstn->c_rights == 7 && !pstn->ep_sq && !pstn->h_clk && !pstn->check);
    TEST_ASSERT_TRUE(pstn->big_pieces[WHITE] == 8 && pstn->big_pieces[BLACK] == 8);
    assert_fen(pstn, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/4P3/1pN2Q1p/PPPBBPPP/R3KR2 w Qkq - 0 1");
    free_position(pstn);
}

void test_update_position_case_07(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "k7/8/3p4/8/3P4/8/8/7K b - - 0 1"));
    TEST_ASSERT_TRUE(pstn->side == BLACK && !pstn->c_rights && !pstn->ep_sq && !pstn->h_clk && !pstn->check);
    TEST_ASSERT_TRUE(pstn->big_pieces[WHITE] == 1 && pstn->big_pieces[BLACK] == 1);
    assert_fen(pstn, "k7/8/3p4/8/3P4/8/8/7K b - - 0 1");
    free_position(pstn);
}

void test_update_position_case_08(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3 1"));
    TEST_ASSERT_TRUE(
        pstn->side == BLACK && pstn->c_rights == 12 && !pstn->ep_sq &&
        pstn->h_clk == 3 && pstn->check == DISTANT_CHECK && pstn->fst_checker == H4
    );
    TEST_ASSERT_TRUE(pstn->big_pieces[WHITE] == 4 && pstn->big_pieces[BLACK] == 6);
    assert_fen(pstn, "r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3 1");
    free_position(pstn);
}

void test_update_position_case_09(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "1nbqkbnr/r2pp2p/ppp2pB1/4P2Q/3P4/2N5/PPP2PPP/R1B1K1NR b KQk - 0 1"));
    TEST_ASSERT_TRUE(
        pstn->side == BLACK && pstn->c_rights == 14 && !pstn->ep_sq &&
        !pstn->h_clk && pstn->check == DISTANT_CHECK && pstn->fst_checker == G6
    );
    TEST_ASSERT_TRUE(pstn->big_pieces[WHITE] == 8 && pstn->big_pieces[BLACK] == 8);
    assert_fen(pstn, "1nbqkbnr/r2pp2p/ppp2pB1/4P2Q/3P4/2N5/PPP2PPP/R1B1K1NR b KQk - 0 1");
    free_position(pstn);
}

void test_update_position_case_10(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r2k3r/p1ppqNb1/bn2pQp1/3P4/1p2P3/2N4p/PPPBBPPP/R3K2R b KQ - 0 1"));
    TEST_ASSERT_TRUE(
        pstn->side == BLACK && pstn->c_rights == 12 && !pstn->ep_sq &&
        !pstn->h_clk && pstn->check == CONTACT_CHECK && pstn->fst_checker == F7
    );
    TEST_ASSERT_TRUE(pstn->big_pieces[WHITE] == 8 && pstn->big_pieces[BLACK] == 7);
    assert_fen(pstn, "r2k3r/p1ppqNb1/bn2pQp1/3P4/1p2P3/2N4p/PPPBBPPP/R3K2R b KQ - 0 1");
    free_position(pstn);
}

void test_update_position_case_11(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "4k2r/8/8/8/8/8/8/R1r1K2R w KQk - 1 1"));
    TEST_ASSERT_TRUE(
        pstn->side == WHITE && pstn->c_rights == 14 && !pstn->ep_sq &&
        pstn->h_clk == 1 && pstn->check == DISTANT_CHECK && pstn->fst_checker == C1
    );
    TEST_ASSERT_TRUE(pstn->big_pieces[WHITE] == 3 && pstn->big_pieces[BLACK] == 3);
    assert_fen(pstn, "4k2r/8/8/8/8/8/8/R1r1K2R w KQk - 1 1");
    free_position(pstn);
}

void test_update_position_case_12(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r3k2r/p1pq1Pb1/bn1p1np1/1B2N3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R b KQkq - 0 1"));
    TEST_ASSERT_TRUE(
        pstn->side == BLACK && pstn->c_rights == 15 && !pstn->ep_sq &&
        !pstn->h_clk && pstn->check == CONTACT_CHECK && pstn->fst_checker == F7
    );
    TEST_ASSERT_TRUE(pstn->big_pieces[WHITE] == 8 && pstn->big_pieces[BLACK] == 8);
    assert_fen(pstn, "r3k2r/p1pq1Pb1/bn1p1np1/1B2N3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R b KQkq - 0 1");
    free_position(pstn);
}

void test_update_position_case_13(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "8/8/3p4/1Pp4r/1K3R2/6k1/4P1P1/8 w - c6 0 1"));
    TEST_ASSERT_TRUE(
        pstn->side == WHITE && !pstn->c_rights && pstn->ep_sq == C6 &&
        !pstn->h_clk && pstn->check == CONTACT_CHECK && pstn->fst_checker == C5
    );
    TEST_ASSERT_TRUE(pstn->big_pieces[WHITE] == 2 && pstn->big_pieces[BLACK] == 2);
    assert_fen(pstn, "8/8/3p4/1Pp4r/1K3R2/6k1/4P1P1/8 w - c6 0 1");
    free_position(pstn);
}

void test_update_position_case_14(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbn1/pp6/8/8/4Kr2/8/PP4b1/R3Q1BR w q - 0 1"));
    TEST_ASSERT_TRUE(pstn->side == WHITE && pstn->c_rights == 1 && !pstn->ep_sq && !pstn->h_clk);
    TEST_ASSERT_EQUAL_INT(DOUBLE_CHECK, pstn->check);
    TEST_ASSERT_TRUE(
        (pstn->fst_checker == G2 && pstn->snd_checker == F4) ||
        (pstn->fst_checker == F4 && pstn->snd_checker == G2)
    );
    TEST_ASSERT_TRUE(pstn->big_pieces[WHITE] == 5 && pstn->big_pieces[BLACK] == 9);
    assert_fen(pstn, "rnbqkbn1/pp6/8/8/4Kr2/8/PP4b1/R3Q1BR w q - 0 1");
    free_position(pstn);
}

void test_update_position_case_15(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r5k1/ppp3p1/3pb3/7p/8/5r1K/PB3bPP/RN1Q3R w - - 0 1"));
    TEST_ASSERT_TRUE(pstn->side == WHITE && !pstn->c_rights && !pstn->ep_sq && !pstn->h_clk);
    TEST_ASSERT_EQUAL_INT(DOUBLE_CHECK, pstn->check);
    TEST_ASSERT_TRUE(
        (pstn->fst_checker == F3 && pstn->snd_checker == E6) ||
        (pstn->fst_checker == E6 && pstn->snd_checker == F3)
    );
    TEST_ASSERT_TRUE(pstn->big_pieces[WHITE] == 6 && pstn->big_pieces[BLACK] == 5);
    assert_fen(pstn, "r5k1/ppp3p1/3pb3/7p/8/5r1K/PB3bPP/RN1Q3R w - - 0 1");
    free_position(pstn);
}

void test_update_position_case_16(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r1bq1r2/pp2n3/4N1Pk/3pPp2/1b1n2Q1/2N5/PP3PP1/R1B1K2R b KQ - 0 1"));
    TEST_ASSERT_TRUE(pstn->side == BLACK && pstn->c_rights == 12 && !pstn->ep_sq && !pstn->h_clk);
    TEST_ASSERT_EQUAL_INT(DOUBLE_CHECK, pstn->check);
    TEST_ASSERT_TRUE(
        (pstn->fst_checker == C1 && pstn->snd_checker == H1) ||
        (pstn->fst_checker == H1 && pstn->snd_checker == C1)
    );
    TEST_ASSERT_TRUE(pstn->big_pieces[WHITE] == 7 && pstn->big_pieces[BLACK] == 8);
    assert_fen(pstn, "r1bq1r2/pp2n3/4N1Pk/3pPp2/1b1n2Q1/2N5/PP3PP1/R1B1K2R b KQ - 0 1");
    free_position(pstn);
}

void test_is_square_attacked_case_01(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnb1kbnr/1pp1pppp/p7/8/2p5/NQ1qB3/PP2PPPP/R3KBNR w KQkq - 0 1"));
    TEST_ASSERT_TRUE(is_square_attacked(pstn, D1, BLACK));
    free_position(pstn);
}

void test_is_square_attacked_case_02(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r3k2r/p1ppqpb1/bn2pnN1/3P4/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1"));
    TEST_ASSERT_TRUE(is_square_attacked(pstn, E2, WHITE));
    free_position(pstn);
}

void test_is_square_attacked_case_03(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r3k2r/Pppp1ppp/5nbN/nP6/BBP1P3/q4N2/Pp1P1bPP/R2Q2K1 w kq - 0 1"));
    TEST_ASSERT_TRUE(is_square_attacked(pstn, G1, BLACK));
    free_position(pstn);
}

void test_is_square_attacked_case_04(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r3k2r/p1ppqpb1/Bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R b KQkq - 0 1"));
    TEST_ASSERT_FALSE(is_square_attacked(pstn, F8, WHITE));
    free_position(pstn);
}

void test_is_square_attacked_case_05(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1"));
    TEST_ASSERT_TRUE(is_square_attacked(pstn, D8, WHITE));
    free_position(pstn);
}

void run_position_tests(void) {
    RUN_TEST(test_update_position_case_01);
    RUN_TEST(test_update_position_case_02);
    RUN_TEST(test_update_position_case_03);
    RUN_TEST(test_update_position_case_04);
    RUN_TEST(test_update_position_case_05);
    RUN_TEST(test_update_position_case_06);
    RUN_TEST(test_update_position_case_07);
    RUN_TEST(test_update_position_case_08);
    RUN_TEST(test_update_position_case_09);
    RUN_TEST(test_update_position_case_10);
    RUN_TEST(test_update_position_case_11);
    RUN_TEST(test_update_position_case_12);
    RUN_TEST(test_update_position_case_13);
    RUN_TEST(test_update_position_case_14);
    RUN_TEST(test_update_position_case_15);
    RUN_TEST(test_update_position_case_16);
    RUN_TEST(test_is_square_attacked_case_01);
    RUN_TEST(test_is_square_attacked_case_02);
    RUN_TEST(test_is_square_attacked_case_03);
    RUN_TEST(test_is_square_attacked_case_04);
    RUN_TEST(test_is_square_attacked_case_05);
}