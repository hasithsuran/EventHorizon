// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

/**
 * @file Model.h
 * @brief GPU-resident 3D model loaded through Assimp.
 */

#pragma once
#include <QOpenGLBuffer>
#include <QOpenGLExtraFunctions>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QString>
#include <QVector3D>
#include <QVector4D>
#include <memory>
#include <vector>

#include <renderer/Material.h>
#include <renderer/Mesh.h>

struct aiScene;
struct aiMaterial;
class QOpenGLShaderProgram;

namespace EvH::SimFy::Rendering {

/**
 * @class Model
 * @brief Loads a model file (glTF/GLB, OBJ, FBX, ...) with Assimp and uploads it to the GPU.
 *
 * Node transforms are baked into the vertices at load time, so the model is
 * drawn with a single model matrix. Each material supports a base color
 * factor, an optional base color texture (embedded or external) and
 * alpha blending. Opaque meshes are drawn before blended ones.
 *
 * @note All methods except the bounds accessors require a current OpenGL context.
 */
class Model : protected QOpenGLExtraFunctions
{
public:
    Model();
    ~Model();

    Model(const Model &)            = delete;
    Model &operator=(const Model &) = delete;

    /**
     * @brief Loads a model file and uploads its meshes and textures to the GPU.
     * @param path  Path to the model file.
     * @return      true on success; on failure errorString() describes the problem.
     */
    bool load(const QString &path);

    /**
     * @brief Draws all meshes with the given (already bound) shader program.
     *
     * Sets the uniforms @c uBaseColor, @c uHasTexture and binds the base color
     * texture to unit 0 for each mesh.
     */
    void draw(QOpenGLShaderProgram &program);

    /// @brief Minimum corner of the model's axis-aligned bounding box.
    QVector3D boundsMin() const { return m_boundsMin; }
    /// @brief Maximum corner of the model's axis-aligned bounding box.
    QVector3D boundsMax() const { return m_boundsMax; }
    /// @brief Description of the last load() failure.
    QString errorString() const { return m_error; }

private:
    void loadMaterial(const aiScene *scene, const aiMaterial *src, const QString &baseDir);
    void drawMesh(QOpenGLShaderProgram &program, const Mesh &mesh);

    std::vector<std::unique_ptr<Mesh>> m_meshes;
    std::vector<Material>              m_materials;
    QVector3D                          m_boundsMin;
    QVector3D                          m_boundsMax;
    QString                            m_error;
};

} // namespace EvH::SimFy::Rendering
