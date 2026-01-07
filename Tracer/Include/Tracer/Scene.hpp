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
    void AddSurface(Surface* surface);
    std::vector<Object*>& GetObjects();
    std::vector<Surface*>& GetSurfaces();

private:
    std::vector<Object*> m_objects;
    std::vector<Surface*> m_surfaces;
};

};