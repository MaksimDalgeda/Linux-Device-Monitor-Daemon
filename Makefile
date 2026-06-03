CC = gcc

TARGET = linux-monitor

CFLAGS = -Wall -Wextra -pthread -I./src

SRC = \
	src/main.c \
	src/monitor/monitor.c \
	src/gpu/gpu.c \
	src/network/server.c \
	src/logger/logger.c \
	src/config/config.c \
	src/daemon/daemon.c \
	src/shared/shared_data.c \
	src/thread/monitor_thread.c \
    src/thread/server_thread.c

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

re: clean all