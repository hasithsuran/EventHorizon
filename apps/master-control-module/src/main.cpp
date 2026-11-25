// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

/**
 * @file main.cpp
 * @brief Application entry point.
 *
 * Creates the ControlPanel backend and loads the QML control panel.
 */

#include <ControlPanel.h>
#include <QCommandLineParser>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QUrl>
#include <QtQml/qqmlextensionplugin.h>
#include <cstdlib>

// The UI controls live in a static QML module (EvH.MasterControl.UI);
// its plugin must be imported explicitly when linked statically.
Q_IMPORT_QML_PLUGIN(EvH_MasterControl_UIPlugin)

/**
 * @brief Application entry point.
 *
 * Options:
 * - @c --can-interface <name>  SocketCAN interface to use (default: vcan0)
 *
 * @note Do not set an organization domain or desktop file name: Qt derives the
 *       Wayland app_id from them, and the compositor tiles this window by the
 *       app_id "master-control-module" (the executable name).
 *
 * @param argc  Argument count passed from the OS.
 * @param argv  Argument vector passed from the OS.
 * @return      Exit code returned by QGuiApplication::exec().
 */
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QCoreApplication::setApplicationVersion(QStringLiteral(XECU_MCM_VERSION));

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("xECU-pro master control module"));
    parser.addHelpOption();
    parser.addVersionOption();
    const QCommandLineOption canOption(
        QStringLiteral("can-interface"),
        QStringLiteral("SocketCAN interface to use (default: vcan0)."),
        QStringLiteral("name"),
        QStringLiteral("vcan0"));
    parser.addOption(canOption);
    parser.process(app);

    // Declared before the engine so it outlives every QML binding that uses it
    EvH::MasterControl::ControlPanel panel(parser.value(canOption));

    QQmlApplicationEngine engine;
    // Default import path for qt_add_qml_module resources from Qt 6.5; added for Qt 6.4
    engine.addImportPath(QStringLiteral("qrc:/qt/qml"));
    engine.setInitialProperties({
        {QStringLiteral("panel"), QVariant::fromValue(&panel)},
        {QStringLiteral("buildDate"), QStringLiteral(XECU_MCM_BUILD_DATE)},
    });
    engine.load(QUrl(QStringLiteral("qrc:/qt/qml/EvH/MasterControl/Main.qml")));
    if (engine.rootObjects().isEmpty()) return EXIT_FAILURE;

    return app.exec();
}
