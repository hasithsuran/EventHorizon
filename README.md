# xECU-pro

A compact, dark-themed desktop control panel built with Qt6. Designed for embedded and ECU test environments where you need a clean, hardware-feel interface to monitor channel states and drive outputs from a PC.

---

## Features

- **Power flip switch** — animated side-to-side toggle with ON/OFF labels
- **4 channel bat switches** — image-based toggle switches with per-channel labels
- **4 LED indicators** — L1–L4 with full glow rendering in green, amber, and red
- **Dark gradient UI** — no-chrome window with a brushed steel aesthetic
- Smooth switch animations with 150 ms easing

---

## Requirements

- Qt 6.4 or later (Widgets module)
- CMake 3.20+
- C++17

---

## Build

```bash
cmake -S . -B build
cmake --build build
