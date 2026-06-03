#include <string.h>

#include "shared_data.h"

int shared_data_init(SharedData *data)
{
    if (!data)
        return -1;

    memset(
        data,
        0,
        sizeof(SharedData));

    return pthread_mutex_init(
        &data->mutex,
        NULL);
}
void shared_data_destroy(
    SharedData *data)
{
    if (!data)
        return;

    pthread_mutex_destroy(
        &data->mutex);
}