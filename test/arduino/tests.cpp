#ifdef ARDUINO
#ifdef UNIT_TEST

#include <Arduino.h>
#include <unity.h>

#include "narrowbandcore.h"
#include "nbutils.h"

using namespace Narrowband;

#include "mockcommandadapter.h"

void test_nbcore_ready() {
    MockCommandAdapter mca;

    NarrowbandCore nbc(mca);

    mca.clear();
    mca.setRetvalBool(true);

    TEST_ASSERT_EQUAL(nbc.ready(), true);
    TEST_ASSERT_EQUAL(mca.getNumCalls(MockCommandAdapter::_send_cmd_waitfor_reply), 1);
    TEST_ASSERT_EQUAL_STRING(mca.getLastCmd(), "AT");

    mca.clear();
    mca.setRetvalBool(false);
    TEST_ASSERT_EQUAL(nbc.ready(), false);
    TEST_ASSERT_EQUAL(mca.getNumCalls(MockCommandAdapter::_send_cmd_waitfor_reply), 1);
    TEST_ASSERT_EQUAL_STRING(mca.getLastCmd(), "AT");

}

void test_nbcore_reboot() {
    MockCommandAdapter mca;

    NarrowbandCore nbc(mca);

    mca.clear();
    mca.setRetvalBool(true);

    nbc.reboot(0);
    TEST_ASSERT_EQUAL(mca.getNumCalls(MockCommandAdapter::_send_cmd), 1);
    TEST_ASSERT_EQUAL_STRING(mca.getLastCmd(), "AT");

    mca.clear();
    mca.setRetvalBool(false);
    TEST_ASSERT_EQUAL(nbc.ready(), false);
    TEST_ASSERT_EQUAL(mca.getNumCalls(MockCommandAdapter::_send_cmd_waitfor_reply), 1);
    TEST_ASSERT_EQUAL_STRING(mca.getLastCmd(), "AT");

}

// int split_csv_line(char *buf, size_t n, char *arr_res[], int n_max_arr, const char *p_expect_cmdstring = NULL);
void test_nbcore_split_csv_line() {

    char        buf[1024];
    int         res;
    char        *p_arr[16];

    //
    strcpy(buf,"+TEST");

    res = split_csv_line(buf, sizeof(buf), p_arr, 16, "+XYZ");
    TEST_ASSERT_EQUAL(res, 0);

    //
    strcpy(buf,"+TEST");

    res = split_csv_line(buf, sizeof(buf), p_arr, 16, "+TEST");
    TEST_ASSERT_EQUAL(res, 0);

    //
    strcpy(buf,"+TEST:a,b,1,2,\"this\",-1");

    res = split_csv_line(buf, sizeof(buf), p_arr, 16, "+TEST");

    TEST_ASSERT_EQUAL(res, 6);
    TEST_ASSERT_EQUAL_STRING(p_arr[0], "a");
    TEST_ASSERT_EQUAL_STRING(p_arr[1], "b");
    TEST_ASSERT_EQUAL_STRING(p_arr[2], "1");
    TEST_ASSERT_EQUAL_STRING(p_arr[3], "2");
    TEST_ASSERT_EQUAL_STRING(p_arr[4], "\"this\"");
    TEST_ASSERT_EQUAL_STRING(p_arr[5], "-1");

}

void setup() {
    delay(5000);
    UNITY_BEGIN();

    RUN_TEST(test_nbcore_split_csv_line);
    RUN_TEST(test_nbcore_ready);
    RUN_TEST(test_nbcore_reboot);

    UNITY_END();
}

void loop() {

}

#endif
#endif