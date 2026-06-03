#ifndef CONFIG_H
#define CONFIG_H

typedef struct
{
    int server_port;

    int update_interval;

    int history_size;

} Config;

extern Config g_config;

int load_config(
    const char *filename);

#endif