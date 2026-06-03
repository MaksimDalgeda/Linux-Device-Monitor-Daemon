# Linux Device Monitor Daemon

Linux Device Monitor Daemon is a multi-threaded monitoring service written in C for Linux systems.

The daemon periodically collects CPU, RAM and GPU statistics and exposes them through a TCP command interface.

The project was created to practice Linux system programming concepts including threads, synchronization, networking, system services and hardware monitoring.

# Technologies Used

## C

Main programming language used throughout the entire project.

Files:

* src/main.c
* src/thread/*
* src/monitor/*
* src/gpu/*

Reason:

* Low-level Linux programming
* Direct interaction with operating system interfaces
* Widely used in Embedded Linux development

## POSIX Threads (pthreads)

Used to execute monitoring and server functionality concurrently.

Files:

* src/thread/monitor_thread.c
* src/thread/server_thread.c
* src/shared/shared_data.c

Reason:

* Monitoring continues independently from client connections
* Multiple tasks can execute simultaneously

## TCP Sockets

Used to expose monitoring information over the network.

Files:

* src/thread/server_thread.c

Reason:

* Remote clients can request monitoring information
* Allows future integration with desktop applications or dashboards

Example:

nc localhost 5000

## Linux procfs (/proc)

Used to collect CPU, RAM and uptime information directly from the Linux kernel.

Files:

* src/monitor/monitor.c

Files accessed:

* /proc/stat
* /proc/meminfo
* /proc/uptime

Reason:

* Standard Linux monitoring interface
* No external dependencies

## NVIDIA SMI

Used to collect GPU utilization, temperature and VRAM statistics.

Files:

* src/gpu/gpu.c

Reason:

* Provides accurate NVIDIA GPU metrics
* Commonly used on Linux systems

Command used:

nvidia-smi

## systemd

Used to run the application as a Linux service.

Files:

* service/linux-monitor.service

Reason:

* Automatic startup after reboot
* Standard Linux service management
* Production-style deployment

Commands:

sudo systemctl start linux-monitor

sudo systemctl stop linux-monitor

sudo systemctl restart linux-monitor

sudo systemctl status linux-monitor

## Mutex Synchronization

Used to protect shared monitoring data between threads.

Files:

* src/shared/shared_data.c
* src/thread/monitor_thread.c
* src/thread/server_thread.c

Reason:

* Prevents race conditions
* Ensures safe access to shared memory

## Ring Buffer

Used to store historical monitoring samples.

Files:

* src/thread/monitor_thread.c
* src/common/types.h

Reason:

* Constant memory usage
* Old samples are automatically overwritten
* Suitable for long-running daemons

Current capacity:

HISTORY_SIZE = 3600

With a 1 second update interval, approximately one hour of history can be stored.

# Architecture

Main Thread

Responsibilities:

* Load configuration
* Initialize shared data
* Start monitoring thread
* Start TCP server thread

Monitor Thread

Responsibilities:

* Collect CPU statistics
* Collect RAM statistics
* Collect GPU statistics
* Update shared data
* Store history samples

Server Thread

Responsibilities:

* Listen for incoming TCP connections
* Process client commands
* Return monitoring information

SharedData

Responsibilities:

* Store current statistics
* Store history buffer
* Synchronize access using mutexes

# Building

Compile the project:

make

# Deployment

Deploy using:

./scripts/deploy.sh

The script performs the following steps:

1. Build project
2. Stop running service
3. Copy binary to deployment directory
4. Start service
5. Display service status

# Viewing Logs

View live service logs:

./scripts/logs.sh

or

journalctl -u linux-monitor -f

# Running as a Service

Start service:

sudo systemctl start linux-monitor

Stop service:

sudo systemctl stop linux-monitor

Restart service:

sudo systemctl restart linux-monitor

Check service status:

sudo systemctl status linux-monitor

Enable startup on boot:

sudo systemctl enable linux-monitor

# Connecting to the Server

Connect using Netcat:

nc localhost 5000

The server listens on port 5000 by default.

# Available Commands

HELP

Displays available commands.

STATUS

Displays all currently collected statistics.

CPU

Displays current CPU utilization percentage.

RAM

Displays current RAM utilization percentage.

GPU

Displays current GPU utilization percentage.

TEMP

Displays current GPU temperature.

VRAM

Displays current VRAM usage.

UPTIME

Displays system uptime.

INFO

Displays daemon information and configuration.

HISTORY

Displays recently collected monitoring samples.

QUIT

Disconnects from the server.

# Example Session

nc localhost 5000

STATUS

CPU: 3.20%
RAM: 14.10%

GPU: 1%
GPU_TEMP: 50C

VRAM: 1200/12288 MB

UPTIME: 14500 sec

CPU

CPU: 3.20%

GPU

GPU: 1%

INFO

Linux Device Monitor Daemon

Version: 1.0

Monitoring:

* CPU
* RAM
* GPU

Server:

* TCP
* Port 5000

History:

* Enabled
* Samples: 125

QUIT

Goodbye.

# Features

* Multi-threaded architecture
* CPU monitoring
* RAM monitoring
* NVIDIA GPU monitoring
* Historical sample storage
* TCP command interface
* Configurable runtime settings
* Logging support
* Linux daemon mode
* systemd integration
* Thread-safe shared data access
* Circular history buffer
