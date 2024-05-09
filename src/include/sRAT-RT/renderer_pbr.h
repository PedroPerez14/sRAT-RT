#pragma once
#ifndef _RENDERER_PBR_CLASS_H_
#define _RENDERER_PBR_CLASS_H_

#include <sRAT-RT/app.h>
#include <sRAT-RT/settings.h>
#include <sRAT-RT/renderer.h>
#include <rgb2spec/rgb2spec.h>
#include <sRAT-RT/colorspace.h>
#include <sRAT-RT/response_curve.h>

#define FRAMEBUFFER_TEX_NUM 8       // max for my testing device, should I change this???

class RendererPBR : public Renderer
{
public:
    RendererPBR(App* app);

    void render_scene(Scene* scene);
    void render_ui();
    void handle_resize(int w, int h);

private:
    //// DEFINITIONS ////
    struct lut_as_tex3d
    {
        unsigned int tex_ids[3];        // LUT as opengl texture 3d, size NxNxN
        int res;                        // N
    };
    const enum WavelengthIntervalStrategy {STRAT_EQUISPACED, STRAT_ALT1, STRAT_ALT2, STRAT_COUNT};
    const std::string m_wl_interval_strat_names[STRAT_COUNT] = { "Equispaced", "MAX_WL", "MIN_WL" };
    float* (RendererPBR::*m_wl_sampling_funcs[STRAT_COUNT])() = 
    { &RendererPBR::sample_equispaced, &RendererPBR::sample_alt1, &RendererPBR::sample_alt2 };

    //// ATTRIBUTES ////
    App* m_app_ptr;                             // Reference to the app, in case we need info like the current deltatiem
    std::string m_app_version;
    Scene* m_last_rendered_scene;               // The last scene we were asked to render
    Shader* m_deferred_lighting_pass_shader;    // draw the screen based on the contents of the framebuffer
    Shader* m_postprocess_pass_shader;          // if null there is no postprocess (do gamma correction here??)
    colorspace working_colorspace;
    GLFrameBufferRGBA<FRAMEBUFFER_TEX_NUM>* m_deferred_framebuffer;
    GLFrameBufferRGBA<FRAMEBUFFER_TEX_NUM>* m_pprocess_framebuffer;
    std::unordered_map<colorspace, lut_as_tex3d>* m_lut_textures;
    std::unordered_map<std::string, ResponseCurve*>* m_response_curves_render;
    std::vector<std::string> m_response_curve_names;
    unsigned int m_fullscreen_vao;
    unsigned int m_sampled_wls_tex_id;
    bool m_do_spectral;                 // if false, we will just render to rgb
    bool m_enable_fog;                  // quick zbuffer fog rendered on top of everything else
    bool m_resample_wls;                // true if we need to resample the wavelengths that we'll use for our computation
    bool m_is_response_in_xyz;          // if true, the response curve will be treated as in xyz space, otherwise, rgb
    bool m_resize_flag;                 // true if we resized the window in the last frame, to know if we need to resize the camera
    int m_sampling_strat;
    int m_num_wavelengths;
    int m_selected_resp_curve = 0;
    float m_wl_min;
    float m_wl_max;

    float m_illumination_multiplier;    // Debug variable for the UI
    float m_fog_sigma_a_mult;           // Debug variable for the UI
    float m_fog_sigma_s_mult;           // Debug variable for the UI
    
    //// METHODS & FUNCS ////
    
    /// TODO: Add more methods here !!!!
    void init_fullscreen_quad();
    void render_quad();
    //    unsigned int texture_from_file(const char* path, const std::string& directory);
    void lut_textures_create(std::unordered_map<colorspace, RGB2Spec*>* look_up_tables);
    void reload_shaders();
    void populate_resp_curves_list();
    void gen_sampled_wls_tex1d();
    void check_wls_range(Scene* scene);

    colorspace get_colorspace() const;
    void set_colorspace(colorspace _c);

    float* sample_equispaced();
    float* sample_alt1();
    float* sample_alt2();

    /// TODO: To be implemented (most likely tomorrow)
    void deferred_geometry_pass(Scene* scene);
    void deferred_lighting_pass(Scene* scene);
    void forward_pass(Scene* scene);
    void post_processing_pass(Scene* scene);
    void blit_depth_buffer(GLFrameBufferRGBA<FRAMEBUFFER_TEX_NUM>* orig, 
                            GLFrameBufferRGBA<FRAMEBUFFER_TEX_NUM>* dest);
    void set_deferred_lighting_shader_uniforms(Scene* scene);

    // UI stuff
    bool Combo(const char* label, int* current_item, const std::vector<std::string>& items, int items_count, int height_in_items);
    bool SliderFloatWithSteps(const char* label, int* v, float v_min, float v_max, float v_step, const char* display_format);
    void main_menu_bar();           // test (for now, it'll stay most likely)
    std::string get_fps_text();     // same
};

#endif