// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

#include "Model.h"

#include <QFileInfo>
#include <QImage>
#include <QOpenGLShaderProgram>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <cstddef>
#include <limits>
#include <string_view>

namespace {

struct Vertex
{
    float position[3];
    float normal[3];
    float uv[2];
};

/// Converts an Assimp texture (compressed or raw BGRA texels) to a QImage.
QImage imageFromEmbedded(const aiTexture *tex)
{
    if (tex->mHeight == 0) {
        // Compressed (PNG/JPEG): mWidth holds the byte size
        return QImage::fromData(reinterpret_cast<const uchar *>(tex->pcData),
                                static_cast<int>(tex->mWidth));
    }
    // Raw aiTexel (b, g, r, a) matches QImage::Format_ARGB32 on little-endian
    QImage img(reinterpret_cast<const uchar *>(tex->pcData), static_cast<int>(tex->mWidth),
               static_cast<int>(tex->mHeight), QImage::Format_ARGB32);
    return img.copy(); // detach from Assimp-owned memory
}

} // namespace

Model::Model() = default;

// Members release their GL objects; the owner must make the context current.
Model::~Model() = default;

bool Model::load(const QString &path)
{
    initializeOpenGLFunctions();
    m_meshes.clear();
    m_materials.clear();
    m_error.clear();

    // FlipUVs restores glTF's top-left UV origin, matching the unflipped
    // QImage rows uploaded in loadMaterial().
    Assimp::Importer importer;
    const aiScene   *scene = importer.ReadFile(
        path.toStdString(), aiProcess_Triangulate | aiProcess_PreTransformVertices
                                | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices
                                | aiProcess_FlipUVs);
    if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
        m_error = QString::fromUtf8(importer.GetErrorString());
        return false;
    }

    const QString baseDir = QFileInfo(path).absolutePath();
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
        loadMaterial(scene, scene->mMaterials[i], baseDir);

    constexpr float inf = std::numeric_limits<float>::max();
    m_boundsMin         = QVector3D(inf, inf, inf);
    m_boundsMax         = QVector3D(-inf, -inf, -inf);

    for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
        const aiMesh *src = scene->mMeshes[m];

        std::vector<Vertex> vertices(src->mNumVertices);
        for (unsigned int v = 0; v < src->mNumVertices; ++v) {
            const aiVector3D &p = src->mVertices[v];
            const aiVector3D  n = src->HasNormals() ? src->mNormals[v] : aiVector3D(0, 1, 0);
            const aiVector3D  t
                = src->HasTextureCoords(0) ? src->mTextureCoords[0][v] : aiVector3D(0, 0, 0);
            vertices[v] = {{p.x, p.y, p.z}, {n.x, n.y, n.z}, {t.x, t.y}};

            const QVector3D qp(p.x, p.y, p.z);
            m_boundsMin = QVector3D(qMin(m_boundsMin.x(), qp.x()), qMin(m_boundsMin.y(), qp.y()),
                                    qMin(m_boundsMin.z(), qp.z()));
            m_boundsMax = QVector3D(qMax(m_boundsMax.x(), qp.x()), qMax(m_boundsMax.y(), qp.y()),
                                    qMax(m_boundsMax.z(), qp.z()));
        }

        std::vector<GLuint> indices;
        indices.reserve(static_cast<size_t>(src->mNumFaces) * 3);
        for (unsigned int f = 0; f < src->mNumFaces; ++f) {
            const aiFace &face = src->mFaces[f];
            if (face.mNumIndices != 3) continue; // skip leftover points/lines
            indices.insert(indices.end(), face.mIndices, face.mIndices + 3);
        }
        if (indices.empty()) continue;

        auto mesh        = std::make_unique<Mesh>();
        mesh->indexCount = static_cast<GLsizei>(indices.size());
        mesh->material   = src->mMaterialIndex;
        mesh->vao        = std::make_unique<QOpenGLVertexArrayObject>();
        mesh->vao->create();
        mesh->vao->bind();

        mesh->vbo.create();
        mesh->vbo.bind();
        mesh->vbo.allocate(vertices.data(), static_cast<int>(vertices.size() * sizeof(Vertex)));

        mesh->ebo.create();
        mesh->ebo.bind();
        mesh->ebo.allocate(indices.data(), static_cast<int>(indices.size() * sizeof(GLuint)));

        constexpr auto stride = static_cast<GLsizei>(sizeof(Vertex));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                              reinterpret_cast<void *>(offsetof(Vertex, position)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                              reinterpret_cast<void *>(offsetof(Vertex, normal)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                              reinterpret_cast<void *>(offsetof(Vertex, uv)));

        mesh->vao->release();
        m_meshes.push_back(std::move(mesh));
    }

    if (m_meshes.empty()) {
        m_error = QStringLiteral("Model contains no triangle meshes");
        return false;
    }
    return true;
}

void Model::loadMaterial(const aiScene *scene, const aiMaterial *src, const QString &baseDir)
{
    Material mat;

    aiColor4D color;
    if (src->Get(AI_MATKEY_BASE_COLOR, color) == AI_SUCCESS
        || src->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
        mat.baseColor = QVector4D(color.r, color.g, color.b, color.a);

    aiString alphaMode;
    if (src->Get("$mat.gltf.alphaMode", 0, 0, alphaMode) == AI_SUCCESS)
        mat.blend = (std::string_view(alphaMode.C_Str()) == "BLEND");

    aiString texPath;
    if (src->GetTexture(aiTextureType_BASE_COLOR, 0, &texPath) == AI_SUCCESS
        || src->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
        QImage image;
        if (const aiTexture *embedded = scene->GetEmbeddedTexture(texPath.C_Str()))
            image = imageFromEmbedded(embedded);
        else
            image.load(baseDir + '/' + QString::fromUtf8(texPath.C_Str()));

        if (!image.isNull()) {
            mat.texture = std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D);
            mat.texture->setData(image); // rows uploaded top-down (no mirroring)
            mat.texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
            mat.texture->setMagnificationFilter(QOpenGLTexture::Linear);
            mat.texture->setWrapMode(QOpenGLTexture::Repeat);
        } else {
            qWarning("SimFy: could not load texture '%s'", texPath.C_Str());
        }
    }

    m_materials.push_back(std::move(mat));
}

void Model::draw(QOpenGLShaderProgram &program)
{
    // Opaque first, then alpha-blended surfaces without depth writes
    for (const auto &mesh : m_meshes)
        if (!m_materials[mesh->material].blend) drawMesh(program, *mesh);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    for (const auto &mesh : m_meshes)
        if (m_materials[mesh->material].blend) drawMesh(program, *mesh);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void Model::drawMesh(QOpenGLShaderProgram &program, const Mesh &mesh)
{
    const Material &mat = m_materials[mesh.material];
    program.setUniformValue("uBaseColor", mat.baseColor);
    program.setUniformValue("uHasTexture", mat.texture != nullptr);
    if (mat.texture) mat.texture->bind(0);

    mesh.vao->bind();
    glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, nullptr);
    mesh.vao->release();
}
