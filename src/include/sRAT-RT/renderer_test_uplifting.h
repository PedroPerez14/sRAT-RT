#pragma once

#ifndef RENDERER_TEST_UPLIFTING_H
#define RENDERER_TEST_UPLIFTING_H

#include <sRAT-RT/renderer.h>
#include <sRAT-RT/colorspace.h>
#include <rgb2spec/rgb2spec.h>

#define NUM_WAVELENGTHS 12
#define FRAMEBUFFER_TEX_NUM (NUM_WAVELENGTHS / 4 + 1)

class RendererTestUplifting : public Renderer
{
public:

    RendererTestUplifting(unsigned int fb_w, unsigned int fb_h, std::unordered_map<colorspace, RGB2Spec*>* look_up_tables, 
                            std::unordered_map<std::string, ResponseCurve*>* response_curves, colorspace _colorspace=SRGB, int n_wls=40, float wl_min=360.0, float wl_max=830.0);

    /// @brief This renderer will render a texture in fullscreen, for some testing
    /// @param scene 
    void render_scene(Scene* scene) const;

    void render_ui();

    void handle_resize(int w, int h);

    colorspace get_colorspace() const;
    void set_colorspace(colorspace _c);

private:

    struct lut_as_tex3d
    {
        unsigned int tex_ids[3];        // LUT as opengl texture 3d, size NxNxN
        int res;                        // N
    };

    /// TODO: Define more than 1 strategy for sampling wavelengths!
    const enum WavelengthIntervalStrategy {STRAT_EQUISPACED, STRAT_ALT1, STRAT_ALT2, STRAT_COUNT};
    const std::string wl_interval_strat_names[STRAT_COUNT] = { "Equispaced", "ALT_1", "ALT_2" };

    GLFrameBufferRGBA<FRAMEBUFFER_TEX_NUM>* m_deferred_framebuffer;
    std::unordered_map<colorspace, lut_as_tex3d>* lut_textures;
    std::unordered_map<std::string, ResponseCurve*>* response_curves_render;
    Shader* m_uplifting_shader;
    Shader* m_final_pass_shader;
    colorspace working_colorspace;
    unsigned int m_fullscreen_vao;
    unsigned int tex_test;
    unsigned int tex_lut;
    bool do_spectral;           // if false, we will just render to rgb
    int num_wavelengths;
    float wl_min;
    float wl_max;
    int sampling_strat;

    void set_shader_camera_uniforms(Shader* shader, Camera* cam, int width, int height) const;
    void set_scene_lighting_uniforms(Shader* shader, Camera* cam) const;
    void render_quad() const;
    void init_fullscreen_quad();
    unsigned int texture_from_file(const char* path, const std::string& directory);
    void lut_textures_create(std::unordered_map<colorspace, RGB2Spec*>* look_up_tables);
    void reload_shaders();
};

#endif