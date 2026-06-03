#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <fcntl.h>

#include "daemon.h"

int daemonize(void)
{
    pid_t pid =
        fork();

    if (pid < 0)
    {
        return -1;
    }

    if (pid > 0)
    {
        exit(0);
    }

    if (setsid() < 0)
    {
        return -1;
    }

    chdir("/");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    int fd =
        open(
            "/dev/null",
            O_RDWR);

    dup(fd);
    dup(fd);

    return 0;
}