# Compositor

<a href="../../html/index.html">&larr; Back to xECU-pro overview</a>

## Overview

The compositor combines the xECU-pro Qt Widgets apps into one window. It is a
nested Wayland compositor built with Qt Wayland Compositor (QML scene, C++
host): it opens a normal window on the current desktop, creates its own
Wayland socket (`xecu-wayland-0`) and launches each app as a Wayland client.

The apps themselves are unchanged; they are separate processes that render
through Wayland instead of X11.

## Layout

```
┌──────────────────────── compositor window ─────────────────────────┐
│ ┌─────────────┐ ┌────────────────────────────────────────────────┐ │
│ │   master-   │ │                                                │ │
│ │   control-  │ │                    SimFy                       │ │
│ │   module    │ │       (configured to fill the remaining area)  │ │
│ │(full height)│ │                                                │ │
│ └─────────────┘ └────────────────────────────────────────────────┘ │
└────────────────────────────────────────────────────────────────────┘
```

Windows are tiled by their xdg `app_id`, which Qt sets to the executable name:

| app_id | Placement |
|--------|-----------|
| `master-control-module` | Left column, 280 px wide, full window height |
| `SimFy` | Remaining area |
| anything else | Floats at the top-left corner |

Both tiles are re-sized whenever the compositor window is resized. Mouse and
keyboard input is forwarded to whichever client is under the pointer.

### Display scaling

The Wayland output always reports scale 1, but the compositor window itself
may be scaled by the desktop (HiDPI, `Xft.dpi`, `QT_SCALE_FACTOR`). To keep
clients the same size as their tiles, the compositor:

1. launches clients with `QT_SCALE_FACTOR` set to its own device pixel ratio, and
2. sends configure sizes in device pixels (tile size × scale factor).

## Components

| Part | Role |
|------|------|
| ClientLauncher | Starts each app with `WAYLAND_DISPLAY`, `QT_QPA_PLATFORM=wayland`, the compositor's scale factor and client decorations disabled; kills them on exit |
| `qml/main.qml` | `WaylandCompositor` scene: output window, xdg-shell, server-side decoration hint and the tiling logic |
| `src/main.cpp` | Ensures `XDG_RUNTIME_DIR` exists, loads the QML scene, launches the clients |

## Notes

- Client executables are taken from their CMake build locations, so build the
  whole project (building `compositor` also builds both apps).
- Without GPU/EGL support the compositor logs
  `Failed to initialize EGL display` and clients use shared-memory buffers.
  This is expected in the dev container and does not affect output.
- Any other Wayland client can be started manually against the socket:
  `WAYLAND_DISPLAY=xecu-wayland-0 QT_QPA_PLATFORM=wayland <app>`

## See Also

- <a href="../../html/index.html">xECU-pro system architecture</a>
