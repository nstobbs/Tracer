#pragma once

#include "Core/Types.hpp"
#include "Core/Ray.hpp"
#include "Core/Scene.hpp"
#include "Core/Image.hpp"
#include "Core/Interval.hpp"

#include "Object/LightSource.hpp"

#include "Surface/Surface.hpp"

#include <unordered_map>

namespace Tracer {

class Material {
public:
    virtual bool scatter(const Ray& incoming, const HitInfo& info, Ray& outgoing) = 0;
    virtual LightFilterEvent createFilter(const Ray& ray, const HitInfo& info) const = 0;
protected:
    /* Random Stuff */
    std::random_device m_rd;
    std::mt19937 m_gen;
    std::uniform_real_distribution<f32> m_dist;
    void initRandom();
    Vector3 randomUnitVector();

    std::unordered_map<std::string, Surface*> m_surfaces;
    u64 m_version = {0}; 
};

class DiffuseMaterial : public Material {
public:
    DiffuseMaterial(Color4 color);
    DiffuseMaterial(Image* image, const std::string& layer);

    bool scatter(const Ray& incoming, const HitInfo& info, Ray& outgoing) override;
    LightFilterEvent createFilter(const Ray& ray, const HitInfo& info) const override;
};

class GlassMaterial : public Material {
public:
    GlassMaterial(f32 IOR);
    GlassMaterial(Color4 color, f32 IOR);

    bool scatter(const Ray& incoming, const HitInfo& info, Ray& outgoing) override;
    LightFilterEvent createFilter(const Ray& ray, const HitInfo& info) const override;
protected:
    static f32 reflectance(f32 cosine, f32 refractionIndex);

    Color4 m_tint = {1.0f, 1.0f, 1.0f, 1.0f};
    f32 m_indexOfRefraction = {0.0f};
};

class MetalMaterial : public Material {
public:
    MetalMaterial(f32 fuzz);
    MetalMaterial(Color4 color, f32 fuzz);

    bool scatter(const Ray& incoming, const HitInfo& info, Ray& outgoing) override;
    LightFilterEvent createFilter(const Ray& ray, const HitInfo& info) const override;
protected:
    Color4 m_tint = {1.0f, 1.0f, 1.0f, 1.0f};
    f32 m_fuzz = {1.0f};
};

}