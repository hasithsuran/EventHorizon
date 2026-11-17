// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

#include "SceneView.h"
#include "Model.h"

#include <QMouseEvent>
#include <QtMath>
#include <cmath>

namespace {

constexpr float kFovDegrees      = 40.0F;
constexpr float kDegreesPerPixel = 0.4F;  // mouse drag sensitivity
constexpr float kMaxPitchDeg     = 85.0F; // keep away from the poles

const char *kVertexShader = R"(
#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;

out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vUv;

void main()
{
    vec4 world  = uModel * vec4(aPosition, 1.0);
    vWorldPos   = world.xyz;
    vNormal     = uNormalMatrix * aNormal;
    vUv         = aUv;
    gl_Position = uProjection * uView * world;
}
)";

// Lighting is done in linear space: textures are sRGB, output is re-encoded.
const char *kFragmentShader = R"(
#version 330 core
in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vUv;

uniform sampler2D uBaseColorTex;
uniform bool      uHasTexture;
uniform vec4      uBaseColor;
uniform vec3      uCameraPos;

out vec4 fragColor;

const vec3 kKeyDir   = normalize(vec3(-0.5, -1.0, -0.4)); // direction light travels
const vec3 kKeyColor = vec3(1.0, 0.96, 0.9) * 1.6;
const vec3 kFillDir  = normalize(vec3(0.7, -0.3, 0.6));
const vec3 kFillColor = vec3(0.45, 0.55, 0.7) * 0.5;
const vec3 kSky      = vec3(0.35, 0.40, 0.50);
const vec3 kGround   = vec3(0.12, 0.10, 0.09);

void main()
{
    vec4 base = uBaseColor;
    if (uHasTexture) {
        vec4 tex = texture(uBaseColorTex, vUv);
        base *= vec4(pow(tex.rgb, vec3(2.2)), tex.a);
    }

    vec3 N = normalize(vNormal);
    if (!gl_FrontFacing) N = -N;
    vec3 V = normalize(uCameraPos - vWorldPos);

    vec3 color = base.rgb * mix(kGround, kSky, N.y * 0.5 + 0.5);

    vec3  L    = -kKeyDir;
    float diff = max(dot(N, L), 0.0);
    float spec = pow(max(dot(N, normalize(L + V)), 0.0), 48.0) * 0.3;
    color += (base.rgb * diff + spec) * kKeyColor;

    L     = -kFillDir;
    color += base.rgb * max(dot(N, L), 0.0) * kFillColor;

    color     = color / (color + vec3(1.0));      // Reinhard tone map
    fragColor = vec4(pow(color, vec3(1.0 / 2.2)), base.a);
}
)";

} // namespace

SceneView::SceneView(const QString &modelPath, QWidget *parent)
    : QOpenGLWidget(parent)
    , m_modelPath(modelPath)
{
    setWindowTitle("SimFy");
}

SceneView::~SceneView()
{
    // GL resources must be released with the context current
    makeCurrent();
    m_model.reset();
    m_program.removeAllShaders();
    doneCurrent();
}

void SceneView::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.05F, 0.06F, 0.08F, 1.0F);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    if (!m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, kVertexShader)
        || !m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, kFragmentShader)
        || !m_program.link()) {
        qCritical("SimFy: shader error: %s", qPrintable(m_program.log()));
        return;
    }

    m_model = std::make_unique<Model>();
    if (!m_model->load(m_modelPath)) {
        qCritical("SimFy: failed to load '%s': %s", qPrintable(m_modelPath),
                  qPrintable(m_model->errorString()));
        m_model.reset();
        return;
    }
    frameModel();
}

void SceneView::frameModel()
{
    m_target      = (m_model->boundsMin() + m_model->boundsMax()) * 0.5F;
    m_sceneRadius = (m_model->boundsMax() - m_model->boundsMin()).length() * 0.5F;
    m_distance    = m_sceneRadius / qSin(qDegreesToRadians(kFovDegrees * 0.5F)) * 0.7F;

    // Initial three-quarter view from front-right, slightly above
    const QVector3D viewDir = QVector3D(1.0F, 0.55F, 1.2F).normalized();
    m_yawDeg                = qRadiansToDegrees(std::atan2(viewDir.x(), viewDir.z()));
    m_pitchDeg              = qRadiansToDegrees(std::asin(viewDir.y()));
    updateView();
}

void SceneView::updateView()
{
    const float     yaw   = qDegreesToRadians(m_yawDeg);
    const float     pitch = qDegreesToRadians(m_pitchDeg);
    const QVector3D dir(std::cos(pitch) * std::sin(yaw), std::sin(pitch),
                        std::cos(pitch) * std::cos(yaw));

    m_cameraPos = m_target + dir * m_distance;
    m_view.setToIdentity();
    m_view.lookAt(m_cameraPos, m_target, QVector3D(0.0F, 1.0F, 0.0F));
}

void SceneView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging     = true;
        m_lastMousePos = event->position().toPoint();
        setCursor(Qt::ClosedHandCursor);
    }
}

void SceneView::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_dragging || !m_model) return;

    const QPoint pos   = event->position().toPoint();
    const QPoint delta = pos - m_lastMousePos;
    m_lastMousePos     = pos;

    // Drag right -> model turns right; drag down -> view from higher up
    m_yawDeg = std::fmod(m_yawDeg - float(delta.x()) * kDegreesPerPixel, 360.0F);
    m_pitchDeg
        = qBound(-kMaxPitchDeg, m_pitchDeg + float(delta.y()) * kDegreesPerPixel, kMaxPitchDeg);
    updateView();
    update();
}

void SceneView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        unsetCursor();
    }
}

void SceneView::resizeGL(int w, int h)
{
    const float aspect = h > 0 ? float(w) / float(h) : 1.0F;
    m_projection.setToIdentity();
    m_projection.perspective(kFovDegrees, aspect, m_sceneRadius * 0.05F, m_sceneRadius * 20.0F);
}

void SceneView::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (!m_model) return;

    const QMatrix4x4 model; // identity: node transforms are baked in at load
    m_program.bind();
    m_program.setUniformValue("uModel", model);
    m_program.setUniformValue("uView", m_view);
    m_program.setUniformValue("uProjection", m_projection);
    m_program.setUniformValue("uNormalMatrix", model.normalMatrix());
    m_program.setUniformValue("uCameraPos", m_cameraPos);
    m_program.setUniformValue("uBaseColorTex", 0);
    m_model->draw(m_program);
    m_program.release();
}
