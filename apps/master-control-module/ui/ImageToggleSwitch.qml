// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

// Image-based on/off toggle (e.g. a bat switch).
//
// Controlled component, like FlipSwitch: bind `checked` to the model and handle
// toggleRequested(on). Both images are loaded up front so switching state never
// waits on decoding.

import QtQuick
import QtQuick.Templates as T

T.AbstractButton {
    id: control

    /// Emitted when the user asks to change the state; `on` is the requested state
    signal toggleRequested(bool on)

    /// Image shown when off
    property url offSource
    /// Image shown when on
    property url onSource

    implicitWidth: 90
    implicitHeight: 45
    padding: 0
    checkable: false
    focusPolicy: Qt.StrongFocus

    Accessible.role: Accessible.CheckBox
    Accessible.checkable: true
    Accessible.checked: checked

    onClicked: toggleRequested(!checked)

    HoverHandler {
        cursorShape: Qt.PointingHandCursor
    }

    background: Rectangle {
        // Keyboard focus ring only; the images provide the visuals
        color: "transparent"
        radius: 4
        border.color: "#5fa8d3"
        border.width: 1
        visible: control.visualFocus
    }

    contentItem: Item {
        component StateImage: Image {
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            smooth: true
            mipmap: true
            onStatusChanged: {
                if (status === Image.Error)
                    console.warn("ImageToggleSwitch: failed to load", source)
            }
        }

        StateImage {
            source: control.offSource
            visible: !control.checked
        }

        StateImage {
            source: control.onSource
            visible: control.checked
        }
    }
}
