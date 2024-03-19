#include <sRAT-RT/renderer_def_fwd.h>

#define FRAMEBUFFER_TEX_NUM 3

RendererDeferredAndForward::RendererDeferredAndForward(unsigned int fb_w, unsigned int fb_h)
{
    m_deferred_framebuffer = new GLFrameBufferRGBA<FRAMEBUFFER_TEX_NUM>();
    m_deferred_framebuffer->init(fb_w, fb_h);
    m_deferred_lighting_pass_shader = new Shader(
    "../src/shaders/vertex_deferred_lighting.glsl", 
    "../src/shaders/fragment_deferred_lighting.glsl");

    init_fullscreen_quad();
}

/// @brief This renderer will render the scene using deferred shading and another forward pass totalling in 3 passes,\n 
///         2 deferred (gbuffer creation + lighting) and the forward one
/// @param scene 
void RendererDeferredAndForward::render_scene(Scene* scene) const
{
    Camera* cam = scene->get_camera();

    // now clear the gbuffer and write to it
    m_deferred_framebuffer->bind();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    m_deferred_framebuffer->clear();

    /// 1.- Deferred geometry pass: Render stuff into G-Buffer
    std::vector<Model> models = scene->get_models();
    for(Model& model : models)
    {
        /// TODO: Do the uniforms later
        set_shader_camera_uniforms(model.get_deferred_shader(), cam, m_deferred_framebuffer->getWidth(), m_deferred_framebuffer->getHeight());
        model.draw((*model.get_deferred_shader()));             // Put the data inside our deferred GBuffer
    }
    m_deferred_framebuffer->unbind();

    // 2. Deferred lighting pass: use g-buffer to calculate the scene’s lighting
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear default framebuffer (the screen)

    m_deferred_lighting_pass_shader->use();
    set_scene_lighting_uniforms(m_deferred_lighting_pass_shader, cam);
    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    glBindTexture(GL_TEXTURE_2D, m_deferred_framebuffer->getTextureID(2));
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

void RendererDeferredAndForward::set_shader_camera_uniforms(Shader* shader, Camera* cam, int width, int height) const
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

void RendererDeferredAndForward::set_scene_lighting_uniforms(Shader* shader, Camera* cam) const
{
    /// TODO: Cómo cojones hago yo para poner los uniforms de un shader cualquiera xdd
    shader->use();
    shader->setFloat("exposure", 0.75);
    shader->setInt("flipY", 0);
}

void RendererDeferredAndForward::render_quad() const
{
    glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded   
    // Fullscreen triangle
    glBindVertexArray(m_fullscreen_vao);
    GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 3)); // Vertices created in vertex shader without vert buffer
    glEnable(GL_DEPTH_TEST); // re enable depth test if needed
    glBindVertexArray(0);
}
    
/// Do only once during initialization !
void RendererDeferredAndForward::init_fullscreen_quad()
{
    GL_CHECK(glGenVertexArrays(1, &m_fullscreen_vao));
    GL_CHECK(glBindVertexArray(m_fullscreen_vao));
    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL));
}
