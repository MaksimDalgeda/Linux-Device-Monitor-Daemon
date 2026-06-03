#include <stdio.h>
#include <time.h>

#include "logger.h"

void log_info(
    const char *message)
{
    FILE *fp =
        fopen(
            "logs/system.log",
            "a");

    if (!fp)
        return;

    time_t now =
        time(NULL);

    struct tm *tm_info =
        localtime(&now);

    fprintf(
        fp,

        "[%04d-%02d-%02d %02d:%02d:%02d] [INFO] %s\n",

        tm_info->tm_year + 1900,
        tm_info->tm_mon + 1,
        tm_info->tm_mday,

        tm_info->tm_hour,
        tm_info->tm_min,
        tm_info->tm_sec,

        message);

    fclose(fp);
}