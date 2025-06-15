#include <zephyr/ztest.h>
#include <zephyr/kernel.h>

#define SCHEDULER_TEST1_TIMEOUT                 0
#define SCHEDULER_TEST2_TIMEOUT                 1
#define SCHEDULER_COUNT                         2       // Number of scheduled events, must be the last one and one more than the last timeout
#include "sleep_scheduler.h"

ZTEST(sleep_scheduler, test_set_and_clear_timeout)
{
    set_sleep_scheduler_timeout(SCHEDULER_TEST1_TIMEOUT, 1000);
    zassert_true(is_sleep_scheduler_timeout_active(SCHEDULER_TEST1_TIMEOUT), "Timeout should be active");
    clear_sleep_scheduler_timeout(SCHEDULER_TEST1_TIMEOUT);
    zassert_false(is_sleep_scheduler_timeout_active(SCHEDULER_TEST1_TIMEOUT), "Timeout should not be active");
}

ZTEST(sleep_scheduler, test_check_timeout)
{
    set_sleep_scheduler_timeout(SCHEDULER_TEST1_TIMEOUT, 100);
    set_sleep_scheduler_timeout(SCHEDULER_TEST2_TIMEOUT, 120);
    zassert_false(check_sleep_scheduler_timeout(SCHEDULER_TEST1_TIMEOUT, true), "Timeout 1 should not be expired yet");
    zassert_false(check_sleep_scheduler_timeout(SCHEDULER_TEST2_TIMEOUT, true), "Timeout 2 should not be expired yet");
    k_msleep(80);
    zassert_false(check_sleep_scheduler_timeout(SCHEDULER_TEST1_TIMEOUT, true), "Timeout 1 should be close to expiring but not yet");
    zassert_false(check_sleep_scheduler_timeout(SCHEDULER_TEST2_TIMEOUT, true), "Timeout 2 must not be expired yet");
    k_msleep(30);
    zassert_true(check_sleep_scheduler_timeout(SCHEDULER_TEST1_TIMEOUT, true), "Timeout 1 should have expired by now");
    zassert_false(check_sleep_scheduler_timeout(SCHEDULER_TEST2_TIMEOUT, true), "Timeout 2 should be close to expiring but not yet");
    k_msleep(20);
    zassert_true(check_sleep_scheduler_timeout(SCHEDULER_TEST1_TIMEOUT, true), "Timeout 1 should still be expited");
    zassert_true(check_sleep_scheduler_timeout(SCHEDULER_TEST2_TIMEOUT, true), "Timeout 2 should have expired by now");
}

ZTEST(sleep_scheduler, test_timeout_active_check)
{
    clear_sleep_scheduler_timeout(SCHEDULER_TEST1_TIMEOUT);
    zassert_false(is_sleep_scheduler_timeout_active(SCHEDULER_TEST1_TIMEOUT), "Timeout should not be active after clearing");
    zassert_false(check_sleep_scheduler_timeout(SCHEDULER_TEST1_TIMEOUT, true), "Timeout 1 should not be expired if inactive");
    zassert_true(check_sleep_scheduler_timeout(SCHEDULER_TEST1_TIMEOUT, false), "Timeout 1 have expired if not checking if active");
    set_sleep_scheduler_timeout(SCHEDULER_TEST1_TIMEOUT, 100);
    zassert_true(is_sleep_scheduler_timeout_active(SCHEDULER_TEST1_TIMEOUT), "Timeout should be active after setting");
}

ZTEST(sleep_scheduler, test_calculate_sleep_time)
{
    clear_sleep_scheduler_timeout(SCHEDULER_TEST2_TIMEOUT);
    set_sleep_scheduler_timeout(SCHEDULER_TEST1_TIMEOUT, 3000);
    k_timeout_t sleep_time = calculate_sleep_time();
    uint64_t ms = k_ticks_to_ms_floor64(sleep_time.ticks);
    zassert_true(ms > 2990, "Sleep time should be greater than 2990");
    zassert_true(ms < 3010, "Sleep time should be less than 3010");

    clear_sleep_scheduler_timeout(SCHEDULER_TEST1_TIMEOUT);
    sleep_time = calculate_sleep_time();
    
    zassert_equal(sleep_time.ticks, K_FOREVER.ticks, "Sleep time should be forever after clearing all timeouts");
}

struct data_item_type {
    uint32_t field1;
};

ZTEST(sleep_scheduler, test_calculate_sleep_time_msgq)
{
    char test_msgq_buffer[10 * sizeof(struct data_item_type)];
    struct k_msgq test_msgq;

    k_msgq_init(&test_msgq, test_msgq_buffer, sizeof(struct data_item_type), 10);
    
    set_sleep_scheduler_timeout(SCHEDULER_TEST1_TIMEOUT, 1000);
    k_timeout_t sleep_time = calculate_sleep_time_msgq(&test_msgq);
    uint64_t ms = k_ticks_to_ms_floor64(sleep_time.ticks);
    zassert_true(ms > 900, "Sleep time should be greater than 900");

    // Simulate a message in the queue
    struct data_item_type item = { .field1 = 42 };
    k_msgq_put(&test_msgq, &item, K_NO_WAIT);
    sleep_time = calculate_sleep_time_msgq(&test_msgq);
    zassert_true(k_msgq_num_used_get(&test_msgq) > 0, "Message queue should have items");
    zassert_equal(sleep_time.ticks, K_NO_WAIT.ticks, "Sleep time should be no wait when messages are present");
}

ZTEST_SUITE(sleep_scheduler, NULL, NULL, NULL, NULL, NULL);


