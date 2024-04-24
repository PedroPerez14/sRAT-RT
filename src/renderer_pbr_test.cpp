#include <sRAT-RT/renderer_pbr_test.h>

RendererPBRTest::RendererPBRTest(Settings* settings, std::unordered_map<colorspace, RGB2Spec*>* look_up_tables,
                    std::unordered_map<std::string, ResponseCurve*>* response_curves, std::string version)
{
    m_deferred_lighting_pass_shader = new Shader();
    m_postprocess_pass_shader = new Shader();       // or nullptr if we want to ignore this pass, it's optional
    m_app_version = version;
    m_deferred_framebuffer = new GLFrameBufferRGBA<FRAMEBUFFER_TEX_NUM>();  // check tex number for your machine
    unsigned int _width = settings->get_window_width();
    unsigned int _height = settings->get_window_width();
    /// TODO: INIT THE CAMERA ??????
    m_deferred_framebuffer->init(_width, _height);
    lut_textures_create(look_up_tables);            // Convert the LUTs from data to 3D textures
    m_response_curves_render = response_curves;
    /// TODO: Change the default selected curve to the CIE one ! (easier to debug this way)
    m_selected_resp_curve = 9;
    populate_resp_curves_list();                    // For UI purposes
    m_resample_wls = true;
    m_is_response_in_xyz = false;
    m_num_wavelengths = settings->get_num_wavelengths();
    m_wl_min = settings->get_wl_min();
    m_wl_max = settings->get_wl_max();
    check_wls_range();
    m_sampling_strat = (int)STRAT_EQUISPACED;
    glGenTextures(1, &m_sampled_wls_tex_id);        // Create and populate 1D tex with chosen WLs
    gen_sampled_wls_tex1d();
    init_fullscreen_quad();                         // Finally, generate our full screen quad
}

void RendererPBRTest::render_scene(Scene* scene)
{
    /// TODO: IMPLEMENTAR DE LO ÚLTIMO PARA ASEGURARME DE QUE HAGO BIEN LAS RENDER PASSES
}

void RendererPBRTest::render_ui()
{
    /// TODO: IMPLEMENTAR DE LO ÚLTIMO PARA ASEGURARME DE QUE HAGO BIEN LAS RENDER PASSES
}

void RendererPBRTest::handle_resize(int w, int h)
{
    m_deferred_framebuffer->resize(w, h);
}


///////// TODO: AQUI ME HE QUEDADO