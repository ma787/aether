#include "aether.h"
#include "unity.h"

void test_check_move_f3h5_no_check(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, string_to_move(pstn, "f3h5")));
    TEST_ASSERT_EQUAL_INT(NO_CHECK, pstn->check);
    free_position(pstn);
}

void test_check_move_a4c6_distant(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/pp2pppp/2pp4/8/Q7/2P5/PP1PPPPP/RNB1KBNR w KQkq - 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, string_to_move(pstn, "a4c6")));
    TEST_ASSERT_EQUAL_INT(DISTANT_CHECK, pstn->check);
    TEST_ASSERT_EQUAL_INT(C6, pstn->fst_checker);
    free_position(pstn);
}

void test_check_move_b7b8q_distant(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "Q7/1PP5/2k5/8/8/8/4Kppp/8 w - - 1 1"));
    TEST_ASSERT_TRUE(make_move(pstn, string_to_move(pstn, "b7b8q")));
    TEST_ASSERT_EQUAL_INT(DISTANT_CHECK, pstn->check);
    TEST_ASSERT_EQUAL_INT(A8, pstn->fst_checker);
    free_position(pstn);
}

void test_check_move_a6b5_distant(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r3k2r/p1p1qpb1/bn1ppnp1/1B1PN3/1p2P3/2N2Q1p/PPPB1PPP/R4K1R b kq - 1 1"));
    TEST_ASSERT_TRUE(make_move(pstn, string_to_move(pstn, "a6b5")));
    TEST_ASSERT_EQUAL_INT(DISTANT_CHECK, pstn->check);
    TEST_ASSERT_EQUAL_INT(B5, pstn->fst_checker);
    free_position(pstn);
}

void test_check_move_d1d7_contact_queen_file_d(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/pppp1ppp/8/8/4P3/5N2/PpP2PPP/R1BQKB1R w KQkq - 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, string_to_move(pstn, "d1d7")));
    TEST_ASSERT_EQUAL_INT(CONTACT_CHECK, pstn->check);
    TEST_ASSERT_EQUAL_INT(D7, pstn->fst_checker);
    free_position(pstn);
}

void test_check_move_d1d7_contact_queen_file_b(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/pppp1ppp/8/1Q6/4P3/5N2/PpP2PPP/R1BQKB1R w KQkq - 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, string_to_move(pstn, "d1d7")));
    TEST_ASSERT_EQUAL_INT(CONTACT_CHECK, pstn->check);
    TEST_ASSERT_EQUAL_INT(D7, pstn->fst_checker);
    free_position(pstn);
}

void test_check_move_b5c6_distant_b4(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r6r/Pp1pkppp/1P3nbN/nPp5/BB2P3/q4N2/Pp1P2PP/R2Q1RK1 w - c6 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, string_to_move(pstn, "b5c6")));
    TEST_ASSERT_EQUAL_INT(DISTANT_CHECK, pstn->check);
    TEST_ASSERT_EQUAL_INT(B4, pstn->fst_checker);
    free_position(pstn);
}

void test_check_move_d5c6_no_check(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "2kr3r/p2pqpb1/bn2pnp1/2pPN3/1p2P3/2Q4p/PPPBBPPP/RN2K2R w KQ c6 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, string_to_move(pstn, "d5c6")));
    TEST_ASSERT_EQUAL_INT(NO_CHECK, pstn->check);
    free_position(pstn);
}

void test_check_move_black_castle_g8_distant_f8(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "4k2r/8/8/8/8/8/8/5K2 b k - 1 1"));
    TEST_ASSERT_TRUE(make_move(pstn, string_to_move(pstn, "e8g8")));
    TEST_ASSERT_EQUAL_INT(DISTANT_CHECK, pstn->check);
    TEST_ASSERT_EQUAL_INT(F8, pstn->fst_checker);
    free_position(pstn);
}

void test_check_move_b5c6_bishop_distant_c6(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "k7/B7/1B6/1B6/8/8/8/K6b w - - 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, string_to_move(pstn, "b5c6")));
    TEST_ASSERT_EQUAL_INT(DISTANT_CHECK, pstn->check);
    TEST_ASSERT_EQUAL_INT(C6, pstn->fst_checker);
    free_position(pstn);
}

void test_check_move_h3g2_contact(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R4K1R b kq - 1 1"));
    TEST_ASSERT_TRUE(make_move(pstn, string_to_move(pstn, "h3g2")));
    TEST_ASSERT_EQUAL_INT(CONTACT_CHECK, pstn->check);
    TEST_ASSERT_EQUAL_INT(G2, pstn->fst_checker);
    free_position(pstn);
}

void run_check_tests(void) {
    RUN_TEST(test_check_move_f3h5_no_check);
    RUN_TEST(test_check_move_a4c6_distant);
    RUN_TEST(test_check_move_b7b8q_distant);
    RUN_TEST(test_check_move_a6b5_distant);
    RUN_TEST(test_check_move_d1d7_contact_queen_file_d);
    RUN_TEST(test_check_move_d1d7_contact_queen_file_b);
    RUN_TEST(test_check_move_b5c6_distant_b4);
    RUN_TEST(test_check_move_d5c6_no_check);
    RUN_TEST(test_check_move_black_castle_g8_distant_f8);
    RUN_TEST(test_check_move_b5c6_bishop_distant_c6);
    RUN_TEST(test_check_move_h3g2_contact);
}
