#pragma once

#include "Object/Mesh.hpp"
#include "Surface/UVTexture.hpp"

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
    const bool isComingFromLightSource() const;
    /* Returns the a Color3 from all of the LightFilterEvents */
    const Color3 collapse() const; 
    void push(LightFilterEvent& event);

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
    virtual Color3 applyRecord(const LightFilterRecord& record, const HitInfo& info) const = 0;
    void setColor(const Color3 color) { m_color = color; }
    void setIntensity(const f32 value) { m_intensity = value; }

    virtual Color4 calculateSurface(const HitInfo& info) const = 0;

    Color3 color() const { return m_color; }
    f32 intensity() const { return m_intensity; }

    static std::string ClassString() { return "LightSource"; }

protected:
    Color3 m_color;
    f32 m_intensity;
};

/* AreaLight - A rectangle LightSource */
class AreaLight : public LightSource {
public:
    AreaLight();
    /* LightSource Virtual Functions */
    Ray fireRay() const override;
    /* Applies the LightEventRecord to the LightSource and returns the final pixel color. */
    Color3 applyRecord(const LightFilterRecord& record, const HitInfo& info) const override;

    /* Object Virtual Functions */
    bool isHit(const Ray& ray, HitInfo& hitInfo, Interval interval) override;
    Color4 calculateSurface(const HitInfo& info) const override;
    
    BBox bbox() const override { return m_mesh.bbox(); }
    Transform& transform() override { return m_mesh.transform(); }

    static std::string ClassString() { return "AreaLight"; }

private:
    Mesh m_mesh;
};

/* DomeLight - A sphere LightSource that emitter rays from outside in.
    Normally used as a SkyDome with HDRI Textures */
class DomeLight : public LightSource {
public:
    DomeLight(Image* texture, const std::string& layer);
    /* LightSource Virtual Functions */
    Ray fireRay() const override;
    Color3 applyRecord(const LightFilterRecord& record, const HitInfo& info) const override;

    /* Object Virtual Functions */
    bool isHit(const Ray& ray, HitInfo& hitInfo, Interval interval) override;
    Color4 calculateSurface(const HitInfo& info) const override;
    BBox bbox() const override { return m_mesh.bbox(); }
    Transform& transform() override { return m_mesh.transform(); }

    static std::string ClassString() { return "DomeLight"; }
private:
    Mesh m_mesh;
    Image* m_image = {nullptr};
    UVTexture m_textureUV;
};

}