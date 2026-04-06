#include "aether.h"
#include "unity.h"

static void assert_hash_consistent_after_move(POSITION *pstn, char *mstr) {
    move_t mv = string_to_move(pstn, mstr);
    TEST_ASSERT_TRUE(make_move(pstn, mv));
    uint64_t z_hash = pstn->key;
    set_hash(pstn);
    TEST_ASSERT_EQUAL_UINT64(z_hash, pstn->key);
}

void test_hash_a2a3_from_start(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, START_POS));
    assert_hash_consistent_after_move(pstn, "a2a3");
    free_position(pstn);
}

void test_hash_e7e6(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/pppppppp/8/8/8/P7/1PPPPPPP/RNBQKBNR b KQkq - 0 1"));
    assert_hash_consistent_after_move(pstn, "e7e6");
    free_position(pstn);
}

void test_hash_b1c3_from_start(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, START_POS));
    assert_hash_consistent_after_move(pstn, "b1c3");
    free_position(pstn);
}

void test_hash_b1a3_from_start(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, START_POS));
    assert_hash_consistent_after_move(pstn, "b1a3");
    free_position(pstn);
}

void test_hash_c3b5(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/p1pppppp/8/1p6/8/2N5/PPPPPPPP/R1BQKBNR w KQkq b6 0 1"));
    assert_hash_consistent_after_move(pstn, "c3b5");
    free_position(pstn);
}

void test_hash_b7b5(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/ppppppp1/7p/2P5/8/8/PP1PPPPP/RNBQKBNR b KQkq - 0 1"));
    assert_hash_consistent_after_move(pstn, "b7b5");
    free_position(pstn);
}

void test_hash_d5e6_en_passant(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/1ppp1ppp/p7/3Pp3/8/8/PPP1PPPP/RNBQKBNR w KQkq e6 0 1"));
    assert_hash_consistent_after_move(pstn, "d5e6");
    free_position(pstn);
}

void test_hash_b4a3(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/p1pppppp/8/8/Pp6/6PP/1PPPPP2/RNBQKBNR b KQkq a3 0 1"));
    assert_hash_consistent_after_move(pstn, "b4a3");
    free_position(pstn);
}

void test_hash_e1c1(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/4pppp/pppp4/8/3P4/2NQB3/PPP1PPPP/R3KBNR w KQkq - 0 1"));
    assert_hash_consistent_after_move(pstn, "e1c1");
    free_position(pstn);
}

void test_hash_e1g1(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/3ppppp/ppp5/8/8/3BP2N/PPPP1PPP/RNBQK2R w KQkq - 0 1"));
    assert_hash_consistent_after_move(pstn, "e1g1");
    free_position(pstn);
}

void test_hash_b7b8q(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r1bqkbnr/pPpppp2/p1n5/6pp/8/4P3/P1PP1PPP/RNBQK1NR w KQkq - 0 1"));
    assert_hash_consistent_after_move(pstn, "b7b8q");
    free_position(pstn);
}

void test_hash_h1h2(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "4k3/8/8/8/8/8/8/4K2R w K - 0 1"));
    assert_hash_consistent_after_move(pstn, "h1h2");
    free_position(pstn);
}

void run_hash_tests(void) {
    RUN_TEST(test_hash_a2a3_from_start);
    RUN_TEST(test_hash_e7e6);
    RUN_TEST(test_hash_b1c3_from_start);
    RUN_TEST(test_hash_b1a3_from_start);
    RUN_TEST(test_hash_c3b5);
    RUN_TEST(test_hash_b7b5);
    RUN_TEST(test_hash_d5e6_en_passant);
    RUN_TEST(test_hash_b4a3);
    RUN_TEST(test_hash_e1c1);
    RUN_TEST(test_hash_e1g1);
    RUN_TEST(test_hash_b7b8q);
    RUN_TEST(test_hash_h1h2);
}
