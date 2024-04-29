#pragma once
#ifndef SCENE_CLASS_H
#define SCENE_CLASS_H

#include <glm/glm.hpp>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <string>
#include <sRAT-RT/model.h>
#include <sRAT-RT/light.h>
#include <sRAT-RT/camera.h>
#include <sRAT-RT/gl_check.h>
#include <sRAT-RT/framebuffer.h>

class Scene
{
private:
    Camera* camera;
    std::vector<RenderableObject> m_renderables;
    std::vector<Light> m_scene_lights;

    bool load_renderable(const std::string& model_path, std::string v_deferred, std::string f_deferred, std::string v_forward, std::string f_forward);

    void init_fullscreen_quad();    // Init the quad for the deferred lighting pass

public:
    Scene();
    Scene(const std::string& scene_file_path);

    Camera* get_camera() const;
    std::vector<RenderableObject> get_renderables() const;
    std::vector<Light> get_lights() const;
    int get_num_lights() const;
};

#endif