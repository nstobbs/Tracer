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
};