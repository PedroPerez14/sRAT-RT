#include <sRAT-RT/scene.h>
#include <sRAT-RT/model.h>

#include <vector>

Scene::Scene()
{
    /// Default scene?
    // idk man im a bit tired today
    /// TODO: Define later a proper scene format,
    ///     once I'm more sure about how I'm going to implement all this.
    Camera* camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    m_models.empty();
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


/// Needs the framebuffer to be bound beforehand! 
//  (call framebuffer->bind() in run() beofre doing scene->draw() and it's ok)

void wasd(std::vector<GLuint> &vector)
{
    vector.clear();
}

template <GLuint INTERNAL_FORMAT, GLuint FORMAT, size_t N_TEXTURES>
void Scene::draw(GLFrameBuffer<INTERNAL_FORMAT, FORMAT, N_TEXTURES>* framebuffer, Camera* cam)
{
    framebuffer->bind();
    glClearColor(0.0f, 0.0f, 0,0f, 1.0f);
    framebuffer->clear();
    /// 1.- Deferred geometry pass: Render stuff into G-Buffer
    for(Model model : m_models)
    {
        /// TODO: Do the uniforms later
        set_shader_camera_uniforms();
        model.draw(model.m_shader_deferred);        /// TODO: CREO QUE NO ES ASÍ
    }

    // 2. Deferred ighting pass: use g-buffer to calculate the scene’s lighting
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    lightingPassShader.use();
    BindAllGBufferTextures();
    SetLightingUniforms();
    RenderQuad();
}