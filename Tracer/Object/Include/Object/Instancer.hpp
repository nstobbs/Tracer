#pragma once
#include "Core/Scene.hpp"
#include "Object/Transform.hpp"
#include "Material/Material.hpp"

#include <unordered_map>

namespace Tracer {

class Instancer;

class InstanceObject : public Object {
public:
    InstanceObject(Instancer* instancer, i32 index);
    bool isHit(const Ray& ray, HitInfo& hitInfo, Interval interval) override;
    Transform& transform() override;
    BBox bbox() const override;

    static std::string ClassString() { return "InstanceObject"; }

protected:
    Instancer* m_instancer = {nullptr};
    i32 m_index = {-1};
};

class Instancer {
public:
    Instancer(Scene* scene, Object* object);

    void setScene(Scene* scene);
    void setObject(Object* object);
    void setObjectCount(i32 value);

    void addMaterial(const std::string& materialName, Material* material);
    void setInstanceObjectMaterial(const std::string& materialName, i32 index);

    void checkAndReBuild();

    Transform& transform(i32 index);
    Material* material(const std::string& materialName);
    const i32 objectCount() const { return m_objectCount; }

protected:
    friend InstanceObject;

    void populateScene(); /* Creates Instances into the Scene */
    void depopulateScene(); /* Removes Instances From the Scene */
    const bool isDirty() const;

    u32 m_version = {0};
    u32 m_lastVersion = {0};

    bool m_firstTime = {true};
    Object* m_object = {nullptr};
    Scene* m_scene = {nullptr};

    u32 m_sceneBeginIndex = {0}; /* Index of the first Instance Object within the Scene Object Array */
    u32 m_objectCount = {0};

    std::vector<InstanceObject> m_instances;
    std::vector<Transform> m_transforms;
    std::unordered_map<std::string, Material*> m_materials;
};

}

