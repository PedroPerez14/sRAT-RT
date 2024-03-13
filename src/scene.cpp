#include <sRAT-RT/scene.h>

Scene::Scene()
{
    /// Default scene?
    // idk man im a bit tired today
}

Scene::Scene(const std::string& scene_file_path)
{
    
}

bool Scene::load_model(const std::string& model_path)
{
    return true;
}

Camera* Scene::get_camera()
{
    return camera;
}