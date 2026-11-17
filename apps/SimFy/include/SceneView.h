// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

/**
 * @file SceneView.h
 * @brief OpenGL viewport that renders the SimFy scene.
 */

#pragma once
#include <QMatrix4x4>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QPoint>
#include <QVector3D>
#include <memory>

namespace Rendering {
class Model;
}

/**
 * @class SceneView
 * @brief QOpenGLWidget that renders a single model with an orbit camera.
 *
 * After the model loads, the camera is placed so the model's bounding box
 * fills the view. Dragging with the left mouse button orbits the camera
 * around the model's center: horizontal drag changes yaw, vertical drag
 * changes pitch (clamped to avoid flipping over the poles). There is no
 * panning or zoom. Lights are fixed in world space.
 * Shading is Blinn-Phong with a key light, a fill light and hemisphere ambient.
 */
class SceneView : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the view.
     * @param modelPath  Model file to load once the OpenGL context exists.
     * @param parent     Optional Qt parent.
     */
    explicit SceneView(const QString &modelPath, QWidget *parent = nullptr);
    ~SceneView() override;

    QSize sizeHint() const override { return {1280, 720}; }

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void frameModel();
    void updateView();

    QString                           m_modelPath;
    std::unique_ptr<Rendering::Model> m_model;
    QOpenGLShaderProgram              m_program;
    QMatrix4x4                        m_view;
    QMatrix4x4                        m_projection;
    QVector3D                         m_cameraPos;
    QVector3D                         m_target;
    float                             m_sceneRadius = 1.0F;
    float                             m_distance    = 1.0F;
    float                             m_yawDeg      = 0.0F; ///< rotation around the world Y axis
    float                             m_pitchDeg = 0.0F; ///< elevation above the horizontal plane
    QPoint                            m_lastMousePos;
    bool                              m_dragging = false;
};
