// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

// Animated OFF/ON power flip switch.
//
// Controlled component: `checked` is owned by the caller (bind it to the model).
// Clicking, or Space while focused, emits toggleRequested(!checked); the switch
// only moves once the caller updates `checked`. This keeps the UI in step with
// the backend even when the state is changed from elsewhere (e.g. over CAN).

import QtQuick
import QtQuick.Templates as T

T.AbstractButton {
    id: control

    /// Emitted when the user asks to change the state; `on` is the requested state
    signal toggleRequested(bool on)

    /// Thumb position: 0 = OFF, 1 = ON. Animated; drives thumb and track colour.
    property real thumbPosition: checked ? 1 : 0

    readonly property color offColor: Qt.rgba(55 / 255, 55 / 255, 55 / 255, 1)
    readonly property color onColor: Qt.rgba(45 / 255, 150 / 255, 90 / 255, 1)

    implicitWidth: 80
    implicitHeight: 34
    padding: 0
    checkable: false
    focusPolicy: Qt.StrongFocus

    Accessible.role: Accessible.CheckBox
    Accessible.checkable: true
    Accessible.checked: checked

    onClicked: toggleRequested(!checked)

    Behavior on thumbPosition {
        NumberAnimation {
            duration: 150
            easing.type: Easing.InOutQuad
        }
    }

    HoverHandler {
        cursorShape: Qt.PointingHandCursor
    }

    background: Rectangle {
        radius: height / 2
        color: Qt.rgba(control.offColor.r + control.thumbPosition * (control.onColor.r - control.offColor.r),
                       control.offColor.g + control.thumbPosition * (control.onColor.g - control.offColor.g),
                       control.offColor.b + control.thumbPosition * (control.onColor.b - control.offColor.b),
                       1)
        border.color: "#141414"
        border.width: 1

        // Keyboard focus ring
        Rectangle {
            anchors.fill: parent
            anchors.margins: -3
            radius: height / 2
            color: "transparent"
            border.color: "#5fa8d3"
            border.width: 1
            visible: control.visualFocus
        }
    }

    contentItem: Item {
        component SideLabel: Text {
            width: parent.width / 2
            height: parent.height
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: Qt.rgba(1, 1, 1, 180 / 255)
            font.pointSize: 8
            font.bold: true
            font.letterSpacing: 1
        }

        SideLabel {
            x: 0
            text: qsTr("OFF")
        }

        SideLabel {
            x: parent.width / 2
            text: qsTr("ON")
        }

        GradientEllipse {
            id: thumb

            readonly property real pad: 3

            width: control.height - 2 * pad
            height: width
            x: pad + control.thumbPosition * (control.width - 2 * pad - width)
            y: pad

            strokeColor: "#5a5a5a"
            strokeWidth: 1
            stops: [[0.0, "#f0f0f0"], [1.0, "#aaaaaa"]]
        }
    }
}
