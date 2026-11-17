# SimFy

<a href="../../html/index.html">&larr; Back to xECU-pro overview</a>

## Overview

SimFy is the xECU-pro 3D world simulator. It is a Qt Widgets application that
hosts an OpenGL 3.3 core context and renders the vehicle model
(`assets/space_rover.glb`) loaded with [Assimp](https://github.com/assimp/assimp).

The camera starts framed on the model. Drag with the left mouse button to
orbit around it: left/right rotates around the vertical axis, up/down tilts
the view (limited to ±85°). There is no panning or zoom.

## Components

| Class | Role |
|-------|------|
| SceneView | `QOpenGLWidget` owning the shader program, mouse-driven orbit camera and lighting; draws the model |
| Model | Loads a model file through Assimp, bakes node transforms, uploads meshes/materials/textures to the GPU |

## Rendering Pipeline

```
 space_rover.glb ──▶ Assimp (triangulate, pre-transform, flip UVs)
                          │
                          ▼
                 ┌──────────────────┐   VAO/VBO/EBO + base color textures
                 │      Model       │ ─────────────────────────────────────┐
                 └──────────────────┘                                      │
                                                                           ▼
                 ┌──────────────────┐  view/projection, lights   ┌──────────────────┐
                 │    SceneView     │ ─────────────────────────▶ │  GLSL 330 shader │
                 └──────────────────┘                            └──────────────────┘
```

- Materials: glTF base color factor and base color texture (embedded PNG/JPEG supported)
- Opaque meshes are drawn first; `BLEND` materials are drawn last with depth writes off
- Shading: Blinn-Phong key + fill light, hemisphere ambient, linear-space lighting with tone mapping
- Not yet used: normal, metallic/roughness and occlusion maps

## Runtime Assets

The `.glb` file is too large to embed through Qt's resource compiler, so the
build copies `assets/` next to the `SimFy` executable and the app loads it from there.

## See Also

- <a href="../../html/index.html">xECU-pro system architecture</a>
