# xECU-pro — Dev Environment Setup

## Prerequisites

- Fedora host with Docker installed
- VS Code with Dev Containers extension
- This repository cloned locally

---

## Host Setup (run once after each reboot)

### 1. Load the vcan kernel module and bring up the interface

```bash
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0
```

Verify:
```bash
ip link show vcan0
# expected: <NOARP,UP,LOWER_UP> ... state UNKNOWN
```

### 2. Make vcan0 persist across reboots (run once ever)

```bash
echo "vcan" | sudo tee /etc/modules-load.d/vcan.conf
```

Create `/etc/systemd/network/10-vcan0.netdev`:
```ini
[NetDev]
Name=vcan0
Kind=vcan
```

Create `/etc/systemd/network/10-vcan0.network`:
```ini
[Match]
Name=vcan0

[Link]
RequiredForOnline=no
```

Enable networkd:
```bash
sudo systemctl enable --now systemd-networkd
```

---

## devcontainer.json — Required runArgs

The container must run with `--privileged` and `--network=host` to access
the host's vcan0 interface:

```json
"runArgs": [
    "--privileged",
    "--network=host",
    "--env=DISPLAY",
    "--volume=/tmp/.X11-unix:/tmp/.X11-unix"
]
```

After any change to `devcontainer.json`:
`Ctrl+Shift+P` → **Dev Containers: Rebuild Container**

---

## Containerfile — Required Packages

Ensure these are present:

```dockerfile
# CAN headers
RUN apt-get update && apt-get install -y --no-install-recommends \
    linux-libc-dev \
    libc6-dev

# Qt6 SerialBus (for Qt CAN path)
RUN apt-get update && apt-get install -y --no-install-recommends \
    qt6-serialbus-dev

# Networking + CAN tools
RUN apt-get update && apt-get install -y --no-install-recommends \
    iproute2 \
    can-utils
```

---

## Build the Project (inside container)

```bash
cmake -B build -G Ninja
cmake --build build
```

---

## Run and Test

**Inside container — start the CAN reader:**
```bash
./build/xecu-pro
```

Expected output:
```
socket opened
interface index: 4
bound, waiting for frames...
```

**On host — send a test frame:**
```bash
cansend vcan0 123#DEADBEEF
```

**Expected output in container:**
```
ID: 0x123  DLC: 4  Data: DE AD BE EF
```

**Send frames repeatedly:**
```bash
watch -n 1 cansend vcan0 123#DEADBEEF
```

---

## Troubleshooting

| Symptom | Cause | Fix |
|---|---|---|
| `if_nametoindex: No such device` | vcan0 not up on host | Run modprobe + ip link steps above |
| `Device "vcan0" does not exist` inside container | `--network=host` not applied | Rebuild devcontainer, verify NetworkMode is `host` |
| `NetworkMode: bridge` | runArgs not picked up | Rebuild container after saving devcontainer.json |
| No output after `cansend` | vcan0 down on host | `sudo ip link set up vcan0` |
