# Master Control Module

<a href="../../html/index.html">&larr; Back to xECU-pro overview</a>

## Overview

The master control module is xECU-pro's Central Domain Controller (CDC). It
owns the ECU register space, exchanges register traffic with other nodes over
SocketCAN (`vcan0` by default), and provides a Qt control panel for driving
and observing the register state.

## Components

| Class | Role |
|-------|------|
| RegisterMap | Data model for the 8-bit register space; emits a change signal only when a value actually changes |
| CanBus | Thin SocketCAN wrapper: sends raw frames and emits received frames and errors |
| MainWindow | Wires the register map, CAN bus and UI widgets together; handles register write/read requests |
| FlipSwitch | Animated power flip switch widget |
| ImageToggleSwitch | Image-based on/off toggle widget |
| LedIndicator | Coloured status LED widget |

## Data Flow

```
 CAN bus (vcan0)                      UI widgets
       │                                  │ toggled()
       ▼ frameReceived()                  ▼
  ┌─────────┐   write / read req   ┌────────────┐  write()  ┌─────────────┐
  │ CanBus  │ ───────────────────▶ │ MainWindow │ ────────▶ │ RegisterMap │
  │         │ ◀─────────────────── │            │ ◀──────── │             │
  └─────────┘  sendFrame()         └────────────┘ registerChanged()
```

## See Also

- <a href="../../html/index.html">xECU-pro system architecture</a>
