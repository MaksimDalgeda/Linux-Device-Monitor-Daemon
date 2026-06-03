#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include "network/server.h"
#include "common/constants.h"

// Create, bind and listen on a TCP socket. Returns listening fd or -1.
static int create_listen_socket(int port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("socket");
        return -1;
    }

    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        close(fd);
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        close(fd);
        return -1;
    }

    if (listen(fd, 5) < 0)
    {
        perror("listen");
        close(fd);
        return -1;
    }

    return fd;
}

// Start the server, accept a single connection and return. The `stats` pointer
// is currently unused but kept for API compatibility.
int start_server(SystemStats *stats)
{
    (void)stats;

    int server_fd = create_listen_socket(SERVER_PORT);
    if (server_fd < 0)
        return -1;

    printf("Listening on port %d\n", SERVER_PORT);

    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0)
    {
        perror("accept");
        close(server_fd);
        return -1;
    }

    printf("Client connected\n");

    close(client_fd);
    close(server_fd);

    return 0;
}