#include "Object/Instancer.hpp"

namespace Tracer {

InstanceObject::InstanceObject(Instancer* instancer, i32 index) {
    m_instancer = instancer;
    m_index = index;
}

bool InstanceObject::isHit(const Ray& ray, HitInfo& hitInfo, Interval interval) {
    if (!m_instancer) {
        return false;
    }

    if (auto* object = m_instancer->m_object) {
        const bool hit = object->isHit(transform().transformRay(ray), hitInfo, interval);
        hitInfo = transform().transformHitInfo(hitInfo, ray);
        hitInfo.object = static_cast<Object*>(this);
        return hit;
    }

    return false;
}

Transform& InstanceObject::transform() {
    if (m_instancer) {
        return m_instancer->transform(m_index); 
    }
}
BBox InstanceObject::bbox() const {
    if (auto* object = m_instancer->m_object) {
        return object->bbox();
    }
    return {};
}

Instancer::Instancer(Scene* scene, Object* object) {
    m_object = object;
    m_scene = scene;
    if (m_object) {
        m_object->transform().build();
    }
    setObjectCount(12);
}

void Instancer::setScene(Scene* scene) {
    m_version++;
    m_scene = scene;
}

void Instancer::setObject(Object* object) {
    m_version++;
    m_object = object;
    if (m_object) {
        m_object->transform().build();
    }
}

void Instancer::setObjectCount(i32 value) {
    m_version++;
    m_objectCount = value;
}

void Instancer::addMaterial(const std::string& materialName, Material* material) {
    if (material) {
        m_version++;
        m_materials.emplace(materialName, material);
    }
}

void Instancer::setInstanceObjectMaterial(const std::string& materialName, i32 index) {
    if (m_materials.find(materialName) != m_materials.end()) {
        m_instances.at(index).setMaterial(m_materials.at(materialName));
    }
}

Transform& Instancer::transform(i32 index) {
    return m_transforms.at(index);
}

Material* Instancer::material(const std::string& materialName) {
    if (m_materials.find(materialName) != m_materials.end()) {
        return m_materials.at(materialName);
    }

    return nullptr;
}

void Instancer::populateScene() {
    if (m_lastVersion != m_version) {
        depopulateScene();
        m_sceneBeginIndex = m_scene->objects().size();
        m_instances.reserve(m_objectCount);
        m_transforms.reserve(m_objectCount);
        for (i32 i = 0; i < m_objectCount; i++) {
            m_instances.emplace_back(InstanceObject(this, i));
            m_transforms.emplace_back(Transform());
            m_scene->addObject(static_cast<Object*>(&m_instances.at(i)));
        }
        m_lastVersion = m_version;
    }
}

void Instancer::depopulateScene() {
    m_version++;
    if (!m_instances.empty()) {
        std::vector<Object*>::iterator startIter = m_scene->objects().begin() + m_sceneBeginIndex;
        std::vector<Object*>::iterator endIter = startIter + m_objectCount;
        m_scene->objects().erase(startIter, endIter);
        m_instances.clear();
        m_transforms.clear();
    }
}

const bool Instancer::isDirty() const {
    return m_version != m_lastVersion;
}

void Instancer::checkAndReBuild() {
    if (isDirty() && m_firstTime) {
        m_firstTime = false;
        populateScene();
    } else if (isDirty()) {
        depopulateScene();
        populateScene();
    }
}

}