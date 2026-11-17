# xECU-pro Architecture

## Overview

xECU-pro is a modular automotive ECU simulation platform. It consists of a 3D world simulator
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

xECU-pro's rover simulation is modelled on NASA's Curiosity rover. The control units below
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

The following Curiosity subsystems are the most relevant for xECU-pro SIL simulation:

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
