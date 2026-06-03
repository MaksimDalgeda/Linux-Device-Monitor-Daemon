#ifndef TYPES_H
#define TYPES_H
#define HISTORY_SIZE 3600 //max one hour with 1 sec interval

typedef struct
{
    float cpu_usage;

    float ram_usage;

    float gpu_usage;

    unsigned int gpu_memory_used;

    unsigned int gpu_memory_total;

    int gpu_temperature;

    long uptime;
} SystemStats;

typedef struct
{
    SystemStats samples[HISTORY_SIZE];

    int current_index;

    int sample_count;

} StatsHistory;

#endif