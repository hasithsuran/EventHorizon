# CLAUDE.md

Context for Claude Code sessions working on this repository.

## Project

**EventHorizon** (formerly xECU-pro; many files and the repo folder still use the old name) is a
Mars-rover-style simulator modelled on NASA's Curiosity. Qt 6 / C++20 on Linux, built with CMake,
developed inside a VS Code dev container (`/workspaces/xECU-pro`). SocketCAN on `vcan0` must be set
up on the host — see `setup-guide.md`.

## Layout

| Path | What |
|------|------|
| `apps/compositor` | Nested Wayland compositor (QML + C++); tiles the other apps in one window, socket `xecu-wayland-0` |
| `apps/master-control-module` | Qt Widgets operator panel; currently also owns `CanBus` + `RegisterMap` (to be moved to a service) |
| `apps/SimFy` | OpenGL 3.3 / Assimp 3D rover viewer |
| `services/` | Empty — planned backend services live here |
| `docs/overview.md` | System overview, Curiosity reference, and the planned runtime architecture |
| `cmake/` | `XecuDocs.cmake` (`xecu_add_component_docs()`), `XecuFormat.cmake` (`format`, `format-check` targets) |

## Build

- Full configure needs Qt 6, assimp (SimFy) and optionally Doxygen. The `build/` dir is configured
  for the dev container path; on the host use a separate build directory.
- Targets: per-app executables, `docs`, `<component>-docs`, `docs-serve`, `format`, `format-check`.
- Sources are picked up with `file(GLOB_RECURSE ...)` — new files/subdirs need no CMake edits
  (re-run configure).
- Known issue: `master-control-module/src/MainWindow.cpp` fails on the host's Qt 6.10
  (`QByteArray({char, char})` is ambiguous). Pre-existing; not yet fixed.

## Code Conventions

- **Namespaces:** root `EvH`, second level is the module: `EvH::SimFy`, `EvH::MasterControl`.
  Sub-areas get a nested namespace **and** a matching subdirectory under both `include/` and `src/`:
  - `EvH::SimFy::Rendering` → `include/renderer/`, `src/renderer/`
  - `EvH::MasterControl::UI` → `include/ui/`, `src/ui/` (FlipSwitch, ImageToggleSwitch, LedIndicator)
- Use `namespace A::B::C { ... } // namespace A::B::C` (C++17 nested form).
- File-local helpers go in an unnamed namespace *inside* the module namespace (not `static`).
- Forward declarations must be in the correct namespace block.
- Includes use angle brackets relative to `include/`: `#include <ui/FlipSwitch.h>`.
- Every file starts with the MIT copyright/SPDX header; Doxygen `@file`/`@brief` on documented files.
- Formatting: `.clang-format` at the root.

## Architecture Direction (decided in design discussions, not yet built)

Full write-up: `docs/overview.md` → *Runtime Architecture — Applications and Services (Planned)*.

- **Apps vs services:** apps are thin Qt clients; services are the simulated rover and run headless.
- **Services as systemd user units**, grouped by `evh.target`:
  - `evh-hw.service` — simulated hardware: power distribution (starts/stops OBCs via systemd D-Bus
    API), watchdogs (on sim time), A/B side latch, boot counters, hardware command decoder,
    NVM fault injection, sim clock. `Type=notify`, `WatchdogSec=`, `Restart=always`.
  - `evh-obc@A|B.service` — master control flight software (CAN, register map, mode state
    machine, persistence). Same binary, side from `%i`. `Restart=no` — only `evh-hw` restarts it.
  - `evh-link.service` — Earth↔rover link sim (light-time delay, dropouts, comm windows).
- **Binaries must run without systemd** (dev container has none): `sd_notify` optional,
  `evh-hw` falls back to launching OBCs directly.
- **Watchdog kicks** only from the flight software main loop after health checks pass.
- **Two communication levels** (bus communication and register mapping move out of Qt apps into
  backend services):
  - *Level 1 — application plane:* Qt apps ↔ backend services via middleware + proxies; also
    services ↔ `evh-hw` (HAL signals: power, watchdog kick, GPIO, NVM) and high-rate sim data.
    Represents everything outside the simulated rover.
  - *Level 2 — bus plane:* backend service ↔ backend service over SocketCAN (`vcan0`), modelling
    the rover's internal wiring between avionics units (master computer, later mobility/power
    controllers). One shared CAN library, evolved from `CanBus`.
  - Rules: apps never touch CAN; units never use middleware as a shortcut for bus traffic; one
    source of truth per piece of state (services may expose bus-derived state on Level 1).
  - Assessed as *not* overcomplicated — standard SIL/HIL pattern (data plane vs control plane),
    as long as the boundary stays clean. `vcan` has no bitrate/arbitration timing.
- **IPC (Level 1):** proposed Eclipse Zenoh (shm locally, network-capable, non-Qt friendly) with FlatBuffers
  or Protobuf. Qt apps use per-service proxy `QObject`s (worker thread → queued signals,
  rate-limited to UI refresh). Topics: `evh/<area>/...`. D-Bus considered and rejected for
  high-frequency data; ROS 2 interop kept possible via Zenoh/DDS.
- **Persistence:** `IRegisterStore` interface, SQLite (WAL) default backend, one NVM store per side.
- **State machine:** Qt SCXML or Boost.SML (not chosen yet); models boot / nominal / safe mode.
- **Migration path:** first extract a `Core` class from `MainWindow` (in-process), then move it
  into `services/` behind the IPC proxy.
- Positioning: build the parts ROS 2 doesn't cover (avionics/hardware emulation, fault recovery,
  operator console); reuse standard middleware rather than reinventing it.

## Open Items

- Rename remaining xECU references (CMake project, `Xecu*.cmake`, `xecu_add_component_docs`,
  Wayland socket, window titles, docs, devcontainer name) — not done yet; ask before doing it.
- Fix the `QByteArray` brace-init build error in `MainWindow.cpp`.
- Choose IPC library and state machine library definitively.
