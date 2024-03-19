#pragma once

#ifndef RENDERER_DEF_FWD_H
#define RENDERER_DEF_FWD_H

#include <sRAT-RT/renderer.h>

#define FRAMEBUFFER_TEX_NUM 3

class RendererDeferredAndForward : public Renderer
{
public:

    RendererDeferredAndForward(unsigned int fb_w, unsigned int fb_h);

    /// @brief This renderer will render the scene using deferred shading and another forward pass totalling in 3 passes,\n 
    ///         2 deferred (gbuffer creation + lighting) and the forward one
    /// @param scene 
    void render_scene(Scene* scene) const;
   
private:
    GLFrameBufferRGBA<FRAMEBUFFER_TEX_NUM>* m_deferred_framebuffer;
    Shader* m_deferred_lighting_pass_shader;
    unsigned int m_fullscreen_vao;

    void set_shader_camera_uniforms(Shader* shader, Camera* cam, int width, int height) const;
    void set_scene_lighting_uniforms(Shader* shader, Camera* cam) const;
    void render_quad() const;
    void init_fullscreen_quad();
};

#endif