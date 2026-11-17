// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

// xECU-pro compositor scene.
//
// Tiles client windows by their xdg app_id (Qt uses the executable name):
//   master-control-module -> left column, full height
//   SimFy                 -> fills the remaining space
//   anything else         -> floats at the top-left corner
// Tiled clients are re-configured whenever the compositor window is resized.
//
// Sizes: QML items are in logical pixels, but clients interpret configure
// sizes in surface pixels (the output reports scale 1 and clients run with
// QT_SCALE_FACTOR = clientScaleFactor). Configure sizes are therefore
// logical size * clientScaleFactor, which maps 1:1 onto the window's pixels.

import QtQuick
import QtQuick.Window
import QtWayland.Compositor
import QtWayland.Compositor.XdgShell

WaylandCompositor {
    id: compositor

    socketName: compositorSocket

    readonly property string mcmAppId: "master-control-module"
    readonly property string simAppId: "SimFy"
    readonly property int mcmWidth: 280 // logical; matches the app's fixed width

    property var mcmToplevel: null
    property var simToplevel: null

    WaylandOutput {
        id: output
        sizeFollowsWindow: true

        window: Window {
            id: window
            width: 1600
            height: 720
            visible: true
            title: "xECU-pro"
            color: "#0d0f12"

            Item {
                id: mcmSlot
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: compositor.mcmWidth
                clip: true

                onHeightChanged: compositor.configureTiles()
            }

            Item {
                id: simSlot
                anchors.left: mcmSlot.right
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                clip: true

                onWidthChanged: compositor.configureTiles()
                onHeightChanged: compositor.configureTiles()
            }

            Item {
                id: floatingLayer
                anchors.fill: parent
            }
        }
    }

    XdgShell {
        onToplevelCreated: (toplevel, xdgSurface) => compositor.handleToplevel(toplevel, xdgSurface)
    }

    // Tell clients the compositor owns decorations, so they draw none
    XdgDecorationManagerV1 {
        preferredMode: XdgToplevel.ServerSideDecoration
    }

    Component {
        id: surfaceItemComponent

        ShellSurfaceItem {
            onSurfaceDestroyed: destroy()
        }
    }

    // Qt clients send app_id right after creating the toplevel, so it may not
    // be known yet when toplevelCreated fires.
    function handleToplevel(toplevel, xdgSurface) {
        if (toplevel.appId !== "") {
            place(toplevel, xdgSurface)
            return
        }
        const onAppId = function () {
            toplevel.appIdChanged.disconnect(onAppId)
            place(toplevel, xdgSurface)
        }
        toplevel.appIdChanged.connect(onAppId)
    }

    function place(toplevel, xdgSurface) {
        if (toplevel.appId === mcmAppId) {
            surfaceItemComponent.createObject(mcmSlot, { shellSurface: xdgSurface })
            mcmToplevel = toplevel
            xdgSurface.surface.surfaceDestroyed.connect(() => mcmToplevel = null)
        } else if (toplevel.appId === simAppId) {
            surfaceItemComponent.createObject(simSlot, { shellSurface: xdgSurface })
            simToplevel = toplevel
            xdgSurface.surface.surfaceDestroyed.connect(() => simToplevel = null)
        } else {
            surfaceItemComponent.createObject(floatingLayer, { shellSurface: xdgSurface })
            return
        }
        configureTiles()
    }

    function configureTile(toplevel, slot) {
        if (!toplevel || slot.width <= 0 || slot.height <= 0)
            return
        const size = Qt.size(Math.round(slot.width * clientScaleFactor),
                             Math.round(slot.height * clientScaleFactor))
        // Plain configure (no maximized state), so ShellSurfaceItem keeps it in its slot
        toplevel.sendConfigure(size, [])
    }

    function configureTiles() {
        configureTile(mcmToplevel, mcmSlot)
        configureTile(simToplevel, simSlot)
    }
}
