#!/bin/bash

set -e

echo ""
echo "======================"
echo " Linux Monitor Deploy "
echo "======================"
echo ""

echo "[1/5] Building project..."
make

echo ""
echo "[2/5] Stopping service..."
sudo systemctl stop linux-monitor

echo ""
echo "[3/5] Deploying binary..."
sudo cp linux-monitor /opt/linux-monitor/

echo ""
echo "[4/5] Starting service..."
sudo systemctl start linux-monitor

echo ""
echo "[5/5] Checking status..."
sudo systemctl status linux-monitor --no-pager

echo ""
echo "Deployment completed."