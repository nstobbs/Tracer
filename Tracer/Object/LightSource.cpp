#include "Object/LightSource.hpp"
#include "Surface/Surface.hpp"

namespace Tracer {

/* LightFilterrecord */
const bool LightFilterRecord::isComingFromLightSource() const {
    return m_fromLightSource;
}

const Color3 LightFilterRecord::collapse() const {
    auto output = Color3(1.0f);
    for (auto event : m_events) {
        output *= event.lightEvent;
    }
    return output;
}

void LightFilterRecord::push(LightFilterEvent& event) {
    m_events.push_back(event);
}

/* AreaLight */
AreaLight::AreaLight() {
    m_mesh = Mesh::RetangleMesh();
    m_color = Color3(1.0f, 1.0f, 1.0f);
    m_intensity = 1.0f;
}

Ray AreaLight::fireRay() const {
    /* https://articles.alexcastronovo.com/article/1/a-simple-efficient-and-unbaised-approach-to-uniformly-sampling-a-mesh */
    return Ray();// TODO: Implement for Next Event Estimation
}

Color3 AreaLight::applyRecord(const LightFilterRecord& record) const {
    return (m_color * Color3(m_intensity)) * record.collapse();
}

bool AreaLight::isHit(const Ray& ray, HitInfo& hitInfo, Interval interval) {
    return m_mesh.isHit(ray, hitInfo, interval);
}

Color4 AreaLight::calculateSurface(const HitInfo& info) const {
    return m_surface->CalculateColor(info);
}

}

