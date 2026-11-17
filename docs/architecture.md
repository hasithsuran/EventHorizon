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

- Full API docs are generated per component via Doxygen, each from its own `docs/Doxyfile.in`
- See each component's `docs/` folder for internal design details
- Class names mentioned on this page link straight into the component's API docs (via Doxygen tag files)
- Build all docs: `cmake --build . --target docs`
- Build one component: `cmake --build . --target master-control-module-docs`
- Browse locally: `cmake --build . --target docs-serve`, then open http://localhost:8080/html/
