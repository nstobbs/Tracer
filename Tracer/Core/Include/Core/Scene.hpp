#pragma once

#include "Core/Light.hpp"
#include "Object/Object.hpp"

#include <vector>

namespace Tracer {

class Scene {
public:
    Scene() = default;
    ~Scene() = default;

    void addObject(Object* object);
    void addObjects(std::vector<Object*> objects);

    /* Unused */
    //void addLight(Light* light);
    //void addLights(std::vector<Light*>);

    std::vector<Object*>& getObjects() { return m_objects; }
    //std::vector<Light*>& getLights() { return m_lights; }
    u64 getVersion() const { return m_version; }

    std::vector<Object*> findHitObjects(const Ray& ray);/* Returns all Objects that BBox intersect with the incoming ray.*/

protected:
    u64 m_version = {0};

    std::vector<Object*> m_objects;
    //std::vector<Light*> m_lights;
};
};