#include <stdio.h>
#include <unistd.h>

#include "common/types.h"

//for CPU
static int read_cpu_times(
    unsigned long long *idle,
    unsigned long long *total)
{
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp)
        return -1;

    char line[256];
    unsigned long long user = 0, nice = 0, system = 0, idle_time = 0;
    unsigned long long iowait = 0, irq = 0, softirq = 0, steal = 0;

    if (!fgets(line, sizeof(line), fp))
    {
        fclose(fp);
        return -1;
    }

    // Parse first line which starts with 'cpu '
    int scanned = sscanf(line, "%*s %llu %llu %llu %llu %llu %llu %llu %llu",
                         &user, &nice, &system, &idle_time, &iowait, &irq, &softirq, &steal);

    fclose(fp);

    if (scanned < 4)
        return -1;

    *idle = idle_time + iowait;
    *total = user + nice + system + idle_time + iowait + irq + softirq + steal;

    return 0;
}

float get_ram_usage(void)
{
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp)
        return -1.0f;

    long total = 0;
    long available = 0;
    char line[256];

    while (fgets(line, sizeof(line), fp))
    {
        if (sscanf(line, "MemTotal: %ld kB", &total) == 1)
            ;
        else if (sscanf(line, "MemAvailable: %ld kB", &available) == 1)
            ;

        if (total > 0 && available > 0)
            break;
    }

    fclose(fp);

    if (total <= 0)
        return -1.0f;

    return 100.0f * (float)(total - available) / (float)total;
}

long get_uptime(void)
{
    FILE *fp = fopen("/proc/uptime", "r");

    if (!fp)
        return -1;

    double uptime;

    fscanf(fp, "%lf", &uptime);

    fclose(fp);

    return (long)uptime;
}


float get_cpu_usage(void)
{
    unsigned long long idle1;
    unsigned long long total1;

    unsigned long long idle2;
    unsigned long long total2;

    if (read_cpu_times(&idle1, &total1) != 0)
        return -1.0f;

    usleep(200000); // short sample interval

    if (read_cpu_times(&idle2, &total2) != 0)
        return -1.0f;

    unsigned long long idle_delta =
        idle2 - idle1;

    unsigned long long total_delta =
        total2 - total1;

    if (total_delta == 0)
        return 0.0f;

    float usage =
        100.0f *
        (1.0f -
         ((float)idle_delta /
          (float)total_delta));

    return usage;
}

int get_system_stats(SystemStats *stats)
{
    if (!stats)
        return -1;

    stats->cpu_usage = get_cpu_usage();

    stats->ram_usage = get_ram_usage();

    stats->uptime = get_uptime();

    return 0;
}