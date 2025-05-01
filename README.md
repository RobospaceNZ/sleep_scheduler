# Sleep Scheduler

## How to use

The sleep scheduler only consists of an h file, all functions are static. To use it, a couple of defines are needed before the include:

```
#define SCHEDULER_FIRST_TIMEOUT                 0
#define SCHEDULER_SECOND_TIMEOUT                1
#define SCHEDULER_COUNT                         2       // Number of scheduled events, must be the last one and one more than the last timeout
#include "sleep_scheduler.h"
```

set_sleep_scheduler_timeout is used to set a new timeout. For example, to set SCHEDULER_FIRST_TIMEOUT to timeout in 6 second:
```
set_sleep_scheduler_timeout(SCHEDULER_FIRST_TIMEOUT, 6000);
```

clear_sleep_scheduler_timeout set a timeout to 0:
```
clear_sleep_scheduler_timeout(SCHEDULER_FIRST_TIMEOUT);
```

is_sleep_scheduler_timeout_active returns true if a timeout value is not zero.

check_sleep_scheduler_timeout is called to check if a timeout has expired. In your main loop you would typically call it like this:
```
if (check_sleep_scheduler_timeout(SCHEDULER_FIRST_TIMEOUT, true)) {
    // Do work needed at this interval
    if (repeat_timeout) {
        set_sleep_scheduler_timeout(SCHEDULER_FIRST_TIMEOUT, NEXT_TIMEOUT);
    } else {
        clear_sleep_scheduler_timeout(SCHEDULER_FIRST_TIMEOUT);
    }
}
```

In the example above, check_sleep_scheduler_timeout has true as it's second argument. This will check if the timeout is zero. If it is zero then it will treat it as disabled and will return false. If you need the timeout to be active, even if it is zero then use:
```
if (check_sleep_scheduler_timeout(SCHEDULER_FIRST_TIMEOUT, false)) {
```

calculate_sleep_time is used to calculate the amount of time to sleep until the next timeout. Below is an example of how this will be used in your thread:
```
K_SEM_DEFINE(your_sem, 0, 1);

static void your_thread_fn(void) {
    k_timeout_t sleep_time = K_MSEC(0);

    ...
    while (1) {
        k_sem_take(&your_sem, sleep_time);
        ...
        sleep_time = calculate_sleep_time();
    }
```
The semaphore allows other parts of your code to immediately wake up your thread, otherwise it will sleep until the next timeout.

calculate_sleep_time_msgq is similar to calculate_sleep_time but checks if there are items in your message queue. If so, the thread will be kept alive until all message queue items are processed.

## Import into Zephyr

This code can be cloned as part of the user's code, but can also be cloned into your Zephyr library. The advantage of this is, if it is cloned into Zephyr once, it will be available to all your Zephyr projects.

For the examples below, we are assuming Nordic NCS 2.9.0. Please adjust to your version.

If you are using Nordic's NCS, open west.yml located at:<br>
C:\ncs\v2.9.0\nrf\west.yml

Under remotes, add:
```
    - name: sleep_scheduler
      url-base: https://github.com/RobospaceNZ
```

Under projects, add:
```
    - name: sleep_scheduler
      remote: sleep_scheduler
      revision: V1.0.4
      path: modules/lib/sleep_scheduler
```

Open command prompt in C:\ncs\v2.9.0. Remember to update b620d30767 if you use a different NCS (See C:\ncs\toolchains\toolchains.json for the version codes). Run the following:
```
SET PATH=C:\ncs\toolchains\b620d30767\opt\bin;C:\ncs\toolchains\b620d30767\opt\bin\Scripts;%PATH%
west update
```

The sleep scheduler code will be located at:<br>
C:\ncs\v2.9.0\modules\lib\sleep_scheduler

Add the following to your CMakeLists.txt file:
```
zephyr_include_directories($ENV{ZEPHYR_BASE}/../modules/lib/sleep_scheduler)
```
