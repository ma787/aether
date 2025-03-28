#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "move.h"
#include "position.h"
#include "hashing.h"

int test_fen(char *fen_str){
    char parsed_str[92];
    to_fen(parsed_str);
    
    if (strcmp(fen_str, parsed_str) != 0) {
        return 0;
    }

    return 1;
}

int main(void) {
    /* set_position tests */

    assert(set_position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") == 0);
    assert(side == WHITE && c_rights == 15 && ep_square == 0 && h_clk == 0 && check_info == 0);
    assert(test_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));

    assert(set_position("r3k2r/p1ppqpb1/bn2pnp1/1B1PN3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R w KQkq - 0 1") == 0);
    assert(side == WHITE && c_rights == 15 && !ep_square && !h_clk && !check_info);
    assert(test_fen("r3k2r/p1ppqpb1/bn2pnp1/1B1PN3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R w KQkq - 0 1"));
    
    assert(set_position("rnbqkbnr/1p1ppppp/8/p1pP4/8/8/PPP1PPPP/RNBQKBNR w KQkq c6 0 1") == 0);
    assert(side == WHITE && c_rights == 15 && ep_square == C6 && !h_clk && !check_info);
    assert(test_fen("rnbqkbnr/1p1ppppp/8/p1pP4/8/8/PPP1PPPP/RNBQKBNR w KQkq c6 0 1"));
    
    assert(set_position("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1") == 0);
    assert(side == WHITE && c_rights == 15 && !ep_square && !h_clk && !check_info);
    assert(test_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"));

    assert(set_position("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1") == 0);
    assert(side == WHITE && !c_rights && !ep_square && !h_clk && !check_info);
    assert(test_fen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"));

    assert(set_position("r3k2r/p1ppqpb1/bn2pnp1/3PN3/4P3/1pN2Q1p/PPPBBPPP/R3KR2 w Qkq - 0 1") == 0);
    assert(side == WHITE && c_rights == 13 && !ep_square && !h_clk && !check_info);
    assert(test_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/4P3/1pN2Q1p/PPPBBPPP/R3KR2 w Qkq - 0 1"));

    assert(set_position("r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3 1") == 0);
    assert(side == BLACK && c_rights == 12 && !ep_square && h_clk == 3); 
    assert(check_info == (DISTANT_CHECK | (H5 << 2)));
    assert(test_fen("r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3 1"));

    assert(set_position("1nbqkbnr/r2pp2p/ppp2pB1/4P2Q/3P4/2N5/PPP2PPP/R1B1K1NR b KQk - 0 1") == 0);
    assert(side == BLACK && c_rights == 14 && !ep_square && !h_clk);
    assert(check_info == (DISTANT_CHECK | (G3 << 2)));
    assert(test_fen("1nbqkbnr/r2pp2p/ppp2pB1/4P2Q/3P4/2N5/PPP2PPP/R1B1K1NR b KQk - 0 1"));

    assert(set_position("r2k3r/p1ppqNb1/bn2pQp1/3P4/1p2P3/2N4p/PPPBBPPP/R3K2R b KQ - 0 1") == 0);
    assert(side == BLACK && c_rights == 12 && !ep_square && !h_clk);
    assert(check_info == (CONTACT_CHECK | (F2 << 2)));
    assert(test_fen("r2k3r/p1ppqNb1/bn2pQp1/3P4/1p2P3/2N4p/PPPBBPPP/R3K2R b KQ - 0 1"));

    assert(set_position("4k2r/8/8/8/8/8/8/R1r1K2R w KQk - 1 1") == 0);
    assert(side == WHITE && c_rights == 11 && !ep_square && h_clk == 1);
    assert(check_info == (DISTANT_CHECK | (C1 << 2)));
    assert(test_fen("4k2r/8/8/8/8/8/8/R1r1K2R w KQk - 1 1"));

    assert(set_position("r3k2r/p1pq1Pb1/bn1p1np1/1B2N3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R b KQkq - 0 1") == 0);
    assert(side == BLACK && c_rights == 15 && ep_square == 0 && h_clk == 0);
    assert(check_info == (CONTACT_CHECK | (F2 << 2)));
    assert(test_fen("r3k2r/p1pq1Pb1/bn1p1np1/1B2N3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R b KQkq - 0 1"));

    assert(set_position("8/8/3p4/1Pp4r/1K3R2/6k1/4P1P1/8 w - c6 0 1") == 0);
    assert(side == WHITE && !c_rights && ep_square == C6 && !h_clk); 
    assert(check_info == (CONTACT_CHECK | (C5 << 2)));
    assert(test_fen("8/8/3p4/1Pp4r/1K3R2/6k1/4P1P1/8 w - c6 0 1"));

    assert(set_position("rnbqkbn1/pp6/8/8/4Kr2/8/PP4b1/R3Q1BR w q - 0 1") == 0);
    assert(side == WHITE && c_rights == 4 && !ep_square && !h_clk);
    assert(
        check_info == (DOUBLE_CHECK | (G2 << 2) | (F4 << 10)) 
        || check_info == (DOUBLE_CHECK | (F4 << 2) | (G2 << 10))
    );
    assert(test_fen("rnbqkbn1/pp6/8/8/4Kr2/8/PP4b1/R3Q1BR w q - 0 1"));

    assert(set_position("r5k1/ppp3p1/3pb3/7p/8/5r1K/PB3bPP/RN1Q3R w - - 0 1") == 0);
    assert(side == WHITE && !c_rights && !ep_square && !h_clk);
    assert(
        check_info == (DOUBLE_CHECK | (F3 << 2) | (E6 << 10))
        || check_info == (DOUBLE_CHECK | (E6 << 2) | (F3 << 10))
    );
    assert(test_fen("r5k1/ppp3p1/3pb3/7p/8/5r1K/PB3bPP/RN1Q3R w - - 0 1"));

    assert(set_position("r1bq1r2/pp2n3/4N1Pk/3pPp2/1b1n2Q1/2N5/PP3PP1/R1B1K2R b KQ - 0 1") == 0);
    assert(side == BLACK && c_rights == 12 && !ep_square && !h_clk);
    assert(
        check_info == (DOUBLE_CHECK | (C8 << 2) | (H8 << 10))
        || check_info == (DOUBLE_CHECK | (H8 << 2) | (C8 << 10))
    );
    assert(test_fen("r1bq1r2/pp2n3/4N1Pk/3pPp2/1b1n2Q1/2N5/PP3PP1/R1B1K2R b KQ - 0 1"));

    /* is_square_attacked tests */

    assert(set_position("rnb1kbnr/1pp1pppp/p7/8/2p5/NQ1qB3/PP2PPPP/R3KBNR w KQkq - 0 1") == 0);
    assert(is_square_attacked(D1));

    assert(set_position("r3k2r/p1ppqpb1/bn2pnN1/3P4/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1") == 0);
    assert(is_square_attacked(E7));

    assert(set_position("r3k2r/Pppp1ppp/5nbN/nP6/BBP1P3/q4N2/Pp1P1bPP/R2Q2K1 w kq - 0 1") == 0);
    assert(is_square_attacked(G1));

    assert(set_position("r3k2r/p1ppqpb1/Bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R b KQkq - 0 1") == 0);
    assert(!is_square_attacked(F1));

    assert(set_position("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1") == 0);
    assert(is_square_attacked(D1));

    /* of_string tests */

    assert(set_position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") == 0);
    move_t mv = of_string("f2f3");
    assert(mv.start == F2 && mv.dest == F3 && !mv.flags && !mv.captured_piece);

    assert(set_position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") == 0);
    mv = of_string("e2e4");
    assert(mv.start == E2 && mv.dest == E4 && mv.flags == DPP_FLAG && !mv.captured_piece);

    assert(set_position("rnbqkbnr/3ppppp/ppp5/8/8/3BP2N/PPPP1PPP/RNBQK2R w KQkq - 0 1") == 0);
    mv = of_string("e1g1");
    assert(mv.start == E1 && mv.dest == G1 && mv.flags == K_CASTLE_FLAG && !mv.captured_piece);

    assert(set_position("r3kbnr/pppqpppp/n2p4/5b2/8/PPPP3P/4PPP1/RNBQKBNR b KQkq - 0 1") == 0);
    mv = of_string("e8c8");
    assert(mv.start == E1 && mv.dest == C1 && mv.flags == Q_CASTLE_FLAG && !mv.captured_piece);

    assert(set_position("rnbqkbnr/1ppppppp/p7/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1") == 0);
    mv = of_string("f1a6");
    assert(
        mv.start == F1 && mv.dest == A6 && mv.flags == CAPTURE_FLAG 
        && (mv.captured_piece & 0xFF) == (BLACK | PAWN)
    );

    assert(set_position("rnbqkbnr/1ppp1ppp/p7/3Pp3/8/8/PPP1PPPP/RNBQKBNR w KQkq e6 0 1") == 0);
    mv = of_string("d5e6");
    assert(
        mv.start == D5 && mv.dest == E6 && mv.flags == EP_FLAG 
        && (mv.captured_piece & 0xFF) == (BLACK | PAWN)
    );

    assert(set_position("r1bqkbnr/pPpppp2/p1n5/6pp/8/4P3/P1PP1PPP/RNBQK1NR w KQkq - 0 1") == 0);
    mv = of_string("b7b8q");
    assert(mv.start == B7 && mv.dest == B8 && mv.flags == (PROMO_FLAG | 3) && !mv.captured_piece);

    assert(set_position("rnbqkbnr/pp1ppppp/8/7P/8/2N5/PpPPPPP1/R1BQKBNR b KQkq - 0 1") == 0);
    mv = of_string("b2a1n");
    assert(
        mv.start == B7 && mv.dest == A8 && mv.flags == (CAPTURE_FLAG | PROMO_FLAG)
        && (mv.captured_piece & 0xFF) == (BLACK | ROOK)
    );

    /* make_move tests */

    assert(set_position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") == 0);
    assert(make_move(of_string("a2a3")) == 0);
    assert(test_fen("rnbqkbnr/pppppppp/8/8/8/P7/1PPPPPPP/RNBQKBNR b KQkq - 0 1"));

    assert(set_position("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1") == 0);
    assert(make_move(of_string("e4d5")) == 0);
    assert(test_fen("rnbqkbnr/ppp1pppp/8/3P4/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1"));

    assert(set_position("rnbqkbnr/pppp1ppp/8/1N6/4p3/8/PPPPPPPP/R1BQKBNR w KQkq - 0 1") == 0);
    assert(make_move(of_string("f2f4")) == 0);
    assert(test_fen("rnbqkbnr/pppp1ppp/8/1N6/4pP2/8/PPPPP1PP/R1BQKBNR b KQkq f3 0 1"));

    assert(set_position("rnbqkbnr/1ppppppp/p7/3P4/8/8/PPP1PPPP/RNBQKBNR b KQkq - 0 1") == 0);
    assert(make_move(of_string("e7e5")) == 0);
    assert(test_fen("rnbqkbnr/1ppp1ppp/p7/3Pp3/8/8/PPP1PPPP/RNBQKBNR w KQkq e6 0 1"));

    assert(set_position("r1bqkbnr/pPpppp2/p1n5/6pp/8/4P3/P1PP1PPP/RNBQK1NR w KQkq - 0 1") == 0);
    assert(make_move(of_string("b7b8q")) == 0);
    assert(test_fen("rQbqkbnr/p1pppp2/p1n5/6pp/8/4P3/P1PP1PPP/RNBQK1NR b KQkq - 0 1"));

    assert(set_position("r1bqkbnr/pppppppp/n7/8/P7/8/1PPPPPPP/RNBQKBNR w KQkq - 1 1") == 0);
    assert(make_move(of_string("a1a2")) == 0);
    assert(test_fen("r1bqkbnr/pppppppp/n7/8/P7/8/RPPPPPPP/1NBQKBNR b Kkq - 2 1"));

    assert(set_position("rnbqkbnr/1ppppppp/p7/P7/8/8/1PPPPPPP/RNBQKBNR b KQkq - 0 1") == 0);
    assert(make_move(of_string("a8a7")) == 0);
    assert(test_fen("1nbqkbnr/rppppppp/p7/P7/8/8/1PPPPPPP/RNBQKBNR w KQk - 1 1"));

    assert(set_position("r1bqkbnr/pppppppp/n7/8/7P/8/PPPPPPP1/RNBQKBNR w KQkq - 1 1") == 0);
    assert(make_move(of_string("h1h2")) == 0);
    assert(test_fen("r1bqkbnr/pppppppp/n7/8/7P/8/PPPPPPPR/RNBQKBN1 b Qkq - 2 1"));

    assert(set_position("rnbqkbnr/ppppppp1/7p/7P/8/8/PPPPPPP1/RNBQKBNR b KQkq - 0 1") == 0);
    assert(make_move(of_string("h8h7")) == 0);
    assert(test_fen("rnbqkbn1/pppppppr/7p/7P/8/8/PPPPPPP1/RNBQKBNR w KQq - 1 1"));

    assert(set_position("r1bqkbnr/pppppppp/n7/8/8/4P3/PPPP1PPP/RNBQKBNR w KQkq - 1 1") == 0);
    assert(make_move(of_string("e1e2")) == 0);
    assert(test_fen("r1bqkbnr/pppppppp/n7/8/8/4P3/PPPPKPPP/RNBQ1BNR b kq - 2 1"));

    assert(set_position("rnbqkbnr/pppp1ppp/4p3/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1") == 0);
    assert(make_move(of_string("e8e7")) == 0);
    assert(test_fen("rnbq1bnr/ppppkppp/4p3/8/4P3/8/PPPP1PPP/RNBQKBNR w KQ - 1 1"));

    assert(set_position("rnbqkbnr/4pppp/pppp4/8/3P4/2NQB3/PPP1PPPP/R3KBNR w KQkq - 0 1") == 0);
    assert(make_move(of_string("e1c1")) == 0);
    assert(test_fen("rnbqkbnr/4pppp/pppp4/8/3P4/2NQB3/PPP1PPPP/2KR1BNR b kq - 1 1"));

    assert(set_position("rnbqkbnr/3ppppp/ppp5/8/8/3BP2N/PPPP1PPP/RNBQK2R w KQkq - 0 1") == 0);
    assert(make_move(of_string("e1g1")) == 0);
    assert(test_fen("rnbqkbnr/3ppppp/ppp5/8/8/3BP2N/PPPP1PPP/RNBQ1RK1 b kq - 1 1"));

    assert(set_position("rnbqkbnr/1ppp1ppp/p7/3Pp3/8/8/PPP1PPPP/RNBQKBNR w KQkq e6 0 1") == 0);
    assert(make_move(of_string("d5e6")) == 0);
    assert(test_fen("rnbqkbnr/1ppp1ppp/p3P3/8/8/8/PPP1PPPP/RNBQKBNR b KQkq - 0 1"));

    /* unmake_move tests */

    assert(set_position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") == 0);
    assert(make_move(mv = of_string("a2a3")) == 0);
    unmake_move(mv);
    assert(test_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));

    assert(set_position("r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1") == 0);
    assert(make_move(mv = of_string("e8d8")) == 0);
    unmake_move(mv);
    assert(test_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1"));

    assert(set_position("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1") == 0);
    assert(make_move(mv = of_string("g2g1q")) == 0);
    unmake_move(mv);
    assert(test_fen("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1"));

    assert(set_position("rnbqkbnr/4pppp/pppp4/8/3P4/2NQB3/PPP1PPPP/R3KBNR w KQkq - 0 1") == 0);
    assert(make_move(mv = of_string("e1c1")) == 0);
    unmake_move(mv);
    assert(test_fen("rnbqkbnr/4pppp/pppp4/8/3P4/2NQB3/PPP1PPPP/R3KBNR w KQkq - 0 1"));

    assert(set_position("rnbqkbnr/3ppppp/ppp5/8/8/3BP2N/PPPP1PPP/RNBQK2R w KQkq - 0 1") == 0);
    assert(make_move(mv = of_string("e1g1")) == 0);
    unmake_move(mv);
    assert(test_fen("rnbqkbnr/3ppppp/ppp5/8/8/3BP2N/PPPP1PPP/RNBQK2R w KQkq - 0 1"));

    assert(set_position("r1bqkbnr/pppppppp/n7/8/P7/8/1PPPPPPP/RNBQKBNR w KQkq - 1 1") == 0);
    assert(make_move(mv = of_string("a1a2")) == 0);
    unmake_move(mv);
    assert(test_fen("r1bqkbnr/pppppppp/n7/8/P7/8/1PPPPPPP/RNBQKBNR w KQkq - 1 1"));

    assert(set_position("rnbqkbnr/1ppppppp/p7/P7/8/8/1PPPPPPP/RNBQKBNR b KQkq - 0 1") == 0);
    assert(make_move(mv = of_string("a8a7")) == 0);
    unmake_move(mv);
    assert(test_fen("rnbqkbnr/1ppppppp/p7/P7/8/8/1PPPPPPP/RNBQKBNR b KQkq - 0 1"));

    assert(set_position("r1bqkbnr/pppppppp/n7/8/7P/8/PPPPPPP1/RNBQKBNR w KQkq - 1 1") == 0);
    assert(make_move(mv = of_string("h1h2")) == 0);
    unmake_move(mv);
    assert(test_fen("r1bqkbnr/pppppppp/n7/8/7P/8/PPPPPPP1/RNBQKBNR w KQkq - 1 1"));

    assert(set_position("rnbqkbnr/ppppppp1/7p/7P/8/8/PPPPPPP1/RNBQKBNR b KQkq - 0 1") == 0);
    assert(make_move(mv = of_string("h8h7")) == 0);
    unmake_move(mv);
    assert(test_fen("rnbqkbnr/ppppppp1/7p/7P/8/8/PPPPPPP1/RNBQKBNR b KQkq - 0 1"));

    assert(set_position("r1bqkbnr/pppppppp/n7/8/8/4P3/PPPP1PPP/RNBQKBNR w KQkq - 1 1") == 0);
    assert(make_move(mv = of_string("e1e2")) == 0);
    unmake_move(mv);
    assert(test_fen("r1bqkbnr/pppppppp/n7/8/8/4P3/PPPP1PPP/RNBQKBNR w KQkq - 1 1"));

    assert(set_position("rnbqkbnr/pppp1ppp/4p3/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1") == 0);
    assert(make_move(mv = of_string("e8e7")) == 0);
    unmake_move(mv);
    assert(test_fen("rnbqkbnr/pppp1ppp/4p3/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1"));

    assert(set_position("r1bqkbnr/p1pppppp/n7/1p6/7P/8/PPPPPPP1/RNBQKBNR w Qkq - 0 1") == 0);
    assert(make_move(mv = of_string("a1a2")) == 0);
    unmake_move(mv);
    assert(test_fen("r1bqkbnr/p1pppppp/n7/1p6/7P/8/PPPPPPP1/RNBQKBNR w Qkq - 0 1"));

    assert(set_position("rnbqkbnr/1ppppppp/8/8/p7/4P3/PPPP1PPP/RNBQKBNR w kq - 0 1") == 0);
    assert(make_move(mv = of_string("e1e2")) == 0);
    unmake_move(mv);
    assert(test_fen("rnbqkbnr/1ppppppp/8/8/p7/4P3/PPPP1PPP/RNBQKBNR w kq - 0 1"));

    assert(set_position("rn1qkbn1/ppp1ppp1/3p3r/P7/6b1/3P4/1PP1PPP1/RN1QKBNR b KQq - 0 1") == 0);
    assert(make_move(mv = of_string("h6h1")) == 0);
    unmake_move(mv);
    assert(test_fen("rn1qkbn1/ppp1ppp1/3p3r/P7/6b1/3P4/1PP1PPP1/RN1QKBNR b KQq - 0 1"));

    assert(set_position("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1") == 0);
    assert(make_move(mv = of_string("d5e6")) == 0);
    unmake_move(mv);
    assert(test_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"));

    /* update_check tests */

    assert(set_position("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1") == 0);
    assert(make_move(of_string("f3h5")) == 0);
    assert(check_info == NO_CHECK);

    assert(set_position("rnbqkbnr/pp2pppp/2pp4/8/Q7/2P5/PP1PPPPP/RNB1KBNR w KQkq - 0 1") == 0);
    assert(make_move(of_string("a4c6")) == 0);
    assert(check_info == (DISTANT_CHECK | (C3 << 2)));

    assert(set_position("Q7/1PP5/2k5/8/8/8/4Kppp/8 w - - 1 1") == 0);
    assert(make_move(of_string("b7b8q")) == 0);
    assert(check_info == (DISTANT_CHECK | (A1 << 2)));

    assert(set_position("r3k2r/p1p1qpb1/bn1ppnp1/1B1PN3/1p2P3/2N2Q1p/PPPB1PPP/R4K1R b kq - 1 1") == 0);
    assert(make_move(of_string("a6b5")) == 0);
    assert(check_info == (DISTANT_CHECK | (B5 << 2)));

    assert(set_position("rnbqkbnr/pppp1ppp/8/8/4P3/5N2/PpP2PPP/R1BQKB1R w KQkq - 0 1") == 0);
    assert(make_move(of_string("d1d7")) == 0);
    assert(check_info == (CONTACT_CHECK | (D2 << 2)));

    assert(set_position("r6r/Pp1pkppp/1P3nbN/nPp5/BB2P3/q4N2/Pp1P2PP/R2Q1RK1 w - c6 0 1") == 0);
    assert(make_move(of_string("b5c6")) == 0);
    assert(check_info == (DISTANT_CHECK | (B5 << 2)));

    assert(set_position("2kr3r/p2pqpb1/bn2pnp1/2pPN3/1p2P3/2Q4p/PPPBBPPP/RN2K2R w KQ c6 0 1") == 0);
    assert(make_move(of_string("d5c6")) == 0);
    assert(check_info == NO_CHECK);

    assert(set_position("4k2r/8/8/8/8/8/8/5K2 b k - 1 1") == 0);
    assert(make_move(of_string("e8g8")) == 0);
    assert(check_info == (DISTANT_CHECK | (F8 << 2)));

    /* update_hash tests */

    uint64_t new_hash;

    assert(set_position(START_POS) == 0);
    mv = of_string("a2a3");
    new_hash = update_hash(zobrist_hash(), mv);
    assert(make_move(mv) == 0);
    assert(zobrist_hash() == new_hash);

    assert(set_position("rnbqkbnr/pppppppp/8/8/8/P7/1PPPPPPP/RNBQKBNR b KQkq - 0 1") == 0);
    mv = of_string("e7e6");
    new_hash = update_hash(zobrist_hash(), mv);
    assert(make_move(mv) == 0);
    assert(zobrist_hash() == new_hash);

    assert(set_position(START_POS) == 0);
    mv = of_string("b1c3");
    new_hash = update_hash(zobrist_hash(), mv);
    assert(make_move(mv) == 0);
    assert(zobrist_hash() == new_hash);

    assert(set_position("rnbqkbnr/p1pppppp/8/1p6/8/2N5/PPPPPPPP/R1BQKBNR w KQkq b6 0 1") == 0);
    mv = of_string("c3b5");
    new_hash = update_hash(zobrist_hash(), mv);
    assert(make_move(mv) == 0);
    assert(zobrist_hash() == new_hash);

    assert(set_position("rnbqkbnr/ppppppp1/7p/2P5/8/8/PP1PPPPP/RNBQKBNR b KQkq - 0 1") == 0);
    mv = of_string("b7b5");
    new_hash = update_hash(zobrist_hash(), mv);
    assert(make_move(mv) == 0);
    assert(zobrist_hash() == new_hash);

    assert(set_position("rnbqkbnr/1ppp1ppp/p7/3Pp3/8/8/PPP1PPPP/RNBQKBNR w KQkq e6 0 1") == 0);
    mv = of_string("d5e6");
    new_hash = update_hash(zobrist_hash(), mv);
    assert(make_move(mv) == 0);
    assert(zobrist_hash() == new_hash);

    assert(set_position("rnbqkbnr/p1pppppp/8/8/Pp6/6PP/1PPPPP2/RNBQKBNR b KQkq a3 0 1") == 0);
    mv = of_string("b4a3");
    new_hash = update_hash(zobrist_hash(), mv);
    assert(make_move(mv) == 0);
    assert(zobrist_hash() == new_hash);

    assert(set_position("rnbqkbnr/4pppp/pppp4/8/3P4/2NQB3/PPP1PPPP/R3KBNR w KQkq - 0 1") == 0);
    mv = of_string("e1c1");
    new_hash = update_hash(zobrist_hash(), mv);
    assert(make_move(mv) == 0);
    assert(zobrist_hash() == new_hash);

    assert(set_position("rnbqkbnr/3ppppp/ppp5/8/8/3BP2N/PPPP1PPP/RNBQK2R w KQkq - 0 1") == 0);
    mv = of_string("e1g1");
    new_hash = update_hash(zobrist_hash(), mv);
    assert(make_move(mv) == 0);
    assert(zobrist_hash() == new_hash);

    assert(set_position("r1bqkbnr/pPpppp2/p1n5/6pp/8/4P3/P1PP1PPP/RNBQK1NR w KQkq - 0 1") == 0);
    mv = of_string("b7b8q");
    new_hash = update_hash(zobrist_hash(), mv);
    assert(make_move(mv) == 0);
    assert(zobrist_hash() == new_hash);

    return 0;
}