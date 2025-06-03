#include <zephyr/ztest.h>

#define SCHEDULER_TEST1_TIMEOUT                 0
#define SCHEDULER_TEST2_TIMEOUT                 1
#define SCHEDULER_COUNT                         2       // Number of scheduled events, must be the last one and one more than the last timeout
#include "sleep_scheduler.h"

#include "sleep_scheduler.h"

ZTEST(sleep_scheduler, test_set_and_clear_timeout)
{
    set_sleep_scheduler_timeout(SCHEDULER_TEST1_TIMEOUT, 1000);
    zassert_true(is_sleep_scheduler_timeout_active(SCHEDULER_TEST1_TIMEOUT), "Timeout should be active");
    clear_sleep_scheduler_timeout(SCHEDULER_TEST1_TIMEOUT);
    zassert_false(is_sleep_scheduler_timeout_active(SCHEDULER_TEST1_TIMEOUT), "Timeout should not be active");
}

// ZTEST(sleep_scheduler, test_check_timeout)
// {
//     set_sleep_scheduler_timeout(1, 2000);
//     zassert_true(check_sleep_scheduler_timeout(1, true), "Timeout should be active and checked");
    
//     clear_sleep_scheduler_timeout(1);
//     zassert_false(check_sleep_scheduler_timeout(1, true), "Timeout should not be active after clearing");
// }

// ZTEST(sleep_scheduler, test_calculate_sleep_time)
// {
//     set_sleep_scheduler_timeout(2, 3000);
//     k_timeout_t sleep_time = calculate_sleep_time();
    
//     zassert_true(sleep_time.ticks > 0, "Sleep time should be greater than zero");
    
//     clear_sleep_scheduler_timeout(2);
//     sleep_time = calculate_sleep_time();
    
//     zassert_equal(sleep_time.ticks, K_FOREVER.ticks, "Sleep time should be forever after clearing all timeouts");
// }

// ZTEST(sleep_scheduler, test_calculate_sleep_time_msgq)
// {
//     struct k_msgq msgq;
//     k_msgq_init(&msgq, NULL, 0, 0);
    
//     set_sleep_scheduler_timeout(3, 4000);
//     k_timeout_t sleep_time = calculate_sleep_time_msgq(&msgq);
    
//     zassert_true(sleep_time.ticks > 0, "Sleep time should be greater than zero with no messages in queue");
    
//     // Simulate a message in the queue
//     k_msgq_put(&msgq, NULL, K_NO_WAIT);
//     sleep_time = calculate_sleep_time_msgq(&msgq);
    
//     zassert_equal(sleep_time.ticks, K_NO_WAIT.ticks, "Sleep time should be no wait when messages are present");
    
//     clear_sleep_scheduler_timeout(3);
// }

ZTEST_SUITE(sleep_scheduler, NULL, NULL, NULL, NULL, NULL);


