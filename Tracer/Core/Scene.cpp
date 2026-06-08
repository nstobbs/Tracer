#include "Core/Scene.hpp"
#include "Object/Light.hpp"
#include "Object/Object.hpp"

namespace Tracer {

void Scene::addObject(Object* object) {
    m_objects.push_back(object);
    m_version++;
};

void Scene::addLightSource(LightSource* light) {
    m_lights.push_back(light);
    m_version++;
};

void Scene::addObjects(std::vector<Object*> objects) {
    for (auto object : objects) {
        addObject(object);
    }
};

void Scene::addLightSources(std::vector<LightSource*> lights) {
    for (auto light : lights) {
        addLightSource(light);
    }
};

std::vector<Object*> Scene::findHitObjects(const Ray& ray) {
    std::vector<Object*> found;
    for (auto& object : m_objects) {
        if (object->bbox().isHit(object->transform().transformRay(ray))) {
            found.push_back(object);
        }
    }
    return found;
}

}