// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

// Coloured status LED with glow, housing, domed body and specular highlight.
// Geometry scales with the item size (designed at 26 x 26).

pragma ComponentBehavior: Bound

import QtQuick

Item {
    id: led

    // Values match the LED register encoding (0..3) and ControlPanel.LedColor
    enum Color {
        Off,
        Green,
        Amber,
        Red
    }

    /// One of LedIndicator.Color; out-of-range values render as Off
    property int color: LedIndicator.Off

    readonly property bool lit: color >= LedIndicator.Green && color <= LedIndicator.Red

    // [base, glow] per colour, indexed by LedIndicator.Color
    readonly property var ledPalette: [
        ["#2d2d2d", "#414141"],   // Off
        ["#14b446", "#32ff6e"],   // Green
        ["#d28200", "#ffb91e"],   // Amber
        ["#be1919", "#ff3c3c"]    // Red
    ]
    readonly property color baseColor: ledPalette[lit ? color : LedIndicator.Off][0]
    readonly property color glowColor: ledPalette[lit ? color : LedIndicator.Off][1]

    // Lens geometry: 4 px housing margin at the 26 px design size
    readonly property real cx: width / 2
    readonly property real cy: height / 2
    readonly property real radius: Math.min(width, height) / 2 - 4

    implicitWidth: 26
    implicitHeight: 26

    Accessible.role: Accessible.Indicator
    Accessible.description: ["off", "green", "amber", "red"][lit ? color : LedIndicator.Off]

    // Glow rings, drawn outermost first
    Repeater {
        model: led.lit ? 3 : 0

        Rectangle {
            required property int index
            readonly property int ring: 3 - index
            readonly property real r: led.radius + ring * 3

            x: led.cx - r
            y: led.cy - r
            width: 2 * r
            height: 2 * r
            radius: r
            color: Qt.rgba(led.glowColor.r, led.glowColor.g, led.glowColor.b, 25 * ring / 255)
        }
    }

    // Housing
    Rectangle {
        x: led.cx - led.radius
        y: led.cy - led.radius
        width: 2 * led.radius
        height: 2 * led.radius
        radius: led.radius
        color: "#1c1c1c"
        border.color: "#0f0f0f"
        border.width: 1.5
    }

    // LED body, lit from the upper left
    GradientEllipse {
        x: led.cx - led.radius + 2
        y: led.cy - led.radius + 2
        width: 2 * led.radius - 4
        height: width

        // Gradient centre is offset toward the top-left, in body coordinates
        gradientCenterX: led.cx - led.radius * 0.30 - x
        gradientCenterY: led.cy - led.radius * 0.35 - y
        gradientRadius: led.radius * 1.1
        stops: [
            [0.0, led.lit ? Qt.lighter(led.glowColor, 1.2) : "#3c3c3c"],
            [0.5, led.baseColor],
            [1.0, Qt.darker(led.baseColor, 2.0)]
        ]
    }

    // Specular highlight
    GradientEllipse {
        visible: led.lit
        x: led.cx - led.radius * 0.32
        y: led.cy - led.radius * 0.60
        width: led.radius * 0.45
        height: led.radius * 0.32
        gradientRadius: width / 2

        stops: [[0.0, Qt.rgba(1, 1, 1, 190 / 255)], [1.0, Qt.rgba(1, 1, 1, 0)]]
    }
}
