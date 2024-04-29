#include <vector>

#include <sRAT-RT/scene.h>
#include <sRAT-RT/model.h>

Scene::Scene()
{
    /// Default scene?
    // idk man im a bit tired today
    /// TODO: Define later a proper scene format,
    ///     once I'm more sure about how I'm going to implement all this.
    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    m_renderables.empty();
}

Scene::Scene(const std::string& scene_file_path)
{
    std::cout << "APP::STATUS::INIT::SCENE_INIT: Initializing the scene..." << std::endl;
    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    m_renderables.empty();

    /////////////////////////
    /// TODO: Under construction, will finish this at some point in development
    //          For now I'm only going to use a test scene so who cares
    //          THIS IS HARDCODED, PROCEED WITH CAUTION
    bool wasd = load_renderable("../resources/objects/backpack/backpack.obj", 
                "../src/shaders/vertex_deferred_gbuffer.glsl", "../src/shaders/fragment_deferred_gbuffer.glsl",
                "../src/shaders/vertex_forward.glsl", "../src/shaders/fragment_forward.glsl");
    if(wasd)
    {
        std::cout << "Scene successfully loaded!" << std::endl;
    }
    else
    {
        std::cout << "APP::STATUS::SCENE_LOAD::ERROR: Test model was not properly loaded!" << std::endl;
    }
    /////////////////////////
}

bool Scene::load_renderable(const std::string& model_path, std::string v_deferred, std::string f_deferred, std::string v_forward, std::string f_forward)
{
    /// Néstor's code
    m_renderables.emplace_back();
    RenderableObject& ro = m_renderables.back();

    /// TODO: Substitute this with a method that loads both a RO and  [...]
    if(!ro.load_model(model_path)) {
        std::cout << "APP::STATUS::INIT::SCENE_ERROR: Could not load model " << model_path << std::endl;
        return false;
    }
    // Shader* _deferred = new Shader(v_deferred.c_str(), f_deferred.c_str());
    // Shader* _forward = new Shader(v_forward.c_str(), f_forward.c_str());
    // model.set_deferred_shader(_deferred);
    // model.set_forward_shader(_forward);

    return true;
}

Camera* Scene::get_camera() const
{
    return camera;
}

std::vector<RenderableObject> Scene::get_renderables() const
{
    return m_renderables;
}

std::vector<Light> Scene::get_lights() const
{
    return m_scene_lights;
}

int Scene::get_num_lights() const 
{
    return m_scene_lights.size();
}