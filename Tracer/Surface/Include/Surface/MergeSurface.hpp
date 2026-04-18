#include "Surface/Surface.hpp"

namespace Tracer {

/* Displays two SurfaceShaders Merged Together */
class MergeSurfaceShader : public Surface {
public:
    //FIXME: this ends up becoming a massive line to just pick a operation!
    //SurfaceShader::MergeSurfaceShader::MergeOperation::Plus
    enum class MergeOperation {Over, Plus}; 
    MergeSurfaceShader(Surface* surfaceA, Surface* surfaceB, MergeOperation mergeOp)
    : m_surfaceA(surfaceA), m_surfaceB(surfaceB), m_operation(mergeOp) { };

    Color4 CalculateColor(const HitInfo& info) override {
        Color4 output(0.0f, 0.0f, 0.0f, 0.0f);
        if (m_surfaceA && m_surfaceB) {
            switch (m_operation) {
                case MergeOperation::Plus:
                    output = m_surfaceA->CalculateColor(info) + m_surfaceB->CalculateColor(info);
                    break;

                case MergeOperation::Over:
                    break;
            }
        }
        return output;
    };

private:
    Surface* m_surfaceA = {nullptr};
    Surface* m_surfaceB = {nullptr};
    MergeOperation m_operation;
};

}