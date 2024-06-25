#include <vector>

#include <sRAT-RT/scene.h>
#include <sRAT-RT/model.h>

#include <sRAT-RT/dir_light.h>          /// TODO: Delete after hardcoding my test scene
#include <sRAT-RT/point_light.h>        /// TODO: Delete after hardcoding my test scene

#include <sRAT-RT/hardcoded_scenes.h>   /// TODO: Delete?

Scene::Scene()
{
    /// Default scene?
    /// TODO: Define later a proper scene format,
    ///     once I'm more sure about how I'm going to implement all this.
    camera = new Camera(glm::vec3(0.0f, 0.0f, 2.0f));
    m_renderables.empty();
}

Scene::Scene(const std::string& scene_file_path)
{
    std::cout << "APP::STATUS::INIT::SCENE_INIT: Initializing the scene..." << std::endl;
    //camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    m_renderables.empty();

    /////////////////////////
    /// TODO: Under construction, will finish this at some point in development
    //          For now I'm only going to use a test scene so who cares
    //          THIS IS HARDCODED, PROCEED WITH CAUTION
    // bool wasd = load_renderable("../resources/objects/backpack/backpack.obj", 
    //             "../src/shaders/vertex_deferred_gbuffer.glsl", "../src/shaders/fragment_deferred_gbuffer.glsl",
    //             "../src/shaders/vertex_forward.glsl", "../src/shaders/fragment_forward.glsl");
        
    bool wasd = diffuse_spheres_scene();
    // bool wasd = test_scene();
    // bool wasd = reef_scene();

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

/// TODO: Fix all this once the hardcoded scene works, so I can load scenes properly :)
bool Scene::load_renderable(const std::string& model_path, std::string v_deferred, std::string f_deferred, std::string v_forward, std::string f_forward)
{
    /// Néstor's code
    // m_renderables.emplace_back();
    // RenderableObject& ro = m_renderables.back();

    /// TODO: Substitute this with a method that loads both a RO and  [...]
    // if(!ro.load_model(model_path)) {
    //     std::cout << "APP::STATUS::INIT::SCENE_ERROR: Could not load model " << model_path << std::endl;
    //     return false;
    // }
    // Shader* _deferred = new Shader(v_deferred.c_str(), f_deferred.c_str());
    // Shader* _forward = new Shader(v_forward.c_str(), f_forward.c_str());
    // model.set_deferred_shader(_deferred);
    // model.set_forward_shader(_forward);

    return true;
}

void Scene::generate_emission_tex_array()
{
    glGenTextures(1, &m_lights_emission_tex_array_id);
    glBindTexture(GL_TEXTURE_1D_ARRAY, m_lights_emission_tex_array_id);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Determine the maximum size among all spectral data arrays
    GLsizei maxArraySize = 0;
    for (auto light : m_scene_lights) {
        GLsizei arraySize = light->get_spectrum()->get_n_samples();
        if (arraySize > maxArraySize) {
            maxArraySize = arraySize;
        }
    }

    // Allocate storage for the texture array
    GLsizei numLayers = m_scene_lights.size();
    glTexStorage2D(GL_TEXTURE_1D_ARRAY, 1, GL_R32F, maxArraySize, numLayers);

    // Upload data to the texture array
    for (size_t i = 0; i < numLayers; i++) {
        auto light = m_scene_lights.at(i);
        GLsizei arraySize = light->get_spectrum()->get_n_samples();
        float* data = new float[arraySize];
        for(int j = 0; j < arraySize; j++)
        {
            data[j] = light->get_spectrum()->get_responses()->at(j).response;
        }
        glTexSubImage2D(GL_TEXTURE_1D_ARRAY, 0, 0, i, arraySize, 1, GL_RED, GL_FLOAT,  data);
        delete data;
    }
}

Camera* Scene::get_camera() const
{
    return camera;
}

Volume* Scene::get_global_volume() const
{
    return global_volume;
}

std::vector<RenderableObject*> Scene::get_renderables() const
{
    return m_renderables;
}

std::vector<Light*> Scene::get_lights() const
{
    return m_scene_lights;
}

int Scene::get_num_lights() const 
{
    return m_scene_lights.size();
}

unsigned int Scene::get_emission_tex_array_id() const
{
    return m_lights_emission_tex_array_id;
}
