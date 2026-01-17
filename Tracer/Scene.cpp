#include "Tracer/Scene.hpp"

namespace Tracer {

void Scene::AddObject(Object* object) {
    m_objects.push_back(object);
};

void Scene::AddObjects(std::vector<Object*> objects) {
    for (auto object : objects) {
        m_objects.push_back(object);
    }
};

void Scene::AddSurface(Surface* surface) {
    m_surfaces.push_back(surface);
};

}