#pragma once

#ifndef RENDERER_TEST_UPLIFTING_H
#define RENDERER_TEST_UPLIFTING_H

#include <sRAT-RT/renderer.h>

#define NUM_WAVELENGTHS 40
#define FRAMEBUFFER_TEX_NUM (NUM_WAVELENGTHS / 4 + 1)

class RendererTestUplifting : public Renderer
{
public:

    RendererTestUplifting(unsigned int fb_w, unsigned int fb_h);

    /// @brief This renderer will render a texture in fullscreen, for some testing
    /// @param scene 
    void render_scene(Scene* scene) const;
   
private:
    GLFrameBufferRGBA<FRAMEBUFFER_TEX_NUM>* m_deferred_framebuffer;
    Shader* m_deferred_lighting_pass_shader;
    unsigned int m_fullscreen_vao;
    unsigned int tex_test;
    unsigned int tex_lut;

    void set_shader_camera_uniforms(Shader* shader, Camera* cam, int width, int height) const;
    void set_scene_lighting_uniforms(Shader* shader, Camera* cam) const;
    void render_quad() const;
    void init_fullscreen_quad();
    unsigned int texture_from_file(const char* path, const std::string& directory);
    unsigned int lut_tex_create(/* TODO: SOMETHING GOES HERE */);
    
};

#endif