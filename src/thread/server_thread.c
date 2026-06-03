#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#include "config/config.h"
#include "common/constants.h"
#include "server_thread.h"
#include "logger/logger.h"
#include "common/types.h"

static int send_all(int fd, const char *buf, size_t len)
{
    size_t total = 0;

    while (total < len)
    {
        ssize_t sent = send(fd, buf + total, len - total, 0);

        if (sent < 0)
        {
            if (errno == EINTR)
                continue;
            return -1;
        }

        total += (size_t)sent;
    }

    return 0;
}

static const char *WELCOME_MESSAGE =
    "\n"
    "Linux Device Monitor Daemon\n"
    "\n"
    "Available commands:\n"
    "\n"
    "HELP - Display available commands\n"
    "STATUS - Display system status\n"
    "CPU - Display CPU usage\n"
    "RAM - Display RAM usage\n"
    "GPU - Display GPU usage\n"
    "TEMP - Display GPU temperature information\n"
    "VRAM - Display VRAM usage\n"
    "UPTIME - Display system uptime\n"
    "INFO - Display daemon information\n"
    "HISTORY - Display recent history samples\n"
    "QUIT - Disconnect from the server\n"
    "\n"
    "> ";


// Send a NUL-terminated text block to the client. Returns 0 on success.
static int send_text(int client_fd, const char *text)
{
    if (send_all(client_fd, text, strlen(text)) < 0)
    {
        perror("send");
        return -1;
    }
    return 0;
}

static void fetch_stats(SharedData *shared, SystemStats *out)
{
    pthread_mutex_lock(&shared->mutex);
    *out = shared->stats;
    pthread_mutex_unlock(&shared->mutex);

}


// Handle a single client text command. Returns 0 to continue, 1 to disconnect.
static int handle_command(int client_fd, SharedData *shared, const char *command)
{
    if (strcmp(command, "HELP") == 0)
    {
        send_text(client_fd, WELCOME_MESSAGE);
        return 0;
    }

    if (strcmp(command, "QUIT") == 0)
    {
        send_text(client_fd, "Goodbye.\n");
        return 1;
    }

    if (strcmp(command, "CPU") == 0)
    {
        SystemStats s;
        fetch_stats(shared, &s);
        char buf[128];
        snprintf(buf, sizeof(buf), "CPU: %.2f%%\n> ", s.cpu_usage);
        send_text(client_fd, buf);
        return 0;
    }

    if (strcmp(command, "RAM") == 0)
    {
        SystemStats s;
        fetch_stats(shared, &s);
        char buf[128];
        snprintf(buf, sizeof(buf), "RAM: %.2f%%\n> ", s.ram_usage);
        send_text(client_fd, buf);
        return 0;
    }

    if (strcmp(command, "GPU") == 0)
    {
        SystemStats s;
        fetch_stats(shared, &s);
        char buf[128];
        snprintf(buf, sizeof(buf), "GPU: %.0f%%\n> ", s.gpu_usage);
        send_text(client_fd, buf);
        return 0;
    }

    if (strcmp(command, "TEMP") == 0)
    {
        SystemStats s;
        fetch_stats(shared, &s);
        char buf[128];
        snprintf(buf, sizeof(buf), "GPU_TEMP: %dC\n> ", s.gpu_temperature);
        send_text(client_fd, buf);
        return 0;
    }

    if (strcmp(command, "VRAM") == 0)
    {
        SystemStats s;
        fetch_stats(shared, &s);
        char buf[128];
        snprintf(buf, sizeof(buf), "VRAM: %u/%u MB\n> ", s.gpu_memory_used, s.gpu_memory_total);
        send_text(client_fd, buf);
        return 0;
    }

    if (strcmp(command, "UPTIME") == 0)
    {
        SystemStats s;
        fetch_stats(shared, &s);
        char buf[128];
        snprintf(buf, sizeof(buf), "UPTIME: %ld sec\n> ", s.uptime);
        send_text(client_fd, buf);
        return 0;
    }

    if (strcmp(command, "INFO") == 0)
    {
        char buf[1024];
        pthread_mutex_lock(&shared->mutex);
        int samples = shared->history.sample_count;
        pthread_mutex_unlock(&shared->mutex);

        snprintf(buf, sizeof(buf),
                 "Linux Device Monitor Daemon\n\n"
                 "Version: 1.0\n\n"
                 "Monitoring:\n"
                 " - CPU\n"
                 " - RAM\n"
                 " - GPU\n\n"
                 "Server:\n"
                 " - TCP\n"
                 " - Port %d\n\n"
                 "History:\n"
                 " - Enabled\n"
                 " - Samples: %d\n\n> ",
                 g_config.server_port,
                 samples);

        send_text(client_fd, buf);
        return 0;
    }

    if (strcmp(command, "HISTORY") == 0)
    {
        char buf[4096];
        int offset = 0;

        pthread_mutex_lock(&shared->mutex);

        offset += snprintf(buf + offset, sizeof(buf) - offset,
                           "Last History Samples\n\n"
                           "#    CPU     RAM     GPU\n");

        int count = shared->history.sample_count;
        if (count > 20)
            count = 20;

        int start = shared->history.current_index - count;
        if (start < 0)
            start += HISTORY_SIZE;

        for (int i = 0; i < count; i++)
        {
            int index = (start + i) % HISTORY_SIZE;
            SystemStats *s = &shared->history.samples[index];
            offset += snprintf(buf + offset, sizeof(buf) - offset,
                               "%-4d %-7.1f %-7.1f %-7.0f\n",
                               i + 1,
                               s->cpu_usage,
                               s->ram_usage,
                               s->gpu_usage);
        }

        pthread_mutex_unlock(&shared->mutex);

        offset += snprintf(buf + offset, sizeof(buf) - offset, "\n> ");
        send_text(client_fd, buf);
        return 0;
    }

    if (strcmp(command, "STATUS") == 0)
    {
        SystemStats s;
        fetch_stats(shared, &s);
        char buf[512];
        snprintf(buf, sizeof(buf),
                 "CPU: %.2f%%\n"
                 "RAM: %.2f%%\n\n"
                 "GPU: %.0f%%\n"
                 "GPU_TEMP: %dC\n\n"
                 "VRAM: %u/%u MB\n\n"
                 "UPTIME: %ld sec\n\n> ",
                 s.cpu_usage,
                 s.ram_usage,
                 s.gpu_usage,
                 s.gpu_temperature,
                 s.gpu_memory_used,
                 s.gpu_memory_total,
                 s.uptime);

        send_text(client_fd, buf);
        return 0;
    }

    /* Unknown command */
    send_text(client_fd, "Unknown command\n> ");
    return 0;

}

void *server_thread(void *arg)
{
    SharedData *shared =
        (SharedData *)arg;

    int server_fd =
        socket(AF_INET,
               SOCK_STREAM,
               0);

    if (server_fd < 0)
    {
        perror("socket");
        return NULL;
    }

    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(SERVER_PORT);

    if (bind(server_fd,
             (struct sockaddr *)&addr,
             sizeof(addr)) < 0)
    {
        perror("bind");

        close(server_fd);

        return NULL;
    }

    if (listen(server_fd, 5) < 0)
    {
        perror("listen");

        close(server_fd);

        return NULL;
    }

    printf("TCP server listening on %d\n",
           SERVER_PORT);

    while (1)
    {
        int client_fd =
            accept(server_fd,
                   NULL,
                   NULL);

        if (client_fd < 0)
        {
            perror("accept");
            continue;
        }

        printf("Client connected\n");
        log_info("Client connected");

        send_text(
            client_fd,
            WELCOME_MESSAGE);

        char command[128];

        // Main per-client receive/command loop
        while (1)
        {
            memset(command, 0, sizeof(command));

            int received = recv(client_fd, command, sizeof(command) - 1, 0);
            if (received <= 0)
                break;

            // Strip newline characters
            command[strcspn(command, "\r\n")] = '\0';

            // Handle the received command. If handle_command returns non-zero,
            // the client should be disconnected.
            int cmd_result = handle_command(client_fd, shared, command);
            if (cmd_result != 0)
                break;
        }

        close(client_fd);

        printf("Client disconnected\n");
        log_info("Client disconnected");
    }

    close(server_fd);

    return NULL;
}