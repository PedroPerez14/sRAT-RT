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
#include <sRAT-RT/volume.h>
#include <sRAT-RT/gl_check.h>
#include <sRAT-RT/framebuffer.h>

class Scene
{
private:
    Camera* camera;
    Volume* global_volume;
    std::vector<RenderableObject*> m_renderables;
    std::vector<Light*> m_scene_lights;
    unsigned int m_lights_emission_tex_array_id;

    bool load_renderable(const std::string& model_path, std::string v_deferred, std::string f_deferred, std::string v_forward, std::string f_forward);

    void generate_emission_tex_array();



    /// TODO: Delete the test scenes at some point (and add a proper scene file loader!)
    bool test_scene();
    bool diffuse_spheres_scene();


public:
    Scene();
    Scene(const std::string& scene_file_path);

    Camera* get_camera() const;
    Volume* get_global_volume() const;
    
    std::vector<RenderableObject*> get_renderables() const;
    std::vector<Light*> get_lights() const;
    int get_num_lights() const;
    unsigned int get_emission_tex_array_id() const;
};

#endif