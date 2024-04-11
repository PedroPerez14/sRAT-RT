#pragma once

#ifndef RENDERER_TEST_UPLIFTING_H
#define RENDERER_TEST_UPLIFTING_H

#include <sRAT-RT/settings.h>
#include <sRAT-RT/renderer.h>
#include <sRAT-RT/colorspace.h>
#include <rgb2spec/rgb2spec.h>

/// TODO: CHECK THIS (WILL CAUSE PROBLEMS LATER, 100% SURE)
#define FRAMEBUFFER_TEX_NUM 1

class RendererTestUplifting : public Renderer
{
public:

    RendererTestUplifting(Settings* settings, std::unordered_map<colorspace, RGB2Spec*>* look_up_tables, 
                std::unordered_map<std::string, ResponseCurve*>* response_curves, std::string version);

    /// @brief This renderer will render a texture in fullscreen, for some testing
    /// @param scene 
    void render_scene(Scene* scene);

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

    // Wavelength sampling stuff, it's very dirty to be honest but I can't come up with something better for now
    const enum WavelengthIntervalStrategy {STRAT_EQUISPACED, STRAT_ALT1, STRAT_ALT2, STRAT_COUNT};
    const std::string wl_interval_strat_names[STRAT_COUNT] = { "Equispaced", "ALT_1", "ALT_2" };
    float* (RendererTestUplifting::*wl_sampling_funcs[STRAT_COUNT])() = 
    { &RendererTestUplifting::sample_equispaced, &RendererTestUplifting::sample_alt1, &RendererTestUplifting::sample_alt2 };

    std::string app_version;
    GLFrameBufferRGBA<FRAMEBUFFER_TEX_NUM>* m_deferred_framebuffer;
    std::unordered_map<colorspace, lut_as_tex3d>* lut_textures;
    std::unordered_map<std::string, ResponseCurve*>* response_curves_render;
    Shader* m_uplifting_shader;
    Shader* m_final_pass_shader;
    colorspace working_colorspace;
    unsigned int m_fullscreen_vao;
    unsigned int tex_test;
    unsigned int sampled_wls_tex_id;
    bool do_spectral;                   // if false, we will just render to rgb
    bool resample_wls;                  // true if we need to resample the wavelengths that we'll use for our computation
    int num_wavelengths;
    float wl_min;
    float wl_max;
    int sampling_strat;
    std::vector<std::string> response_curve_names;
    int selected_resp_curve = 0;

    void set_shader_camera_uniforms(Shader* shader, Camera* cam, int width, int height) const;
    void set_scene_lighting_uniforms(Shader* shader, Camera* cam) const;
    void render_quad() const;
    void init_fullscreen_quad();
    unsigned int texture_from_file(const char* path, const std::string& directory);
    void lut_textures_create(std::unordered_map<colorspace, RGB2Spec*>* look_up_tables);
    void reload_shaders();
    void populate_resp_curves_list();
    void gen_sampled_wls_tex1d();

    float* sample_equispaced();
    float* sample_alt1();         // dummy, will change it in the future
    float* sample_alt2();         // dummy, will change it in the future
};

#endif