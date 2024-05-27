#pragma once
#include <iomanip>
#include <imgui.cpp>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <sRAT-RT/dir_light.h>
#include <sRAT-RT/point_light.h>
#include <sRAT-RT/renderer_pbr.h>


RendererPBR::RendererPBR(App* app)
{
    m_app_ptr = app;
    Settings* settings = m_app_ptr->get_settings();

    m_deferred_lighting_pass_shader = new Shader("../src/shaders/pbr_spectral/vertex_deferred_lighting.glsl", "../src/shaders/pbr_spectral/fragment_deferred_lighting.glsl");
    m_postprocess_pass_shader = new Shader("../src/shaders/pbr_spectral/vertex_postprocess.glsl", "../src/shaders/pbr_spectral/fragment_postprocess.glsl");
    m_app_version = m_app_ptr->get_app_version();
    working_colorspace = settings->get_colorspace();                        // just use sRGB pls
    m_deferred_framebuffer = new GLFrameBufferRGBA<FRAMEBUFFER_TEX_NUM>();  // check tex number for your machine
    m_pprocess_framebuffer = new GLFrameBufferRGBA<FRAMEBUFFER_TEX_NUM>();  // For the postprocessing stage
    unsigned int _width = settings->get_window_width();
    unsigned int _height = settings->get_window_height();
    m_deferred_framebuffer->init(_width, _height);
    m_pprocess_framebuffer->init(_width, _height);
    lut_textures_create(m_app_ptr->get_look_up_tables());                         // Convert the LUTs from data to 3D textures
    m_response_curves_render = m_app_ptr->get_response_curves();
    m_selected_resp_curve = 0;                                              // Would be much better if the def. curve was the CIE one
    populate_resp_curves_list();                                            // For UI purposes
    m_resample_wls = true;
    m_is_response_in_xyz = false;
    m_do_spectral = true;
    m_enable_fog = false;
    m_num_wavelengths = settings->get_num_wavelengths();
    m_wl_min = settings->get_wl_min();
    m_wl_max = settings->get_wl_max();
    m_sampling_strat = (int)STRAT_EQUISPACED;
    glGenTextures(1, &m_sampled_wls_tex_id);        // Create and populate 1D tex with chosen WLs
    gen_sampled_wls_tex1d();
    init_fullscreen_quad();                         // Finally, generate our full screen quad

    m_illumination_multiplier = 1.0f;
    m_fog_sigma_a_mult = 1.0f;
    m_fog_sigma_s_mult = 1.0f;
    jerlov_KD_mult = 1.0f;

    water_depth = 5.0f;
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
        std::cout << "RESIZE CAMERA! " << std::endl;
        scene->get_camera()->cam_height = m_deferred_framebuffer->getHeight();
        scene->get_camera()->cam_width = m_deferred_framebuffer->getWidth();
        scene->get_camera()->updateCameraVectors();
        m_resize_flag = false;
    }
    check_wls_range(scene);

    GL_CHECK(glClearColor(0.0, 0.0, 0.0, 1.0));
    // Clear default framebuffer (the screen)
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    // Clear the framebuffers from the last frame
    m_pprocess_framebuffer->bind();
    m_pprocess_framebuffer->clear();
    m_deferred_framebuffer->bind();
    m_deferred_framebuffer->clear();
    
    // First rendering pass, the Deferred Geometry Pass: Fill the GBuffer with data
    deferred_geometry_pass(scene);          // Render into the framebuffer
    
    m_deferred_framebuffer->unbind();
    m_pprocess_framebuffer->bind();

    // Second pass, Deferred Shading: Use a giant shader to light all the scene at once
    deferred_lighting_pass(scene);          // Draw the scene at once to the postprocessing buffer

    //blit_depth_buffer(m_deferred_framebuffer, nullptr); // if nullptr in dest, it's the default FB

    // Third pass, optional: Forward rendering for some objects that may need it (i.e area lights)
    //forward_pass(scene);                  // Also draws to gbuffer (should be a 2nd FBO) [EMPTY FOR NOW]

    m_pprocess_framebuffer->unbind();       // The next pass will render directly into the screen buffer  

    // Fourth pass, optional: Post processing, visual effects, tonemapping and gamma correction
    post_processing_pass(scene);            // Takes the postprocessing buffer as input
}

bool RendererPBR::Combo(const char* label, int* current_item, const std::vector<std::string>& items, int items_count, int height_in_items = -1)
{
   return ImGui::Combo(label, current_item, [](void* data, int idx, const char** out_text) { *out_text = ((const std::vector<std::string>*)data)->at(idx).c_str(); return true; }, (void*)&items, items_count, height_in_items);
}

bool RendererPBR::SliderFloatWithSteps(const char* label, int* v, float v_min, float v_max, float v_step, const char* display_format)
{
	if (!display_format)
		display_format = "%d";

	char text_buf[64] = {};
    
	ImFormatString(text_buf, IM_ARRAYSIZE(text_buf), display_format, *v);

	// Map from [v_min,v_max] to [0,N]
	const int countValues = int((v_max-v_min)/v_step);
	int v_i = int((*v - v_min)/v_step);
	const bool value_changed = ImGui::SliderInt(label, &v_i, 0, countValues, text_buf);

	// Remap from [0,N] to [v_min,v_max]
	*v = v_min + float(v_i) * v_step;
	return value_changed;
}

std::string RendererPBR::get_fps_text()
{
    float frametime = m_app_ptr->get_deltatime();
    float fps = 1.0f / frametime;
    std::stringstream stream;
    std::stringstream stream2;
    stream << std::fixed << std::setprecision(8) << (frametime * 1000.0f);
    std::string _frametime = stream.str();
    stream2 << std::fixed << std::setprecision(2) << fps;
    std::string _fps = stream2.str();
    return std::string("FPS: " + _fps + " (" + _frametime + " ms)");
}

void RendererPBR::main_menu_bar()
{
    if (ImGui::BeginMainMenuBar())
    {
        ImGui::TextUnformatted(get_fps_text().c_str());

        if (ImGui::BeginMenu("File (non functional)"))
        {
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
}

void RendererPBR::render_ui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //ImGui::SliderInt("Number of Wavelengths: ", &num_wavelengths, 4, 40, "%d");
    static bool showDemo = false;

    main_menu_bar();


    ImGui::Begin("Config");
    ImGui::TextUnformatted(("sRAT-RT v" + m_app_version).c_str());
    ImGui::SeparatorText(" SPECTRAL CONFIGURATION: ");
    if(ImGui::Checkbox("Do spectral rendering", &m_do_spectral))
    {
        if(m_do_spectral)
        {
            m_resample_wls = true;
        }
    }

    ImGui::Checkbox("THE FOG IS COMING", &m_enable_fog);

    if(m_do_spectral)
    {
        if(SliderFloatWithSteps("num_wavelengths ", &m_num_wavelengths, 4, 200, 4, "%d"))
        {
            m_resample_wls = true;
        }

        if(ImGui::SliderFloat("min_wl", &m_wl_min, 300, 860))
        {
            m_resample_wls = true;
            float wl_min_rc = m_response_curves_render->at(m_response_curve_names.at(m_selected_resp_curve))->get_wl_min();
            float wl_max_rc = m_response_curves_render->at(m_response_curve_names.at(m_selected_resp_curve))->get_wl_max();
            if(m_wl_min < wl_min_rc)
                m_wl_min = wl_min_rc;
            if(m_wl_min > wl_max_rc)
            {
                m_wl_min = wl_min_rc;
                m_wl_max = wl_max_rc;
            }
            if(m_wl_min > m_wl_max)
                m_wl_max = m_wl_min;
        }
            
        if(ImGui::SliderFloat("max_wl", &m_wl_max, 300, 860))
        {
            m_resample_wls = true;
            float wl_min_rc = m_response_curves_render->at(m_response_curve_names.at(m_selected_resp_curve))->get_wl_min();
            float wl_max_rc = m_response_curves_render->at(m_response_curve_names.at(m_selected_resp_curve))->get_wl_max();
            if(m_wl_max > wl_max_rc)
                m_wl_max = wl_max_rc;
            if(m_wl_max < wl_min_rc)
            {
                m_wl_max = wl_max_rc;
                m_wl_min = wl_min_rc;
            }
            if(m_wl_max < m_wl_min)
                m_wl_min = m_wl_max;
        }
            
        if(ImGui::SliderInt("wl_sample_strat", &m_sampling_strat, 0, STRAT_COUNT - 1, m_wl_interval_strat_names[m_sampling_strat].c_str()))
        {
            m_resample_wls = true;
        }
        
        if (Combo("response_curve", &m_selected_resp_curve, m_response_curve_names, m_response_curve_names.size()))
        {
            m_resample_wls = true;
            auto iter = m_response_curves_render->begin();
            std::advance(iter,  m_selected_resp_curve);
            // std::cout << "RESPONSE CURVE SELECTED: " << response_curve_names[selected_resp_curve] << ", " << iter->first << ", " << response_curves_render->at(response_curve_names[selected_resp_curve]) << std::endl;
            float wl_min_rc = m_response_curves_render->at(m_response_curve_names.at(m_selected_resp_curve))->get_wl_min();
            float wl_max_rc = m_response_curves_render->at(m_response_curve_names.at(m_selected_resp_curve))->get_wl_max();
            if(m_wl_max > wl_max_rc || m_wl_max < wl_min_rc)
                m_wl_max = wl_max_rc;
            if(m_wl_min < wl_min_rc || m_wl_min > wl_max_rc)
                m_wl_min = wl_min_rc;
        }
        ImGui::Checkbox("Is response XYZ?", &m_is_response_in_xyz);
    }

    ImGui::SeparatorText(" Lights Config:");
    if(ImGui::SliderFloat("power_mult_light", &m_illumination_multiplier, 0.0, 100.0))
    {
        /// TODO: Do for all the lights !!! (and move away from here)
        for(Light* light : m_last_rendered_scene->get_lights())
        {
            light->set_power_multiplier(m_illumination_multiplier);
        }
    }

    if(m_enable_fog)
    {
        ImGui::SeparatorText(" Jerlov Water Config: ");
        ImGui::SliderFloat("Absorption multiplier", &m_fog_sigma_a_mult, 0.0, 10.0);
        ImGui::SliderFloat("Scattering multiplier", &m_fog_sigma_s_mult, 0.0, 10.0);
        ImGui::SliderFloat("KD multiplier", &jerlov_KD_mult, 0.0, 10.0);
        ImGui::SliderFloat("Water depth", &water_depth, 0.0, 100.0);
    }

    ImGui::SeparatorText(" Other Options: ");
    if(ImGui::Button("Reload All Shaders"))
    {
        reload_shaders();
    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void RendererPBR::handle_resize(int w, int h)
{
    m_deferred_framebuffer->resize(w, h);
    m_pprocess_framebuffer->resize(w, h);
    std::cout << "RESIZE FRAMEBUFFERS! " << std::endl;
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
    glDeleteTextures(1, &m_sampled_wls_tex_id);
    glGenTextures(1, &m_sampled_wls_tex_id);
    glBindTexture(GL_TEXTURE_1D, m_sampled_wls_tex_id);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, m_num_wavelengths, 0, GL_RED, GL_FLOAT, wavelengths);
    delete wavelengths;
}

void RendererPBR::check_wls_range(Scene* scene)
{
    // init to unfeasible values
    float overall_max_min_wl = m_wl_min;        // the biggest min wl among emitters, chosen max wl, fog and response curve range
    float overall_min_max_wl = m_wl_max;        // the smallest max wl among emitters, chosen max wl, fog and response curve range

    // Compare against all the lights in the scene
    
    for(Light* light : scene->get_lights())
    {
        Spectrum* spec = light->get_spectrum();
        float light_min_wl = spec->get_wl_min();
        float light_max_wl = spec->get_wl_max();

        if(light_min_wl > overall_max_min_wl)
            overall_max_min_wl = light_min_wl;
        if(light_max_wl < overall_min_max_wl)
            overall_min_max_wl = light_max_wl;
    }

    // now, with the chosen response curve
    float wl_min_rc = m_response_curves_render->at(m_response_curve_names.at(m_selected_resp_curve))->get_wl_min();
    float wl_max_rc = m_response_curves_render->at(m_response_curve_names.at(m_selected_resp_curve))->get_wl_max();
    if(wl_max_rc < overall_min_max_wl)
        overall_min_max_wl = wl_max_rc;
    if(wl_min_rc > overall_max_min_wl)
        overall_max_min_wl = wl_min_rc;

    // Check the fog (only if it's enabled)
    if(m_enable_fog)
    {
        ResponseCurve* rc = scene->get_global_volume()->get_response_curve();
        float max_wl_fog = rc->get_wl_max();
        float min_wl_fog = rc->get_wl_min();

        if(max_wl_fog < overall_min_max_wl)
        overall_min_max_wl = max_wl_fog;
        if(min_wl_fog > overall_max_min_wl)
            overall_max_min_wl = min_wl_fog;
    }

    // Finally, set the wl range properly
    m_wl_min = overall_max_min_wl;
    m_wl_max = overall_min_max_wl;
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
    m_deferred_lighting_pass_shader->use();
    unsigned int lut_3d_ids[3];
    memcpy(lut_3d_ids, m_lut_textures->at(get_colorspace()).tex_ids, 3 * sizeof(unsigned int));
    for(int i = 0; i < 3; i++)
    {
        // LUTs , 0..2
        GL_CHECK(glActiveTexture(GL_TEXTURE0 + i));
        GL_CHECK(glBindTexture(GL_TEXTURE_3D, lut_3d_ids[i]));
    }

    // Sampled wavelengths, 3
    GL_CHECK(glActiveTexture(GL_TEXTURE3));
    GL_CHECK(glBindTexture(GL_TEXTURE_1D, m_sampled_wls_tex_id));

    // Response curve, 4
    unsigned int r_crv_tex_id = m_response_curves_render->at(m_response_curve_names.at(m_selected_resp_curve))->get_tex_id();
    GL_CHECK(glActiveTexture(GL_TEXTURE4));
    GL_CHECK(glBindTexture(GL_TEXTURE_1D, r_crv_tex_id));

    // Scene lights spectral response, 5
    GL_CHECK(glActiveTexture(GL_TEXTURE5));
    GL_CHECK(glBindTexture(GL_TEXTURE_1D_ARRAY, scene->get_emission_tex_array_id()));

    // Volume spectral parameters, 6
    GL_CHECK(glActiveTexture(GL_TEXTURE6));
    GL_CHECK(glBindTexture(GL_TEXTURE_1D, scene->get_global_volume()->get_spectral_tex_id()));

    // Framebuffer contents, 7..14 if additional textures are required by the material
    for(int i = 0; i < FRAMEBUFFER_TEX_NUM; i++)
    {
        GL_CHECK(glActiveTexture(GL_TEXTURE7 + i));
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

    glActiveTexture(GL_TEXTURE0);         // Bind the texture
    glBindTexture(GL_TEXTURE_2D, m_pprocess_framebuffer->getTextureID(0));

    render_quad();                                  // Render (to screen)
}


/// TODO: Not default framebuffer, but the postprocessing framebuffer!
void RendererPBR::blit_depth_buffer(GLFrameBufferRGBA<FRAMEBUFFER_TEX_NUM>* orig,
                                     GLFrameBufferRGBA<FRAMEBUFFER_TEX_NUM>* dest)
{
    unsigned int dest_id = dest->getID();
    if(dest == nullptr)
    {
        dest_id = 0;
    }
    orig->bindForReading();
    unsigned int _w = orig->getWidth();
    unsigned int _h = orig->getHeight();
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dest_id);
    //m_pprocess_framebuffer->bind();
    glBlitFramebuffer(
    0, 0, _w, _h, 0, 0, _w, _h, GL_DEPTH_BUFFER_BIT, GL_NEAREST
    );
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);       
}

void RendererPBR::set_deferred_lighting_shader_uniforms(Scene* scene)
{
    m_deferred_lighting_pass_shader->use();
    ResponseCurve* rc = m_response_curves_render->at(m_response_curve_names.at(m_selected_resp_curve));
    Volume* g_vol = scene->get_global_volume();
    
    m_deferred_lighting_pass_shader->setBool("do_spectral_uplifting", m_do_spectral);

    m_deferred_lighting_pass_shader->setBool("enable_fog", m_enable_fog);
    m_deferred_lighting_pass_shader->setVec3("vol_sigma_s_rgb", g_vol->get_sigma_s_rgb());    // m^-1
    m_deferred_lighting_pass_shader->setVec3("vol_sigma_a_rgb", g_vol->get_sigma_a_rgb());    // m^-1
    m_deferred_lighting_pass_shader->setVec3("vol_KD_rgb", g_vol->get_KD_rgb());              // unit?
    m_deferred_lighting_pass_shader->setFloat("wl_min_vol", g_vol->get_response_curve()->get_wl_min());
    m_deferred_lighting_pass_shader->setFloat("wl_max_vol", g_vol->get_response_curve()->get_wl_max());
    m_deferred_lighting_pass_shader->setFloat("sigma_a_mult", m_fog_sigma_a_mult);
    m_deferred_lighting_pass_shader->setFloat("sigma_s_mult", m_fog_sigma_s_mult);
    m_deferred_lighting_pass_shader->setFloat("jerlov_KD_mult", jerlov_KD_mult);
    m_deferred_lighting_pass_shader->setFloat("water_y_camera_offset", water_depth);

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
            m_deferred_lighting_pass_shader->setVec3("scene_lights[" + std::to_string(i) + "].attenuation", glm::vec3(1.0f, 1.0f, 1.0f));
            m_deferred_lighting_pass_shader->setVec3("scene_lights[" + std::to_string(i) + "].emission_rgb", dl->get_spectrum()->get_responses_rgb());
            m_deferred_lighting_pass_shader->setFloat("scene_lights[" + std::to_string(i) + "].emission_mult", dl->get_power_multiplier());
            m_deferred_lighting_pass_shader->setFloat("scene_lights[" + std::to_string(i) + "].wl_min", dl->get_spectrum()->get_wl_min());
            m_deferred_lighting_pass_shader->setFloat("scene_lights[" + std::to_string(i) + "].wl_max", dl->get_spectrum()->get_wl_max());
        }
        else if(POINT_LIGHT)
        {
            PointLight* pl = dynamic_cast<PointLight*>(light);
            glm::vec3 pos = pl->get_transform()->get_pos();
            m_deferred_lighting_pass_shader->setVec4("scene_lights[" + std::to_string(i) + "].position", glm::vec4(pos.x, pos.y, pos.z, 1.0f));
            m_deferred_lighting_pass_shader->setVec3("scene_lights[" + std::to_string(i) + "].direction", glm::vec3(0.0f,0.0f,0.0f));
            m_deferred_lighting_pass_shader->setVec3("scene_lights[" + std::to_string(i) + "].attenuation", pl->get_att_vec());
            m_deferred_lighting_pass_shader->setVec3("scene_lights[" + std::to_string(i) + "].emission_rgb", pl->get_spectrum()->get_responses_rgb());
            m_deferred_lighting_pass_shader->setFloat("scene_lights[" + std::to_string(i) + "].emission_mult", pl->get_power_multiplier());
            m_deferred_lighting_pass_shader->setFloat("scene_lights[" + std::to_string(i) + "].wl_min", pl->get_spectrum()->get_wl_min());
            m_deferred_lighting_pass_shader->setFloat("scene_lights[" + std::to_string(i) + "].wl_max", pl->get_spectrum()->get_wl_max());
        }
        i++;
    }
}
