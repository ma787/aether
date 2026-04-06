#include "aether.h"
#include "unity.h"

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
    RUN_TEST(test_count_captures_case_01);
    RUN_TEST(test_count_captures_case_02);
    RUN_TEST(test_count_captures_case_03);
    RUN_TEST(test_count_captures_case_04);
}
