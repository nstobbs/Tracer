#pragma once

#include "Object/Mesh.hpp"

namespace Tracer {

class Surface;

/* LightFilterEvent - Represents the Material's affect to a given LightSource.
    Like how the energy from a LightSource is absorbed, reflected, emitted, etc... */
struct LightFilterEvent {
    Color3 lightEvent {0.0f};
};

/* LightFilterRecord - Represents all of the Material's interaction within a given ray
    path. This can be from a LightSource, or to a LightSource depending if the record
    is been recorded from the Camera's perspective or from the LightSource's perspective.*/
class LightFilterRecord {
public:
    LightFilterRecord(bool fromLightSource) : m_fromLightSource(fromLightSource) { ; }
    ~LightFilterRecord() = default;

    /* Returns if the Record is built from a incoming LightSource */
    const bool isComingFromLightSource() const {
        return m_fromLightSource;
    }

    /* Returns the a Color3 from all of the LightFilterEvents */
    const Color3 collapse() const {
        auto output = Color3(1.0f);
        for (auto event : m_events) {
            output *= event.lightEvent;
        }
        return output;
    } 

    void push(LightFilterEvent& event) {
        m_events.push_back(event);
    }
    /* Returns a new LightFilterRecord by combining two LightFilterRecord together. */
    static LightFilterRecord CombineLightFilerRecords(LightFilterRecord toLightSource, LightFilterRecord fromLightSource);
private:
    /* Indicates if the record is for rays heading towards a LightSource or from rays coming from a LightSource */
    const bool m_fromLightSource;
    std::vector<LightFilterEvent> m_events;
};

/* LightSource - Base Class for Lighting Objects  */
class LightSource : public Object {
public:
    virtual Ray fireRay() const = 0;
    virtual Color3 applyRecord(const LightFilterRecord& record) const = 0;
    void setColor(const Color4 color) { m_color = color; }
    void setIntensity(const f32 value) { m_intensity = value; }

protected:
    Color3 m_color;
    f32 m_intensity;
};

/* AreaLight - A rectangle LightSource */
class AreaLight : public LightSource {
public:
    AreaLight() {
        m_mesh = Mesh::RetangleMesh();
        m_color = Color3(1.0f, 1.0f, 1.0f);
        m_intensity = 10.0f;
        m_bbox = m_mesh.bbox();
    }
    /* LightSource Virtual Functions */
    Ray fireRay() const override {
        /* https://articles.alexcastronovo.com/article/1/a-simple-efficient-and-unbaised-approach-to-uniformly-sampling-a-mesh */
        return Ray();// TODO: Implement for Next Event Estimation
    }

    /* Applies the LightEventRecord to the LightSource and returns the final pixel color. */
    Color3 applyRecord(const LightFilterRecord& record) const override {
        return m_color * record.collapse();
    }

    /* Object Virtual Functions */
    bool isHit(const Ray& ray, HitInfo& hitInfo, Interval interval) override {
        return m_mesh.isHit(ray, hitInfo, interval);
    }

    void setSurface(Surface* surface) override
    { 
        m_mesh.setSurface(surface);
    }

    Transform& transform() override {
        return m_mesh.transform();
    }

private:
    Mesh m_mesh;
};

/* DomeLight - A sphere LightSource that emitter rays from outside in.
    Normally used as a SkyDome with HDRI Textures */
class DomeLight : public LightSource {
public:
    DomeLight() {
        m_mesh = Mesh::SphereMesh();
    }

    /* LightSource Virtual Functions */
    Ray fireRay() const override;
    Color3 applyRecord(const LightFilterRecord& record) const override;

    /* Object Virtual Functions */
    bool isHit(const Ray& ray, HitInfo& hitInfo, Interval interval) override {
        return m_mesh.isHit(ray, hitInfo, interval);
    }

private:
    Mesh m_mesh;
};

}