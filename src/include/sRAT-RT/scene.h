#pragma once
#ifndef SCENE_CLASS_H
#define SCENE_CLASS_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <string>
#include <sRAT-RT/model.h>
#include <sRAT-RT/camera.h>
#include <sRAT-RT/gl_check.h>
#include <sRAT-RT/framebuffer.h>

class Scene
{
private:
    Camera* camera;
    Shader* m_deferred_lighting_pass_shader;
    unsigned int m_fullscreen_vao;
    std::vector<Model> m_models;

    bool load_model(const std::string& model_path);

    /// HERE: I'll probably have to change this later on, for now it's fine
    void set_shader_camera_uniforms(Shader* shader, Camera* cam);
    void set_scene_lighting_uniforms(Shader* shader, Camera* cam);

    void init_fullscreen_quad();    // Init the quad for the deferred lighting pass
    
    template <GLuint INTERNAL_FORMAT, GLuint FORMAT, size_t N_TEXTURES>
    void render_quad(GLFrameBuffer<INTERNAL_FORMAT, FORMAT, N_TEXTURES>* framebuffer)             // The deferred lighting pass itself
    {
        m_deferred_lighting_pass_shader->use();
        // Diffuse Texture x:
        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        glBindTexture(GL_TEXTURE_2D, framebuffer->getTextureID(0));
        m_deferred_lighting_pass_shader->setVec2("exposure", glm::vec2(0.02, 0.75));     // Invent

        glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded
        
        // Fullscreen triangle
        glBindVertexArray(m_fullscreen_vao);
        GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 3)); // Vertices created in vertex shader without vert buffer
        glEnable(GL_DEPTH_TEST); // re enable depth test if needed
    }

public:
    Scene();
    Scene(const std::string& scene_file_path);

    Camera* get_camera();

    template <GLuint INTERNAL_FORMAT, GLuint FORMAT, size_t N_TEXTURES>
    void draw(GLFrameBuffer<INTERNAL_FORMAT, FORMAT, N_TEXTURES>* framebuffer, Camera* cam)
    {
        framebuffer->bind();
        glClearColor(0.0, 0.0, 0.0, 1.0);
        framebuffer->clear();
        /// 1.- Deferred geometry pass: Render stuff into G-Buffer
        for(Model& model : m_models)
        {
            /// TODO: Do the uniforms later
            set_shader_camera_uniforms(model.get_deferred_shader(), camera); //todo
            model.draw((*model.get_deferred_shader()));        // Put everything in our deferred GBuffer
        }
        framebuffer->unbind();

        // 1.5. Move the depth data from the gbuffer to the default framebuffer
        framebuffer->bindForReading();              // glBindFramebuffer(GL_READ_FRAMEBUFFER, 0)
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);  // write to default framebuffer
        glBlitFramebuffer(0, 0, framebuffer->getWidth(), framebuffer->getHeight(), 0, 0, framebuffer->getWidth(), framebuffer->getHeight(),
        GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. Deferred lighting pass: use g-buffer to calculate the scene’s lighting
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // We have unbinded the gbuffer, so this is the default framebuffer (the "screen"?)
        
        framebuffer->bindTextures();
        m_deferred_lighting_pass_shader->use();
        /// TODO: Do the uniforms later? Add "lights" parameter?? who knows
        set_scene_lighting_uniforms(m_deferred_lighting_pass_shader, camera);                          
        render_quad(framebuffer);
    }
};

#endif