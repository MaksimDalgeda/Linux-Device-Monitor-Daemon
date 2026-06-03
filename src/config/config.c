#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"

Config g_config;

int load_config(
    const char *filename)
{
    FILE *fp =
        fopen(filename, "r");

    if (!fp)
    {
        perror("config");

        return -1;
    }

    char line[256];

    while (fgets(
               line,
               sizeof(line),
               fp))
    {
        char key[128];
        char value[128];

        if (sscanf(
                line,
                "%127[^=]=%127s",
                key,
                value) != 2)
        {
            continue;
        }

        if (strcmp(
                key,
                "server_port") == 0)
        {
            g_config.server_port =
                atoi(value);
        }

        else if (strcmp(
                     key,
                     "update_interval") == 0)
        {
            g_config.update_interval =
                atoi(value);
        }

        else if (strcmp(
                     key,
                     "history_size") == 0)
        {
            g_config.history_size =
                atoi(value);
        }
    }

    fclose(fp);

    return 0;
}