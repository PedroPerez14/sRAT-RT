#include <sRAT-RT/scene.h>
#include <sRAT-RT/model.h>

Scene::Scene()
{
    /// Default scene?
    // idk man im a bit tired today
    /// TODO: Define later a proper scene format,
    ///     once I'm more sure about how I'm going to implement all this.
}

Scene::Scene(const std::string& scene_file_path)
{
    Camera* camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    m_models.empty();
    /// TODO: Under construction, will finish this at some point in development
    //          For now I'm only going to use a test scene so who cares
}

bool Scene::load_model(const std::string& model_path)
{
    /// Néstor's code
    m_models.emplace_back();
    Model& model = m_models.back();

    if(!model.loadModel(model_path)) {
        std::cout << "APP::STATUS::INIT::SCENE_ERROR: Could not load model " << model_path << std::endl;
        return false;
    }
    return true;
}

Camera* Scene::get_camera()
{
    return camera;
}