#include "Material/Material.hpp"

#include "Surface/SolidColor.hpp"
#include "Surface/UVTexture.hpp"
#include "Surface/SurfaceNormals.hpp"

namespace Tracer {

void DiffuseMaterial::initRandom() {
    m_gen = std::mt19937(m_rd());
    m_dist = std::uniform_real_distribution<f32>(0.0f, 1.0f);
}

DiffuseMaterial::DiffuseMaterial(Color4 color) {
    initRandom();
    SolidColor* solidColorSurface = new SolidColor(color);
    m_surfaces.emplace("BaseColor", static_cast<Surface*>(solidColorSurface));
    m_version++;
}

DiffuseMaterial::DiffuseMaterial(Image* image, const std::string& layer) {
    initRandom();
    UVTexture* textureSurface = new UVTexture(image, layer);
    m_surfaces.emplace("BaseColor", static_cast<Surface*>(textureSurface));
    m_version++;
}

bool DiffuseMaterial::scatter(const Ray incoming, const HitInfo info, Ray& outgoing) {
    Vector3 randomPoint;
    f32 length;

    do {
        randomPoint.x = 2.0f * m_dist(m_gen) - 1.0f;
        randomPoint.y = 2.0f * m_dist(m_gen) - 1.0f;
        randomPoint.z = 2.0f * m_dist(m_gen) - 1.0f;
        length = randomPoint.x * randomPoint.x +
                 randomPoint.y * randomPoint.y +
                 randomPoint.z * randomPoint.y;
    } while (length > 1.0f || length == 0.0f);

    f32 invLength = 1.0f / std::sqrt(length);
    Vector3 scattered = {randomPoint.x * invLength,
                            randomPoint.y * invLength,
                            randomPoint.z * invLength};

    f32 dot = glm::dot(scattered, info.normal);

    if (dot < 0.0f) {
        scattered *= -1.0f;
    }
    outgoing.origin = info.position;
    outgoing.direction = scattered;
    return true;
}

LightFilterEvent DiffuseMaterial::createFilter(const Ray ray, const HitInfo info) const {
    LightFilterEvent filer{};
    filer.lightEvent = m_surfaces.at("BaseColor")->CalculateColor(info);
    return filer;
}

}