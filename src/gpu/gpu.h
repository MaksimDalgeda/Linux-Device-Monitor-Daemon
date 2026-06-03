#ifndef GPU_H
#define GPU_H

#include "common/types.h"

float get_gpu_usage(void);

unsigned int get_gpu_memory_used(void);

unsigned int get_gpu_memory_total(void);

int get_gpu_temperature(void);

int update_gpu_stats(SystemStats *stats);
#endif