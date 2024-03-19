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

    m_deferred_lighting_pass_shader = new Shader(
        "../src/shaders/vertex_deferred_lighting.glsl", 
        "../src/shaders/fragment_deferred_lighting.glsl");
    m_models.empty();

    init_fullscreen_quad();
}

Scene::Scene(const std::string& scene_file_path)
{
    std::cout << "APP::STATUS::INIT::SCENE_INIT: Initializing the scene..." << std::endl;
    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    m_deferred_lighting_pass_shader = new Shader(
        "../src/shaders/vertex_deferred_lighting.glsl", 
        "../src/shaders/fragment_deferred_lighting.glsl");
    m_models.empty();

    init_fullscreen_quad();         // for deferred shading, little hack i guess?

    /////////////////////////
    /// TODO: Under construction, will finish this at some point in development
    //          For now I'm only going to use a test scene so who cares
    //          THIS IS HARDCODED, PROCEED WITH CAUTION
    bool wasd = load_model("../resources/objects/backpack/backpack.obj", 
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

bool Scene::load_model(const std::string& model_path, std::string v_deferred, std::string f_deferred, std::string v_forward, std::string f_forward)
{
    /// Néstor's code
    m_models.emplace_back();
    Model& model = m_models.back();

    if(!model.loadModel(model_path)) {
        std::cout << "APP::STATUS::INIT::SCENE_ERROR: Could not load model " << model_path << std::endl;
        return false;
    }
    Shader* _deferred = new Shader(v_deferred.c_str(), f_deferred.c_str());
    Shader* _forward = new Shader(v_forward.c_str(), f_forward.c_str());
    model.set_deferred_shader(_deferred);
    model.set_forward_shader(_forward);

    return true;
}

void Scene::set_shader_camera_uniforms(Shader* shader, Camera* cam, int width, int height)
{
    /// TODO: HARDCODEADO, CAMBIAR LUEGO
    shader->use();
    glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom), (float)width / (float) height, 0.1f, 100.0f);
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));

    shader->setMat4("projection", projection);
    shader->setMat4("view", view);
    shader->setMat4("model", model);
}

/// TODO: HARDCODED!
void Scene:: set_scene_lighting_uniforms(Shader* shader, Camera* cam)
{
    /// TODO: Cómo cojones hago yo para poner los uniforms de un shader cualquiera xdd
    shader->use();
    shader->setFloat("exposure", 0.75);
    shader->setInt("flipY", 0);
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