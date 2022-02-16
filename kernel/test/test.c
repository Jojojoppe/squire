#include "../config.h"
#include "minunit.h"

// TESTS ------------------

#include "../src/common/main.c"
MU_TEST(test_main_check){
    mu_check(9==8);
}


// ------------------------

MU_TEST_SUITE(test_suite){
    MU_RUN_TEST(test_main_check);
}

int main(int argc, char ** argv){
    MU_RUN_SUITE(test_suite);
    MU_REPORT();
    return 0;
}