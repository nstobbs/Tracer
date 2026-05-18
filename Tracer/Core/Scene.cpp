#include "Core/Scene.hpp"

namespace Tracer {

void Scene::addObject(Object* object) {
    m_objects.push_back(object);
};

void Scene::addObjects(std::vector<Object*> objects) {
    for (auto object : objects) {
        m_objects.push_back(object);
    }
};

std::vector<Object*> Scene::findHitObjects(const Ray& ray) {
    std::vector<Object*> found;
    for (auto& object : m_objects) {
        if (object->getBBox().isHit(ray)) {
            found.push_back(object);
        }
    }
    return found;
}

}