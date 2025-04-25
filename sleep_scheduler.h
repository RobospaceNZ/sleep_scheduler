#ifndef CUSTOM_SLEEP_SCHEDULER_H
#define CUSTOM_SLEEP_SCHEDULER_H

/* To use this scheduler, you fir st need to have the following defines and then include this h file:
#define SCHEDULER_FIRST_TIMEOUT                 0
#define SCHEDULER_SECOND_TIMEOUT                1
...
#define SCHEDULER_COUNT                         6       // Number of scheduled events, must be the last one and one more than the last timeout
#include "sleep_scheduler.h"
*/

#define SCHEDULER_TIME_SLIPPAGE                 3      // ms, make sure it is past the time scheduled to avoid unnecessary wakeups

static int64_t scheduler_times[SCHEDULER_COUNT];

inline static void set_sleep_scheduler_timeout(uint32_t index, int64_t timeout) {
    scheduler_times[index] = k_uptime_get() + timeout;
}

inline static void clear_sleep_scheduler_timeout(uint32_t index) {
    scheduler_times[index] = 0;
}

inline static bool is_sleep_scheduler_timeout_active(uint32_t index) {
    return scheduler_times[index] != 0;
}

// check_active == true : return false if scheduler time == 0
inline static bool check_sleep_scheduler_timeout(uint32_t index, bool check_active) {
    if (check_active) {
        if (scheduler_times[index] && (k_uptime_get() >= scheduler_times[index])) {
            return true;
        }
    } else {
        if (k_uptime_get() >= scheduler_times[index]) {
            return true;
        }
    }
    return false;
}

inline static k_timeout_t calculate_sleep_time(void) {
    int64_t now = k_uptime_get();
    int64_t next_service_time = 0;
    uint8_t i;
    bool found = false;

    for (i = 0; i < SCHEDULER_COUNT; i++) {
        if (scheduler_times[i] > now) {
            if (!next_service_time) {
                next_service_time = scheduler_times[i];
            } else if (scheduler_times[i] < next_service_time) {
                next_service_time = scheduler_times[i];
            }
            found = true;
        }
    }
    if (found) {
        next_service_time -= now;
        next_service_time += SCHEDULER_TIME_SLIPPAGE;
        return K_MSEC(next_service_time);
    }
    return K_FOREVER;   // No events scheduled
}

#endif // CUSTOM_SLEEP_SCHEDULER_H