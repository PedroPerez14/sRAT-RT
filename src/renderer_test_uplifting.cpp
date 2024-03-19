#include <sRAT-RT/renderer_test_uplifting.h>
#include <sRAT-RT/stb_image.h>

RendererTestUplifting::RendererTestUplifting(unsigned int fb_w, unsigned int fb_h)
{
    m_deferred_framebuffer = new GLFrameBufferRGBA<FRAMEBUFFER_TEX_NUM>();
    m_deferred_framebuffer->init(fb_w, fb_h);

    m_deferred_lighting_pass_shader = new Shader(
    "../src/shaders/test_uplifting/fragment_uplifting.glsl", 
    "../src/shaders/test_uplifting/vertex_uplifting.glsl");

    /// TODO: Probar a cargar varias texturas !!!!
    tex_test = texture_from_file(/* blablablabla path goes brr */);
    tex_lut = lut_tex_create(/* something something something */);

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

    m_deferred_lighting_pass_shader->use();
    set_scene_lighting_uniforms(m_deferred_lighting_pass_shader, cam);
    GL_CHECK(glActiveTexture(GL_TEXTURE0));                 // Bind the 2D texture for the test
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, tex_test));
    GL_CHECK(glActiveTexture(GL_TEXTURE1));
    GL_CHECK(glBindTexture(GL_TEXTURE_3D, tex_lut));
    /// TODO: Now, 


    // Diffuse Texture x:
    //framebuffer->bindTextures();
    render_quad();

    // 2.5. Move the depth data from the gbuffer to the default framebuffer
    m_deferred_framebuffer->bindForReading();              // glBindFramebuffer(GL_READ_FRAMEBUFFER, 0)
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);  // write to default framebuffer
    glBlitFramebuffer(0, 0, m_deferred_framebuffer->getWidth(), m_deferred_framebuffer->getHeight(), 
                        0, 0, m_deferred_framebuffer->getWidth(), m_deferred_framebuffer->getHeight(),
                        GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /// 3. Forward pass (in case we want to do it)
    /// TODO: Should I let the users define a null forward shader to skip this pass entirely?
    ///         Or just use a standard forward shader that does nothing???? hmmm
    for(Model& model : models)
    {
        /// TODO: Do the uniforms later
        set_shader_camera_uniforms(model.get_forward_shader(), cam, m_deferred_framebuffer->getWidth(), m_deferred_framebuffer->getHeight()); /// TODO NOT TRIED FORWARD PASS YET!
        model.draw((*model.get_forward_shader()));                      
    }

    // We don't call glfwSwapBuffers here, 
    //  App::run() is already doing it on its rendering loop
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
    shader->use();
    shader->setFloat("exposure", 0.75);
    shader->setInt("flipY", 0);
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

unsigned int RendererTestUplifting::lut_tex_create(/* SOMETHING WOULD GO HERE */)
{
    return 0;
}
