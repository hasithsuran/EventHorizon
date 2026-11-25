// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

// Master control panel window.
//
//   left column : POWER flip switch, CH 1..CH 4 bat switches, build/bus info
//   right column: L1..L4 status LEDs
//
// All state comes from `panel` (ControlPanel, the C++ backend). Controls only
// request changes; the backend updates the register map and the bindings
// below reflect it, whether the change came from the UI or over CAN.
//
// The window is 280 px wide (the compositor tiles it in a 280 px column) and
// may be given any height.

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Window
// Explicit: Qt 6.4's implicit directory import does not expose the module's C++ types
import EvH.MasterControl
import EvH.MasterControl.UI as UI

Window {
    id: root

    required property ControlPanel panel
    required property string buildDate

    width: 280
    minimumWidth: 280
    maximumWidth: 280
    height: 680
    visible: true
    title: "xECU-pro"
    color: "#232323"

    component PanelLabel: Text {
        horizontalAlignment: Text.AlignHCenter
        color: "#999999"
        font.pointSize: 8
        font.letterSpacing: 2
    }

    component FooterText: Text {
        horizontalAlignment: Text.AlignHCenter
        color: "#555555"
        font.pointSize: 7
        font.letterSpacing: 1
    }

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            orientation: Gradient.Horizontal
            GradientStop { position: 0.0; color: "#232323" }
            GradientStop { position: 1.0; color: "#3d3d3d" }
        }
    }

    // --- Left panel: switches ------------------------------------------------
    Item {
        id: leftPanel

        // Content is centred between 16 px (left) and 8 px (right) margins
        readonly property real centerOffset: (16 - 8) / 2

        anchors.left: parent.left
        anchors.right: rightPanel.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        Column {
            anchors.top: parent.top
            anchors.topMargin: 24
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: leftPanel.centerOffset
            spacing: 4

            UI.FlipSwitch {
                anchors.horizontalCenter: parent.horizontalCenter
                checked: root.panel.powerOn
                Accessible.name: qsTr("Power")
                onToggleRequested: on => root.panel.setPower(on)
            }

            PanelLabel {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("POWER")
            }

            Item { width: 1; height: 8 }

            Repeater {
                model: root.panel.channelCount

                Column {
                    id: channel

                    required property int index
                    readonly property string name: qsTr("CH %1").arg(index + 1)

                    spacing: 4
                    bottomPadding: 6

                    UI.ImageToggleSwitch {
                        anchors.horizontalCenter: parent.horizontalCenter
                        offSource: Qt.resolvedUrl("assets/switch_off.png")
                        onSource: Qt.resolvedUrl("assets/switch_on.png")
                        checked: root.panel.channelStates[channel.index] ?? false
                        Accessible.name: channel.name
                        onToggleRequested: on => root.panel.setChannel(channel.index, on)
                    }

                    PanelLabel {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: channel.name
                    }
                }
            }
        }

        Column {
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 24
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: leftPanel.centerOffset
            spacing: 6

            FooterText {
                anchors.horizontalCenter: parent.horizontalCenter
                text: root.panel.busOnline ? qsTr("CAN %1 online").arg(root.panel.canInterface)
                                           : qsTr("CAN %1 offline").arg(root.panel.canInterface)
                color: root.panel.busOnline ? "#555555" : "#c8821e"
            }

            FooterText {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("xECU-pro  v%1\nBuild %2").arg(Qt.application.version).arg(root.buildDate)
            }
        }
    }

    // --- Right panel: status LEDs --------------------------------------------
    Item {
        id: rightPanel

        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: ledColumn.width + 8 + 16

        Column {
            id: ledColumn

            x: 8
            y: 24
            width: 26
            spacing: 2

            Repeater {
                model: root.panel.ledCount

                Column {
                    id: ledCell

                    required property int index

                    width: parent.width
                    spacing: 2
                    bottomPadding: 8

                    UI.LedIndicator {
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: root.panel.ledColors[ledCell.index] ?? UI.LedIndicator.Off
                        Accessible.name: qsTr("L%1").arg(ledCell.index + 1)
                    }

                    PanelLabel {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: qsTr("L%1").arg(ledCell.index + 1)
                    }
                }
            }
        }
    }
}
