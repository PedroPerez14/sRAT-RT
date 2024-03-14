#ifndef SCENE_CLASS_H
#define SCENE_CLASS_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <string>
#include <sRAT-RT/model.h>
#include <sRAT-RT/camera.h>
#include <sRAT-RT/framebuffer.h>

class Scene
{
private:
    Camera* camera;
    Shader* m_deferred_gbuffer_pass_shader;
    std::vector<Model> m_models;

    bool load_model(const std::string& model_path);
    void set_shader_camera_uniforms(Shader& shader, Camera* cam);

public:
    Scene();
    Scene(const std::string& scene_file_path);

    Camera* get_camera();

    template <GLuint INTERNAL_FORMAT, GLuint FORMAT, size_t N_TEXTURES>
    void draw(GLFrameBuffer<INTERNAL_FORMAT, FORMAT, N_TEXTURES>* framebuffer);

    /// TODO: std::to_string();
};

#endif