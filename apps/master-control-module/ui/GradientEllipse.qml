// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

// Internal: an antialiased ellipse filling the item, with a radial gradient fill
// and an optional outline.
//
// Drawn with Canvas (QPainter into an image) rather than Shape: it is
// antialiased on every Qt version and scene graph backend, including software
// rendering and GL drivers without multisampling (e.g. Mesa llvmpipe in the
// compositor). These ellipses are small and repaint only when an input changes.

import QtQuick

Canvas {
    id: root

    /// Gradient centre and radius, in item coordinates (focal point = centre)
    property real gradientCenterX: width / 2
    property real gradientCenterY: height / 2
    property real gradientRadius: Math.min(width, height) / 2

    /// Gradient stops: [[position, color], ...] with position in 0..1
    property var stops: []

    property color strokeColor: "transparent"
    /// Outline width; 0 disables the outline
    property real strokeWidth: 0

    onGradientCenterXChanged: requestPaint()
    onGradientCenterYChanged: requestPaint()
    onGradientRadiusChanged: requestPaint()
    onStopsChanged: requestPaint()
    onStrokeColorChanged: requestPaint()
    onStrokeWidthChanged: requestPaint()

    onPaint: {
        const ctx = getContext("2d")
        ctx.reset()

        const inset = strokeWidth > 0 ? strokeWidth / 2 : 0
        const w = width - 2 * inset
        const h = height - 2 * inset
        if (w <= 0 || h <= 0 || gradientRadius <= 0)
            return

        const gradient = ctx.createRadialGradient(gradientCenterX, gradientCenterY, 0,
                                                  gradientCenterX, gradientCenterY, gradientRadius)
        for (const stop of stops)
            gradient.addColorStop(stop[0], stop[1])

        ctx.beginPath()
        ctx.ellipse(inset, inset, w, h)
        ctx.fillStyle = gradient
        ctx.fill()

        if (strokeWidth > 0) {
            ctx.lineWidth = strokeWidth
            ctx.strokeStyle = strokeColor
            ctx.stroke()
        }
    }
}
