# Master Control Module

<a href="../../html/index.html">&larr; Back to xECU-pro overview</a>

## Overview

The master control module is xECU-pro's Central Domain Controller (CDC). It
owns the ECU register space, exchanges register traffic with other nodes over
SocketCAN (`vcan0` by default), and provides a Qt Quick (QML) control panel for
driving and observing the register state.

The application is split into a C++ backend and a QML front end:

- **Backend (C++)** — protocol, CAN I/O and the register map. No UI code.
- **Front end (QML)** — the panel window and a small library of hardware-style
  controls. It only binds to backend properties and calls backend methods.

## Components

| Part | Kind | Role |
|------|------|------|
| RegisterMap | C++ | Data model for the 8-bit register space; emits a change signal only when a value actually changes. Single source of truth for all UI state |
| CanBus | C++ | Non-blocking SocketCAN wrapper: sends raw frames and emits received frames, errors and open/closed state |
| ControlPanel | C++ (QML type) | Controller: owns CanBus and RegisterMap, handles register write/read requests, and exposes power/channel/LED state to QML as properties |
| Protocol.h | C++ | CAN IDs and register addresses |
| `qml/Main.qml` | QML | Panel window: layout, labels, footer; binds controls to ControlPanel |
| `EvH.MasterControl.UI` | QML module | Reusable controls in `ui/`: FlipSwitch, ImageToggleSwitch, LedIndicator |

## QML modules

| URI | CMake target | Contents |
|-----|--------------|----------|
| `EvH.MasterControl` | `master-control-module` (executable) | ControlPanel (C++), `Main.qml`, switch images |
| `EvH.MasterControl.UI` | `master-control-module-ui` (static) | FlipSwitch, ImageToggleSwitch, LedIndicator, internal GradientEllipse |

Both modules are compiled with `qmlcachegen` and embedded as resources under
`qrc:/qt/qml/`; the executable has no runtime dependency on files next to it.
`qmllint` runs over both with `cmake --build . --target all_qmllint`.

## Data Flow

```
 CAN bus (vcan0)                                            QML (Main.qml)
       │                                                      │       ▲
       ▼ frameReceived()                    setPower()        │       │ property bindings
  ┌─────────┐  write / read req   ┌──────────────┐ ◀─────────┘       │ (powerOn, channelStates,
  │ CanBus  │ ──────────────────▶ │ ControlPanel │ ──────────────────┘  ledColors, busOnline)
  │         │ ◀────────────────── │              │
  └─────────┘ sendFrame()         └──────────────┘
                                     │      ▲
                             write() │      │ registerChanged()
                                     ▼      │
                                  ┌─────────────┐
                                  │ RegisterMap │
                                  └─────────────┘
```

The controls are *controlled components*: a click emits `toggleRequested(on)`
and the control only moves once the backend has updated the register and the
bound property changes. The panel therefore always shows the register map, also
when a register is written over CAN (`0x100`).

## Runtime options and logging

| Option | Default | Meaning |
|--------|---------|---------|
| `--can-interface <name>` | `vcan0` | SocketCAN interface. If it is missing the panel still runs and shows "CAN … offline" |

| Logging category | Content |
|------------------|---------|
| `evh.mcm.can` | Interface open/errors; every received frame at debug level |
| `evh.mcm.panel` | Controller warnings (CAN errors, bad requests) |

Example: `QT_LOGGING_RULES="evh.mcm.can.debug=false" ./master-control-module`.
Some distributions (e.g. Fedora) disable debug output by default and send Qt
logs to the journal; use `QT_LOGGING_RULES="evh.*.debug=true"` and
`QT_FORCE_STDERR_LOGGING=1` to see them on the terminal.

## Compositor integration

The compositor tiles this window by its Wayland app_id, which Qt derives from
the executable name (`master-control-module`). Do not set an organization
domain or desktop file name in `main.cpp`, as either changes the app_id. The
window has a fixed width of 280 px and no height limits, so the compositor can
size it to the full column height.

## Qt version support

Qt 6.4 (Debian 12 dev container) through current Qt 6. Version-sensitive
choices:

- QML module resources use an explicit `RESOURCE_PREFIX /qt/qml`, and
  `main.cpp` adds `qrc:/qt/qml` to the import path (default only from Qt 6.5).
- Gradient ellipses are drawn with `Canvas` rather than `Shape`: antialiased
  Shapes need either the curve renderer (Qt 6.6+) or multisampling, which is
  unavailable with Mesa llvmpipe inside the compositor.

## See Also

- <a href="../../html/index.html">xECU-pro system architecture</a>
