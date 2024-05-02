#include <sRAT-RT/dir_light.h>
#include <sRAT-RT/renderer_pbr.h>

RendererPBR::RendererPBR(Settings* settings, std::unordered_map<colorspace, RGB2Spec*>* look_up_tables,
                    std::unordered_map<std::string, ResponseCurve*>* response_curves, std::string version)
{
    m_deferred_lighting_pass_shader = new Shader("../src/shaders/pbr_spectral/vertex_deferred_lighting.glsl", "../src/shaders/pbr_spectral/fragment_deferred_lighting.glsl");
    m_postprocess_pass_shader = new Shader("../src/shaders/pbr_spectral/vertex_postprocess.glsl", "../src/shaders/pbr_spectral/fragment_postprocess.glsl");
    m_app_version = version;
    working_colorspace = settings->get_colorspace();                        // Just use sRGB
    m_deferred_framebuffer = new GLFrameBufferRGBA<FRAMEBUFFER_TEX_NUM>();  // check tex number for your machine
    m_pprocess_framebuffer = new GLFrameBufferRGBA<FRAMEBUFFER_TEX_NUM>();                    // Only for the postprocessing pass
    unsigned int _width = settings->get_window_width();
    unsigned int _height = settings->get_window_height();
    m_deferred_framebuffer->init(_width, _height);
    m_pprocess_framebuffer->init(_width, _height);
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

void RendererPBR::render_scene(Scene* scene)
{
    // Update the reference to the last rendered scene
    m_last_rendered_scene = scene;
    if(m_resample_wls)
    {
        m_resample_wls = false;
        gen_sampled_wls_tex1d();
    }
    if(m_resize_flag)
    {
        /// TODO: do something here if applies?
        /// maybe change camera's aspect ratio by updating width and height?
        m_resize_flag = false;
    }

    GL_CHECK(glClearColor(0.0, 0.0, 0.0, 1.0));
    // Clear default framebuffer (the screen)
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    // Clear the framebuffer from the last frame
    m_deferred_framebuffer->bind();
    m_deferred_framebuffer->clear();
    
    // First rendering pass, the Deferred Geometry Pass: Fill the GBuffer with data
    deferred_geometry_pass(scene);      // Render into the framebuffer
    
    m_deferred_framebuffer->unbind();
    m_pprocess_framebuffer->bind();
    m_pprocess_framebuffer->clear();

    // Second pass, Deferred Shading: Use a giant shader to light all the scene at once
    deferred_lighting_pass(scene);          // Draw the scene at once to the postprocessing buffer

    /// TODO: DO I NEED TO BLIT THE FRAMEBUFFER IF I'M DOING EVERYTHING INSIDE THE SAME FRAMEBUFFER??
    //blit_depth_buffer();                  // In case we need a final forward pass

    // Third pass, optional: Forward rendering for some objects that may need it (i.e area lights)
    //forward_pass(scene);                  // Also draws to gbuffer (should be a 2nd FBO) [EMPTY FOR NOW]

    m_pprocess_framebuffer->unbind();       // The next pass will render directly into the screen buffer  

    // Fourth pass, optional: Post processing, visual effects, tonemapping and gamma correction
    post_processing_pass(scene);            // Takes the postprocessing buffer as input
}

void RendererPBR::render_ui()
{
    /// TODO: IMPLEMENTAR DE LO ÚLTIMO PARA ASEGURARME DE QUE HAGO BIEN LAS RENDER PASSES
}

void RendererPBR::handle_resize(int w, int h)
{
    m_deferred_framebuffer->resize(w, h);
    m_pprocess_framebuffer->resize(w, h);
    m_resize_flag = true;
}

// This should be called only once during initialization, before rendering the scene
void RendererPBR::init_fullscreen_quad()
{
    GL_CHECK(glGenVertexArrays(1, &m_fullscreen_vao));
    GL_CHECK(glBindVertexArray(m_fullscreen_vao));
    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL));
}

void RendererPBR::render_quad()
{
    glDisable(GL_DEPTH_TEST);                   // disable depth test so screen-space quad isn't discarded   
    glBindVertexArray(m_fullscreen_vao);
    GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 3)); // Vertices will be created in vertex shader
    glEnable(GL_DEPTH_TEST);                    // re-enable depth test
    glBindVertexArray(0);
}

void RendererPBR::lut_textures_create(std::unordered_map<colorspace, RGB2Spec*>* look_up_tables)
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
void RendererPBR::reload_shaders()
{
    
    std::vector<RenderableObject*> _ros_scene = m_last_rendered_scene->get_renderables();
    int num_renderables = _ros_scene.size();
    int num_reloaded = 0;
    for (RenderableObject* ro : _ros_scene)
    {
        if(ro->get_material()->reload_shader())
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

    old_shader = m_postprocess_pass_shader;
    new_shader = new Shader(m_postprocess_pass_shader->m_vertexPath, m_postprocess_pass_shader->m_fragmentPath, success);
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

void RendererPBR::populate_resp_curves_list()
{
    m_response_curve_names.empty();
    for (auto i = m_response_curves_render->begin(); i != m_response_curves_render->end(); i++)
    {
        m_response_curve_names.push_back(i->first);
    }
}

void RendererPBR::gen_sampled_wls_tex1d()
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

void RendererPBR::check_wls_range()
{
    float wl_min_rc = m_response_curves_render->at(m_response_curve_names.at(m_selected_resp_curve))->get_wl_min();
    float wl_max_rc = m_response_curves_render->at(m_response_curve_names.at(m_selected_resp_curve))->get_wl_max();
    if(m_wl_max > wl_max_rc || m_wl_max < wl_min_rc)
                m_wl_max = wl_max_rc;
            if(m_wl_min < wl_min_rc || m_wl_min > wl_max_rc)
                m_wl_min = wl_min_rc;
}

colorspace RendererPBR::get_colorspace() const
{
    return working_colorspace;
}

void RendererPBR::set_colorspace(colorspace _c)
{
    working_colorspace = _c;
}

float* RendererPBR::sample_equispaced()
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
float* RendererPBR::sample_alt1()
{
    float* wavelengths = new float[m_num_wavelengths];
    for(int i = 0; i < m_num_wavelengths; i++)
    {
        wavelengths[i] = m_wl_max;
    }
    return wavelengths;
}

/// dummy function, change at some point
float* RendererPBR::sample_alt2()
{
    float* wavelengths = new float[m_num_wavelengths];
    for(int i = 0; i < m_num_wavelengths; i++)
    {
        wavelengths[i] = m_wl_min;
    }
    return wavelengths;
}

void RendererPBR::deferred_geometry_pass(Scene* scene)
{
    // get a pointer to the camera (projection matrix purposes)
    Camera* camera = scene->get_camera();
    // for every renderable in the scene, if it has to be rendered in this pass,
    //  then set its uniforms correctly and render it (put it in the gbuffer)
    std::vector<RenderableObject*> renderables = scene->get_renderables();
    for(RenderableObject* ro : renderables)
    {
        Material* mat = ro->get_material();
        if(mat->get_pass() == DEFERRED_GEOMETRY)
        {
            // set uniforms (incl. Model, View, Projection matrices)
            // call its draw() method
            glm::mat4 model = ro->get_transform()->get_model();
            glm::mat4 view = camera->get_view_matrix();                 // from learnOpenGL, I _should_ redo the camera with Transforms
            glm::mat4 projection = camera->get_projection_matrix();     // needs testing for different resolutions (cam's w and h)
            ro->draw(mat->get_shader(), model, view, projection);       // The shader will fill the gbuffer accordingly
        }
    }
}

void RendererPBR::deferred_lighting_pass(Scene* scene)
{
    //std::cout << "DEFERRED LIGHTING PASS !" << std::endl;
    /// INFO: Bindings for the deferred lighting shader:
    //
    // The LUTs will get the first 3 bindings, 0, 1 and 2.
    // The chosen wavelengths texture will get the 4th (3)
    // And then, the response curve (5th, 4)
    //
    // After that, the Framebuffer textures will be allocated:
    // Position (with material id on its .a component) --> 6th, 5
    // Normals (w/ Metallic coeff on its .a component only for PBRMat) --> 7th, 6
    // Albedo (w/ Roughness coeff on its .a component only for PBRMat) --> 8th, 7
    // Any material specific textures will follow from the 9th (8) place onwards
    m_deferred_lighting_pass_shader->use();
    unsigned int lut_3d_ids[3];
    memcpy(lut_3d_ids, m_lut_textures->at(get_colorspace()).tex_ids, 3 * sizeof(unsigned int));
    for(int i = 0; i < 3; i++)
    {
        // LUTs , 0..2
        GL_CHECK(glActiveTexture(GL_TEXTURE0 + i));
        GL_CHECK(glBindTexture(GL_TEXTURE_3D, lut_3d_ids[i]));
    }

    //Sampled wavelengths, 3
    GL_CHECK(glActiveTexture(GL_TEXTURE3));
    GL_CHECK(glBindTexture(GL_TEXTURE_1D, m_sampled_wls_tex_id));

    // Response curve, 4
    unsigned int r_crv_tex_id = m_response_curves_render->at(m_response_curve_names.at(m_selected_resp_curve))->get_tex_id();
    GL_CHECK(glActiveTexture(GL_TEXTURE4));
    GL_CHECK(glBindTexture(GL_TEXTURE_1D, r_crv_tex_id));

    // Scene lights spectral response, 5
    GL_CHECK(glActiveTexture(GL_TEXTURE5));
    GL_CHECK(glBindTexture(GL_TEXTURE_1D_ARRAY, scene->get_emission_tex_array_id()));

    // Framebuffer contents, 6..15 if additional textures are required by the material
    for(int i = 0; i < FRAMEBUFFER_TEX_NUM; i++)
    {
        GL_CHECK(glActiveTexture(GL_TEXTURE6 + i));
        glBindTexture(GL_TEXTURE_2D, m_deferred_framebuffer->getTextureID(i));
    }
    
    /// Now, set the uniforms:
    set_deferred_lighting_shader_uniforms(scene);

    // Finally, render the fullscreen quad
    render_quad();
}

// I just realized this could be the same method as deferred_geometry_pass 
//      but just adding one more parameter for the pass we want to filter by
void RendererPBR::forward_pass(Scene* scene)
{
    Camera* camera = scene->get_camera();
    // Draw directly to screen (after blit-ing the framebuffer)
    std::vector<RenderableObject*> renderables = scene->get_renderables();
    for(RenderableObject* ro : renderables)
    {
        Material* mat = ro->get_material();
        if(mat->get_pass() == FORWARD_PASS)
        {
            glm::mat4 model = ro->get_transform()->get_model();
            glm::mat4 view = camera->get_view_matrix();
            glm::mat4 projection = camera->get_projection_matrix();
            ro->draw(mat->get_shader(), model, view, projection);
        }
    }
}

void RendererPBR::post_processing_pass(Scene* scene)
{
    // Bind the output texture from our framebuffer
    //  and then render to screen (quad rendering)
    //  after doing the postprocessing (tonemapping, etc.)

    m_postprocess_pass_shader->use();               // Use the shader
    m_postprocess_pass_shader->setBool("do_spectral_uplifting", m_do_spectral);

    GL_CHECK(glActiveTexture(GL_TEXTURE0));         // Bind the texture
    glBindTexture(GL_TEXTURE_2D, m_pprocess_framebuffer->getTextureID(0));

    render_quad();                                  // Render (to screen)
}


/// TODO: Not default framebuffer, but the postprocessing framebuffer!
void RendererPBR::blit_depth_buffer()
{
    m_deferred_framebuffer->bindForReading();
    unsigned int _w = m_deferred_framebuffer->getWidth();
    unsigned int _h = m_deferred_framebuffer->getHeight();
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
    m_pprocess_framebuffer->bind();
    glBlitFramebuffer(
    0, 0, _w, _h, 0, 0, _w, _h, GL_DEPTH_BUFFER_BIT, GL_NEAREST
    );
    glBindFramebuffer(GL_FRAMEBUFFER, 0);       
}

void RendererPBR::set_deferred_lighting_shader_uniforms(Scene* scene)
{
    m_deferred_lighting_pass_shader->use();
    ResponseCurve* rc = m_response_curves_render->at(m_response_curve_names.at(m_selected_resp_curve));
    m_deferred_lighting_pass_shader->setBool("do_spectral_uplifting", m_do_spectral);
    m_deferred_lighting_pass_shader->setBool("convert_xyz_to_rgb", m_is_response_in_xyz);
    m_deferred_lighting_pass_shader->setInt("n_wls", m_num_wavelengths);
    m_deferred_lighting_pass_shader->setFloat("wl_min", m_wl_min);
    m_deferred_lighting_pass_shader->setFloat("wl_max", m_wl_max);
    m_deferred_lighting_pass_shader->setFloat("wl_min_resp", rc->get_wl_min());
    m_deferred_lighting_pass_shader->setFloat("wl_max_resp", rc->get_wl_max());
    m_deferred_lighting_pass_shader->setInt("res", 64);         // Don't touch, LUT related
    m_deferred_lighting_pass_shader->setInt("num_lights", scene->get_num_lights());
    m_deferred_lighting_pass_shader->setVec3("cam_pos", scene->get_camera()->Position);

    // now, set the lights in the scene
    std::vector<Light*> scene_lights = scene->get_lights();
    int i = 0;
    for(Light* light : scene_lights)
    {
        // Don't look at this code, it's embarrasing 
        // (I should consider 2 separate vectors, one for dir lights and another for point lights)
        if(light->get_light_type() == DIR_LIGHT)
        {
            DirLight* dl = dynamic_cast<DirLight*>(light);
            m_deferred_lighting_pass_shader->setVec4("scene_lights[" + std::to_string(i) + "].position", glm::vec4(dl->get_transform()->get_pos(), 0));
            m_deferred_lighting_pass_shader->setVec3("scene_lights[" + std::to_string(i) + "].direction", dl->get_dir());
            m_deferred_lighting_pass_shader->setVec3("scene_lights[" + std::to_string(i) + "].attenuation", glm::vec3(0.0f, 0.0f, 0.0f));
            m_deferred_lighting_pass_shader->setVec3("scene_lights[" + std::to_string(i) + "].emission_rgb", dl->get_spectrum()->get_responses_rgb());
            m_deferred_lighting_pass_shader->setFloat("scene_lights[" + std::to_string(i) + "].emission_mult", dl->get_power_multiplier());
            m_deferred_lighting_pass_shader->setFloat("scene_lights[" + std::to_string(i) + "].wl_min", dl->get_spectrum()->get_wl_min());
            m_deferred_lighting_pass_shader->setFloat("scene_lights[" + std::to_string(i) + "].wl_max", dl->get_spectrum()->get_wl_max());
        }
        else // if(POINT_LIGHT)
        {
            /// TODO: Implement Point Lights, do after the first few tests
            // PointLight* pl = dynamic_cast<PointLight*>(&light);
            // m_deferred_lighting_pass_shader->setVec4("scene_lights[" + std::to_string(i) + "].position", glm::vec4(pl->get_transform().get_pos(), 1));
            // m_deferred_lighting_pass_shader->setVec3("scene_lights[" + std::to_string(i) + "].direction", glm::vec3(0.0f,0.0f,0.0f));
            // m_deferred_lighting_pass_shader->setVec3("scene_lights[" + std::to_string(i) + "].attenuation", pl->get_attenuation());
            // m_deferred_lighting_pass_shader->setVec3("scene_lights[" + std::to_string(i) + "].emission_rgb", pl->get_spectrum()->get_responses_rgb());
            // m_deferred_lighting_pass_shader->setFloat("scene_lights[" + std::to_string(i) + "].emission_mult", pl->get_power_multiplier());
            // m_deferred_lighting_pass_shader->setFloat("scene_lights[" + std::to_string(i) + "].wl_min", pl->get_spectrum()->get_wl_min());
            // m_deferred_lighting_pass_shader->setFloat("scene_lights[" + std::to_string(i) + "].wl_max", pl->get_spectrum()->get_wl_max());
        }
        i++;
    }
}
