# Sleep Scheduler

## How to use

The sleep scheduler only consists of an h file, all functions are static. To use it, a couple of defines are needed before the include:

```
#define SCHEDULER_FIRST_TIMEOUT                 0
#define SCHEDULER_SECOND_TIMEOUT                1
#define SCHEDULER_COUNT                         2       // Number of scheduled events, must be the last one and one more than the last timeout
#include "sleep_scheduler.h"
```

set_sleep_scheduler_timeout is used to set a new timeout. For example, to set SCHEDULER_FIRST_TIMEOUT to timeout in 6 seconds:
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

In the example above, check_sleep_scheduler_timeout has true as its second argument. This will check if the timeout is zero. If it is zero then it will treat it as disabled and will return false. If you need the timeout to be active, even if it is zero then use:
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

SCHEDULER_TIME_SLIPPAGE is added to the sleep time, so the thread wakes up just after the timeout and not just before it. It defaults to 1 ms. To change it, define it before including sleep_scheduler.h.

### Things to be aware of

The timeouts must only be used by the thread that owns them. They are 64 bit values, so reading and writing them is not atomic. If another thread needs to start a timeout, send it a message or give its semaphore instead.

Each file that includes sleep_scheduler.h gets its own timeouts.

calculate_sleep_time only uses timeouts that have not expired yet. A timeout that expires after it was checked, but before calculate_sleep_time is called, is only checked again the next time the thread wakes up. If your loop can block for a long time between checking the timeouts and calculating the sleep time, make sure something else wakes the thread up, for example another timeout that is set again every time it expires.

## Changes in V1.1.0

sleep_scheduler is now a Zephyr module, see [Import into Zephyr](#import-into-zephyr). Enable it with
`CONFIG_SLEEP_SCHEDULER=y` and remove the `zephyr_include_directories` line from your CMakeLists.txt. There are no
changes to the code.

## Import into Zephyr

sleep_scheduler is a Zephyr module (see zephyr/module.yml), so Zephyr finds it by itself once it is in the west workspace or
listed in `ZEPHYR_EXTRA_MODULES`. No changes to CMakeLists.txt or Kconfig are needed, enable it in prj.conf:
```
CONFIG_SLEEP_SCHEDULER=y
```

It is header only, `CONFIG_SLEEP_SCHEDULER` adds sleep_scheduler.h to the include path.

### Workspace application

If your application has its own west.yml (a workspace application, for example one started from Nordic's
ncs-example-application), add the library to that west.yml. Don't edit the west.yml of the SDK (nrf/west.yml), the
change is lost when the SDK is updated and is not part of your project.

Under remotes, add (once for all Robospace libraries):
```
    - name: robospace
      url-base: https://github.com/RobospaceNZ
```

Under projects, add:
```
    - name: sleep_scheduler
      remote: robospace
      revision: V1.1.0
      path: modules/lib/sleep_scheduler
```

Then run `west update` in the workspace folder.

### Freestanding application

If your application uses an SDK installed under C:\ncs, add the library to your application, for example as a git
submodule:
```
git submodule add https://github.com/RobospaceNZ/sleep_scheduler libs/sleep_scheduler
```

Add the following to your CMakeLists.txt file, before `find_package(Zephyr ...)`:
```
list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/libs/sleep_scheduler)
```
