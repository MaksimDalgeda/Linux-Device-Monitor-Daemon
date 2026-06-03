#include <stdio.h>

#include "common/types.h"
#include "gpu.h"

// Command used to query GPU stats via nvidia-smi (WSL path by default).
static const char *NVIDIA_SMI_CMD =
    "/usr/lib/wsl/lib/nvidia-smi --query-gpu=utilization.gpu,memory.used,memory.total,temperature.gpu --format=csv,noheader,nounits";

// Read a single CSV line from nvidia-smi and parse fields.
static int read_gpu_data(float *usage, unsigned int *used_mem, unsigned int *total_mem, int *temperature)
{
    FILE *fp = popen(NVIDIA_SMI_CMD, "r");
    if (!fp)
        return -1;

    char buf[256];
    if (!fgets(buf, sizeof(buf), fp))
    {
        pclose(fp);
        return -1;
    }

    int scanned = sscanf(buf, "%f, %u, %u, %d", usage, used_mem, total_mem, temperature);
    int rc = (scanned == 4) ? 0 : -1;

    pclose(fp);
    return rc;
}

// Populate GPU-related fields of `stats` with a single nvidia-smi invocation.
static int read_gpu_into_stats(SystemStats *stats)
{
    if (!stats)
        return -1;

    float usage = 0.0f;
    unsigned int used = 0, total = 0;
    int temp = 0;

    if (read_gpu_data(&usage, &used, &total, &temp) != 0)
        return -1;

    stats->gpu_usage = usage;
    stats->gpu_memory_used = used;
    stats->gpu_memory_total = total;
    stats->gpu_temperature = temp;

    return 0;
}

float get_gpu_usage(void)
{
    float usage;
    unsigned int used;
    unsigned int total;
    int temp;

    if (read_gpu_data(
            &usage,
            &used,
            &total,
            &temp) != 0)
    {
        return -1.0f;
    }

    return usage;
}

unsigned int get_gpu_memory_used(void)
{
    float usage;
    unsigned int used;
    unsigned int total;
    int temp;

    if (read_gpu_data(
            &usage,
            &used,
            &total,
            &temp) != 0)
    {
        return 0;
    }

    return used;
}

unsigned int get_gpu_memory_total(void)
{
    float usage;
    unsigned int used;
    unsigned int total;
    int temp;

    if (read_gpu_data(
            &usage,
            &used,
            &total,
            &temp) != 0)
    {
        return 0;
    }

    return total;
}

int get_gpu_temperature(void)
{
    float usage;
    unsigned int used;
    unsigned int total;
    int temp;

    if (read_gpu_data(
            &usage,
            &used,
            &total,
            &temp) != 0)
    {
        return -1;
    }

    return temp;
}

int update_gpu_stats(SystemStats *stats)
{
    if (!stats)
        return -1;

    // Use single invocation to fill all GPU fields efficiently.
    if (read_gpu_into_stats(stats) != 0)
        return -1;

    return 0;
}