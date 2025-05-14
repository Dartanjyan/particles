#include <time.h>
// #include <errno.h>

#include "time_mgmt.h"

void tick(float fps) {
    static struct timespec last_time;
    struct timespec current_time;
    long frame_delay_ns = (long)(1e9 / fps);

    clock_gettime(CLOCK_MONOTONIC, &current_time);

    if (last_time.tv_sec != 0) {
        long elapsed_ns = (current_time.tv_sec - last_time.tv_sec) * 1e9 +
                         (current_time.tv_nsec - last_time.tv_nsec);

        if (elapsed_ns < frame_delay_ns) {
            struct timespec sleep_time = {
                .tv_sec = 0,
                .tv_nsec = frame_delay_ns - elapsed_ns
            };
            nanosleep(&sleep_time, NULL);
        }
    }

    last_time = current_time;
}
