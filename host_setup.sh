#!/bin/bash
# setup-vcan.sh — run on host after each reboot

set -e

sudo modprobe vcan

if ip link show vcan0 &>/dev/null; then
    echo "vcan0 already exists, bringing up..."
    sudo ip link set up vcan0
else
    sudo ip link add dev vcan0 type vcan
    sudo ip link set up vcan0
fi

ip link show vcan0
echo "vcan0 is ready"


#enable x11 window forwarding
xhost +local:docker