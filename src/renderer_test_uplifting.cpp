#include <imgui.cpp>
#include <imgui_impl_glfw.h>
#include <sRAT-RT/stb_image.h>
#include <imgui_impl_opengl3.h>
#include <sRAT-RT/response_curve.h>
#include <sRAT-RT/renderer_test_uplifting.h>

RendererTestUplifting::RendererTestUplifting(unsigned int fb_w, unsigned int fb_h, 
                    std::unordered_map<colorspace, RGB2Spec*>* look_up_tables, 
                    std::unordered_map<std::string, ResponseCurve*>* response_curves, 
                    colorspace _colorspace, int n_wls, float wl_min, float wl_max)
{
    m_deferred_framebuffer = new GLFrameBufferRGBA<FRAMEBUFFER_TEX_NUM>();
    m_deferred_framebuffer->init(fb_w, fb_h);
    m_uplifting_shader = new Shader(
    "../src/shaders/test_uplifting/vertex_uplifting.glsl", 
    "../src/shaders/test_uplifting/fragment_uplifting.glsl");
    m_final_pass_shader = new Shader(
    "../src/shaders/test_uplifting/vertex_uplifting_pass2.glsl", 
    "../src/shaders/test_uplifting/fragment_uplifting_pass2.glsl");
    /// TODO: Try uplifting several different textures!
    tex_test = texture_from_file("container.jpg", "../resources/textures");
    // Now, initialize the 3D textures from the LUTs we loaded as 1D arrays
    lut_textures_create(look_up_tables);
    response_curves_render = response_curves;
    selected_resp_curve = 0;
    populate_resp_curves_list();
    working_colorspace = _colorspace;
    num_wavelengths = n_wls;
    this->wl_min = wl_min;
    this->wl_max = wl_max;
    sampling_strat = (int)STRAT_EQUISPACED;
    init_fullscreen_quad();
}

void RendererTestUplifting::render_scene(Scene* scene) const
{
    Camera* cam = scene->get_camera();

    // now clear the gbuffer and write to it
    m_deferred_framebuffer->bind();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    m_deferred_framebuffer->clear();
    //m_deferred_framebuffer->unbind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear default framebuffer (the screen)

    /// Use the shader, set its uniforms (TODO), bind the texture to uplift, 
    ///         as well as the 3D ones that represent the LUTs
    m_uplifting_shader->use();
    set_scene_lighting_uniforms(m_uplifting_shader, cam);

    // Bind the 2D texture for the test
    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, tex_test));

    // Bind the Look up tables corresponding to the desired color space
    unsigned int lut_3d_ids[3];
    memcpy(lut_3d_ids, lut_textures->at(get_colorspace()).tex_ids, 3 * sizeof(unsigned int));
    for(int i = 0; i < 3; i++)
    {
        GL_CHECK(glActiveTexture(GL_TEXTURE1 + i));
        GL_CHECK(glBindTexture(GL_TEXTURE_3D, lut_3d_ids[i]));
    }

    render_quad();

    m_deferred_framebuffer->unbind();

    // 2. Final pass: Draw into the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear default framebuffer (the screen)

    // Set the shader uniforms
    m_final_pass_shader->use();
    m_final_pass_shader->setInt("show_original_tex_or_spec2rgb", 1);
    
    for(int i = 0; i < 4; i++)
    {
        GL_CHECK(glActiveTexture(GL_TEXTURE0 + i));
        glBindTexture(GL_TEXTURE_2D, m_deferred_framebuffer->getTextureID(i));
    }
    render_quad();

    // We don't call glfwSwapBuffers here, 
    // App::run() is already doing it on its rendering loop, so we're done!
}

bool SliderFloatWithSteps(const char* label, int* v, float v_min, float v_max, float v_step, const char* display_format)
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

bool Combo(const char* label, int* current_item, const std::vector<std::string>& items, int items_count, int height_in_items = -1)
{
   return ImGui::Combo(label, current_item, [](void* data, int idx, const char** out_text) { *out_text = ((const std::vector<std::string>*)data)->at(idx).c_str(); return true; }, (void*)&items, items_count, height_in_items);
}

void RendererTestUplifting::render_ui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //ImGui::SliderInt("Number of Wavelengths: ", &num_wavelengths, 4, 40, "%d");
    static bool showDemo = false;
    ImGui::Begin("Example");
    ImGui::SeparatorText(" SPECTRAL CONFIGURATION: ");
    ImGui::Checkbox("Do spectral rendering", &do_spectral);
    if(do_spectral)
    {
        SliderFloatWithSteps("num_wavelengths ", &num_wavelengths, 4, 40, 4, "%d");
        if(ImGui::SliderFloat("min_wl", &wl_min, 300, 800))
            if(wl_min > wl_max)
                wl_max = wl_min;
        if(ImGui::SliderFloat("max_wl", &wl_max, 300, 800))
            if(wl_max < wl_min)
                wl_min = wl_max;
        
        ImGui::SliderInt("wl selection strategy", &sampling_strat, 0, STRAT_COUNT - 1, wl_interval_strat_names[sampling_strat].c_str());
        
        if (Combo("response curve", &selected_resp_curve, response_curve_names, response_curve_names.size()))
        {
            auto iter = response_curves_render->begin();
            std::advance(iter,  selected_resp_curve);
            // std::cout << "RESPONSE CURVE SELECTED: " << response_curve_names[selected_resp_curve] << ", " << iter->first << ", " << response_curves_render->at(response_curve_names[selected_resp_curve]) << std::endl;
            
        }

    }
    
    if (ImGui::Button("Show/Hide ImGui demo"))
      showDemo = !showDemo;
    ImGui::End();
    if (showDemo)
      ImGui::ShowDemoWindow(&showDemo);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void RendererTestUplifting::handle_resize(int w, int h)
{
    m_deferred_framebuffer->resize(w, h);
}

void RendererTestUplifting::set_shader_camera_uniforms(Shader* shader, Camera* cam, int width, int height) const
{
    /// TODO: HARDCODEADO, CAMBIAR LUEGO
    shader->use();
    glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom), (float)width / (float) height, 0.1f, 100.0f);
    glm::mat4 view = cam->GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));

    shader->setMat4("projection", projection);
    shader->setMat4("view", view);
    shader->setMat4("model", model);
}

void RendererTestUplifting::set_scene_lighting_uniforms(Shader* shader, Camera* cam) const
{
    /// TODO: Cómo cojones hago yo para poner los uniforms de un shader cualquiera xdd
    /// De momento hardcodeo los del shader de uplifting y ya veré 
    shader->use();
    //shader->setFloat("exposure", 0.75);
    //shader->setInt("flipY", 0);
    shader->setInt("n_wls", num_wavelengths);
    // uniform float wls[40] ??
}

void RendererTestUplifting::render_quad() const
{
    glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded   
    // Fullscreen triangle
    glBindVertexArray(m_fullscreen_vao);
    GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 3)); // Vertices created in vertex shader without vert buffer
    glEnable(GL_DEPTH_TEST); // re enable depth test if needed
    glBindVertexArray(0);
}
    
/// Do only once during initialization!
void RendererTestUplifting::init_fullscreen_quad()
{
    GL_CHECK(glGenVertexArrays(1, &m_fullscreen_vao));
    GL_CHECK(glBindVertexArray(m_fullscreen_vao));
    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL));
}

unsigned int RendererTestUplifting::texture_from_file(const char* path, const std::string& directory)
{
    std::string filename(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "APP::RENDERER::TEST_UPLIFTING::ERROR: Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void RendererTestUplifting::lut_textures_create(std::unordered_map<colorspace, RGB2Spec*>* look_up_tables)
{
    lut_textures = new std::unordered_map<colorspace, lut_as_tex3d>();

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

        (*lut_textures)[csp] = lut_tex_3d;

        std::cout << "APP::STATUS::INIT::RENDERER::GEN_LUTs_AS_TEX3D: Copied LUT for color space " 
        << colorspace_key << " with size " << lut_textures->at(csp).res << "x" << lut_textures->at(csp).res << "x" << lut_textures->at(csp).res
        << "\n Tex_IDS: " << lut_textures->at(csp).tex_ids[0] << ", " << lut_textures->at(csp).tex_ids[1] << ", " << lut_textures->at(csp).tex_ids[2]
        << std::endl;
    }
}

colorspace RendererTestUplifting::get_colorspace() const
{
    return working_colorspace;
}

void RendererTestUplifting::set_colorspace(colorspace _c)
{
    working_colorspace = _c;
}

void RendererTestUplifting::reload_shaders()
{
    delete m_uplifting_shader;
    delete m_final_pass_shader;
    
    new Shader(
    "../src/shaders/test_uplifting/vertex_uplifting.glsl", 
    "../src/shaders/test_uplifting/fragment_uplifting.glsl");

    m_final_pass_shader = new Shader(
    "../src/shaders/test_uplifting/vertex_uplifting_pass2.glsl", 
    "../src/shaders/test_uplifting/fragment_uplifting_pass2.glsl");
}

void RendererTestUplifting::populate_resp_curves_list()
{
    response_curve_names.empty();

    for (auto i = response_curves_render->begin(); i != response_curves_render->end(); i++)
    {
        response_curve_names.push_back(i->first);
    }
}