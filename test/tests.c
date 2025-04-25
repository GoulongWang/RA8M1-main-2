#include <stdint.h>
#include <unity.h>

#include "utils.h"

//==============================================================================
//                                 MAIN
//==============================================================================
void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void passingTest(void) {
    TEST_ASSERT_EQUAL(1, 1);
}

void failingTest(void) {
    TEST_ASSERT_EQUAL(1, 0);
}

int main(void) {
    delay_ms(200);

    Utils_Init();

    UNITY_BEGIN();

    RUN_TEST(passingTest);
    RUN_TEST(failingTest);

    utils_exit(UNITY_END());
}
