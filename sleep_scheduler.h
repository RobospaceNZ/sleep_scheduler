/*
 * sleep_scheduler.c
 *
 * Copyright (c) 2025 Hubert Jetschko
 *
 * This file is licensed under the MIT License.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef ZEPHYR_SLEEP_SCHEDULER_H
#define ZEPHYR_SLEEP_SCHEDULER_H

#define SCHEDULER_TIME_SLIPPAGE                 1      // ms, make sure it is past the time scheduled to avoid unnecessary wakeups

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

#endif // ZEPHYR_SLEEP_SCHEDULER_H