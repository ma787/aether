#include "unity.h"

void setUp(void) {
}
void tearDown(void) {
}

void run_position_tests(void);
void run_move_tests(void);
void run_check_tests(void);
void run_hash_tests(void);
void run_move_gen_tests(void);


int main(void) {
    UNITY_BEGIN();
    run_position_tests();
    run_move_tests();
    run_check_tests();
    run_hash_tests();
    run_move_gen_tests();
    return UNITY_END();
}