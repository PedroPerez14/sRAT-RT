#ifndef SCENE_CLASS_H
#define SCENE_CLASS_H

#include <string>
#include <sRAT-RT/model.h>
#include <sRAT-RT/camera.h>

class Scene
{
private:
    Camera* camera;
    std::vector<Model> m_models;

    bool load_model(const std::string& model_path);

public:
    Scene();
    Scene(const std::string& scene_file_path);


    Camera* get_camera();

    /// TODO: std::to_string();
};

#endif