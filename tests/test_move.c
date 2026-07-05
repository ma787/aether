#include "aether.h"
#include "unity.h"

static void assert_fen(POSITION *pstn, const char *fen_str) {
    char parsed_str[92];
    board_to_fen(pstn, parsed_str);
    TEST_ASSERT_EQUAL_STRING(fen_str, parsed_str);
}

static bool string_to_move_valid(
    POSITION *pstn, char *mstr, int start, int dest, int flags, int cap_piece_type
) {
    move_t mv = string_to_move(pstn, mstr);
    return !is_null_move(mv)
        && mv.start == start
        && mv.dest == dest
        && mv.flags == flags
        && (mv.captured_piece & 0xFF) == cap_piece_type;
}

void test_string_to_move_f2f3(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
    TEST_ASSERT_TRUE(string_to_move_valid(pstn, "f2f3", F2, F3, Q_FLAG, 0));
    free_position(pstn);
}

void test_string_to_move_e2e4_and_whitespace(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
    TEST_ASSERT_TRUE(string_to_move_valid(pstn, "e2e4", E2, E4, DPP_FLAG, 0));
    TEST_ASSERT_TRUE(string_to_move_valid(pstn, "e2e4 ", E2, E4, DPP_FLAG, 0));
    TEST_ASSERT_TRUE(string_to_move_valid(pstn, "e2e4\n", E2, E4, DPP_FLAG, 0));
    free_position(pstn);
}

void test_string_to_move_white_kingside_castle(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/3ppppp/ppp5/8/8/3BP2N/PPPP1PPP/RNBQK2R w KQkq - 0 1"));
    TEST_ASSERT_TRUE(string_to_move_valid(pstn, "e1g1", E1, G1, K_CASTLE_FLAG, 0));
    free_position(pstn);
}

void test_string_to_move_black_queenside_castle(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r3kbnr/pppqpppp/n2p4/5b2/8/PPPP3P/4PPP1/RNBQKBNR b KQkq - 0 1"));
    TEST_ASSERT_TRUE(string_to_move_valid(pstn, "e8c8", E8, C8, Q_CASTLE_FLAG, 0));
    free_position(pstn);
}

void test_string_to_move_bishop_capture(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/1ppppppp/p7/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1"));
    TEST_ASSERT_TRUE(string_to_move_valid(pstn, "f1a6", F1, A6, CAPTURE_FLAG, BLACK | PAWN));
    free_position(pstn);
}

void test_string_to_move_knight_capture(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkb1r/pppppppp/8/3n4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1"));
    TEST_ASSERT_TRUE(string_to_move_valid(pstn, "e4d5", E4, D5, CAPTURE_FLAG, BLACK | KNIGHT));
    free_position(pstn);
}

void test_string_to_move_en_passant(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/1ppp1ppp/p7/3Pp3/8/8/PPP1PPPP/RNBQKBNR w KQkq e6 0 1"));
    TEST_ASSERT_TRUE(string_to_move_valid(pstn, "d5e6", D5, E6, EP_FLAG, BLACK | PAWN));
    free_position(pstn);
}

void test_string_to_move_promotion_queen(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r1bqkbnr/pPpppp2/p1n5/6pp/8/4P3/P1PP1PPP/RNBQK1NR w KQkq - 0 1"));
    TEST_ASSERT_TRUE(string_to_move_valid(pstn, "b7b8q", B7, B8, (PROMO_FLAG + 3), 0));
    free_position(pstn);
}

void test_string_to_move_underpromotion_capture(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/pp1ppppp/8/7P/8/2N5/PpPPPPP1/R1BQKBNR b KQkq - 0 1"));
    TEST_ASSERT_TRUE(string_to_move_valid(pstn, "b2a1n", B2, A1, CAPTURE_FLAG | PROMO_FLAG, WHITE | ROOK));
    free_position(pstn);
}

static POSITION *new_string_to_move_invalid_position(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/pp1ppppp/8/7P/8/2N5/PpPPPPP1/R1BQKBNR b KQkq - 0 1"));
    return pstn;
}

void test_string_to_move_invalid_empty(void) {
    POSITION *pstn = new_string_to_move_invalid_position();
    TEST_ASSERT_TRUE(is_null_move(string_to_move(pstn, "")));
    free_position(pstn);
}

void test_string_to_move_invalid_e2(void) {
    POSITION *pstn = new_string_to_move_invalid_position();
    TEST_ASSERT_TRUE(is_null_move(string_to_move(pstn, "e2")));
    free_position(pstn);
}

void test_string_to_move_invalid_e2e(void) {
    POSITION *pstn = new_string_to_move_invalid_position();
    TEST_ASSERT_TRUE(is_null_move(string_to_move(pstn, "e2e")));
    free_position(pstn);
}

void test_string_to_move_invalid_i2e4(void) {
    POSITION *pstn = new_string_to_move_invalid_position();
    TEST_ASSERT_TRUE(is_null_move(string_to_move(pstn, "i2e4")));
    free_position(pstn);
}

void test_string_to_move_invalid_e0e4(void) {
    POSITION *pstn = new_string_to_move_invalid_position();
    TEST_ASSERT_TRUE(is_null_move(string_to_move(pstn, "e0e4")));
    free_position(pstn);
}

void test_string_to_move_invalid_e2i4(void) {
    POSITION *pstn = new_string_to_move_invalid_position();
    TEST_ASSERT_TRUE(is_null_move(string_to_move(pstn, "e2i4")));
    free_position(pstn);
}

void test_string_to_move_invalid_e2e0(void) {
    POSITION *pstn = new_string_to_move_invalid_position();
    TEST_ASSERT_TRUE(is_null_move(string_to_move(pstn, "e2e0")));
    free_position(pstn);
}

void test_string_to_move_invalid_e2e4x(void) {
    POSITION *pstn = new_string_to_move_invalid_position();
    TEST_ASSERT_TRUE(is_null_move(string_to_move(pstn, "e2e4x")));
    free_position(pstn);
}

void test_string_to_move_invalid_b7b8p(void) {
    POSITION *pstn = new_string_to_move_invalid_position();
    TEST_ASSERT_TRUE(is_null_move(string_to_move(pstn, "b7b8p")));
    free_position(pstn);
}

void test_make_move_a2a3(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, START_POS));
    TEST_ASSERT_TRUE(make_move(pstn, string_to_move(pstn, "a2a3")));
    assert_fen(pstn, "rnbqkbnr/pppppppp/8/8/8/P7/1PPPPPPP/RNBQKBNR b KQkq - 0 1");
    free_position(pstn);
}

void test_make_move_e4xd5(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, string_to_move(pstn, "e4d5")));
    assert_fen(pstn, "rnbqkbnr/ppp1pppp/8/3P4/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1");
    free_position(pstn);
}

void test_make_move_f2f4_ep_target(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/pppp1ppp/8/1N6/4p3/8/PPPPPPPP/R1BQKBNR w KQkq - 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, string_to_move(pstn, "f2f4")));
    assert_fen(pstn, "rnbqkbnr/pppp1ppp/8/1N6/4pP2/8/PPPPP1PP/R1BQKBNR b KQkq f3 0 1");
    free_position(pstn);
}

void test_make_move_e7e5(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/1ppppppp/p7/3P4/8/8/PPP1PPPP/RNBQKBNR b KQkq - 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, string_to_move(pstn, "e7e5")));
    assert_fen(pstn, "rnbqkbnr/1ppp1ppp/p7/3Pp3/8/8/PPP1PPPP/RNBQKBNR w KQkq e6 0 2");
    free_position(pstn);
}

void test_make_move_b7b8q(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r1bqkbnr/pPpppp2/p1n5/6pp/8/4P3/P1PP1PPP/RNBQK1NR w KQkq - 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, string_to_move(pstn, "b7b8q")));
    assert_fen(pstn, "rQbqkbnr/p1pppp2/p1n5/6pp/8/4P3/P1PP1PPP/RNBQK1NR b KQkq - 0 1");
    free_position(pstn);
}

void test_make_move_a1a2(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r1bqkbnr/pppppppp/n7/8/P7/8/1PPPPPPP/RNBQKBNR w KQkq - 1 1"));
    TEST_ASSERT_TRUE(make_move(pstn, string_to_move(pstn, "a1a2")));
    assert_fen(pstn, "r1bqkbnr/pppppppp/n7/8/P7/8/RPPPPPPP/1NBQKBNR b Kkq - 2 1");
    free_position(pstn);
}

void test_make_move_a8a7(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/1ppppppp/p7/P7/8/8/1PPPPPPP/RNBQKBNR b KQkq - 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, string_to_move(pstn, "a8a7")));
    assert_fen(pstn, "1nbqkbnr/rppppppp/p7/P7/8/8/1PPPPPPP/RNBQKBNR w KQk - 1 2");
    free_position(pstn);
}

void test_make_move_h1h2(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r1bqkbnr/pppppppp/n7/8/7P/8/PPPPPPP1/RNBQKBNR w KQkq - 1 1"));
    TEST_ASSERT_TRUE(make_move(pstn, string_to_move(pstn, "h1h2")));
    assert_fen(pstn, "r1bqkbnr/pppppppp/n7/8/7P/8/PPPPPPPR/RNBQKBN1 b Qkq - 2 1");
    free_position(pstn);
}

void test_make_move_h8h7(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/ppppppp1/7p/7P/8/8/PPPPPPP1/RNBQKBNR b KQkq - 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, string_to_move(pstn, "h8h7")));
    assert_fen(pstn, "rnbqkbn1/pppppppr/7p/7P/8/8/PPPPPPP1/RNBQKBNR w KQq - 1 2");
    free_position(pstn);
}

void test_make_move_e1e2(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "r1bqkbnr/pppppppp/n7/8/8/4P3/PPPP1PPP/RNBQKBNR w KQkq - 1 1"));
    TEST_ASSERT_TRUE(make_move(pstn, string_to_move(pstn, "e1e2")));
    assert_fen(pstn, "r1bqkbnr/pppppppp/n7/8/8/4P3/PPPPKPPP/RNBQ1BNR b kq - 2 1");
    free_position(pstn);
}

void test_make_move_e8e7(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/pppp1ppp/4p3/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, string_to_move(pstn, "e8e7")));
    assert_fen(pstn, "rnbq1bnr/ppppkppp/4p3/8/4P3/8/PPPP1PPP/RNBQKBNR w KQ - 1 2");
    free_position(pstn);
}

void test_make_move_e1c1(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/4pppp/pppp4/8/3P4/2NQB3/PPP1PPPP/R3KBNR w KQkq - 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, string_to_move(pstn, "e1c1")));
    assert_fen(pstn, "rnbqkbnr/4pppp/pppp4/8/3P4/2NQB3/PPP1PPPP/2KR1BNR b kq - 1 1");
    free_position(pstn);
}

void test_make_move_e1g1(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/3ppppp/ppp5/8/8/3BP2N/PPPP1PPP/RNBQK2R w KQkq - 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, string_to_move(pstn, "e1g1")));
    assert_fen(pstn, "rnbqkbnr/3ppppp/ppp5/8/8/3BP2N/PPPP1PPP/RNBQ1RK1 b kq - 1 1");
    free_position(pstn);
}

void test_make_move_en_passant_d5e6(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/1ppp1ppp/p7/3Pp3/8/8/PPP1PPPP/RNBQKBNR w KQkq e6 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, string_to_move(pstn, "d5e6")));
    assert_fen(pstn, "rnbqkbnr/1ppp1ppp/p3P3/8/8/8/PPP1PPPP/RNBQKBNR b KQkq - 0 1");
    free_position(pstn);
}

void test_unmake_move_a2a3(void) {
    POSITION *pstn = new_position();
    move_t mv;
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, mv = string_to_move(pstn, "a2a3")));
    unmake_move(pstn, mv);
    assert_fen(pstn, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    free_position(pstn);
}

void test_unmake_move_e8d8(void) {
    POSITION *pstn = new_position();
    move_t mv;
    TEST_ASSERT_TRUE(update_position(pstn, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, mv = string_to_move(pstn, "e8d8")));
    unmake_move(pstn, mv);
    assert_fen(pstn, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1");
    free_position(pstn);
}

void test_unmake_move_g2g1q(void) {
    POSITION *pstn = new_position();
    move_t mv;
    TEST_ASSERT_TRUE(update_position(pstn, "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, mv = string_to_move(pstn, "g2g1q")));
    unmake_move(pstn, mv);
    assert_fen(pstn, "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1");
    free_position(pstn);
}

void test_unmake_move_e1c1(void) {
    POSITION *pstn = new_position();
    move_t mv;
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/4pppp/pppp4/8/3P4/2NQB3/PPP1PPPP/R3KBNR w KQkq - 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, mv = string_to_move(pstn, "e1c1")));
    unmake_move(pstn, mv);
    assert_fen(pstn, "rnbqkbnr/4pppp/pppp4/8/3P4/2NQB3/PPP1PPPP/R3KBNR w KQkq - 0 1");
    free_position(pstn);
}

void test_unmake_move_e1g1(void) {
    POSITION *pstn = new_position();
    move_t mv;
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/3ppppp/ppp5/8/8/3BP2N/PPPP1PPP/RNBQK2R w KQkq - 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, mv = string_to_move(pstn, "e1g1")));
    unmake_move(pstn, mv);
    assert_fen(pstn, "rnbqkbnr/3ppppp/ppp5/8/8/3BP2N/PPPP1PPP/RNBQK2R w KQkq - 0 1");
    free_position(pstn);
}

void test_unmake_move_a1a2(void) {
    POSITION *pstn = new_position();
    move_t mv;
    TEST_ASSERT_TRUE(update_position(pstn, "r1bqkbnr/pppppppp/n7/8/P7/8/1PPPPPPP/RNBQKBNR w KQkq - 1 1"));
    TEST_ASSERT_TRUE(make_move(pstn, mv = string_to_move(pstn, "a1a2")));
    unmake_move(pstn, mv);
    assert_fen(pstn, "r1bqkbnr/pppppppp/n7/8/P7/8/1PPPPPPP/RNBQKBNR w KQkq - 1 1");
    free_position(pstn);
}

void test_unmake_move_a8a7(void) {
    POSITION *pstn = new_position();
    move_t mv;
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/1ppppppp/p7/P7/8/8/1PPPPPPP/RNBQKBNR b KQkq - 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, mv = string_to_move(pstn, "a8a7")));
    unmake_move(pstn, mv);
    assert_fen(pstn, "rnbqkbnr/1ppppppp/p7/P7/8/8/1PPPPPPP/RNBQKBNR b KQkq - 0 1");
    free_position(pstn);
}

void test_unmake_move_h1h2(void) {
    POSITION *pstn = new_position();
    move_t mv;
    TEST_ASSERT_TRUE(update_position(pstn, "r1bqkbnr/pppppppp/n7/8/7P/8/PPPPPPP1/RNBQKBNR w KQkq - 1 1"));
    TEST_ASSERT_TRUE(make_move(pstn, mv = string_to_move(pstn, "h1h2")));
    unmake_move(pstn, mv);
    assert_fen(pstn, "r1bqkbnr/pppppppp/n7/8/7P/8/PPPPPPP1/RNBQKBNR w KQkq - 1 1");
    free_position(pstn);
}

void test_unmake_move_h8h7(void) {
    POSITION *pstn = new_position();
    move_t mv;
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/ppppppp1/7p/7P/8/8/PPPPPPP1/RNBQKBNR b KQkq - 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, mv = string_to_move(pstn, "h8h7")));
    unmake_move(pstn, mv);
    assert_fen(pstn, "rnbqkbnr/ppppppp1/7p/7P/8/8/PPPPPPP1/RNBQKBNR b KQkq - 0 1");
    free_position(pstn);
}

void test_unmake_move_e1e2(void) {
    POSITION *pstn = new_position();
    move_t mv;
    TEST_ASSERT_TRUE(update_position(pstn, "r1bqkbnr/pppppppp/n7/8/8/4P3/PPPP1PPP/RNBQKBNR w KQkq - 1 1"));
    TEST_ASSERT_TRUE(make_move(pstn, mv = string_to_move(pstn, "e1e2")));
    unmake_move(pstn, mv);
    assert_fen(pstn, "r1bqkbnr/pppppppp/n7/8/8/4P3/PPPP1PPP/RNBQKBNR w KQkq - 1 1");
    free_position(pstn);
}

void test_unmake_move_e8e7(void) {
    POSITION *pstn = new_position();
    move_t mv;
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/pppp1ppp/4p3/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, mv = string_to_move(pstn, "e8e7")));
    unmake_move(pstn, mv);
    assert_fen(pstn, "rnbqkbnr/pppp1ppp/4p3/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1");
    free_position(pstn);
}

void test_unmake_move_a1a2_pawn_ahead(void) {
    POSITION *pstn = new_position();
    move_t mv;
    TEST_ASSERT_TRUE(update_position(pstn, "r1bqkbnr/p1pppppp/n7/1p6/P6P/8/1PPPPPP1/RNBQKBNR w Qkq - 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, mv = string_to_move(pstn, "a1a2")));
    unmake_move(pstn, mv);
    assert_fen(pstn, "r1bqkbnr/p1pppppp/n7/1p6/P6P/8/1PPPPPP1/RNBQKBNR w Qkq - 0 1");
    free_position(pstn);
}

void test_unmake_move_e1e2_ep_file(void) {
    POSITION *pstn = new_position();
    move_t mv;
    TEST_ASSERT_TRUE(update_position(pstn, "rnbqkbnr/1ppppppp/8/8/p7/4P3/PPPP1PPP/RNBQKBNR w kq - 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, mv = string_to_move(pstn, "e1e2")));
    unmake_move(pstn, mv);
    assert_fen(pstn, "rnbqkbnr/1ppppppp/8/8/p7/4P3/PPPP1PPP/RNBQKBNR w kq - 0 1");
    free_position(pstn);
}

void test_unmake_move_h6h1(void) {
    POSITION *pstn = new_position();
    move_t mv;
    TEST_ASSERT_TRUE(update_position(pstn, "rn1qkbn1/ppp1ppp1/3p3r/P7/6b1/3P4/1PP1PPP1/RN1QKBNR b KQq - 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, mv = string_to_move(pstn, "h6h1")));
    unmake_move(pstn, mv);
    assert_fen(pstn, "rn1qkbn1/ppp1ppp1/3p3r/P7/6b1/3P4/1PP1PPP1/RN1QKBNR b KQq - 0 1");
    free_position(pstn);
}

void test_unmake_move_d5e6(void) {
    POSITION *pstn = new_position();
    move_t mv;
    TEST_ASSERT_TRUE(update_position(pstn, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"));
    TEST_ASSERT_TRUE(make_move(pstn, mv = string_to_move(pstn, "d5e6")));
    unmake_move(pstn, mv);
    assert_fen(pstn, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    free_position(pstn);
}

void test_move_exists_b1a3(void) {
    POSITION *pstn = new_position();
    TEST_ASSERT_TRUE(update_position(pstn, START_POS));
    TEST_ASSERT_TRUE(move_exists(pstn, string_to_move(pstn, "b1a3")));
    free_position(pstn);
}

void run_move_tests(void) {
    RUN_TEST(test_string_to_move_f2f3);
    RUN_TEST(test_string_to_move_e2e4_and_whitespace);
    RUN_TEST(test_string_to_move_white_kingside_castle);
    RUN_TEST(test_string_to_move_black_queenside_castle);
    RUN_TEST(test_string_to_move_bishop_capture);
    RUN_TEST(test_string_to_move_knight_capture);
    RUN_TEST(test_string_to_move_en_passant);
    RUN_TEST(test_string_to_move_promotion_queen);
    RUN_TEST(test_string_to_move_underpromotion_capture);
    RUN_TEST(test_string_to_move_invalid_empty);
    RUN_TEST(test_string_to_move_invalid_e2);
    RUN_TEST(test_string_to_move_invalid_e2e);
    RUN_TEST(test_string_to_move_invalid_i2e4);
    RUN_TEST(test_string_to_move_invalid_e0e4);
    RUN_TEST(test_string_to_move_invalid_e2i4);
    RUN_TEST(test_string_to_move_invalid_e2e0);
    RUN_TEST(test_string_to_move_invalid_e2e4x);
    RUN_TEST(test_string_to_move_invalid_b7b8p);
    RUN_TEST(test_make_move_a2a3);
    RUN_TEST(test_make_move_e4xd5);
    RUN_TEST(test_make_move_f2f4_ep_target);
    RUN_TEST(test_make_move_e7e5);
    RUN_TEST(test_make_move_b7b8q);
    RUN_TEST(test_make_move_a1a2);
    RUN_TEST(test_make_move_a8a7);
    RUN_TEST(test_make_move_h1h2);
    RUN_TEST(test_make_move_h8h7);
    RUN_TEST(test_make_move_e1e2);
    RUN_TEST(test_make_move_e8e7);
    RUN_TEST(test_make_move_e1c1);
    RUN_TEST(test_make_move_e1g1);
    RUN_TEST(test_make_move_en_passant_d5e6);
    RUN_TEST(test_unmake_move_a2a3);
    RUN_TEST(test_unmake_move_e8d8);
    RUN_TEST(test_unmake_move_g2g1q);
    RUN_TEST(test_unmake_move_e1c1);
    RUN_TEST(test_unmake_move_e1g1);
    RUN_TEST(test_unmake_move_a1a2);
    RUN_TEST(test_unmake_move_a8a7);
    RUN_TEST(test_unmake_move_h1h2);
    RUN_TEST(test_unmake_move_h8h7);
    RUN_TEST(test_unmake_move_e1e2);
    RUN_TEST(test_unmake_move_e8e7);
    RUN_TEST(test_unmake_move_a1a2_pawn_ahead);
    RUN_TEST(test_unmake_move_e1e2_ep_file);
    RUN_TEST(test_unmake_move_h6h1);
    RUN_TEST(test_unmake_move_d5e6);
    RUN_TEST(test_move_exists_b1a3);
}
