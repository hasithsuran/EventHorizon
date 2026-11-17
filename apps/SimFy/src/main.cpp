// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

/**
 * @file main.cpp
 * @brief SimFy application entry point.
 *
 * Requests an OpenGL 3.3 core context and opens the 3D scene view.
 */

#include "SceneView.h"
#include <QApplication>
#include <QSurfaceFormat>

/**
 * @brief Application entry point.
 *
 * Sets the default OpenGL surface format (must happen before QApplication),
 * then shows a SceneView rendering @c assets/space_rover.glb located next to
 * the executable.
 *
 * @param argc  Argument count passed from the OS.
 * @param argv  Argument vector passed from the OS.
 * @return      Exit code returned by QApplication::exec().
 */
int main(int argc, char *argv[])
{
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    format.setSamples(4);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication app(argc, argv);

    EvH::SimFy::SceneView view(QApplication::applicationDirPath() + "/assets/space_rover.glb");
    view.show();
    return app.exec();
}
