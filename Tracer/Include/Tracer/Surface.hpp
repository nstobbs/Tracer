#pragma once
#include "Tracer/Types.hpp"
#include "Tracer/Ray.hpp"
#include "Tracer/Image.hpp"

namespace Tracer {

class Surface {
public:
    ~Surface() = default;
    virtual Color4 CalculateColor(const HitInfo& info) = 0;
protected:
    Color4 m_backfaceColor = {1.0f, 0.2f, 1.0f, 1.0f};
};

// TODO: Move theses surfaces into they our files / folder!
/* Surface Shaders */
namespace SurfaceShader {

/* Displays the Vertex Color */
class VertexColor : public Surface {
public:
    Color4 CalculateColor(const HitInfo& info) override;
};

/* Displays the Geometric Normals Created 
    During Intersection Testing  */
class GeometricNormals : public Surface {
public:
    Color4 CalculateColor(const HitInfo& info) override;

};

/* Displays the Surface Normals Created Using
the Vertex Normals and Barycentric system */
class SurfaceNormals : public Surface {
public:
    Color4 CalculateColor(const HitInfo& info) override;

};

/* Displays a Solid Colour */
class SolidColor : public Surface {
public:
    SolidColor(Color4 color) : m_color(color) { };
    Color4 CalculateColor(const HitInfo& info) override;
private:
    Color4 m_color;
};

/* Displays a Wireframe */
class Wireframe : public Surface {
public:
    Wireframe(Color4 color) : m_color(color) { };
    Color4 CalculateColor(const HitInfo& info) override;
private:
    Color4 m_color;
    f32 m_lineThickness = {0.005f};
};

/* Displays two SurfaceShaders Merged Together */
class MergeSurfaceShader : public Surface {
public:
    //FIXME: this ends up becoming a massive line to just pick a operation!
    //SurfaceShader::MergeSurfaceShader::MergeOperation::Plus
    enum class MergeOperation {Over, Plus}; 
    MergeSurfaceShader(Surface* surfaceA, Surface* surfaceB, MergeOperation mergeOp)
    : m_surfaceA(surfaceA), m_surfaceB(surfaceB), m_operation(mergeOp) { };

    Color4 CalculateColor(const HitInfo& info) override;
private:
    Surface* m_surfaceA = {nullptr};
    Surface* m_surfaceB = {nullptr};
    MergeOperation m_operation;
};

/* Display Image Texture */
class UVTexture : public Surface {
public:
    UVTexture(Image* image, const std::string& layer) : m_image(image), m_layerName(layer) {}
    Color4 CalculateColor(const HitInfo& info) override;
private:
    Image* m_image = {nullptr};
    const std::string m_layerName;
};
}
}