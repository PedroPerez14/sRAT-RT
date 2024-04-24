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
    m_deferred_framebuffer->init(_width, _height);
    lut_textures_create(look_up_tables);            // Convert the LUTs from data to 3D textures
    m_response_curves_render = response_curves;
    m_selected_resp_curve = 0;                      // Would be much better if the def. curve was the CIE one
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
    // Update the reference to the last rendered scene
    m_last_rendered_scene = scene;
    if(m_resample_wls)
    {
        m_resample_wls = false;
        gen_sampled_wls_tex1d();
    }
    // Get a reference to the camera (to get the necessary data as the projection matrix)
    Camera* cam = scene->get_camera();

    // Clear the framebuffer from the last frame
    m_deferred_framebuffer->bind();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    m_deferred_framebuffer->clear();
    // Clear default framebuffer (the screen)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // First rendering pass, the Deferred Geometry Pass: Fill the GBuffer with data
    deferred_geometry_pass(scene);   // bind any necessary textures, uniforms and luts

    // Second pass, Deferred Shading: Use a giant shader to light all the scene at once
    deferred_lighting_pass(scene);   // bind the response curves, etc. + draw_quad()

    // Blit the depth buffer before the deferred and the forward draw passes
    blit_depth_buffer();

    // Third pass, optional: Forward rendering for some objects that may need it (i.e area lights)
    forward_pass(scene);             // for every RO in scene with pass = fwd, set_uniforms() + draw(ro.mat->shader)

    // Fourth pass, optional: Post processing, visual effects, tonemapping and gamma correction
    post_processing_pass(scene);     // what if we don't define anything for this pass??? HMMMmm
}

void RendererPBRTest::render_ui()
{
    /// TODO: IMPLEMENTAR DE LO ÚLTIMO PARA ASEGURARME DE QUE HAGO BIEN LAS RENDER PASSES
}

void RendererPBRTest::handle_resize(int w, int h)
{
    m_deferred_framebuffer->resize(w, h);
}

// This should be called only once during initialization, before rendering the scene
void RendererPBRTest::init_fullscreen_quad()
{
    GL_CHECK(glGenVertexArrays(1, &m_fullscreen_vao));
    GL_CHECK(glBindVertexArray(m_fullscreen_vao));
    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL));
}

void RendererPBRTest::render_quad()
{
    glDisable(GL_DEPTH_TEST);                   // disable depth test so screen-space quad isn't discarded   
    glBindVertexArray(m_fullscreen_vao);
    GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 3)); // Vertices will be created in vertex shader
    glEnable(GL_DEPTH_TEST);                    // re-enable depth test
    glBindVertexArray(0);
}

void RendererPBRTest::lut_textures_create(std::unordered_map<colorspace, RGB2Spec*>* look_up_tables)
{
    m_lut_textures = new std::unordered_map<colorspace, lut_as_tex3d>();

    for(auto colorspace_key : colorspace_translations)
    {
        colorspace csp = colorspace_translations_inv.at(colorspace_key);
        RGB2Spec* rgb2spec = look_up_tables->at(csp);

        // Generate 3 3D textures with opengl and store their ids in an unsigned int array with length 3
        unsigned int textures_3d[3];
        float* buffer_tex;

        for(int i = 0; i < 3; i++)
        {
            glGenTextures(1, &textures_3d[i]);
            glBindTexture(GL_TEXTURE_3D, textures_3d[i]);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            // enable triliear interpolation
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

            // Populate the data buffer from which we will create the texture
            int _res = rgb2spec->res;
            buffer_tex = new float[3*_res*_res*_res];
            size_t offset = i * 3 * _res * _res * _res;
            std::memcpy(buffer_tex, &(rgb2spec->data[offset]), 3*_res*_res*_res * sizeof(float));
            glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB32F, _res, _res, _res,
                        0, GL_RGB, GL_FLOAT, buffer_tex);
        }
        // Free the buffer
        delete buffer_tex;
        // Assign the texture IDs to the map's corresponding element
        lut_as_tex3d lut_tex_3d = lut_as_tex3d();

        memcpy(lut_tex_3d.tex_ids, textures_3d, 3 * sizeof(unsigned int));
        lut_tex_3d.res = (int)rgb2spec->res;

        (*m_lut_textures)[csp] = lut_tex_3d;

        std::cout << "APP::STATUS::INIT::RENDERER::GEN_LUTs_AS_TEX3D: Copied LUT for color space " 
        << colorspace_key << " with size " << m_lut_textures->at(csp).res << "x" << m_lut_textures->at(csp).res << "x" << m_lut_textures->at(csp).res
        << "\n Tex_IDS: " << m_lut_textures->at(csp).tex_ids[0] << ", " << m_lut_textures->at(csp).tex_ids[1] << ", " << m_lut_textures->at(csp).tex_ids[2]
        << std::endl;
    }
}

/// TODO: Should I make this return a bool or have 2 ref. bools 
///     for the scene object shaders and the deferred pass one?
void RendererPBRTest::reload_shaders()
{
    
    std::vector<RenderableObject> _ros_scene = m_last_rendered_scene->get_renderables();
    int num_renderables = _ros_scene.size();
    int num_reloaded = 0;
    for (auto ro : _ros_scene)
    {
        if(ro.get_material()->reload_shader())
            num_reloaded++;
    }

    bool success = true;    // can be initialized with any value, the constructor will corerctly set it
    Shader* old_shader = m_deferred_lighting_pass_shader;
    Shader* new_shader = new Shader(old_shader->m_vertexPath, old_shader->m_fragmentPath, success);
    if(success)
    {
        m_deferred_lighting_pass_shader = new_shader;
        delete old_shader;
        std::cout << "APP::STATUS::RUNNING::RENDERER::INFO: Correctly reloaded the deferred lighting pass shader!" << std::endl; 
    }
    else
    {
        std::cout << "APP::STATUS::RUNNING::RENDERER::INFO: Could not reload the deferred lighting pass shader!" << std::endl; 
    }

    Shader* old_shader = m_postprocess_pass_shader;
    Shader* new_shader = new Shader(m_postprocess_pass_shader->m_vertexPath, m_postprocess_pass_shader->m_fragmentPath, success);
    if(success)
    {
        m_postprocess_pass_shader = new_shader;
        delete old_shader;
        std::cout << "APP::STATUS::RUNNING::RENDERER::INFO: Correctly reloaded the post-processing pass shader!" << std::endl; 
    }
    else
    {
        std::cout << "APP::STATUS::RUNNING::RENDERER::INFO: Could not reload the post-processing pass shader!" << std::endl; 
    }

    std::cout << "APP::STATUS::RUNNING::RENDERER::INFO: Correctly reloaded " << num_reloaded 
    << " / " << num_renderables << " material shaders for renderable objects in the scene!" << std::endl;
}

void RendererPBRTest::populate_resp_curves_list()
{
    m_response_curve_names.empty();
    for (auto i = m_response_curves_render->begin(); i != m_response_curves_render->end(); i++)
    {
        m_response_curve_names.push_back(i->first);
    }
}

void RendererPBRTest::gen_sampled_wls_tex1d()
{
    float* wavelengths = std::invoke(m_wl_sampling_funcs[m_sampling_strat], this);

    for(int i = 0; i < m_num_wavelengths; i++)
    {
        std::cout << "WAVELENGTHS[" << i << "]: " << wavelengths[i] << std::endl;
    }

    glBindTexture(GL_TEXTURE_1D, m_sampled_wls_tex_id);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, m_num_wavelengths, 0, GL_RED, GL_FLOAT, wavelengths);
    delete wavelengths;
}

void RendererPBRTest::check_wls_range()
{
    float wl_min_rc = m_response_curves_render->at(m_response_curve_names.at(m_selected_resp_curve))->get_wl_min();
    float wl_max_rc = m_response_curves_render->at(m_response_curve_names.at(m_selected_resp_curve))->get_wl_max();
    if(m_wl_max > wl_max_rc || m_wl_max < wl_min_rc)
                m_wl_max = wl_max_rc;
            if(m_wl_min < wl_min_rc || m_wl_min > wl_max_rc)
                m_wl_min = wl_min_rc;
}

float* RendererPBRTest::sample_equispaced()
{
    float* wavelengths = new float[m_num_wavelengths];
    float delta = (m_wl_max - m_wl_min) / (float)m_num_wavelengths;
    for(int i = 0; i < m_num_wavelengths; i++)
    {
        // Riemann middle sum
        wavelengths[i] = m_wl_min + (i * delta) + (delta / 2.0f);
    }
    return wavelengths;
}

/// dummy function, change at some point
float* RendererPBRTest::sample_alt1()
{
    float* wavelengths = new float[m_num_wavelengths];
    for(int i = 0; i < m_num_wavelengths; i++)
    {
        wavelengths[i] = m_wl_max;
    }
    return wavelengths;
}

/// dummy function, change at some point
float* RendererPBRTest::sample_alt2()
{
    float* wavelengths = new float[m_num_wavelengths];
    for(int i = 0; i < m_num_wavelengths; i++)
    {
        wavelengths[i] = m_wl_min;
    }
    return wavelengths;
}

void RendererPBRTest::deferred_geometry_pass(Scene* scene)
{
    // for every renderable in the scene, if it has to be rendered in this pass,
    //  then set its uniforms correctly and render it (put it in the gbuffer)
    std::vector<RenderableObject> renderables;
    for(auto ro : renderables)
    {
        if(ro.get_material()->get_pass() == DEFERRED_GEOMETRY)
        {
            // set uniforms (incl. Model, View, Projection matrices)
            // call its draw() method
        }
    }
}

void RendererPBRTest::deferred_lighting_pass(Scene* scene)
{
    // set uniforms for the spectral uplifting
    // for(wl : wls) { blablabla; Riemann_sum(); apply_curve() }
}

void RendererPBRTest::forward_pass(Scene* scene)
{
    // Draw directly to screen (after blit-ing the framebuffer)
}

void RendererPBRTest::post_processing_pass(Scene* scene)
{
    // Call the global shader (draw_quad?)
    // I need to take as entry the screen buffer 
    // (if the other 2 previous passes draw directly to screen)
}

void RendererPBRTest::blit_depth_buffer()
{
    // just a copypaste I guess?
}