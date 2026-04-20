#pragma once

#include "Tracer/Types.hpp"
#include "Tracer/Ray.hpp"
#include "Tracer/Scene.hpp"
#include "Tracer/Image.hpp"
#include "Tracer/Interval.hpp"

#include "Surface/Surface.hpp"
#include "Surface/SolidColor.hpp"
#include "Surface/UVTexture.hpp"
#include "Surface/SurfaceNormals.hpp"

#include <cmath>
#include <random>

namespace Tracer {

class Material {
public:
    virtual Ray scatterRay(const Ray& ray, HitInfo& info, const Color4 missColor, Color4& attenuation) const = 0;
    Color4 rayColor(const Ray& ray, i32 rayDepth, const Color4 missColor, Scene* scene) const {
        if (rayDepth <= 0) {
            return missColor;
        }

        HitInfo info{};
        Color4 attenuation;
        for (auto object : scene->findHitObjects(ray)) {
            if (object->isHit(ray, info, Interval())) {
                Ray scattered = scatterRay(ray, info, missColor, attenuation);
                return (attenuation * rayColor(scattered, rayDepth - 1, missColor, scene));
            }
        }
        return attenuation;
    }

    u64 getVersion() const { return m_version; }
protected:
    std::vector<Surface*> m_surfaces;
    u64 m_version = {0};
};

class Diffuse : public Material {
public:
    Diffuse(Color4 baseColor) {
        m_albedoSurface = std::make_unique<SolidColor>(baseColor);
        m_normalsSurface = std::make_unique<SurfaceNormals>();
    }
    
    Diffuse(const std::string& texturePath) {
        m_albedoTexture = std::make_unique<Image>(Image::ReadImage(texturePath, "albedo"));
        m_albedoSurface = std::make_unique<UVTexture>(m_albedoTexture.get(), "albedo");
        m_normalsSurface = std::make_unique<SurfaceNormals>();
    }

    /* FIXME: Really don't like that fact that this returns a ray and modifites the color attenuation.
    Maybe Return an Struct of ColoredRay. Ray + Color4 */
    Ray scatterRay(const Ray& ray, HitInfo& info, const Color4 missColor, Color4& attenuation) const override {
        Vector3 scatterDirection = info.normal + randomUnitVector();
        Ray scatteredRay = Ray(info.position, scatterDirection);
        attenuation = m_albedoSurface->CalculateColor(info);
        return scatteredRay;
    }
    
private:
    /* Move this into it's own maths or vector file.*/
    inline Vector3 randomUnitVector() const {
        std::default_random_engine m_rd;
        std::uniform_real_distribution<f32> m_dist (-1.0f, 1.0f);
        //while (true) {
            /* Random Vector Between -1.0f to 1.0f */
            auto p = Vector3(m_dist(m_rd), m_dist(m_rd), m_dist(m_rd));
            auto lensq = std::powf(p.length(), 2.0f);
            //if (1e-160 < lensq && lensq <= 1.0f) {
                return p / std::sqrt(lensq);
            //}
        //}
    } 

    UniquePtr<Surface> m_normalsSurface;
    UniquePtr<Surface> m_albedoSurface;
    UniquePtr<Image> m_albedoTexture;
};

}/* End of Tracer namespace*/