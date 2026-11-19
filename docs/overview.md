# EventHorizon Architecture

## Overview

EventHorizon is a modular automotive ECU simulation platform. It consists of a 3D world simulator
(SimFy), a master control module, a component configurator, and a set of core communication
services.

## Components

| Component | Description |
|-----------|-------------|
| <a href="../SimFy/html/index.html">SimFy</a> | 3D world and vehicle model rendering (key classes: SceneView, Model) |
| <a href="../master-control-module/html/index.html">master-control-module</a> | Central Domain Controller — decision making and arbitration (key classes: RegisterMap, CanBus) |
| <a href="../compositor/html/index.html">compositor</a> | Wayland compositor that hosts SimFy and master-control-module side by side (key class: ClientLauncher) |
| [configurator](../configurator/docs/) | Add/configure control module software components and types |

## High-Level Topology

~~~
┌─────────────┐     ┌────────────────────────┐     ┌───────────────┐
│    SimFy    │────▶│  Master Control Module  │◀───▶│ Configurator  │
│ (Simulator) │     │         (CDC)           │     │               │
└─────────────┘     └────────────┬────────────┘     └───────────────┘
                                 │
                    ┌────────────┼────────────┐
                    │            │            │
               ┌────┴───┐  ┌────┴────┐  ┌────┴─────┐
               │Watchdog│  │ Storage │  │  Logger  │
               │Service │  │ Service │  │  Service │
               └────────┘  └─────────┘  └──────────┘
~~~

## Documentation

- Full API docs are generated per component via Doxygen, each from its own `docs/Doxyfile.in`
- See each component's `docs/` folder for internal design details
- Class names mentioned on this page link straight into the component's API docs (via Doxygen tag files)
- Build all docs: `cmake --build . --target docs`
- Build one component: `cmake --build . --target master-control-module-docs`
- Browse locally: `cmake --build . --target docs-serve`, then open http://localhost:8080/html/

---

## Space Rover Reference — Curiosity (Mars Science Laboratory)

EventHorizon's rover simulation is modelled on NASA's Curiosity rover. The control units below
map directly to simulated subsystems in SimFy and master-control-module.

### Main Computers

| Unit | Full Name | Purpose |
|------|-----------|---------|
| RCE A/B | Rover Compute Element | Main brain — runs flight software, redundant pair |
| RPCM | Remote Power Control Module | Power switching and distribution to all subsystems |

### Mobility & Mechanism Controllers

| Unit | Purpose |
|------|---------|
| MPFL Motor Controller | Drives 6 wheels independently via rocker-bogie suspension |
| RSM Motor Controller | Remote Sensing Mast (camera tower) pan/tilt |
| Arm Motor Controller | Robotic arm — 5 degrees of freedom |
| Drill Controller | Percussive drill on arm end effector |

### Science Instrument Controllers

| Unit | Purpose |
|------|---------|
| ChemCam | Laser + spectrometer controller |
| RAD | Radiation detector |
| REMS | Weather station — wind, temperature, pressure |
| SAM / CheMin | Chemistry lab controllers |

### Power & Thermal

| Unit | Purpose |
|------|---------|
| MMRTG Controller | Nuclear power (radioisotope generator) management |
| Pyro Controller | Fires explosive bolts for deployments |
| Thermal Controller | Heaters for electronics survival (−130 °C nights) |

### Simulated Subsystems for SIL Testing

The following Curiosity subsystems are the most relevant for EventHorizon SIL simulation:

| Subsystem | Simulation Role |
|-----------|----------------|
| RCE | Autonomy and navigation brain — maps to master-control-module |
| Mobility Controller | Wheel torque, steering, rocker-bogie kinematics — maps to SimFy vehicle model |
| REMS | Sensor inputs (wind, temperature, pressure) — maps to SimFy environment model |
| Power | Battery state, MMRTG input — maps to SimFy power subsystem |

### Mars Environment Notes

- Atmosphere is 1% of Earth's density — wind exists but exerts minimal force
- Dust devils and global dust storms are the primary visible weather phenomena
- Sky colour: butterscotch/orange-pink due to suspended dust
- Surface temperature range: −130 °C (night) to +20 °C (day at equator)
- No magnetic field — navigation relies on star trackers and wheel odometry

---

## Runtime Architecture — Applications and Services (Planned)

> **Status:** proposed design. The applications exist today; the services described here are
> not implemented yet (`services/` is empty). Service, unit and topic names use the `evh`
> prefix.

EventHorizon separates the simulator into two kinds of software component:

- **Applications** (`apps/`) — Qt front ends that people look at and interact with. They hold no
  rover state of their own and can be closed or restarted at any time without affecting the
  simulated rover.
- **Services** (`services/`) — headless background processes that *are* the simulated rover:
  its flight software, its hardware and its radio link. They run as **Linux system services**
  managed by systemd — the Linux equivalent of Windows services — so the operating system starts
  them in the right order, keeps them running without anyone logged in to a UI, restarts them
  if they fail, and collects their logs in one place.

The split mirrors a real mission: the rover keeps operating whether or not anyone on the ground
is watching, and the ground console is just a client of it.

### Component Overview

~~~
 Applications (Qt, launched by the compositor)              Services (systemd units, always running)
┌────────────────────────────────────────────┐            ┌──────────────────────────────────────────┐
│ compositor                                 │            │ evh-hw.service      simulated hardware   │
│  ├─ master-control-module (ground console) │◀── IPC ──▶ │ evh-obc@A.service   flight software A    │
│  └─ SimFy (3D world view)                  │  (proxies) │ evh-obc@B.service   flight software B    │
└────────────────────────────────────────────┘            │ evh-link.service    radio link simulator │
                                                          └──────────────────────────────────────────┘
                                                                    all grouped under evh.target
~~~

### Applications

| Application | Role |
|-------------|------|
| compositor | Nested Wayland compositor; one window that tiles the other apps. Launches applications only — never services. |
| master-control-module | Operator console for the master computer: switches, LEDs and register views. Moves to a thin client of the master control service; its widgets live in `EvH::MasterControl::UI`. |
| SimFy | 3D world and rover visualisation. Consumes rover state published by the services. |

### Services and Their Purpose

| Unit | Simulates | Purpose |
|------|-----------|---------|
| `evh-hw.service` | Rover avionics hardware (cf. Curiosity's RPCM and the RCE's hardware support) | Everything that on a real rover is hardware and keeps working when a computer fails: **power distribution** (turns flight computers on/off), **watchdog timers**, the **A/B side-select latch**, **boot counters**, the **hardware command decoder** (recovery commands that bypass the flight software), NVM fault injection and the **simulation clock**. It is the only component allowed to start, stop or reset a flight computer. |
| `evh-obc@A.service` / `evh-obc@B.service` | Main computer, redundant pair (cf. RCE A/B) | The master control flight software: owns the CAN bus, the register map, the mode state machine (boot, nominal, safe mode) and persistent state. Same binary for both sides, selected by the unit instance (`%i`). Side B is a cold spare and is only powered on by `evh-hw`. |
| `evh-link.service` | Radio link between Earth and rover | Relays telecommands up and telemetry down with configurable light-time delay, dropouts and communication windows. |
| `evh.target` | "Rover powered on" | Groups all services so the whole rover is started or stopped with one command. |

The Watchdog / Storage / Logger services in the *High-Level Topology* above map onto this design
as follows: watchdogs live in `evh-hw`; storage is per-side NVM owned by each flight computer
(behind an `IRegisterStore` interface, SQLite in WAL mode as the default backend); logging is
handled by journald for all units.

### Service Lifecycle Rules

1. **Flight computers never restart themselves.** `evh-obc@.service` uses `Restart=no`; recovery
   is decided by the simulated watchdog and boot counter in `evh-hw`, exactly as on a rover.
2. **Simulated hardware is always available.** `evh-hw.service` runs with `Type=notify`,
   `WatchdogSec=` and `Restart=always`, so systemd itself guards the component that guards the
   flight computers.
3. **Power is controlled through systemd.** `evh-hw` starts, stops and kills OBC units through
   systemd's D-Bus API (`StartUnit`, `StopUnit`, `KillUnit`).
4. **The watchdog is kicked only when healthy.** Flight software kicks the watchdog from its main
   loop after health checks pass — never from an independent timer thread.
5. **User services during development.** Units are installed as `systemctl --user` services, so
   no root access or polkit rules are needed.

### Communication — Two Levels

Bus communication and register mapping move out of the Qt applications into backend services.
Communication then happens on two levels, each with its own purpose:

~~~
 ┌─────────────────────────┐   ┌───────────────┐
 │ master-control-module   │   │     SimFy     │                 Qt applications
 │  (MasterControl proxy)  │   │ (state proxy) │
 └────────────┬────────────┘   └───────┬───────┘
              │                        │
 ═════════════╪════════════════════════╪══════════════════════  Level 1 — application plane
              │      middleware (Zenoh) + proxies               (outside the simulated rover)
 ┌────────────┴───┐  ┌────────────────┐  ┌────────────────┐
 │ evh-obc@A      │  │ subsystem svc  │  │ subsystem svc  │     backend services
 │ (master ctrl)  │  │ (e.g. mobility)│  │ (e.g. power)   │
 └────────────┬───┘  └───────┬────────┘  └───────┬────────┘
              │              │                   │
 ═════════════╧══════════════╧═══════════════════╧═════════════  Level 2 — bus plane
                     SocketCAN (vcan0)                           (the rover's internal wiring)
~~~

| | Level 1 — application plane | Level 2 — bus plane |
|---|---|---|
| Represents | Everything *outside* the simulated rover: operator console, 3D view, simulation control, fault injection, simulator plumbing | The rover's *internal wiring* between its avionics units |
| Participants | Qt applications ↔ backend services; services ↔ `evh-hw` | Backend service ↔ backend service |
| Transport | Middleware — proposed **Eclipse Zenoh** with **FlatBuffers** or **Protobuf** messages | **SocketCAN** on `vcan0` (CAN / CAN FD frames) |
| API style | Typed proxies, topics `evh/<area>/...` | Raw CAN IDs and frame layouts defined per subsystem |
| Purpose | Convenience, typed APIs, high-rate data, observability | Fidelity — the bus traffic is part of what is being simulated |

**Level 1 — application plane**

- Qt applications never use the middleware or CAN directly. Each service has a generated or
  hand-written **proxy** `QObject` that receives messages on a worker thread, re-emits them as
  queued Qt signals and rate-limits high-frequency data to the UI refresh rate.
- Topics follow the `evh/<area>/...` convention, e.g. `evh/obc/registers`, `evh/hw/obc/A/wdt/kick`,
  `evh/hw/latch/side`, `evh/link/uplink`, `evh/sim/clock`.
- Discrete hardware signals that are not bus traffic (power switching, watchdog kick, GPIO, NVM
  access) also travel on this level: flight software reaches them only through a hardware
  abstraction layer (HAL) whose simulator backend talks to `evh-hw`.
- High-rate simulation data (e.g. SimFy physics feeding sensor services) belongs here even between
  two backend services — it is simulator plumbing, not rover wiring.

**Level 2 — bus plane**

- Backend services that model avionics units (the master computer, and later subsystem
  controllers such as mobility or power) exchange commands, register reads/writes and status
  over SocketCAN, exactly as the corresponding units would over a physical bus.
- All services share one CAN library (evolved from today's `CanBus`) instead of each opening
  raw sockets with their own code.
- `vcan` delivers frames instantly and does not model bitrate, arbitration or bus load. If timing
  fidelity matters later, a bus model can be inserted between services.

**Rules that keep the two levels clean**

1. Applications never touch CAN; they observe and command the rover only through proxies.
2. Avionics units never use the middleware as a shortcut for traffic that would travel on the
   rover's bus.
3. Each piece of state has one source of truth; it is not published redundantly on both levels.
   A service may *expose* what it sees on the bus through its Level 1 API (e.g. the register map),
   but the bus remains the authoritative path between units.

### Fault Injection

Because each service is its own systemd unit (and cgroup), failures can be injected without
modifying code:

| Fault | Injection |
|-------|-----------|
| Flight computer hang | `systemctl --user freeze evh-obc@A` (resume with `thaw`) |
| Flight computer crash | `systemctl --user kill -s SIGKILL evh-obc@A` |
| Reboot loop, flash corruption, bus faults, link loss | Fault-injection commands handled by `evh-hw` / `evh-link` |

Typical recovery scenario: side A hangs → watchdog expires → `evh-hw` power-cycles A → boot
counter exceeds its limit → `evh-hw` switches to side B → telemetry reports safe mode → the
ground console commands recovery.

Follow all components in one timeline with `journalctl --user -u 'evh-*' -f`.

### Development Without systemd

The dev container does not run systemd. Service binaries therefore must not depend on it:
`sd_notify` is a no-op when `NOTIFY_SOCKET` is unset, and `evh-hw` falls back to launching OBC
processes directly when systemd is unavailable. Unit files are kept in the repository and
installed by CMake.
