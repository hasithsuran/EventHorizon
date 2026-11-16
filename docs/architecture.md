# xECU-pro Architecture

## Overview

xECU-pro is a modular automotive ECU simulation platform. It consists of a 3D world simulator
(SimFy), a master control module, a component configurator, and a set of core communication
services.

## Components

| Component | Description |
|-----------|-------------|
| [simulator](../simulator/docs/) | SimFy — 3D world and vehicle model rendering |
| [master-control-module](../master-control-module/docs/) | Central Domain Controller — decision making and arbitration |
| [configurator](../configurator/docs/) | Add/configure control module software components and types |

## High-Level Topology

```
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
```

## Documentation

- Full API docs are generated per component via Doxygen
- See each component's `docs/` folder for internal design details
- Build all docs: `cmake --build . --target docs`
