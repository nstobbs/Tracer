#pragma once
#include <vector>
#include "Core/Types.hpp"
#include "Core/Ray.hpp"

namespace Tracer {
class LightSource;
class Object;

class Scene {
public:
    Scene() = default;
    ~Scene() = default;

    void addObject(Object* object);
    void addObjects(std::vector<Object*> objects);

    void addLightSource(LightSource* light);
    void addLightSources(std::vector<LightSource*> lights);

    std::vector<Object*>& objects() { return m_objects; }
    std::vector<LightSource*>& lightSources() { return m_lights; }
    
    u64 version() const { return m_version; }

    /* Returns all Objects or LightSources that BBox intersect with the incoming ray.*/
    std::vector<Object*> findHitObjects(const Ray& ray);
    std::vector<LightSource*> findHitLightSources(const Ray& ray); 

protected:
    u64 m_version = {0};

    std::vector<Object*> m_objects;
    std::vector<LightSource*> m_lights;
};
};