// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

/**
 * @file AppLauncher.h
 * @brief Starts Qt applications as Wayland clients of the compositor.
 */

#pragma once
#include <QObject>
#include <QProcessEnvironment>
#include <QString>
#include <QStringList>

/**
 * @class AppLauncher
 * @brief Launches client applications connected to the compositor's Wayland socket.
 *
 * Each client inherits the compositor's environment with these overrides:
 * - @c WAYLAND_DISPLAY set to the compositor socket
 * - @c QT_QPA_PLATFORM=wayland so Qt apps render through Wayland
 * - @c QT_WAYLAND_DISABLE_WINDOWDECORATION=1 so clients draw no title bar
 * - @c QT_SCALE_FACTOR set to the compositor's device pixel ratio, so clients
 *   render at the same scale as the compositor window (the Wayland output
 *   itself always reports scale 1)
 *
 * Client processes are owned by the launcher; they are terminated when the
 * launcher is destroyed (and exit on their own if the compositor goes away).
 */
class AppLauncher : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a launcher for the given compositor socket.
     * @param socketName   Wayland socket name (relative to @c XDG_RUNTIME_DIR).
     * @param scaleFactor  Device pixel ratio of the compositor window.
     * @param parent       Optional Qt parent.
     */
    AppLauncher(const QString &socketName, qreal scaleFactor, QObject *parent = nullptr);

    /**
     * @brief Starts a client application.
     * @param program    Path to the executable.
     * @param arguments  Command-line arguments.
     */
    void launch(const QString &program, const QStringList &arguments = {});

private:
    QProcessEnvironment m_environment;
};
