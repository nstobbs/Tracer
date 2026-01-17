#pragma once

#include "Tracer/Object.hpp"
#include "Tracer/Surface.hpp"

#include <vector>

namespace Tracer {

class Scene {
public:
    Scene() = default;
    ~Scene() = default;

    void AddObject(Object* object);
    void AddObjects(std::vector<Object*> objects);
    void AddSurface(Surface* surface);
    std::vector<Object*>& GetObjects() { return m_objects; };
    std::vector<Surface*>& GetSurfaces() { return m_surfaces; };

private:
    std::vector<Object*> m_objects;
    std::vector<Surface*> m_surfaces;
};

};