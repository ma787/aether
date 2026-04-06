#include "aether.h"
#include "unity.h"

void test_perft_case_01(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, START_POS));
    TEST_ASSERT_EQUAL_UINT64(20ULL, perft(pstn, 1));
    TEST_ASSERT_EQUAL_UINT64(400ULL, perft(pstn, 2));
    TEST_ASSERT_EQUAL_UINT64(8902ULL, perft(pstn, 3));
    TEST_ASSERT_EQUAL_UINT64(197281ULL, perft(pstn, 4));
    TEST_ASSERT_EQUAL_UINT64(4865609ULL, perft(pstn, 5));
    free_position(pstn);
}

void test_perft_case_02(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"));
    TEST_ASSERT_EQUAL_UINT64(48ULL, perft(pstn, 1));
    TEST_ASSERT_EQUAL_UINT64(2039ULL, perft(pstn, 2));
    TEST_ASSERT_EQUAL_UINT64(97862ULL, perft(pstn, 3));
    TEST_ASSERT_EQUAL_UINT64(4085603ULL, perft(pstn, 4));
    free_position(pstn);
}

void test_perft_case_03(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"));
    TEST_ASSERT_EQUAL_UINT64(14ULL, perft(pstn, 1));
    TEST_ASSERT_EQUAL_UINT64(191ULL, perft(pstn, 2));
    TEST_ASSERT_EQUAL_UINT64(2812ULL, perft(pstn, 3));
    TEST_ASSERT_EQUAL_UINT64(43238ULL, perft(pstn, 4));
    TEST_ASSERT_EQUAL_UINT64(674624ULL, perft(pstn, 5));
    TEST_ASSERT_EQUAL_UINT64(11030083ULL, perft(pstn, 6));
    free_position(pstn);
}

void test_perft_case_04(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"));
    TEST_ASSERT_EQUAL_UINT64(6ULL, perft(pstn, 1));
    TEST_ASSERT_EQUAL_UINT64(264ULL, perft(pstn, 2));
    TEST_ASSERT_EQUAL_UINT64(9467ULL, perft(pstn, 3));
    TEST_ASSERT_EQUAL_UINT64(422333ULL, perft(pstn, 4));
    TEST_ASSERT_EQUAL_UINT64(15833292ULL, perft(pstn, 5));
    free_position(pstn);
}

void test_perft_case_05(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"));
    TEST_ASSERT_EQUAL_UINT64(44ULL, perft(pstn, 1));
    TEST_ASSERT_EQUAL_UINT64(1486ULL, perft(pstn, 2));
    TEST_ASSERT_EQUAL_UINT64(62379ULL, perft(pstn, 3));
    TEST_ASSERT_EQUAL_UINT64(2103487ULL, perft(pstn, 4));
    free_position(pstn);
}

void test_perft_case_06(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10"));
    TEST_ASSERT_EQUAL_UINT64(46ULL, perft(pstn, 1));
    TEST_ASSERT_EQUAL_UINT64(2079ULL, perft(pstn, 2));
    TEST_ASSERT_EQUAL_UINT64(89890ULL, perft(pstn, 3));
    TEST_ASSERT_EQUAL_UINT64(3894594ULL, perft(pstn, 4));
    free_position(pstn);
}

void test_count_captures_case_01(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, START_POS));
    TEST_ASSERT_EQUAL_UINT64(0ULL, count_captures(pstn, 1));
    TEST_ASSERT_EQUAL_UINT64(0ULL, count_captures(pstn, 2));
    TEST_ASSERT_EQUAL_UINT64(34ULL, count_captures(pstn, 3));
    TEST_ASSERT_EQUAL_UINT64(1576ULL, count_captures(pstn, 4));
    TEST_ASSERT_EQUAL_UINT64(82719ULL, count_captures(pstn, 5));
    free_position(pstn);
}

void test_count_captures_case_02(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"));
    TEST_ASSERT_EQUAL_UINT64(8ULL, count_captures(pstn, 1));
    TEST_ASSERT_EQUAL_UINT64(351ULL, count_captures(pstn, 2));
    TEST_ASSERT_EQUAL_UINT64(17102ULL, count_captures(pstn, 3));
    free_position(pstn);
}

void test_count_captures_case_03(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"));
    TEST_ASSERT_EQUAL_UINT64(1ULL, count_captures(pstn, 1));
    TEST_ASSERT_EQUAL_UINT64(14ULL, count_captures(pstn, 2));
    TEST_ASSERT_EQUAL_UINT64(209ULL, count_captures(pstn, 3));
    TEST_ASSERT_EQUAL_UINT64(3348ULL, count_captures(pstn, 4));
    TEST_ASSERT_EQUAL_UINT64(52051ULL, count_captures(pstn, 5));
    free_position(pstn);
}

void test_count_captures_case_04(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"));
    TEST_ASSERT_EQUAL_UINT64(0ULL, count_captures(pstn, 1));
    TEST_ASSERT_EQUAL_UINT64(87ULL, count_captures(pstn, 2));
    TEST_ASSERT_EQUAL_UINT64(1021ULL, count_captures(pstn, 3));
    TEST_ASSERT_EQUAL_UINT64(131393ULL, count_captures(pstn, 4));
    free_position(pstn);
}

void run_move_gen_tests(void) {
    RUN_TEST(test_perft_case_01);
    RUN_TEST(test_perft_case_02);
    RUN_TEST(test_perft_case_03);
    RUN_TEST(test_perft_case_04);
    RUN_TEST(test_perft_case_05);
    RUN_TEST(test_perft_case_06);
    RUN_TEST(test_count_captures_case_01);
    RUN_TEST(test_count_captures_case_02);
    RUN_TEST(test_count_captures_case_03);
    RUN_TEST(test_count_captures_case_04);
}
