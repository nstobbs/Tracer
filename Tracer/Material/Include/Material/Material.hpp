#pragma once

#include "Core/Types.hpp"
#include "Core/Ray.hpp"
#include "Core/Scene.hpp"
#include "Core/Image.hpp"
#include "Core/Interval.hpp"

#include "Object/Light.hpp"

#include "Surface/Surface.hpp"
#include "Surface/SolidColor.hpp"
#include "Surface/UVTexture.hpp"
#include "Surface/SurfaceNormals.hpp"

#include <unordered_map>

namespace Tracer {

class Material {
public:
    //FIXME: Make this thread safe and const
    virtual bool scatter(const Ray incoming, const HitInfo info, Ray& outgoing, u32 depth);
    virtual LightFilterEvent createFilter(const Ray ray, const HitInfo info) const;
protected:
    std::unordered_map<std::string, Surface*> m_surfaces;
    u64 m_version = {0}; 
};

class DiffuseMaterial : public Material {
public:
    DiffuseMaterial(Color4 color) {
        initRandom();
        SolidColor* solidColorSurface = new SolidColor(color);
        m_surfaces.emplace("BaseColor", static_cast<Surface*>(solidColorSurface));
        m_version++;
    };

    DiffuseMaterial(Image* image, const std::string& layer) {
        initRandom();
        UVTexture* textureSurface = new UVTexture(image, layer);
        m_surfaces.emplace("BaseColor", static_cast<Surface*>(textureSurface));
        m_version++;
    };

    //FIXME: Make this thread safe and const
    bool scatter(const Ray incoming, const HitInfo info, Ray& outgoing, u32 depth) override {
        Vector3 randomPoint;
        f32 length;

        auto dist = std::uniform_real_distribution<f32>(0.0f, 1.0f);
        do {
            randomPoint.x = 2.0f * dist(m_gen) - 1.0f;
            randomPoint.y = 2.0f * dist(m_gen) - 1.0f;
            randomPoint.z = 2.0f * dist(m_gen) - 1.0f;
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

    LightFilterEvent createFilter(const Ray ray, const HitInfo info) const override {
        LightFilterEvent filer{};
        filer.lightEvent = m_surfaces.at("BaseColor")->CalculateColor(info);
        return filer;
    }

private:
    std::random_device m_rd;
    std::mt19937 m_gen;
    
    void initRandom() {
            m_gen = std::mt19937(m_rd());
        }

};

}