#pragma once
#include "Object/BoundingBox.hpp"
#include "Object/Transform.hpp"

#include "Core/Ray.hpp"
#include "Core/Interval.hpp"
#include "Core/Camera.hpp"

namespace Tracer {
class Material;
class Surface;

//TODO: remove surface 
class Object {
public:
    bool virtual isHit(const Ray& ray, HitInfo& hitInfo, Interval interval) = 0;

    void setMaterial(Material* material) { m_material = material; }  
    Material* material() const { return m_material; }
    
    // TODO: Not sure how I feel about having these setters functions
    // as virtuals. Only doing this so LightSource can mostly re-use
    // the object base class. And so that we can re-use Object's like the mesh inside
    // of the LightSource class to be renderable via normal render pipeline
    // and use the same functionality with the lighting pipeline.
    // The issue is, for Lights that I want to be renderable as an visible object, will
    // have an private member of a Mesh. Mesh will only use the transforms and surface
    // on it's own class instead of using the members from the LightSource. 
    // So having virtual setters and getters means that we can pass the correct
    // one betweens to two class.

    virtual void setSurface(Surface* surface) { m_surface = surface; }
    Surface* surface() const { return m_surface; }

    BBox bbox() const { return m_bbox; };
    virtual Transform& transform() { return m_transform; }

protected:
    Material* m_material = {nullptr};
    Surface* m_surface = {nullptr};
    BBox m_bbox;
    Transform m_transform;
};
}