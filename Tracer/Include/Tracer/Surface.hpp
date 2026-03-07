#pragma once
#include "Tracer/Types.hpp"
#include "Tracer/Ray.hpp"

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

/* Displays the Normals */
class PreviewNormals : public Surface {
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
}
}