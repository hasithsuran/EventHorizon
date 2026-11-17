// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

/**
 * @file Material.h
 * @brief A Structure designed to persist 3d rendering material and texture data together.
 */

#pragma once

#include <QOpenGLTexture>
#include <QVector4D>

namespace EvH::SimFy::Rendering {

struct Material
{
    QVector4D                       baseColor{1.0F, 1.0F, 1.0F, 1.0F};
    std::unique_ptr<QOpenGLTexture> texture;
    bool                            blend = false;
};

} // namespace EvH::SimFy::Rendering
