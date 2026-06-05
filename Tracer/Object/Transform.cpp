#include "Object/Transform.hpp"

namespace Tracer {

void Transform::setTranslate(Vector3 translate) {
    m_version++;
    m_translate = translate;
};

void Transform::setRotation(Vector3 rotation) {
    m_version++;
    m_rotation = rotation;
};

void Transform::setScale(Vector3 scale) {
    m_version++;
    m_scale = scale;
};

bool Transform::isDirty() const {
    return m_version != m_lastVersion;
};

void Transform::build() {
    if (isDirty()) {
        Matrix4 mat(1.0f);
        auto translate = glm::translate(mat, m_translate);
        auto rotationX = glm::rotate(translate, m_rotation.x, Vector3(1.0f, 0.0f, 0.0f)); /* Rotate on X*/
        auto rotationY = glm::rotate(rotationX, m_rotation.y, Vector3(0.0f, 1.0f, 0.0f)); /* Rotate on Y*/
        auto rotationZ = glm::rotate(rotationY, m_rotation.z, Vector3(0.0f, 0.0f, 1.0f)); /* Rotate on Z*/
        auto scale = glm::scale(rotationZ, m_scale);
        
        /* Set Matrix */
        m_model = scale;
        m_invModel = glm::inverse(m_model);
        m_invModelT = glm::transpose(m_invModel);
        m_lastVersion = m_version;
    }
};

Point3 Transform::pointToObject(Point3 point) const {
    return glm::vec3(m_invModel * glm::vec4(point, 1.0f));
};

Point3 Transform::pointToWorld(Point3 point) const {
    return glm::vec3(m_model * glm::vec4(point, 1.0f));
};

Vector3 Transform::vectorToObject(Vector3 vector) const {
    return glm::vec3(m_invModel * glm::vec4(vector, 0.0f));
};

Vector3 Transform::vectorToWorld(Vector3 vector) const {
    return glm::normalize(glm::vec3(m_invModelT * glm::vec4(vector, 0.0f)));
};

Ray Transform::transformRay(const Ray& ray) const {
    Ray objectSpaceRay;
    objectSpaceRay.origin = pointToObject(ray.origin);
    objectSpaceRay.direction = vectorToObject(ray.direction);
    return objectSpaceRay;
};

HitInfo Transform::transformHitInfo(const HitInfo& info, const Ray& ray) const {
    HitInfo worldSpaceHitInfo{};
    worldSpaceHitInfo = info;
    worldSpaceHitInfo.position = pointToWorld(info.position);
    worldSpaceHitInfo.normal = vectorToWorld(info.normal);
    worldSpaceHitInfo.distance = glm::length(worldSpaceHitInfo.position - ray.origin);
    return worldSpaceHitInfo;
};

}