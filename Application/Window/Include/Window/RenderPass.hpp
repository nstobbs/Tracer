#pragma once

#include "Tracer/Camera.hpp"

using namespace Tracer;

struct RenderContext {
    Camera* camera;
    Layer* layer;
};

class RenderPass {
public:
    virtual ~RenderPass() = default;

    virtual bool init() = 0;
    virtual void render(RenderContext& context) = 0;
    virtual void cleanup() = 0;

    void enable() { m_enabled = true; }
    void disable() { m_enabled = false; }
    bool isEnabled() const { return m_enabled; }

    /* Calls render if the pass is enabled. */
    void process(RenderContext& context) {
        if (m_enabled) {
            render(context);
        }
    }
    
protected:
    bool m_enabled = {true};
};