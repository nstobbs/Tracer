#include "Material/Material.hpp"

#include "Surface/SolidColor.hpp"
#include "Surface/UVTexture.hpp"
#include "Surface/SurfaceNormals.hpp"

namespace Tracer {

void Material::initRandom() {
    m_gen = std::mt19937(m_rd());
    m_dist = std::uniform_real_distribution<f32>(0.0f, 1.0f);
}

Vector3 Material::randomUnitVector() {
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
    Vector3 output = {randomPoint.x * invLength,
                            randomPoint.y * invLength,
                            randomPoint.z * invLength};
    return output;
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

bool DiffuseMaterial::scatter(const Ray& incoming, const HitInfo& info, Ray& outgoing) {
    auto scattered = info.normal + randomUnitVector();
    f32 dot = glm::dot(scattered, info.normal);

    if (dot < 0.0f) {
        scattered *= -1.0f;
    }
    outgoing.origin = info.position;
    outgoing.direction = scattered;
    return true;
}

LightFilterEvent DiffuseMaterial::createFilter(const Ray& ray, const HitInfo& info) const {
    LightFilterEvent filer{};
    filer.lightEvent = m_surfaces.at("BaseColor")->CalculateColor(info);
    return filer;
}

GlassMaterial::GlassMaterial(f32 IOR) {
    initRandom();
    m_indexOfRefraction = IOR;
}

GlassMaterial::GlassMaterial(Color4 color, f32 IOR) {
    initRandom();
    m_tint = color;
    m_indexOfRefraction = IOR;
}

bool GlassMaterial::scatter(const Ray& incoming, const HitInfo& info, Ray& outgoing) {
    const auto IOR = (info.isFrontFace) ? (1.0f/m_indexOfRefraction) : m_indexOfRefraction;
    
    const auto normalizedDirection = glm::normalize(incoming.direction);
    const auto cosTheta = std::fmin(glm::dot(-normalizedDirection, info.normal), 1.0f);
    const auto sinTheta = std::sqrt(1.0 - cosTheta*cosTheta);

    const bool cannotRefract = (IOR * sinTheta) > 1.0f;
    if (cannotRefract || reflectance(cosTheta, IOR) > m_dist(m_gen)) {
        outgoing.direction = glm::reflect(normalizedDirection, info.normal);
    } else {
        outgoing.direction = glm::refract(normalizedDirection, info.normal, IOR);
    }

    outgoing.origin = info.position;
    return true;
}

LightFilterEvent GlassMaterial::createFilter(const Ray& ray, const HitInfo& info) const {
    LightFilterEvent filter{};
    filter.lightEvent = Color3(m_tint.r, m_tint.g, m_tint.b);
    return filter;
}

f32 GlassMaterial::reflectance(f32 cosine, f32 refractionIndex) {
    auto r0 = (1.0f - refractionIndex / (1.0f + refractionIndex));
    r0 = r0*r0;
    return r0 + (1.0f-r0)*std::pow((1.0f - cosine), 5.0f); 
}

MetalMaterial::MetalMaterial(f32 fuzz) {
    initRandom();
    m_fuzz = fuzz;
}

MetalMaterial::MetalMaterial(Color4 color, f32 fuzz) {
    initRandom();
    m_tint = color;
    m_fuzz = fuzz;
}

bool MetalMaterial::scatter(const Ray& incoming, const HitInfo& info, Ray& outgoing) {
    Vector3 reflected = glm::reflect(incoming.direction, info.normal);
    outgoing.direction = glm::normalize(reflected) + (m_fuzz * randomUnitVector());
    outgoing.origin = info.position;
    return (glm::dot(outgoing.direction, info.normal) > 0.0f);
}

LightFilterEvent MetalMaterial::createFilter(const Ray& ray, const HitInfo& info) const {
    LightFilterEvent filter{};
    filter.lightEvent = Color3(m_tint.r, m_tint.g, m_tint.b);
    return filter;
}

}