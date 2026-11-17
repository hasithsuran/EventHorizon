// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

#include "ClientLauncher.h"

#include <QFileInfo>
#include <QProcess>

ClientLauncher::ClientLauncher(const QString &socketName, qreal scaleFactor, QObject *parent)
    : QObject(parent)
    , m_environment(QProcessEnvironment::systemEnvironment())
{
    m_environment.insert("WAYLAND_DISPLAY", socketName);
    m_environment.insert("QT_QPA_PLATFORM", "wayland");
    m_environment.insert("QT_WAYLAND_DISABLE_WINDOWDECORATION", "1");

    // Match the compositor's scale exactly; per-screen overrides refer to
    // X11 screens and would not apply to the Wayland output.
    m_environment.insert("QT_SCALE_FACTOR", QString::number(scaleFactor));
    m_environment.remove("QT_SCREEN_SCALE_FACTORS");
}

void ClientLauncher::launch(const QString &program, const QStringList &arguments)
{
    const QString name = QFileInfo(program).fileName();

    // Parented to the launcher: ~QProcess kills the client on shutdown
    auto *process = new QProcess(this);
    process->setProcessEnvironment(m_environment);
    process->setProcessChannelMode(QProcess::ForwardedChannels);
    process->setWorkingDirectory(QFileInfo(program).absolutePath());

    connect(process, &QProcess::errorOccurred, this, [name](QProcess::ProcessError error) {
        qWarning("compositor: client '%s' failed (QProcess error %d)", qPrintable(name), error);
    });
    connect(process, &QProcess::finished, this, [name](int exitCode, QProcess::ExitStatus status) {
        qInfo("compositor: client '%s' exited (code %d%s)", qPrintable(name), exitCode,
              status == QProcess::CrashExit ? ", crashed" : "");
    });

    qInfo("compositor: launching '%s'", qPrintable(program));
    process->start(program, arguments);
}
