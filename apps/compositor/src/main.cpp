// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

/**
 * @file main.cpp
 * @brief xECU-pro compositor entry point.
 *
 * Starts a nested Wayland compositor (a window on the current X11/Wayland
 * desktop) and launches master-control-module and SimFy as its clients,
 * tiled side by side.
 * TODO: More individual apps to be added later so the tiling could change in the future.
 */

#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QScreen>
#include <unistd.h>

#include <AppLauncher.h>
namespace {

/// Wayland socket the compositor listens on; clients receive it via WAYLAND_DISPLAY.
constexpr auto kSocketName = "xecu-wayland-0";

/// Wayland sockets live in XDG_RUNTIME_DIR, which containers often leave unset.
void ensureRuntimeDir()
{
    if (qEnvironmentVariableIsSet("XDG_RUNTIME_DIR")) return;

    const QString dir = QDir::tempPath() + "/xecu-runtime-" + QString::number(getuid());
    QDir().mkpath(dir);
    QFile::setPermissions(dir, QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner);
    qputenv("XDG_RUNTIME_DIR", dir.toLocal8Bit());
}

} // namespace

/**
 * @brief Application entry point.
 *
 * Loads the QML compositor scene, which creates the Wayland socket, then
 * launches the client applications against that socket.
 *
 * @param argc  Argument count passed from the OS.
 * @param argv  Argument vector passed from the OS.
 * @return      Exit code returned by QGuiApplication::exec().
 */
int main(int argc, char *argv[])
{
    ensureRuntimeDir();

    QGuiApplication app(argc, argv);

    // Scale of the desktop the compositor window lives on (Xft.dpi,
    // QT_SCALE_FACTOR, ...). Clients render at this scale, and the QML scene
    // uses it to send window sizes in device pixels.
    const qreal scaleFactor = app.primaryScreen()->devicePixelRatio();

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("compositorSocket", QString(kSocketName));
    engine.rootContext()->setContextProperty("clientScaleFactor", scaleFactor);
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (engine.rootObjects().isEmpty()) return 1;

    AppLauncher launcher(kSocketName, scaleFactor);

#ifndef XECU_MCM_EXECUTABLE
    static_assert(false, "XECU_MCM_EXECUTABLE not defined — Please check "
                         "compile definitions in CMakeLists.txt");
#endif

    launcher.launch(XECU_MCM_EXECUTABLE);
#ifndef XECU_SIMFY_EXECUTABLE
    static_assert(false, "XECU_SIMFY_EXECUTABLE not defined — Please check "
                         "compile definitions in CMakeLists.txt");
#endif

    launcher.launch(XECU_SIMFY_EXECUTABLE);

    return app.exec();
}
