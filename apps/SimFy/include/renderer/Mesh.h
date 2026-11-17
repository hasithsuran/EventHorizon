// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

/**
 * @file Mesh.h
 * @brief A Structure designed to persist 3d mesh data such as vertices and indices.
 */

#pragma once

#include <QOpenGLBuffer>

namespace Rendering {

struct Mesh
{
    std::unique_ptr<QOpenGLVertexArrayObject> vao;
    QOpenGLBuffer                             vbo{QOpenGLBuffer::VertexBuffer};
    QOpenGLBuffer                             ebo{QOpenGLBuffer::IndexBuffer};
    GLsizei                                   indexCount = 0;
    unsigned int                              material   = 0;
};

} // namespace Rendering
