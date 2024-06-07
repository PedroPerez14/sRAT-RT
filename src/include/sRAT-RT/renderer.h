#ifndef RENDERER_CLASS_H
#define RENDERER_CLASS_H

#include <sRAT-RT/scene.h>

/// Virtual class that will have to be overriden by the
///     different renderers (to easily define rendering pipelines)
///     INFO: The camera is not included as member here, it's part of the Scene
class Renderer
{
public:
    virtual ~Renderer()
    {
        
    }

    virtual void render_scene(Scene* scene) = 0;
    virtual void render_ui() = 0;
    virtual void handle_resize(int width, int height) = 0;

    virtual void take_screenshot() = 0;

protected:
};

#endif