#include <vector>

#include <sRAT-RT/scene.h>
#include <sRAT-RT/model.h>

Scene::Scene()
{
    /// Default scene?
    // idk man im a bit tired today
    /// TODO: Define later a proper scene format,
    ///     once I'm more sure about how I'm going to implement all this.
    Camera* camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

    m_deferred_lighting_pass_shader = new Shader(
        "./shaders/vertex_deferred_lighting.glsl", 
        "./shaders/fragment_deferred_lighting.glsl");
    m_models.empty();

    init_fullscreen_quad();
}

Scene::Scene(const std::string& scene_file_path)
{
    Camera* camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    m_deferred_lighting_pass_shader = new Shader(
        "./src/shaders/vertex_deferred_lighting.glsl", 
        "./src/shaders/fragment_deferred_lighting.glsl");
    m_models.empty();

    init_fullscreen_quad();
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

void Scene::set_shader_camera_uniforms(Shader* shader, Camera* cam)
{
    /// TODO: Cómo cojones hago yo para poner los uniforms de un shader cualquiera xdd
    //shader.set();
}

void Scene:: set_scene_lighting_uniforms(Shader* shader, Camera* cam)
{
    /// TODO: Cómo cojones hago yo para poner los uniforms de un shader cualquiera xdd
    //shader.set();
}

/// Do only once during initialization !
void Scene::init_fullscreen_quad()
{
    GL_CHECK(glGenVertexArrays(1, &m_fullscreen_vao));
    GL_CHECK(glBindVertexArray(m_fullscreen_vao));
    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL));
}

Camera* Scene::get_camera()
{
    return camera;
}


/// Needs the framebuffer to be bound beforehand! 
//  (call framebuffer->bind() in run() beofre doing scene->draw() and it's ok)

void wasd(std::vector<GLuint> &vector)
{
    vector.clear();
}