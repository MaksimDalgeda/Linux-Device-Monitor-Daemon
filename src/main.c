#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>


#include "shared/shared_data.h"
#include "thread/monitor_thread.h"
#include "thread/server_thread.h"
#include "config/config.h"
#include "logger/logger.h"
#include "daemon/daemon.h"


static int parse_args(int argc, char *argv[])
{
    if (argc > 1 && strcmp(argv[1], "--daemon") == 0)
        return 1;

    return 0;
}

static void print_startup_info(int daemon_mode)
{
    char buf[128];

    snprintf(buf, sizeof(buf), "Config loaded: port=%d interval=%d", g_config.server_port, g_config.update_interval);
    log_info(buf);

    if (!daemon_mode && isatty(STDOUT_FILENO))
    {   
    printf("Port: %d\n", g_config.server_port);
    printf("Update Interval: %d\n", g_config.update_interval);
    }
}

static int start_threads(SharedData *shared, pthread_t *monitor_tid, pthread_t *server_tid, int daemon_mode)
{
    if (shared_data_init(shared) != 0)
    {
        if (!daemon_mode)
            printf("Failed to initialize shared data\n");
        return -1;
    }

    if (pthread_create(monitor_tid, NULL, monitor_thread, shared) != 0)
    {
        if (!daemon_mode)
            printf("Failed to create monitor thread\n");
        shared_data_destroy(shared);
        return -1;
    }

    

    if (pthread_create(server_tid, NULL, server_thread, shared) != 0)
    {
        if (!daemon_mode)
            printf("Failed to create server thread\n");

        pthread_cancel(*monitor_tid);
        shared_data_destroy(shared);
        return -1;
    }

    log_info("Server started");
    log_info("Monitor thread started");
    log_info("TCP server thread started");

    if (!daemon_mode && isatty(STDOUT_FILENO))
    {
    printf("Linux Monitor Started\n");
    }   

    return 0;
}

int main(int argc, char *argv[])
{
    int daemon_mode = parse_args(argc, argv);

    if (load_config("config/config.ini") != 0)
        return 1;

    if (daemon_mode)
    {
        if (daemonize() != 0)
        {
            printf("Failed to daemonize\n");
            return 1;
        }

        log_info("Daemon mode enabled");
    }

    print_startup_info(daemon_mode);

    SharedData shared;
    pthread_t monitor_tid;
    pthread_t server_tid;

    if (start_threads(&shared, &monitor_tid, &server_tid, daemon_mode) != 0)
        return 1;

   pthread_join(
    monitor_tid,
    NULL);

    pthread_join(
    server_tid,
    NULL);

    return 0;
}