# Linux Device Monitor Daemon

Implemented a multi-threaded Linux Device Monitor Daemon in C.

The daemon periodically collects CPU, RAM and NVIDIA GPU statistics using Linux system interfaces and exposes them through a TCP command interface. The project uses POSIX threads with mutex-protected shared data and a circular history buffer for storing monitoring samples.

I added a configuration system, logging support and systemd integration to make the application behave like a real Linux service rather than a simple console program.

## Main technologies used

C – core application implementation
POSIX Threads (pthreads) – concurrent monitoring and networking
TCP Sockets – remote client communication
Linux procfs (/proc) – CPU, RAM and uptime monitoring
NVIDIA SMI – GPU statistics collection
systemd – Linux service management
Mutex synchronization – thread-safe shared data access
Ring Buffer – fixed-size history storage

## Features

CPU monitoring
RAM monitoring
NVIDIA GPU monitoring
Historical metrics storage
TCP command interface
Configuration file support
Logging system
Daemon mode
systemd service integration

## Build
make

## Deploy
./scripts/deploy.sh

## Start service
sudo systemctl start linux-monitor

## Check status
sudo systemctl status linux-monitor

## Connect to server
nc localhost 5000 (from other cmd)


## Available commands
HELP
INFO
STATUS
CPU
RAM
GPU
TEMP
VRAM
UPTIME
HISTORY
QUIT


## Scripts

deploy.sh -  Automates project build and deployment.

logs.sh - Displays real-time logs from the linux-monitor service.
