#ifdef ARDUINO
#ifdef UNIT_TEST

#include <Arduino.h>
#include <unity.h>

#include "mockcommandadapter.h"
#include <narrowbandcore.h>


void test_nbcore_ready() {
    MockCommandAdapter mca;

    mca.clear();
    mca.setRetvalBool(true);

    NarrowbandCore nbc(mca);

    TEST_ASSERT_EQUAL(nbc.ready(), true);
    TEST_ASSERT_EQUAL(mca.getNumCalls(MockCommandAdapter::_send_cmd_waitfor_reply), 1);
    TEST_ASSERT_EQUAL_STRING(mca.getLastCmd(), "AT");

    mca.clear();
    mca.setRetvalBool(false);
    TEST_ASSERT_EQUAL(nbc.ready(), false);
    TEST_ASSERT_EQUAL(mca.getNumCalls(MockCommandAdapter::_send_cmd_waitfor_reply), 1);
    TEST_ASSERT_EQUAL_STRING(mca.getLastCmd(), "AT");

}

void setup() {
    UNITY_BEGIN();

    RUN_TEST(test_nbcore_ready);

    UNITY_END();
}

void loop() {

}

#endif
#endif