#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <pthread.h>

#include "common/types.h"

typedef struct
{
    SystemStats stats;

    StatsHistory history;

    pthread_mutex_t mutex;

} SharedData;

int shared_data_init(SharedData *data);

void shared_data_destroy(SharedData *data);

#endif