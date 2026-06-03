#ifndef MONITOR_H
#define MONITOR_H

#include "common/types.h"

float get_cpu_usage(void);
float get_ram_usage(void);
long get_uptime(void);

int get_system_stats(SystemStats *stats);

#endif

