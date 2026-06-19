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
    //FIXME: Make this thread safe and const
    virtual bool scatter(const Ray incoming, const HitInfo info, Ray& outgoing) = 0;
    virtual LightFilterEvent createFilter(const Ray ray, const HitInfo info) const = 0;
protected:
    std::unordered_map<std::string, Surface*> m_surfaces;
    u64 m_version = {0}; 
};

class DiffuseMaterial : public Material {
public:
    DiffuseMaterial(Color4 color);
    DiffuseMaterial(Image* image, const std::string& layer);

    bool scatter(const Ray incoming, const HitInfo info, Ray& outgoing) override;
    LightFilterEvent createFilter(const Ray ray, const HitInfo info) const override;

private:
    std::random_device m_rd;
    std::mt19937 m_gen;
    std::uniform_real_distribution<f32> m_dist;
    
    void initRandom();
};

}