#include <unistd.h>
#include <stdio.h>

#include "monitor/monitor.h"
#include "gpu/gpu.h"

#include "monitor_thread.h"

void *monitor_thread(void *arg)
{
    SharedData *shared =
        (SharedData *)arg;

    while (1)
    {
        pthread_mutex_lock(
            &shared->mutex);

        get_system_stats(
            &shared->stats);

        update_gpu_stats(
            &shared->stats);

        shared->history.samples[
            shared->history.current_index] =
            shared->stats;

        shared->history.current_index++;

        if (shared->history.current_index
            >= HISTORY_SIZE)
        {
            shared->history.current_index = 0;
        }

        if (shared->history.sample_count
            < HISTORY_SIZE)
        {
            shared->history.sample_count++;
        }
    
        fflush(stdout);

        pthread_mutex_unlock(
            &shared->mutex);

        sleep(1);
    }

    return NULL;
}